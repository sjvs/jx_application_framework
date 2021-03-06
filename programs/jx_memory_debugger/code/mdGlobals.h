/******************************************************************************
 mdGlobals.h

   Copyright (C) 2010 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_mdGlobals
#define _H_mdGlobals

#include <jXGlobals.h>

// we include these for convenience

#include "MDApp.h"
#include "MDPrefsManager.h"
#include <JXDocumentManager.h>

MDApp*			MDGetApplication();
MDPrefsManager*	MDGetPrefsManager();

const JCharacter*	MDGetVersionNumberStr();
JString				MDGetVersionStr();

	// called by MDApp

JBoolean	MDCreateGlobals(MDApp* app);
void		MDDeleteGlobals();
void		MDCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JCharacter*	MDGetWMClassInstance();
const JCharacter*	MDGetMainWindowClass();

#endif
