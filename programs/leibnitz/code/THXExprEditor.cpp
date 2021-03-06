/******************************************************************************
 THXExprEditor.cpp

	BASE CLASS = JXExprEditor

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THXExprEditor.h"
#include "THXVarList.h"
#include <JXTEBase.h>
#include <JFunction.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

THXExprEditor::THXExprEditor
	(
	const THXVarList*		varList,
	JXMenuBar*				menuBar,
	JXTEBase*				tapeWidget,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXExprEditor(varList, menuBar, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
	itsTapeWidget = tapeWidget;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXExprEditor::~THXExprEditor()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
THXExprEditor::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == '=')
		{
		JIndex selection;
		const JBoolean hadSelection = GetSelection(&selection);
		if (hadSelection)
			{
			ClearSelection();
			}
		else if (!EndEditing())
			{
			return;
			}
		EvaluateSelection();
		if (hadSelection)
			{
			SetSelection(selection);
			}
		}
	else
		{
		JXExprEditor::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 EvaluateSelection (virtual)

 ******************************************************************************/

void
THXExprEditor::EvaluateSelection()
	const
{
	const JFunction* f;
	if (!GetConstSelectedFunction(&f))
		{
		f = GetFunction();
		}

	JComplex value;
	JString valueStr;
	if (f->Evaluate(&value))
		{
		valueStr = JPrintComplexNumber(value);
		}
	else
		{
		valueStr = "Error";
		}

	const JSize textLength = itsTapeWidget->GetTextLength();

	JString newText = f->Print();
	if (textLength > 0)
		{
		newText.Prepend("\n\n");
		}
	newText.AppendCharacter('\n');
	newText.Append(valueStr);

	itsTapeWidget->SetCaretLocation(textLength+1);
	itsTapeWidget->Paste(newText);
}
