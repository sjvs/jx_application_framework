/******************************************************************************
 THXExprEditor.h

	Interface for the THXExprEditor class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THXExprEditor
#define _H_THXExprEditor

#include <JXExprEditor.h>

class JXTEBase;
class THXVarList;

class THXExprEditor : public JXExprEditor
{
public:

	THXExprEditor(const THXVarList* varList, JXMenuBar* menuBar,
				  JXTEBase* tapeWidget,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~THXExprEditor();

	virtual void	EvaluateSelection() const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

private:

	JXTEBase*	itsTapeWidget;	// not owned

private:

	// not allowed

	THXExprEditor(const THXExprEditor& source);
	const THXExprEditor& operator=(const THXExprEditor& source);
};

#endif
