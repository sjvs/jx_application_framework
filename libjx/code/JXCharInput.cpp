/******************************************************************************
 JXCharInput.cpp

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXCharInput.h"
#include "JXEditTable.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCharInput::JXCharInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	GetText()->SetText(JString(" ", kJFalse));
	SetLengthLimits(1,1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCharInput::~JXCharInput()
{
}

/******************************************************************************
 GetCharacter

	This is safe because the input is invalid if length != 1.

 ******************************************************************************/

JUtf8Character
JXCharInput::GetCharacter()
	const
{
	return GetText().GetText().GetFirstCharacter();
}

/******************************************************************************
 SetCharacter

 ******************************************************************************/

void
JXCharInput::SetCharacter
	(
	const JUtf8Character& c
	)
{
	GetText()->SetText(JString(c.GetBytes(), kJFalse));
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXCharInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JXEditTable* table;
	const JBoolean willDie = JI2B( GetTable(&table) &&
								   table->WantsInputFieldKey(c, keySym, modifiers) );

	SelectAll();
	JXInputField::HandleKeyPress(c, keySym, modifiers);
	if (!willDie)
		{
		SelectAll();
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
JXCharInput::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	return kJFalse;
}
