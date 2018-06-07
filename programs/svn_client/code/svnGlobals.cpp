/******************************************************************************
 svnGlobals.cc

	Access to global objects and factories.

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "svnGlobals.h"
#include "SVNApp.h"
#include "SVNPrefsManager.h"
#include "SVNWDManager.h"
#include "SVNMDIServer.h"
#include <JXFSBindingManager.h>
#include <jAssert.h>

static SVNApp*			theApplication  = nullptr;		// owns itself
static SVNPrefsManager*	thePrefsManager = nullptr;
static SVNWDManager*	theWDManager    = nullptr;		// owned by JX
static SVNMDIServer*	theMDIServer    = nullptr;

// string ID's

static const JCharacter* kDescriptionID = "Description::svnGlobals";

/******************************************************************************
 SVNCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
SVNCreateGlobals
	(
	SVNApp* app
	)
{
	theApplication = app;

	JXDisplay* display = app->GetCurrentDisplay();

	JBoolean isNew;
	thePrefsManager	= jnew SVNPrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theWDManager = jnew SVNWDManager(display, kJTrue);
	assert( theWDManager != nullptr );

	theMDIServer = jnew SVNMDIServer;
	assert( theMDIServer != nullptr );

	return isNew;
}

/******************************************************************************
 SVNDeleteGlobals

 ******************************************************************************/

void
SVNDeleteGlobals()
{
	theApplication = nullptr;
	theMDIServer   = nullptr;
	theWDManager   = nullptr;

	JXFSBindingManager::Destroy();

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 SVNCleanUpBeforeSuddenDeath

	This must be the last one called by SVNApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
SVNCleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
//		theMDIServer->JPrefObject::WritePrefs();
		}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 SVNGetApplication

 ******************************************************************************/

SVNApp*
SVNGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 SVNGetPrefsManager

 ******************************************************************************/

SVNPrefsManager*
SVNGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 SVNGetWDManager

 ******************************************************************************/

SVNWDManager*
SVNGetWDManager()
{
	assert( theWDManager != nullptr );
	return theWDManager;
}

/******************************************************************************
 SVNGetMDIServer

 ******************************************************************************/

SVNMDIServer*
SVNGetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 SVNGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
SVNGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 SVNGetVersionStr

 ******************************************************************************/

JString
SVNGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}

/******************************************************************************
 SVNGetWMClassInstance

 ******************************************************************************/

const JCharacter*
SVNGetWMClassInstance()
{
	return "NPS SVN Client";
}

const JCharacter*
SVNGetMainWindowClass()
{
	return "NPS_SVN_Client_Main_Window";
}
