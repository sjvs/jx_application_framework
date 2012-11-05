/******************************************************************************
 CBManPageDocument.h

	Interface for the CBManPageDocument class

	Copyright � 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBManPageDocument
#define _H_CBManPageDocument

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBTextDocument.h"

class CBManPageDocument : public CBTextDocument
{
public:

	static JBoolean	Create(CBManPageDocument** doc, const JCharacter* pageName,
						   const JCharacter pageIndex = ' ', const JBoolean apropos = kJFalse);

	virtual ~CBManPageDocument();

protected:

	CBManPageDocument(const JCharacter* pageName, const JCharacter pageIndex,
					  const JBoolean apropos, CBManPageDocument** trueDoc);

	static JString	GetCmd1(const JCharacter* pageName, const JCharacter pageIndex);
	static JString	GetCmd2(const JCharacter* pageName);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsIgnoreNameChangedFlag;

	static JPtrArray<JString>			theManCmdList;
	static JPtrArray<CBTextDocument>	theManDocList;

private:

	static void	RemoveFromManPageList(CBManPageDocument* doc);

	// not allowed

	CBManPageDocument(const CBManPageDocument& source);
	const CBManPageDocument& operator=(const CBManPageDocument& source);
};

#endif