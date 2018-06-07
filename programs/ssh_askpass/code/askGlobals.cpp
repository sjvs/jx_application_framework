/******************************************************************************
 askGlobals.cc

	Access to global objects and factories.

	Copyright (C) 2006 by New Planet Software, Inc..

 ******************************************************************************/

#include "askGlobals.h"
#include "ASKApp.h"
#include <jAssert.h>

static ASKApp*	theApplication  = nullptr;		// owns itself

// string ID's

static const JCharacter* kDescriptionID = "Description::askGlobals";

/******************************************************************************
 ASKCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

void
ASKCreateGlobals
	(
	ASKApp* app
	)
{
	theApplication = app;
}

/******************************************************************************
 ASKDeleteGlobals

 ******************************************************************************/

void
ASKDeleteGlobals()
{
	theApplication = nullptr;
}

/******************************************************************************
 ASKGetApplication

 ******************************************************************************/

ASKApp*
ASKGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 ASKGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
ASKGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 ASKGetVersionStr

 ******************************************************************************/

JString
ASKGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}
