/******************************************************************************
 JVMGetInitArgs.cpp

	BASE CLASS = CMGetInitArgs

	Copyright � 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "JVMGetInitArgs.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetInitArgs::JVMGetInitArgs
	(
	JXInputField* argInput
	)
	:
	CMGetInitArgs("NOP")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetInitArgs::~JVMGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
}