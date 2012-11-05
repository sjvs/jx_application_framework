/******************************************************************************
 CBPerlCompleter.h

	Copyright � 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPerlCompleter
#define _H_CBPerlCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBPerlCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBPerlCompleter();

protected:

	CBPerlCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBPerlCompleter*	itsSelf;

private:

	// not allowed

	CBPerlCompleter(const CBPerlCompleter& source);
	const CBPerlCompleter& operator=(const CBPerlCompleter& source);
};

#endif