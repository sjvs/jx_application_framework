/******************************************************************************
 XDGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = CMGetInitArgs

	Copyright � 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "XDGetInitArgs.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetInitArgs::XDGetInitArgs
	(
	JXInputField* argInput
	)
	:
	CMGetInitArgs("status")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetInitArgs::~XDGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
}