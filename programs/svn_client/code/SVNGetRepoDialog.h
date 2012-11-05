/******************************************************************************
 SVNGetRepoDialog.h

	Copyright � 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNGetRepoDialog
#define _H_SVNGetRepoDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JXInputField;
class JXStringHistoryMenu;

class SVNGetRepoDialog : public JXDialogDirector, public JPrefObject
{
public:

	SVNGetRepoDialog(JXDirector* supervisor, const JCharacter* windowTitle);

	virtual ~SVNGetRepoDialog();

	const JString&	GetRepo() const;

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXInputField*        itsRepoInput;
	JXStringHistoryMenu* itsRepoHistoryMenu;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* windowTitle);

	// not allowed

	SVNGetRepoDialog(const SVNGetRepoDialog& source);
	const SVNGetRepoDialog& operator=(const SVNGetRepoDialog& source);
};

#endif