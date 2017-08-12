/******************************************************************************
 JXFTCCell.cpp

	Represents a cell in the nested tables constructed by
	JXContainer::ExpandToFitContent.

	BASE CLASS = JXContainer

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXFTCCell.h>
#include <JXWindow.h>
#include <JXDisplay.h>
#include <JXDragPainter.h>
#include <JXDNDManager.h>
#include <JXColormap.h>
#include <JOrderedSetUtil.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXFTCCell::JXFTCCell
	(
	JXContainer*	matchObj,
	JXContainer*	enc,
	const Direction	direction
	)
	:
	JXContainer(enc->GetWindow(), enc),
	itsWidget(matchObj),
	itsDirection(direction),
	itsSyncChildrenFlag(kJFalse),
	itsChildren(NULL),
	itsChildSpacing(NULL),
	itsChildPositions(NULL)
{
	if (itsWidget != NULL)
		{
		itsFrameG = itsWidget->GetFrameForFTC();
		}
	else if (GetEnclosure() != GetWindow())
		{
		itsFrameG = GetEnclosure()->GetFrameGlobal();
		}

	Refresh();

	if (theDebugFTCFlag)
		{
		std::cout << "Creating " << ToString() << std::endl;
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFTCCell::~JXFTCCell()
{
	if (theDebugFTCFlag)
		{
		std::cout << "Deleting " << ToString() << std::endl;
		}

	JXWindow* window = GetWindow();
	if (IsVisible())
		{
		window->RefreshRect(itsFrameG);
		}

	jdelete itsChildren;
	jdelete itsChildSpacing;
	jdelete itsChildPositions;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JXFTCCell::ToString()
	const
{
	std::ostringstream s;
	JXContainer::ToString().Print(s);
	s << (itsDirection == kHorizontal ? " (horiz)" : itsDirection == kVertical ? " (vert)" : "");
	if (itsWidget != NULL)
		{
		s << " (" << itsWidget->ToString() << ")";
		}
	return JString(s.str());
}

/******************************************************************************
 Indent (private)

 ******************************************************************************/

JString
JXFTCCell::Indent
	(
	const JSize extra
	)
	const
{
	JString s;
	const JSize count = GetDepth() + extra;
	for (JIndex i=1; i<=count; i++)
		{
		s += "  ";
		}
	return s;
}

/******************************************************************************
 GetDepth

 ******************************************************************************/

JSize
JXFTCCell::GetDepth()
	const
{
	JSize depth = 0;

	JXContainer* p = GetEnclosure();
	while (p != NULL)
		{
		depth++;
		p = p->GetEnclosure();
		}

	return depth - 1;
}

/******************************************************************************
 Expand

	Expand enclosed objects to fit their content and then enforce
	invariants on the result.  Returns the new size.

 ******************************************************************************/

JCoordinate
JXFTCCell::Expand
	(
	const JBoolean horizontal,
	const JBoolean subLayout
	)
{
	itsSyncChildrenFlag   = kJTrue;
	itsSyncHorizontalFlag = horizontal;

	if (itsWidget != NULL)
		{
		const JCoordinate size = ExpandWidget();
		const JRect r = itsWidget->GetFrameForFTC();
		SetSize(r.width(), r.height());
		return size;
		}

	assert( itsDirection != kNoDirection );

	if (theDebugFTCFlag && GetEnclosure() == GetWindow())
		{
		std::cout << "----------" << std::endl;
		std::cout << "ExpandLayout (" << (horizontal ? "horiz" : "vert") << "):" << std::endl;
		}
	else if (theDebugFTCFlag)
		{
		std::cout << Indent() << "Checking cell " << ToString() << std::endl;
		}

	BuildChildList();
	ComputeInvariants(subLayout);

	const JSize cellCount = itsChildren->GetElementCount();
	JCoordinate max       = 0;
	for (JIndex i=1; i<=cellCount; i++)
		{
		max = JMax(max, itsChildren->GetElement(i)->Expand(horizontal));
		}

	if (( itsSyncHorizontalFlag && itsDirection == kHorizontal) ||
		(!itsSyncHorizontalFlag && itsDirection == kVertical))
		{
		EnforceSpacing();
		CoverChildren();	// contents expanded or contracted
		}
	else if (( itsSyncHorizontalFlag && itsDirection == kVertical) ||
			 (!itsSyncHorizontalFlag && itsDirection == kHorizontal))
		{
		CoverChildren();	// so "full width" is correct
		EnforcePositions();
		}
	else
		{
		assert_msg( 0, "fatal logic error in JXFTCCell::Expand()" );
		}

	return (horizontal ? itsFrameG.width() : itsFrameG.height());
}

