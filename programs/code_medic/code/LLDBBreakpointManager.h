/******************************************************************************
 LLDBBreakpointManager.h

	Copyright � 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_LLDBBreakpointManager
#define _H_LLDBBreakpointManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMBreakpointManager.h"

class LLDBLink;

class LLDBBreakpointManager : public CMBreakpointManager
{
public:

	LLDBBreakpointManager(LLDBLink* link);

	virtual	~LLDBBreakpointManager();

private:

	// not allowed

	LLDBBreakpointManager(const LLDBBreakpointManager& source);
	const LLDBBreakpointManager& operator=(const LLDBBreakpointManager& source);
};

#endif