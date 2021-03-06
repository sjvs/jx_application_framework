/******************************************************************************
 JXMDIServer.h

	Interface for the JXMDIServer class

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMDIServer
#define _H_JXMDIServer

#include <JMDIServer.h>

class JXMDIServer : public JMDIServer
{
public:

	JXMDIServer();

	virtual ~JXMDIServer();

protected:

	virtual JBoolean	CanAcceptMDIRequest();
	virtual void		PreprocessArgList(JPtrArray<JString>* argList);

private:

	// not allowed

	JXMDIServer(const JXMDIServer& source);
	const JXMDIServer& operator=(const JXMDIServer& source);
};

#endif