/******************************************************************************
 BuildChildList (private)

 ******************************************************************************/

void
JXFTCCell::BuildChildList()
{
	JPtrArrayIterator<JXContainer>* iter;
	const JBoolean hasObjs = GetEnclosedObjects(&iter);
	assert( hasObjs );

	assert( itsChildren == NULL );
	itsChildren = jnew JPtrArray<JXFTCCell>(JPtrArrayT::kForgetAll);
	assert( itsChildren != NULL );

	JXContainer* obj;
	while (iter->Next(&obj))
		{
		JXFTCCell* cell = dynamic_cast<JXFTCCell*>(obj);
		assert( cell != NULL );
		itsChildren->AppendElement(cell);
		}

	itsChildren->SetCompareFunction(itsSyncHorizontalFlag ? CompareHorizontally : CompareVertically);
	itsChildren->Sort();

	jdelete iter;
}

/******************************************************************************
 ComputeInvariants (private)

	spacing, elastic item, or positioning

 ******************************************************************************/

void
JXFTCCell::ComputeInvariants
	(
	const JBoolean subLayout
	)
{
	const JSize cellCount = itsChildren->GetElementCount();
	assert( cellCount > 1 );

	// TODO:  check for sizing options, to find elastic column or row

	if (( itsSyncHorizontalFlag && itsDirection == kHorizontal) ||
		(!itsSyncHorizontalFlag && itsDirection == kVertical))
		{
		itsChildSpacing = jnew JArray<JCoordinate>;
		assert( itsChildSpacing != NULL );

		JRect prev = itsChildren->GetFirstElement()->GetFrameGlobal();
		for (JIndex i=2; i<=cellCount; i++)
			{
			const JRect r = itsChildren->GetElement(i)->GetFrameGlobal();
			itsChildSpacing->AppendElement(
				itsSyncHorizontalFlag ? r.left - prev.right : r.top - prev.bottom);

			prev = r;
			}
		}

	itsChildPositions = jnew JArray<JFloat>;
	assert( itsChildPositions != NULL );

	for (JIndex i=1; i<=cellCount; i++)
		{
		const JRect r = itsChildren->GetElement(i)->GetFrameGlobal();

		const JCoordinate objMin  = (itsSyncHorizontalFlag ? r.left : r.top),
						  thisMin = (itsSyncHorizontalFlag ? itsFrameG.left : itsFrameG.top),
						  objMax  = (itsSyncHorizontalFlag ? r.right : r.bottom),
						  thisMax = (itsSyncHorizontalFlag ? itsFrameG.right : itsFrameG.bottom);
		if (objMin == thisMin && objMax == thisMax)
			{
			itsChildPositions->AppendElement(0);
			}
		else if (objMin == thisMin)
			{
			itsChildPositions->AppendElement(-1);
			}
		else if (objMax == thisMax)
			{
			itsChildPositions->AppendElement(+1);
			}
		else if (itsSyncHorizontalFlag)
			{
			itsChildPositions->AppendElement((r.xcenter() - thisMin) / JFloat(itsFrameG.width()));
			}
		else
			{
			itsChildPositions->AppendElement((r.ycenter() - thisMin) / JFloat(itsFrameG.height()));
			}
		}

	if (subLayout)
		{
		JXContainer* encl = GetEnclosure();
		JPtrArrayIterator<JXContainer>* iter;
		if (encl->GetEnclosedObjects(&iter))
			{
			JXContainer* obj;
			iter->Next(&obj);
			JRect covering = obj->GetFrameForFTC();

			while (iter->Next(&obj))
				{
				covering = JCovering(covering, obj->GetFrameForFTC());
				}

			const JRect r     = encl->GetApertureGlobal();
			itsPadding.top    = covering.top - r.top;
			itsPadding.left   = covering.left - r.left;
			itsPadding.bottom = r.bottom - covering.bottom;
			itsPadding.right  = r.right - covering.right;

			if (theDebugFTCFlag)
				{
				std::cout << Indent() << "padding: " << itsPadding << std::endl;
				}
			}

		jdelete iter;
		}
}

