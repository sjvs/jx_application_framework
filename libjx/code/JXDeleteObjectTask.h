/******************************************************************************
 JXDeleteObjectTask.h

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDeleteObjectTask
#define _H_JXDeleteObjectTask

#include <JXIdleTask.h>
#include <JBroadcaster.h>

template <class T>
class JXDeleteObjectTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	static void	Delete(T* obj);

	virtual ~JXDeleteObjectTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

protected:

	JXDeleteObjectTask(T* obj);

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	T*	itsObject;

private:

	// not allowed

	JXDeleteObjectTask(const JXDeleteObjectTask& source);
	const JXDeleteObjectTask& operator=(const JXDeleteObjectTask& source);
};

#include <JXDeleteObjectTask.tmpl>

#endif
