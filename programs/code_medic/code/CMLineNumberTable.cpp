/******************************************************************************
 CMLineNumberTable.cpp

	BASE CLASS = CMLineIndexTable

	Copyright (C) 2016 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "CMLineNumberTable.h"
#include "CMSourceDirector.h"
#include "CMBreakpointManager.h"
#include "CMLink.h"
#include <JXColormap.h>
#include <JOrderedSetUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMLineNumberTable::CMLineNumberTable
	(
	CMSourceDirector*	dir,
	CMSourceText*		text,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CMLineIndexTable(CompareBreakpointLines, dir, text, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMLineNumberTable::~CMLineNumberTable()
{
}

/******************************************************************************
 GetCurrentLineMarkerColor (virtual protected)

 ******************************************************************************/

JColorIndex
CMLineNumberTable::GetCurrentLineMarkerColor()
	const
{
	return GetColormap()->GetCyanColor();
}

/******************************************************************************
 GetLineText (virtual protected)

 ******************************************************************************/

JString
CMLineNumberTable::GetLineText
	(
	const JIndex lineIndex
	)
	const
{
	return JString(lineIndex, JString::kBase10);
}

/******************************************************************************
 GetLongestLineText (virtual protected)

 ******************************************************************************/

JString
CMLineNumberTable::GetLongestLineText
	(
	const JIndex lineCount
	)
	const
{
	return JString(lineCount, JString::kBase10);
}

/******************************************************************************
 GetBreakpointLineIndex (virtual protected)

 ******************************************************************************/

JIndex
CMLineNumberTable::GetBreakpointLineIndex
	(
	const JIndex		bpIndex,
	const CMBreakpoint*	bp
	)
	const
{
	return bp->GetLineNumber();
}

/******************************************************************************
 GetFirstBreakpointOnLine (virtual protected)

 ******************************************************************************/

JBoolean
CMLineNumberTable::GetFirstBreakpointOnLine
	(
	const JIndex	lineIndex,
	JIndex*			bpIndex
	)
	const
{
	CMBreakpoint bp("", lineIndex);
	return GetBreakpointList()->SearchSorted(&bp, JOrderedSetT::kFirstMatch, bpIndex);
}

/******************************************************************************
 BreakpointsOnSameLine (virtual protected)

 ******************************************************************************/

JBoolean
CMLineNumberTable::BreakpointsOnSameLine
	(
	const CMBreakpoint* bp1,
	const CMBreakpoint* bp2
	)
	const
{
	return JI2B(bp1->GetLineNumber() == bp2->GetLineNumber());
}

/******************************************************************************
 GetBreakpoints (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::GetBreakpoints
	(
	JPtrArray<CMBreakpoint>* list
	)
{
	const JString* fullName;
	if (GetDirector()->GetFileName(&fullName))
		{
		(GetLink()->GetBreakpointManager())->GetBreakpoints(*fullName, list);
		}
	else
		{
		list->CleanOut();
		}
}

/******************************************************************************
 SetBreakpoint (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::SetBreakpoint
	(
	const JIndex	lineIndex,
	const JBoolean	temporary
	)
{
	const JString* fullName;
	const JBoolean hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->SetBreakpoint(*fullName, lineIndex, temporary);
}

/******************************************************************************
 RemoveAllBreakpointsOnLine (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::RemoveAllBreakpointsOnLine
	(
	const JIndex lineIndex
	)
{
	const JString* fullName;
	const JBoolean hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->RemoveAllBreakpointsOnLine(*fullName, lineIndex);
}

/******************************************************************************
 RunUntil (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::RunUntil
	(
	const JIndex lineIndex
	)
{
	const JString* fullName;
	const JBoolean hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->RunUntil(*fullName, lineIndex);
}

/******************************************************************************
 SetExecutionPoint (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::SetExecutionPoint
	(
	const JIndex lineIndex
	)
{
	const JString* fullName;
	const JBoolean hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->SetExecutionPoint(*fullName, lineIndex);
}

/******************************************************************************
 CompareBreakpointLines (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
CMLineNumberTable::CompareBreakpointLines
	(
	CMBreakpoint* const & bp1,
	CMBreakpoint* const & bp2
	)
{
	return JCompareIndices(bp1->GetLineNumber(), bp2->GetLineNumber());
}
