/******************************************************************************
 plotter.cpp

	This provides a way to test the JX library and provides sample source
	code for everyone to study.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <GLPlotApp.h>
#include "GLMDIServer.h"
#include "GLPrefsMgr.h"
#include "GLGlobals.h"
#include <jCommandLine.h>
#include <jTime.h>
#include <jWebUtil.h>
#include <jAssert.h>

extern JCharacter* kGLSig;
extern JCharacter* kAppSignature;

void ParseTextOptions(const int argc, char* argv[]);

void PrintHelp();
void PrintVersion();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	ParseTextOptions(argc, argv);

	if (!GLMDIServer::WillBeMDIServer(kAppSignature, argc, argv))
		{
		return 0;
		}

	JBoolean displayAbout;
	JString prevVersStr;

	GLPlotApp* app =
		jnew GLPlotApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	if (displayAbout &&
		!(JGetUserNotification())->AcceptLicense())
		{
		return 0;
		}

	JCheckForNewerVersion(GetPrefsMgr(), kVersionCheckID);

	GLGetMDIServer()->HandleCmdLineOptions(argc, argv);

	if (displayAbout)
		{
		app->DisplayAbout(prevVersStr);
		}

	app->Run();				// never actually returns
	return 0;
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions won't gag.

 ******************************************************************************/

void
ParseTextOptions
	(
	const int	argc,
	char*		argv[]
	)
{
	long index = 1;
	while (index < argc)
		{
		if (JIsVersionRequest(argv[index]))
			{
			GLPlotApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			GLPlotApp::InitStrings();
			PrintHelp();
			exit(0);
			}
		index++;
		}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString("GLCommandLineHelp", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	std::cout << JGetString("GLDescription", map, sizeof(map));
	std::cout << std::endl << std::endl;
}
