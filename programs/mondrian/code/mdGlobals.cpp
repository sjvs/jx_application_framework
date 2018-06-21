/******************************************************************************
 mdGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "mdGlobals.h"
#include "MDApp.h"
#include "MDPrefsManager.h"
#include "MDMDIServer.h"
#include "mdHelpText.h"
#include <jAssert.h>

static MDApp*			theApplication  = nullptr;		// owns itself
static MDPrefsManager*	thePrefsManager = nullptr;
static MDMDIServer*		theMDIServer    = nullptr;

// string ID's

static const JCharacter* kDescriptionID = "Description::mdGlobals";

/******************************************************************************
 MDCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
MDCreateGlobals
	(
	MDApp* app
	)
{
	theApplication = app;

	JBoolean isNew;
	thePrefsManager	= jnew MDPrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theMDIServer = jnew MDMDIServer;
	assert( theMDIServer != nullptr );

	return isNew;
}

/******************************************************************************
 MDDeleteGlobals

 ******************************************************************************/

void
MDDeleteGlobals()
{
	theApplication = nullptr;
	theMDIServer   = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 MDCleanUpBeforeSuddenDeath

	This must be the last one called by MDApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
MDCleanUpBeforeSuddenDeath
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
 MDGetApplication

 ******************************************************************************/

MDApp*
MDGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 MDGetPrefsManager

 ******************************************************************************/

MDPrefsManager*
MDGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 MDGetMDIServer

 ******************************************************************************/

MDMDIServer*
MDGetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 MDGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
MDGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 MDGetVersionStr

 ******************************************************************************/

JString
MDGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}

/******************************************************************************
 MDGetWMClassInstance

 ******************************************************************************/

const JCharacter*
MDGetWMClassInstance()
{
	return "Mondrian";
}

const JCharacter*
MDGetMainWindowClass()
{
	return "Mondrian_Main_Window";
}
