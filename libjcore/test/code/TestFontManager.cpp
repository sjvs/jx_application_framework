/******************************************************************************
 TestFontManager.cpp

	BASE CLASS = JFontManager

	Written by John Lindal.

 ******************************************************************************/

#include "TestFontManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFontManager::TestFontManager()
	:
	JFontManager()
{
	itsFontNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFontNames != NULL );

	itsFontNames->Append(JString("Courier", 0, kJFalse));
	itsFontNames->Append(JString("Times", 0, kJFalse));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestFontManager::~TestFontManager()
{
	jdelete itsFontNames;
}

/******************************************************************************
 GetFontNames (virtual)

 ******************************************************************************/

void
TestFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, kJFalse);
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

 ******************************************************************************/

void
TestFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	fontNames->CopyObjects(*itsFontNames, JPtrArrayT::kDeleteAll, kJFalse);
}

/******************************************************************************
 GetFontSizes (virtual)

 ******************************************************************************/

JBoolean
TestFontManager::GetFontSizes
	(
	const JString&	name,
	JSize*			minSize,
	JSize*			maxSize,
	JArray<JSize>*	sizeList
	)
	const
{
	*minSize = 8;
	*maxSize = 24;
	sizeList->RemoveAll();
	return kJTrue;
}

/******************************************************************************
 IsExact (virtual)

 ******************************************************************************/

JBoolean
TestFontManager::IsExact
	(
	const JFontID id
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 GetLineHeight (virtual)

 ******************************************************************************/

JSize
TestFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
	const
{
	*ascent  = 5;
	*descent = 2;
	return (*ascent + *descent);
}

/******************************************************************************
 GetCharWidth (virtual)

 ******************************************************************************/

JSize
TestFontManager::GetCharWidth
	(
	const JFontID			fontID,
	const JUtf8Character&	c
	)
	const
{
	return 7;
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
TestFontManager::GetStringWidth
	(
	const JFontID	fontID,
	const JString&	str
	)
	const
{
	return 7*str.GetCharacterCount();
}