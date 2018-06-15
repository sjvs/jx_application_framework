/******************************************************************************
 JXProgressTask.h

	Copyright (C) 2000 John Lindal.

 ******************************************************************************/

#ifndef _H_JXProgressTask
#define _H_JXProgressTask

#include "JXIdleTask.h"
#include <JBroadcaster.h>

class JString;
class JProgressDisplay;

class JXProgressTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXProgressTask(const JString& message, const JBoolean allowCancel);
	JXProgressTask(JProgressDisplay* pg);

	virtual ~JXProgressTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JProgressDisplay*	itsPG;
	JBoolean			itsOwnsPGFlag;

private:

	void	JXProgressTaskX(const JString& message, const JBoolean allowCancel);

	// not allowed

	JXProgressTask(const JXProgressTask& source);
	const JXProgressTask& operator=(const JXProgressTask& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kCancelled;

	class Cancelled : public JBroadcaster::Message
		{
		public:

			Cancelled()
				:
				JBroadcaster::Message(kCancelled)
				{ };
		};
};

#endif
