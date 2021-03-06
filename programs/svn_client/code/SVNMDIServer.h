/******************************************************************************
 SVNMDIServer.h

	Copyright (C) 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_SVNMDIServer
#define _H_SVNMDIServer

#include <JXMDIServer.h>

class SVNMDIServer : public JXMDIServer
{
public:

	SVNMDIServer();

	virtual	~SVNMDIServer();

	static void		PrintCommandLineHelp();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	// not allowed

	SVNMDIServer(const SVNMDIServer& source);
	const SVNMDIServer& operator=(const SVNMDIServer& source);
};

#endif
