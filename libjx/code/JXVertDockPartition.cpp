/******************************************************************************
 JXVertDockPartition.cpp

	Maintains a set of compartments arranged vertically.  The user can dock
	a window in any compartment.

	BASE CLASS = JXVertPartition

	Copyright (C) 2002-08 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXVertDockPartition.h>
#include <JXHorizDockPartition.h>
#include <JXDockDirector.h>
#include <JXDockWidget.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXDockTabGroup.h>
#include <jGlobals.h>
#include <jAssert.h>

// Docking menu

static const JCharacter* kDockMenuStr =
	"    Split top compartment horizontally"
	"  | Split top compartment vertically"
	"  | Remove top compartment"
	"%l| Split bottom compartment horizontally"
	"  | Split bottom compartment vertically"
	"  | Remove bottom compartment"
	"%l| Make top compartment elastic"
	"  | Make bottom compartment elastic"
	"  | Make all compartments elastic";

enum
{
	kSplitTopHorizCmd = 1,
	kSplitTopVertCmd,
	kRemoveTopCmd,
	kSplitBottomHorizCmd,
	kSplitBottomVertCmd,
	kRemoveBottomCmd,
	kSetTopElasticCmd,
	kSetBottomElasticCmd,
	kSetAllElasticCmd
};

// string ID's

static const JCharacter* kNoSpaceVertID  = "NoSpaceVert::JXVertDockPartition";
static const JCharacter* kNoSpaceHorizID = "NoSpaceHoriz::JXVertDockPartition";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXVertDockPartition::JXVertDockPartition
	(
	JXDockDirector*				director,
	JXDockWidget*				parent,
	const JArray<JCoordinate>&	widths,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minWidths,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXVertPartition(enclosure, widths, elasticIndex, minWidths,
					hSizing, vSizing, x,y, w,h),
	itsDirector(director),
	itsDockMenu(NULL),
	itsParentDock(parent)
{
	itsDockList = jnew JPtrArray<JXDockWidget>(JPtrArrayT::kForgetAll);
	assert( itsDockList != NULL );

	CreateInitialCompartments();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXVertDockPartition::~JXVertDockPartition()
{
	jdelete itsDockList;
}

/******************************************************************************
 CreateCompartment (virtual protected)

 ******************************************************************************/

JXWidgetSet*
JXVertDockPartition::CreateCompartment
	(
	const JIndex		index,
	const JCoordinate	position,
	const JCoordinate	size
	)
{
	JXWidgetSet* compartment =
		JXVertPartition::CreateCompartment(index, position, size);

	JXDockTabGroup* tabGroup =
		jnew JXDockTabGroup(compartment, kHElastic, kVElastic, 0,0, 100,100);
	assert( tabGroup != NULL );
	tabGroup->FitToEnclosure();

	JXDockWidget* dock =
		jnew JXDockWidget(itsDirector, this, kJFalse, tabGroup,
						 tabGroup->GetCardEnclosure(), kHElastic, kVElastic,
						 0,0, 100,100);
	assert( dock != NULL );
	dock->FitToEnclosure();
	itsDockList->InsertAtIndex(index, dock);

	tabGroup->SetDockWidget(dock);

	return compartment;
}

/******************************************************************************
 FindDock

 ******************************************************************************/

JBoolean
JXVertDockPartition::FindDock
	(
	const JIndex	id,
	JXDockWidget**	dock
	)
{
	const JSize count = GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* d = itsDockList->NthElement(i);
		assert( d != NULL );

		if (d->GetID() == id)
			{
			*dock = d;
			return kJTrue;
			}

		JXHorizDockPartition* p;
		if (d->GetHorizChildPartition(&p) && p->FindDock(id, dock))
			{
			return kJTrue;
			}
		}

	*dock = NULL;
	return kJFalse;
}

/******************************************************************************
 HasWindows

 ******************************************************************************/

