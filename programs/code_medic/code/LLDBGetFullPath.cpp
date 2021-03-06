/******************************************************************************
 LLDBGetFullPath.cpp

	BASE CLASS = CMGetFullPath

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "LLDBGetFullPath.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetFullPath::LLDBGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex	// for convenience
	)
	:
	CMGetFullPath("", fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetFullPath::~LLDBGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetFullPath::HandleSuccess
	(
	const JString& cmdData
	)
{
	Broadcast(FileNotFound(GetFileName()));
	(CMGetLink())->RememberFile(GetFileName(), NULL);
}
