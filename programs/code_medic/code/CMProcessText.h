/******************************************************************************
 CMProcessText.h

	Copyright � 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMProcessText
#define _H_CMProcessText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStaticText.h>

class CMChooseProcessDialog;

class CMProcessText : public JXStaticText
{
public:

	CMProcessText(CMChooseProcessDialog* dir,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual	~CMProcessText();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

private:

	CMChooseProcessDialog*	itsDir;

private:

	// not allowed

	CMProcessText(const CMProcessText& source);
	const CMProcessText& operator=(const CMProcessText& source);
};

#endif