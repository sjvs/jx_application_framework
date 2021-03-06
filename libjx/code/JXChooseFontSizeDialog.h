/******************************************************************************
 JXChooseFontSizeDialog.h

	Interface for the JXChooseFontSizeDialog class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChooseFontSizeDialog
#define _H_JXChooseFontSizeDialog

#include <JXDialogDirector.h>

class JXIntegerInput;

class JXChooseFontSizeDialog : public JXDialogDirector
{
public:

	JXChooseFontSizeDialog(JXWindowDirector* supervisor, const JSize fontSize);

	virtual ~JXChooseFontSizeDialog();

	JSize	GetFontSize() const;

private:

// begin JXLayout

	JXIntegerInput* itsFontSize;

// end JXLayout

private:

	void	BuildWindow(const JSize fontSize);

	// not allowed

	JXChooseFontSizeDialog(const JXChooseFontSizeDialog& source);
	const JXChooseFontSizeDialog& operator=(const JXChooseFontSizeDialog& source);
};

#endif