/******************************************************************************
 EnforceSpacing (private)

 ******************************************************************************/

void
JXFTCCell::EnforceSpacing()
{
	if (theDebugFTCFlag)
		{
		std::cout << Indent(+1) << "--- Enforcing spacing:";

		for (JIndex i=1; i<=itsChildSpacing->GetElementCount(); i++)
			{
			std::cout << ' ' << itsChildSpacing->GetElement(i);
			}
		std::cout << std::endl;
		}

	const JSize cellCount = itsChildren->GetElementCount();
	assert( cellCount > 1 );

	for (JIndex i=2; i<=cellCount; i++)
		{
		const JRect prev       = itsChildren->GetElement(i-1)->GetFrameGlobal();
		const JRect r          = itsChildren->GetElement(i)->GetFrameGlobal();
		const JCoordinate gap  = (itsSyncHorizontalFlag ? r.left - prev.right : r.top - prev.bottom),
						  orig = itsChildSpacing->GetElement(i-1);
		if (gap == orig)
			{
			continue;
			}
		const JCoordinate delta = orig - gap;

		if (theDebugFTCFlag)
			{
			std::cout << Indent() << "--- Adjusting gap from " << gap << " -> " << orig << std::endl;
			}

		for (JIndex j=i; j<=cellCount; j++)
			{
			JXFTCCell* cell = itsChildren->GetElement(j);
			cell->Move(itsSyncHorizontalFlag ? delta : 0, itsSyncHorizontalFlag ? 0 : delta);
			}
		}
}

/******************************************************************************
 EnforcePositions (private)

 ******************************************************************************/

void
JXFTCCell::EnforcePositions()
{
	if (theDebugFTCFlag)
		{
		std::cout << Indent(+1) << "--- Enforcing positions:";

		for (JIndex i=1; i<=itsChildPositions->GetElementCount(); i++)
			{
			std::cout << ' ' << itsChildPositions->GetElement(i);
			}
		std::cout << std::endl;
		}

	const JSize cellCount = itsChildren->GetElementCount();
	assert( cellCount > 1 );

	for (JIndex i=1; i<=cellCount; i++)
		{
		JXFTCCell* cell = itsChildren->GetElement(i);
		const JRect r   = cell->GetFrameGlobal();
		const JFloat p  = itsChildPositions->GetElement(i);

		if (p == 0)						// full width
			{
			cell->AdjustSize(itsSyncHorizontalFlag ? itsFrameG.width() - r.width() : 0,
							 itsSyncHorizontalFlag ? 0 : itsFrameG.height() - r.height());
			}
		else if (p == -1)				// left-aligned
			{
			// nothing to do
			}
		else if (p == +1)				// right-aligned
			{
			cell->Move(itsSyncHorizontalFlag ? itsFrameG.right - r.right : 0,
					   itsSyncHorizontalFlag ? 0 : itsFrameG.bottom - r.bottom);
			}
		else if (itsSyncHorizontalFlag)	// center-line
			{
			const JCoordinate curr = r.xcenter(),
							  want = itsFrameG.left + JRound(itsFrameG.width() * p);
			cell->Move(want - curr, 0);
			}
		else
			{
			const JCoordinate curr = r.ycenter(),
							  want = itsFrameG.top + JRound(itsFrameG.height() * p);
			cell->Move(0, want - curr);
			}
		}
}

/******************************************************************************
 CoverChildren (private)

	resize ourselves to cover contents.

 ******************************************************************************/

void
JXFTCCell::CoverChildren()
{
	const JSize cellCount = itsChildren->GetElementCount();

	JRect r = itsChildren->GetFirstElement()->GetFrameForFTC();
	for (JIndex i=2; i<=cellCount; i++)
		{
		r = JCovering(r, itsChildren->GetElement(i)->GetFrameForFTC());
		}

	itsSyncChildrenFlag = kJFalse;
	SetSize(r.width(), r.height());
	itsSyncChildrenFlag = kJTrue;
}

/******************************************************************************
 ExpandWidget (private)

	Run fit-to-content on contained objects, if any.  Otherwise, adjust to
	min FTC content size

	Returns the new size.

 ******************************************************************************/

