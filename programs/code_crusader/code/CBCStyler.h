/******************************************************************************
 CBCStyler.h

	Copyright � 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCStyler
#define _H_CBCStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStylerBase.h"
#include "CBCScanner.h"

class CBCStyler : public CBStylerBase, public CBCScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBCStyler();

protected:

	CBCStyler();

	virtual void	Scan(istream& input, const TokenExtra& initData);
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JTextEditor::Font>& styles,
										const JIndexRange& modifiedRange,
										const JBoolean deletion,
										JIndexRange* checkRange);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	static CBCStyler*	itsSelf;

private:

	JBoolean	SlurpPPComment(JIndexRange* totalRange);

	// not allowed

	CBCStyler(const CBCStyler& source);
	const CBCStyler& operator=(const CBCStyler& source);
};

#endif