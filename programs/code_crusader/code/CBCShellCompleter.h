/******************************************************************************
 CBCShellCompleter.h

	Copyright � 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCShellCompleter
#define _H_CBCShellCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBCShellCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBCShellCompleter();

protected:

	CBCShellCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;

private:

	static CBCShellCompleter*	itsSelf;

private:

	// not allowed

	CBCShellCompleter(const CBCShellCompleter& source);
	const CBCShellCompleter& operator=(const CBCShellCompleter& source);
};

#endif