JCoordinate
JXFTCCell::ExpandWidget()
{
	JCoordinate v = 0;
	if (itsWidget->NeedsInternalFTC())
		{
		if (theDebugFTCFlag)
			{
			std::cout << "=== Processing internal structure for " << itsWidget->ToString() << std::endl;
			}

		JXFTCCell* root = itsWidget->FTCBuildLayout(itsSyncHorizontalFlag);
		if (root != NULL)
			{
			v = root->Expand(itsSyncHorizontalFlag, kJTrue);

			if (itsSyncHorizontalFlag)
				{
				v += root->itsPadding.left + root->itsPadding.right;
				}
			else
				{
				v += root->itsPadding.top + root->itsPadding.bottom;
				}

			if (theDebugFTCFlag)
				{
				std::cout << "=== Finished processing internal structure" << std::endl;
				}

			jdelete root;
			}
		}

	if (v == 0)
		{
		v = itsWidget->GetFTCMinContentSize(itsSyncHorizontalFlag);
		}

	const JRect apG         = itsWidget->GetApertureGlobal();
	const JCoordinate delta = v - (itsSyncHorizontalFlag ? apG.width() : apG.height());
	if (delta != 0)
		{
		AdjustSize(itsSyncHorizontalFlag ? delta : 0, itsSyncHorizontalFlag ? 0 : delta);
		}

	return (itsSyncHorizontalFlag ? itsFrameG.width() : itsFrameG.height());
}

/******************************************************************************
 SyncWidgetPosition (private)

	Synchronize widgets to new cell positions.

 ******************************************************************************/

void
JXFTCCell::SyncWidgetPosition()
{
	if (itsWidget != NULL)
		{
		const JRect r        = itsWidget->GetFrameForFTC();
		const JCoordinate dx = itsFrameG.left - r.left,
						  dy = itsFrameG.top - r.top;

		if (theDebugFTCFlag && (dx != 0 || dy != 0))
			{
			std::cout << Indent() << "Moving widget: " << itsWidget->ToString() << std::endl;
			std::cout << Indent(+1);
			if (dx != 0) std::cout << "dx=" << dx;
			if (dy != 0) std::cout << "dy=" << dy;
			std::cout << std::endl;
			}

		itsWidget->Move(dx, dy);
		}
}

/******************************************************************************
 SyncWidgetSize (private)

	Synchronize sizes of contained objects.

 ******************************************************************************/

void
JXFTCCell::SyncWidgetSize()
{
	if (itsWidget != NULL)
		{
		const JRect r = itsWidget->GetFrameForFTC();
		const JCoordinate dw = itsFrameG.width() - r.width(),
						  dh = itsFrameG.height() - r.height();

		if (theDebugFTCFlag && (dw != 0 || dh != 0))
			{
			std::cout << Indent() << "Resizing widget: " << itsWidget->ToString() << std::endl;
			std::cout << Indent(+1);
			if (dw != 0) std::cout << "dw=" << dw;
			if (dh != 0) std::cout << "dh=" << dh;
			std::cout << std::endl;
			}

		itsWidget->AdjustSize(dw, dh);
		}
	else
		{
		EnforcePositions();
		}
}

/******************************************************************************
 Refresh (virtual)

 ******************************************************************************/

void
JXFTCCell::Refresh()
	const
{
	JRect visRectG;
	if (IsVisible() && GetEnclosure()->GetVisibleRectGlobal(itsFrameG, &visRectG))
		{
		GetWindow()->RefreshRect(visRectG);
		}
}

/******************************************************************************
 Redraw (virtual)

 ******************************************************************************/

