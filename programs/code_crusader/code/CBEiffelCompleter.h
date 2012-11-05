/******************************************************************************
 CBEiffelCompleter.h

	Copyright � 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEiffelCompleter
#define _H_CBEiffelCompleter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStringCompleter.h"

class CBEiffelCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBEiffelCompleter();

protected:

	CBEiffelCompleter();

	virtual JBoolean	IsWordCharacter(const JString& s, const JIndex index,
										const JBoolean includeNS) const;
	virtual void		MatchCase(const JString& source, JString* target) const;

private:

	static CBEiffelCompleter*	itsSelf;

private:

	// not allowed

	CBEiffelCompleter(const CBEiffelCompleter& source);
	const CBEiffelCompleter& operator=(const CBEiffelCompleter& source);
};

#endif
