/******************************************************************************
 JVMGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright (C) 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JVMGetCompletions.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetCompletions::JVMGetCompletions
	(
	CMCommandInput*			input,
	CMCommandOutputDisplay*	history
	)
	:
	CMGetCompletions("NOP")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetCompletions::~JVMGetCompletions()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetCompletions::HandleSuccess
	(
	const JString& data
	)
{
}
