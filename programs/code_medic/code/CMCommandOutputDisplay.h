/******************************************************************************
 CMCommandOutputDisplay.h

	Interface for the CMCommandOutputDisplay class

	Copyright � 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMCommandOutputDisplay
#define _H_CMCommandOutputDisplay

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMTextDisplayBase.h"

class CMCommandOutputDisplay : public CMTextDisplayBase
{
public:

	CMCommandOutputDisplay(JXMenuBar* menuBar,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual	~CMCommandOutputDisplay();

	void	PlaceCursorAtEnd();

private:

	// not allowed

	CMCommandOutputDisplay(const CMCommandOutputDisplay& source);
	const CMCommandOutputDisplay& operator=(const CMCommandOutputDisplay& source);
};

#endif