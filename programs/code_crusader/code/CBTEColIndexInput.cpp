/******************************************************************************
 CBTEColIndexInput.cpp

	Displays the line that the caret is on.

	BASE CLASS = CBTECaretInputBase

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBTEColIndexInput.h"
#include "CBTELineIndexInput.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTEColIndexInput::CBTEColIndexInput
	(
	CBTELineIndexInput*	lineInput,
	JXStaticText*		label,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CBTECaretInputBase(label, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsLineInput = lineInput;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTEColIndexInput::~CBTEColIndexInput()
{
}

/******************************************************************************
 Act (virtual protected)

 ******************************************************************************/

void
CBTEColIndexInput::Act
	(
	JXTEBase*		te,
	const JIndex	value
	)
{
	JInteger lineIndex;
	const JBoolean ok = itsLineInput->JXIntegerInput::GetValue(&lineIndex);
	assert( ok );

	te->GoToColumn(lineIndex, value);
}

/******************************************************************************
 GetValue (virtual protected)

 ******************************************************************************/

JIndex
CBTEColIndexInput::GetValue
	(
	JXTEBase* te
	)
	const
{
	return te->GetColumnForChar(te->GetInsertionIndex());
}

JIndex
CBTEColIndexInput::GetValue
	(
	const JTextEditor::CaretLocationChanged& info
	)
	const
{
	return info.GetColumnIndex();
}
