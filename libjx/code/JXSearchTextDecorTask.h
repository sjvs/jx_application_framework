/******************************************************************************
 JXSearchTextDecorTask.h

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSearchTextDecorTask
#define _H_JXSearchTextDecorTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXWindow;
class JXTextCheckbox;

class JXSearchTextDecorTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXSearchTextDecorTask(JXWindow* window,
						  JXTextCheckbox* stayOpenCB, JXTextCheckbox* retainFocusCB);

	virtual ~JXSearchTextDecorTask();

	virtual void	Perform();

private:

	JXWindow*		itsWindow;			// not owned
	JXTextCheckbox*	itsStayOpenCB;		// not owned
	JXTextCheckbox*	itsRetainFocusCB;	// not owned

private:

	// not allowed

	JXSearchTextDecorTask(const JXSearchTextDecorTask& source);
	const JXSearchTextDecorTask& operator=(const JXSearchTextDecorTask& source);
};

#endif
