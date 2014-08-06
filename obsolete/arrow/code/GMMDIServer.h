/******************************************************************************
 GMMDIServer.h

	Interface for the GMMDIServer class

	Copyright � 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GMMDIServer
#define _H_GMMDIServer

#include <JXMDIServer.h>

class GMApp;

class GMMDIServer : public JXMDIServer
{
public:

	GMMDIServer(GMApp* app);
	virtual ~GMMDIServer();

	static void	PrintCommandLineHelp();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

//	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	GMApp*	itsApp;

private:

	// not allowed

	GMMDIServer(const GMMDIServer& source);
	const GMMDIServer& operator=(const GMMDIServer& source);

};

#endif