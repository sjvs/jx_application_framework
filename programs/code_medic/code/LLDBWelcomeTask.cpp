/******************************************************************************
 LLDBWelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "LLDBWelcomeTask.h"
#include "LLDBLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBWelcomeTask::LLDBWelcomeTask
	(
	const JCharacter*	msg,
	const JBoolean		restart
	)
	:
	itsMessage(msg),
	itsRestartFlag(restart)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBWelcomeTask::~LLDBWelcomeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
LLDBWelcomeTask::Perform()
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link != NULL)
		{
		link->BroadcastWelcome(itsMessage, itsRestartFlag);
		}
}
