/******************************************************************************
 JStyleTableData.cpp

	Stores font and style information for a table.  The font and size
	are restricted to be table-wide, while the style can be changed for
	each cell.

	BASE CLASS = JAuxTableData<JFontStyle>

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JStyleTableData.h>
#include <JTable.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kVMarginWidth = 1;

// JBroadcaster messages

const JCharacter* JStyleTableData::kFontChanged = "FontChanged::JStyleTableData";

/******************************************************************************
 Constructor

 ******************************************************************************/

JStyleTableData::JStyleTableData
	(
	JTable*				table,
	const JFontManager*	fontManager,
	JColormap*			colormap
	)
	:
	JAuxTableData<JFontStyle>(table, JFontStyle()),
	itsFontManager(fontManager),
	itsColormap(colormap),
	itsFont(fontManager->GetDefaultFont())
{
	AdjustToFont();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStyleTableData::~JStyleTableData()
{
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JStyleTableData::SetFont
	(
	const JFont& font
	)
{
	itsFont = font;
	AdjustToFont();
	Broadcast(FontChanged());
}

void
JStyleTableData::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	SetFont(itsFontManager->GetFont(name, size));
}

/******************************************************************************
 AdjustToFont (private)

 ******************************************************************************/

void
JStyleTableData::AdjustToFont()
{
	const JSize rowHeight = 2*kVMarginWidth + itsFont.GetLineHeight();
	GetTable()->SetDefaultRowHeight(rowHeight);
	GetTable()->SetAllRowHeights(rowHeight);
}

/******************************************************************************
 SetCellStyle

 ******************************************************************************/

void
JStyleTableData::SetCellStyle
	(
	const JPoint&		cell,
	const JFontStyle&	style
	)
{
	const JFontStyle origStyle = GetCellStyle(cell);
	if (style != origStyle)
		{
		SetElement(cell, style);
		}
}

/******************************************************************************
 SetAllCellStyles

 ******************************************************************************/

void
JStyleTableData::SetAllCellStyles
	(
	const JFontStyle& style
	)
{
	SetAllElements(style);
}
