/******************************************************************************
 JXUrgentTask.h

	Interface for the JXUrgentTask class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUrgentTask
#define _H_JXUrgentTask

#include <jTypes.h>

class JXUrgentTask
{
public:

	JXUrgentTask();

	virtual ~JXUrgentTask();

	void	Go();

	virtual void	Perform() = 0;

private:

	// not allowed

	JXUrgentTask(const JXUrgentTask& source);
	const JXUrgentTask& operator=(const JXUrgentTask& source);
};

#endif
