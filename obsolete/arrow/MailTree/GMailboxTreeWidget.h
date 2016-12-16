/******************************************************************************
 GMailboxTreeWidget.h

	Interface for the GMailboxTreeWidget class

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GMailboxTreeWidget
#define _H_GMailboxTreeWidget

#include <JXNamedTreeListWidget.h>
#include <JPoint.h>
#include <JXCursor.h>

class GMailFileTreeNode;
class GMFileTree;

class JNamedTreeList;
class JNamedTreeNode;

class JXImage;
class JXMenuBar;
class JXTextMenu;
class JXTimerTask;
class JXToolBar;

class GMailboxTreeWidget : public JXNamedTreeListWidget
{
public:

	GMailboxTreeWidget(GMFileTree* tree, JNamedTreeList* treeList, JXMenuBar* menubar,
		JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		const HSizingOption hSizing, const VSizingOption vSizing,
		const JCoordinate x, const JCoordinate y,
		const JCoordinate w, const JCoordinate h);

	virtual ~GMailboxTreeWidget();

	void ReadPrefs(std::istream& input);
	void WritePrefs(std::ostream& output) const;

	void Update();

	void AddDefaultButtcons(JXToolBar* toolbar);

	virtual JBoolean IsEditable(const JPoint& cell) const;

	void		AddFile(const JCharacter* fullname);
	void		AddDirectory(const JCharacter* path, const JBoolean openDir = kJTrue);

protected:

	virtual void Draw(JXWindowPainter& p, const JRect& rect);
	virtual void Receive(JBroadcaster* sender, const Message& message);
	virtual void HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void HandleMouseDown(const JPoint& pt, const JXMouseButton button,
								const JSize clickCount,
								const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);
	virtual void HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);
	virtual void HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);
	virtual void HandleKeyPress(const int key,
								const JXKeyModifiers&   modifiers);

	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);
	virtual void	HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void	HandleDNDLeave();
	virtual void	HandleDNDDrop(	const JPoint& pt, const JArray<Atom>& typeList,
									const Atom	action, const Time time,
									const JXWidget* source);
	virtual JBoolean ConvertSelection(	const Atom name, const Atom requestType,
										Atom* returnType, unsigned char** data,
										JSize* dataLength, JSize* bitsPerBlock);
	virtual JBoolean GetImage(const JIndex index, const JXImage** image) const;
	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual JBoolean ExtractInputData(const JPoint& cell);

private:

	JXTextMenu*		itsMailboxMenu;
	GMFileTree*		itsTree;		// We don't own this.
	JPoint			itsDownPt;
	JBoolean		itsDownInCell;
	JIndex			itsCurrentDndHereIndex;
	JBoolean		itsDraggingBranch;
	Atom			itsMessageXAtom;
	Atom			itsMailboxXAtom;

	JXImage*			itsMailIcon;
	JXImage*			itsSelectedMailIcon;
	JXImage*			itsLockedMailIcon;
	JXImage*			itsSelectedFolderIcon;
	JXImage*			itsFolderIcon;

	JBoolean			itsDraggingFiles;
	JBoolean			itsOutsideRoot;
	JBoolean			itsWaitingToEdit;
	JCursorIndex		itsDNDCursor;

	JXTimerTask*		itsEditTask;
	JPoint				itsEditCell;

private:

	void		UpdateGMailboxMenu();
	void		HandleGMailboxMenu(const JIndex index);

	void		HandleReturn();

	void		OpenMailbox(const JIndex index);

	void		AddFile(const JString& path, const JString& file);

	void		CreateMailBox();
	void		CreateDirectory();

	JBoolean	GetNewName(GMailFileTreeNode** parent, JString* name);
	void		InsertNewNode(GMailFileTreeNode* parent, GMailFileTreeNode* node);

	void		DeleteSelected();

	JIndex		ClosestSelection(const JPoint& pt, const JIndex index, const JXNamedTreeListWidget::NodePart part);
	JIndex		LastWritableTopLevel();
	JIndex		WritableParent(GMailFileTreeNode* node);
	JBoolean	NodeIsWritable(GMailFileTreeNode* node);

	void		CheckSelectionForMenus(JBoolean* okDir, JBoolean* okFile, JBoolean* unLock);

	JBoolean	OkToDND();

	JBoolean	AlreadyPresent(const JCharacter* fullname);

	// not allowed

	GMailboxTreeWidget(const GMailboxTreeWidget& source);
	const GMailboxTreeWidget& operator=(const GMailboxTreeWidget& source);

};


/******************************************************************************
 <INLINE_NAME> (note if protected or private)

 *****************************************************************************/

// Define inline function or group of tightly related functions


#endif
