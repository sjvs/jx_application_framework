/******************************************************************************
 XDGetCompletions.h

	Copyright (C) 2007 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetCompletions
#define _H_XDGetCompletions

#include "CMGetCompletions.h"

class CMCommandInput;
class CMCommandOutputDisplay;

class XDGetCompletions : public CMGetCompletions
{
public:

	XDGetCompletions(CMCommandInput* input, CMCommandOutputDisplay* history);

	virtual	~XDGetCompletions();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	JString					itsPrefix;
	CMCommandInput*			itsInput;
	CMCommandOutputDisplay*	itsHistory;

private:

	static JString	BuildCommand(const JCharacter* prefix);

	// not allowed

	XDGetCompletions(const XDGetCompletions& source);
	const XDGetCompletions& operator=(const XDGetCompletions& source);
};

#endif