JBoolean
JXVertDockPartition::HasWindows()
	const
{
	const JSize count = GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* d = itsDockList->NthElement(i);
		assert( d != NULL );

		if (d->HasWindows())
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

JBoolean
JXVertDockPartition::CloseAllWindows()
{
	const JSize count = GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* d = itsDockList->NthElement(i);
		assert( d != NULL );

		if (!d->CloseAllWindows())
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXVertDockPartition::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	JSize compartmentCount;
	input >> compartmentCount;

	while (GetCompartmentCount() < compartmentCount)
		{
		InsertCompartment(1);
		}
	while (GetCompartmentCount() > compartmentCount)
		{
		DeleteCompartment(1);
		}

	ReadGeometry(input);

	if (vers >= 3)
		{
		JBoolean hasElasticIndex;
		JIndex elasticIndex;
		input >> hasElasticIndex >> elasticIndex;
		if (hasElasticIndex)
			{
			SetElasticIndex(elasticIndex);
			}
		}

	for (JIndex i=1; i<=compartmentCount; i++)
		{
		JXDockWidget* dock = itsDockList->NthElement(i);
		assert( dock != NULL );

		JIndex id;
		input >> id;
		dock->SetID(id);

		if (vers >= 2)
			{
			(dock->GetTabGroup())->ReadSetup(input);
			}

		JBoolean hasPartition;
		input >> hasPartition;
		if (hasPartition)
			{
			SplitHoriz(i);

			JXHorizDockPartition* p;
			const JBoolean isSplit = dock->GetHorizChildPartition(&p);
			assert( isSplit );
			p->ReadSetup(input, vers);
			}
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXVertDockPartition::WriteSetup
	(
	std::ostream& output
	)
	const
{
	const JSize count = GetCompartmentCount();
	output << ' ' << count;

	WriteGeometry(output);

	JIndex elasticIndex;
	output << ' ' << GetElasticIndex(&elasticIndex);
	output << ' ' << elasticIndex;

	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* dock = itsDockList->NthElement(i);
		assert( dock != NULL );

		output << ' ' << dock->GetID();

		(dock->GetTabGroup())->WriteSetup(output);

		JXHorizDockPartition* v;
		if (dock->GetHorizChildPartition(&v))
			{
			output << ' ' << kJTrue;
			v->WriteSetup(output);
			}
		else
			{
			output << ' ' << kJFalse;
			}
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXVertDockPartition::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXRightButton)
		{
		if (!FindCompartment(pt.y, &itsCompartmentIndex) ||
			itsCompartmentIndex >= GetCompartmentCount())
			{
			return;
			}

		if (itsDockMenu == NULL)
			{
			itsDockMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
			assert( itsDockMenu != NULL );
			itsDockMenu->SetToHiddenPopupMenu();
			itsDockMenu->SetMenuItems(kDockMenuStr);
			itsDockMenu->SetUpdateAction(JXMenu::kDisableNone);
			ListenTo(itsDockMenu);
			}

		itsDockMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else
		{
		JXVertPartition::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXVertDockPartition::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDockMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateDockMenu();
		}
	else if (sender == itsDockMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleDockMenu(selection->GetIndex());
		}

	else
		{
		JXVertPartition::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDockMenu (private)

 ******************************************************************************/

void
JXVertDockPartition::UpdateDockMenu()
{
	const JBoolean canRemove =
		JI2B(GetEnclosure() != GetWindow() || GetCompartmentCount() > 2);

	itsDockMenu->SetItemEnable(kRemoveTopCmd,    canRemove);
	itsDockMenu->SetItemEnable(kRemoveBottomCmd, canRemove);

	JIndex i;
	if (GetElasticIndex(&i))
		{
		itsDockMenu->SetItemEnable(kSetTopElasticCmd,  JI2B( i != itsCompartmentIndex ));
		itsDockMenu->SetItemEnable(kSetBottomElasticCmd, JI2B( i != itsCompartmentIndex+1 ));
		itsDockMenu->EnableItem(kSetAllElasticCmd);
		}
	else
		{
		itsDockMenu->EnableItem(kSetTopElasticCmd);
		itsDockMenu->EnableItem(kSetBottomElasticCmd);
		itsDockMenu->DisableItem(kSetAllElasticCmd);
		}
}

/******************************************************************************
 HandleDockMenu (private)

 ******************************************************************************/

void
JXVertDockPartition::HandleDockMenu
	(
	const JIndex index
	)
{
	if (itsCompartmentIndex == 0 || itsCompartmentIndex >= GetCompartmentCount())
		{
		return;
		}

	if (index == kSplitTopHorizCmd)
		{
		SplitHoriz(itsCompartmentIndex, NULL, kJTrue);
		}
	else if (index == kSplitTopVertCmd)
		{
		InsertCompartment(itsCompartmentIndex+1, kJTrue);
		}
	else if (index == kRemoveTopCmd)
		{
		DeleteCompartment(itsCompartmentIndex);		// may delete us
		}

	else if (index == kSplitBottomHorizCmd)
		{
		SplitHoriz(itsCompartmentIndex+1, NULL, kJTrue);
		}
	else if (index == kSplitBottomVertCmd)
		{
		InsertCompartment(itsCompartmentIndex+1, kJTrue);
		}
	else if (index == kRemoveBottomCmd)
		{
		DeleteCompartment(itsCompartmentIndex+1);	// may delete us
		}

	else if (index == kSetTopElasticCmd)
		{
		SetElasticIndex(itsCompartmentIndex);
		}
	else if (index == kSetBottomElasticCmd)
		{
		SetElasticIndex(itsCompartmentIndex+1);
		}
	else if (index == kSetAllElasticCmd)
		{
		SetElasticIndex(0);
		}
}

/******************************************************************************
 InsertCompartment

 ******************************************************************************/

void
JXVertDockPartition::InsertCompartment
	(
	const JIndex	index,
	const JBoolean	reportError
	)
{
	if (JPartition::InsertCompartment(index, 10, 10))
		{
		itsDirector->UpdateMinSize();
		}
	else
		{
		(JGetUserNotification())->ReportError(JGetString(kNoSpaceVertID));
		}
}

/******************************************************************************
 SplitHoriz

	returnPartition can be NULL.

 ******************************************************************************/

JBoolean
JXVertDockPartition::SplitHoriz
	(
	const JIndex			index,
	JXHorizDockPartition**	returnPartition,
	const JBoolean			reportError
	)
{
	if (returnPartition != NULL)
		{
		*returnPartition = NULL;
		}

	JXDockWidget* parent = itsDockList->NthElement(index);
	assert( parent != NULL );

	JXHorizDockPartition* p = NULL;
	if (parent->GetHorizChildPartition(&p))
		{
		p->InsertCompartment(p->GetCompartmentCount()+1, reportError);
		}
	else
		{
		JXContainer* encl = GetCompartment(index);
		assert( encl != NULL );

		const JCoordinate w = encl->GetApertureWidth() - kDragRegionSize;
		if (w < 2*JXDockWidget::kDefaultMinSize)
			{
			return kJFalse;
			}

		JArray<JCoordinate> widths;
		widths.AppendElement(w/2);
		widths.AppendElement(w/2 + w%2);

		JArray<JCoordinate> minWidths;
		minWidths.AppendElement(JXDockWidget::kDefaultMinSize);
		minWidths.AppendElement(JXDockWidget::kDefaultMinSize);

		p = jnew JXHorizDockPartition(itsDirector, parent, widths, 0, minWidths, encl,
									 kHElastic, kVElastic, 0,0,
									 encl->GetApertureWidth(), encl->GetApertureHeight());
		assert( p != NULL );

		JXDockWidget* d1 = p->GetDockWidget(1);
		assert( d1 != NULL );

		if (parent->TransferAll(d1))
			{
			parent->SetChildPartition(p);
			}
		else
			{
			d1->TransferAll(parent);
			jdelete p;
			p = NULL;

			if (reportError)
				{
				(JGetUserNotification())->ReportError(JGetString(kNoSpaceHorizID));
				}
			}
		}

	if (returnPartition != NULL)
		{
		*returnPartition = p;
		}
	return JI2B( p != NULL );
}

/******************************************************************************
 DeleteCompartment

	*** This function can delete us!

 ******************************************************************************/

void
JXVertDockPartition::DeleteCompartment
	(
	const JIndex index
	)
{
	if (GetEnclosure() == GetWindow() && GetCompartmentCount() <= 2)
		{
		return;
		}

	itsDockList->RemoveElement(index);
	JPartition::DeleteCompartment(index);

	if (GetCompartmentCount() == 1)
		{
		assert( itsParentDock != NULL );

		JXDockWidget* child = itsDockList->FirstElement();
		assert( child != NULL );

		itsParentDock->SetChildPartition(NULL);		// so docking will be allowed
		child->TransferAll(itsParentDock);
		jdelete this;
		}
}

/******************************************************************************
 UpdateMinSize

 ******************************************************************************/

JPoint
JXVertDockPartition::UpdateMinSize()
{
	JCoordinate minWidth  = JXDockWidget::kDefaultMinSize;
	JCoordinate minHeight = kDragRegionSize * (GetCompartmentCount()-1);

	const JSize count = itsDockList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JPoint minSize = (itsDockList->NthElement(i))->GetMinSize();
		minWidth             = JMax(minSize.x, minWidth);
		minHeight           += minSize.y;

		JPartition::SetMinCompartmentSize(i, minSize.y);
		}

	return JPoint(minWidth, minHeight);
}

/******************************************************************************
 SaveGeometryForLater (virtual protected)

 ******************************************************************************/

JBoolean
JXVertDockPartition::SaveGeometryForLater
	(
	const JArray<JCoordinate>& sizes
	)
{
	return kJFalse;
}
