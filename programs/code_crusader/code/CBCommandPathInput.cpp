/******************************************************************************
 CBCommandPathInput.cpp

	Input field for entering a file path which allows @.

	BASE CLASS = JXPathInput

	Copyright � 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include <CBCommandPathInput.h>
#include <JXColormap.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCommandPathInput::CBCommandPathInput
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
	JXPathInput(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCommandPathInput::~CBCommandPathInput()
{
}

/******************************************************************************
 GetPath (virtual)

	Returns kJTrue if the current path is valid.  In this case, *path is
	set to the full path, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

JBoolean
CBCommandPathInput::GetPath
	(
	JString* path
	)
	const
{
	const JString& text = GetText();
	if (text == "@")
		{
		*path = text;
		return kJTrue;
		}
	else
		{
		return JXPathInput::GetPath(path);
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
CBCommandPathInput::InputValid()
{
	if (GetText() == "@")
		{
		return kJTrue;
		}
	else
		{
		return JXPathInput::InputValid();
		}
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Accept "@"

 ******************************************************************************/

void
CBCommandPathInput::AdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<Font>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	if (buffer == "@")
		{
		const JColormap* colormap = GetColormap();
		const JSize totalLength   = buffer.GetLength();
		Font f                    = styles->GetFirstElement();
		styles->RemoveAll();
		f.style.color = colormap->GetBlackColor();
		styles->AppendElements(f, totalLength);
		*redrawRange += JIndexRange(1, totalLength);
		}
	else
		{
		return JXPathInput::AdjustStylesBeforeRecalc(buffer, styles, recalcRange,
													 redrawRange, deletion);
		}
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the path is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be NULL.  If you use CBCommandPathInput for relative paths, base
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorIndex
CBCommandPathInput::GetTextColor
	(
	const JCharacter*	path,
	const JCharacter*	base,
	const JBoolean		requireWrite,
	const JColormap*	colormap
	)
{
	if (strcmp(path, "@") == 0)
		{
		return colormap->GetBlackColor();
		}
	else
		{
		return JXPathInput::GetTextColor(path, base, requireWrite, colormap);
		}
}

/******************************************************************************
 GetTextForChoosePath (virtual)

	Returns a string that can safely be passed to JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JString
CBCommandPathInput::GetTextForChoosePath()
	const
{
	if (GetText() == "@")
		{
		JString s;
		if (!GetBasePath(&s) &&
			!JGetHomeDirectory(&s))
			{
			s = JGetRootDirectory();
			}
		return s;
		}
	else
		{
		return JXPathInput::GetTextForChoosePath();
		}
}