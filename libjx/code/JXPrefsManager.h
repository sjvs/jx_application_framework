/******************************************************************************
 JXPrefsManager.h

	Copyright � 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPrefsManager
#define _H_JXPrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPrefsManager.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason

class JXPrefsManager : public JPrefsManager
{
public:

	JXPrefsManager(const JFileVersion currentVersion,
				   const JBoolean eraseFileIfOpen);

	virtual ~JXPrefsManager();

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	virtual void	SaveAllBeforeDestruct();
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTimerTask*	itsSafetySaveTask;

private:

	// not allowed

	JXPrefsManager(const JXPrefsManager& source);
	JXPrefsManager& operator=(const JXPrefsManager& source);
};

#endif
