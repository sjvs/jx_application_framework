/******************************************************************************
 CMInitVarNodeTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright � 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMInitVarNodeTask.h"
#include "CMVarNode.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMInitVarNodeTask::CMInitVarNodeTask
	(
	CMVarNode* node
	)
{
	itsNode = node;
	ClearWhenGoingAway(itsNode, &itsNode);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMInitVarNodeTask::~CMInitVarNodeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMInitVarNodeTask::Perform()
{
	if (itsNode != NULL)
		{
		itsNode->NameChanged();
		}
}