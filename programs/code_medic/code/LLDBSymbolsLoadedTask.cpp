/******************************************************************************
 LLDBSymbolsLoadedTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "LLDBSymbolsLoadedTask.h"
#include "LLDBLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBSymbolsLoadedTask::LLDBSymbolsLoadedTask
	(
	const JCharacter* fileName
	)
	:
	itsFileName(fileName)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBSymbolsLoadedTask::~LLDBSymbolsLoadedTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
LLDBSymbolsLoadedTask::Perform()
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link != NULL)
		{
		link->SymbolsLoaded(itsFileName);
		}
}
