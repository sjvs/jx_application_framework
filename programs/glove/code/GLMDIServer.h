/******************************************************************************
 GLMDIServer.h

	Interface for the GLMDIServer class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GLMDIServer
#define _H_GLMDIServer

#include <JXMDIServer.h>

class GLPlotApp;

class GLMDIServer : public JXMDIServer
{
public:

	GLMDIServer(GLPlotApp* app);
	virtual ~GLMDIServer();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

//	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	GLPlotApp*	itsApp;

private:

	// not allowed

	GLMDIServer(const GLMDIServer& source);
	const GLMDIServer& operator=(const GLMDIServer& source);

};

#endif
