/******************************************************************************
 GMailboxTreeDir.h

	Interface for the GMailboxTreeDir class

	Copyright � 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GMailboxTreeDir
#define _H_GMailboxTreeDir

//includes

// Superclass Header
#include <GMManagedDirector.h>
#include <JPrefObject.h>
#include <JPtrArray-JString.h>

class GMFileTree;
class JXTextButton;
class GMailboxTreeWidget;
class JXTimerTask;
class JXTextMenu;
class JXToolBar;
class JXImage;

class GMailboxTreeDir : public GMManagedDirector, public JPrefObject
{
public:

public:

	GMailboxTreeDir(JXDirector* supervisor);
	virtual ~GMailboxTreeDir();

	void	EditToolBar();

	virtual JXImage*	GetMenuIcon() const;
	virtual JBoolean	GetShortcut(JString* shortcut) const;
	virtual JBoolean	GetID(JString* id) const;

	virtual void		Activate();
	virtual JBoolean	Deactivate();

	void				GetTopLevelNodes(JPtrArray<JString>* nodes);

	void				BroadcastMailTreeChanged();

	void				AddFile(const JCharacter* fullname);
	void				AddDirectory(const JCharacter* path);

	void				Update();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	GMFileTree*			itsTree;
	JXTextButton*		itsCloseButton;
	GMailboxTreeWidget*	itsWidget;
	JXTimerTask*		itsTimerTask;
	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsPrefsMenu;
	JXTextMenu*			itsHelpMenu;
	JXToolBar*			itsToolBar;
	JXImage*			itsMenuIcon;

private:

	void		UpdateFileMenu();
	void		HandleFileMenu(const JIndex index);

	void		HandlePrefsMenu(const JIndex index);

	void 		HandleHelpMenu(const JIndex index);

	void BuildWindow();

	// not allowed

	GMailboxTreeDir(const GMailboxTreeDir& source);
	const GMailboxTreeDir& operator=(const GMailboxTreeDir& source);

public:

	static const JCharacter* kMailTreeChanged;

	class MailTreeChanged : public JBroadcaster::Message
	{
		public:
			MailTreeChanged()
			:
			JBroadcaster::Message(kMailTreeChanged)
			{ };
	};
};

/******************************************************************************
 GetMenuIcon (public)

 ******************************************************************************/

inline JXImage*
GMailboxTreeDir::GetMenuIcon()
	const
{
	return itsMenuIcon;
}

#endif