void
JXFTCCell::Redraw()
	const
{
	JRect visRectG;
	if (IsVisible() && GetEnclosure()->GetVisibleRectGlobal(itsFrameG, &visRectG))
		{
		GetWindow()->RedrawRect(visRectG);
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXFTCCell::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXFTCCell::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 DrawBackground (virtual protected)

 ******************************************************************************/

void
JXFTCCell::DrawBackground
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.SetDefaultClipRegion(NULL);
	p.Reset();
	const JSize depth = GetDepth();

	JRect r = frame;
	r.Shrink(depth, depth);

	const JColorIndex colorList[] =
	{
		p.GetColormap()->GetBlueColor(),
		p.GetColormap()->GetRedColor(),
		p.GetColormap()->GetYellowColor(),
		p.GetColormap()->GetLightBlueColor(),
		p.GetColormap()->GetWhiteColor(),
		p.GetColormap()->GetCyanColor()
	};
	const JColorIndex color = colorList[ depth % (sizeof(colorList)/sizeof(JColorIndex)) ];

	p.SetPenColor(color);
	p.JPainter::Rect(r);

	p.JPainter::Line(frame.topLeft(), frame.bottomRight());
	p.JPainter::Line(frame.topRight(), frame.bottomLeft());
}

/******************************************************************************
 GlobalToLocal (virtual)

 ******************************************************************************/

JPoint
JXFTCCell::GlobalToLocal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x - itsFrameG.left, y - itsFrameG.top);
}

/******************************************************************************
 LocalToGlobal (virtual)

 ******************************************************************************/

JPoint
JXFTCCell::LocalToGlobal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x + itsFrameG.left, y + itsFrameG.top);
}

/******************************************************************************
 Place (virtual)

 ******************************************************************************/

void
JXFTCCell::Place
	(
	const JCoordinate enclX,
	const JCoordinate enclY
	)
{
	const JPoint oldPt = GetEnclosure()->GlobalToLocal(itsFrameG.topLeft());
	Move(enclX - oldPt.x, enclY - oldPt.y);
}

/******************************************************************************
 Move (virtual)

 ******************************************************************************/

void
JXFTCCell::Move
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	if (dx != 0 || dy != 0)
		{
		Refresh();		// refresh orig location

		itsFrameG.Shift(dx,dy);

		if (itsSyncChildrenFlag)
			{
			NotifyBoundsMoved(dx,dy);
			SyncWidgetPosition();
			}

		Refresh();		// refresh new location
		}
}

/******************************************************************************
 SetSize (virtual)

 ******************************************************************************/

void
JXFTCCell::SetSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	AdjustSize(w - itsFrameG.width(), h - itsFrameG.height());
}

/******************************************************************************
 AdjustSize (virtual)

 ******************************************************************************/

void
JXFTCCell::AdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (dw != 0 || dh != 0)
		{
		assert( itsFrameG.width() + dw > 0 && itsFrameG.height() + dh > 0 );

		Refresh();		// refresh orig size

		itsFrameG.bottom += dh;
		itsFrameG.right  += dw;

		if (itsSyncChildrenFlag)
			{
			SyncWidgetSize();
			}

		Refresh();		// refresh new size
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

	Update our DragPainter.

 ******************************************************************************/

void
JXFTCCell::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
}

/******************************************************************************
 EnclosingBoundsMoved (virtual protected)

	Move ourselves so we end up in the same place relative to our
	enclosure.

 ******************************************************************************/

void
JXFTCCell::EnclosingBoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	Move(dx,dy);
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXFTCCell::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

 ******************************************************************************/

void
JXFTCCell::EnclosingBoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
}

/******************************************************************************
 GetBoundsGlobal (virtual)

	Returns the bounds in global coordinates.

 ******************************************************************************/

JRect
JXFTCCell::GetBoundsGlobal()
	const
{
	return itsFrameG;
}

/******************************************************************************
 GetFrameGlobal (virtual)

	Returns the frame in global coordinates.

 ******************************************************************************/

JRect
JXFTCCell::GetFrameGlobal()
	const
{
	return itsFrameG;
}

/******************************************************************************
 GetApertureGlobal (virtual)

	Returns the aperture in global coordinates.

 ******************************************************************************/

JRect
JXFTCCell::GetApertureGlobal()
	const
{
	return itsFrameG;
}

/******************************************************************************
 Comparison functions (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXFTCCell::CompareHorizontally
	(
	JXFTCCell* const & c1,
	JXFTCCell* const & c2
	)
{
	return JCompareCoordinates(
		c1->itsFrameG.left,
		c2->itsFrameG.left);
}

JOrderedSetT::CompareResult
JXFTCCell::CompareVertically
	(
	JXFTCCell* const & c1,
	JXFTCCell* const & c2
	)
{
	return JCompareCoordinates(
		c1->itsFrameG.top,
		c2->itsFrameG.top);
}