/******************************************************************************
 CBStylerTableInput.cpp

	Class to edit a string "in place" in a JXEditTable.

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBStylerTableInput.h"
#include "CBStylerTable.h"
#include "cbmUtil.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStylerTableInput::CBStylerTableInput
	(
	const CBTextFileType	fileType,
	CBStylerTable*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsStylerTable = enclosure;
	itsFileType    = fileType;

	CBMPrefsManager* prefsMgr = CBMGetPrefsManager();
	SetCaretColor(prefsMgr->GetColor(CBMPrefsManager::kCaretColorIndex));
	SetSelectionColor(prefsMgr->GetColor(CBMPrefsManager::kSelColorIndex));
	SetSelectionOutlineColor(prefsMgr->GetColor(CBMPrefsManager::kSelLineColorIndex));

	SetCharacterInWordFunction(CBMIsCharacterInWord);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBStylerTableInput::~CBStylerTableInput()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBStylerTableInput::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if ((button == kJXLeftButton && modifiers.meta()) ||
		button == kJXRightButton)
		{
		JPoint cell;
		const JBoolean ok = itsStylerTable->GetEditedCell(&cell);
		assert( ok );
		itsStylerTable->DisplayFontMenu(cell, this, pt, buttonStates, modifiers);
		}
	else
		{
		JXInputField::HandleMouseDown(pt, button, clickCount,
									  buttonStates, modifiers);
		}
}

/******************************************************************************
 VIsCharacterInWord (virtual protected)

 ******************************************************************************/

JBoolean
CBStylerTableInput::VIsCharacterInWord
	(
	const JString&	text,
	const JIndex	charIndex
	)
	const
{
	return JI2B(JXTEBase::VIsCharacterInWord(text, charIndex) ||
				CBIsCharacterInWord(itsFileType, text, charIndex));
}
