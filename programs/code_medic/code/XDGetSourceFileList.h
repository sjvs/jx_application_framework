/******************************************************************************
 XDGetSourceFileList.h

	Copyright (C) 2007 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetSourceFileList
#define _H_XDGetSourceFileList

#include "CMGetSourceFileList.h"

class CMFileListDir;

class XDGetSourceFileList : public CMGetSourceFileList
{
public:

	XDGetSourceFileList(CMFileListDir* fileList);

	virtual	~XDGetSourceFileList();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	void	ScanDirectory(const JCharacter* path);

	// not allowed

	XDGetSourceFileList(const XDGetSourceFileList& source);
	const XDGetSourceFileList& operator=(const XDGetSourceFileList& source);

};

#endif
