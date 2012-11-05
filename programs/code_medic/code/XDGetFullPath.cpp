/******************************************************************************
 XDGetFullPath.cpp

	BASE CLASS = CMGetFullPath

	Copyright � 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "XDGetFullPath.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetFullPath::XDGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex	// for convenience
	)
	:
	CMGetFullPath("status", fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetFullPath::~XDGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetFullPath::HandleSuccess
	(
	const JString& data
	)
{
}