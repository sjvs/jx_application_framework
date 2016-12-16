/******************************************************************************
 GAddressBookTreeWidget.h

	Interface for the GAddressBookTreeWidget class

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GAddressBookTreeWidget
#define _H_GAddressBookTreeWidget

#include <JXNamedTreeListWidget.h>
#include <JPoint.h>
#include <JXCursor.h>

class JXMenuBar;
class JNamedTreeList;
class JTree;
class JXTextMenu;
class JNamedTreeNode;
class JXImage;
class JXInputField;
class GAddressBookTreeNode;
class JXToolBar;

class GAddressBookTreeWidget : public JXNamedTreeListWidget
{
public:

	GAddressBookTreeWidget(JTree* tree, JNamedTreeList* treeList, JXMenuBar* menubar,
		JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		const HSizingOption hSizing, const VSizingOption vSizing,
		const JCoordinate x, const JCoordinate y,
		const JCoordinate w, const JCoordinate h);

	virtual ~GAddressBookTreeWidget();

	void ReadPrefs(std::istream& input);
	void WritePrefs(std::ostream& output) const;

	void			Update();
	virtual void	HandleKeyPress(const int key,
									const JXKeyModifiers&   modifiers);
	void			Save();
	void			AddDefaultButtcons(JXToolBar* toolbar);

	virtual JBoolean	IsEditable(const JPoint& cell) const;

protected:

	virtual void Draw(JXWindowPainter& p, const JRect& rect);
	virtual void TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void Receive(JBroadcaster* sender, const Message& message);
	virtual void HandleMouseDown(const JPoint& pt, const JXMouseButton button,
								const JSize clickCount,
								const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);
	virtual void HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);
	virtual void HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);

	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);
	virtual void	HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void	HandleDNDLeave();
	virtual void	HandleDNDDrop(	const JPoint& pt, const JArray<Atom>& typeList,
									const Atom	action, const Time time,
									const JXWidget* source);
	virtual Atom	 GetDNDAction(const JXContainer* target,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);
	virtual void	 GetDNDAskActions(const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers,
									  JArray<Atom>* askActionList,
									  JPtrArray<JString>* askDescriptionList);
	virtual JBoolean ConvertSelection(	const Atom name, const Atom requestType,
										Atom* returnType, unsigned char** data,
										JSize* dataLength, JSize* bitsPerBlock);
	virtual JBoolean GetImage(const JIndex index, const JXImage** image) const;
	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual JSize	GetMinCellWidth(const JPoint& cell) const;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
									const JCoordinate x, const JCoordinate y,
									const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();
	virtual void			DoubleClickIcon(const JIndex index, const JXKeyModifiers& modifiers);

private:

	enum SelectionType
	{
		kNoneSelected = 1,
		kOneBookSelected,
		kBooksSelected,
		kOneEntrySelected,
		kEntriesSelected,
		kExtraItemsSelected,
		kItemsSelected,
		kRandomSelection
	};

	enum EditType
	{
		kNoneEditing = 1,
		kBookEditing,
		kEntryEditing,
		kItemEditing,
		kExtraItemEditing
	};

	enum DragType
	{
		kDraggingBooks = 1,
		kDraggingEntries,
		kDraggingEMails,
		kDraggingText
	};

private:

	JXTextMenu*		itsAddressBookMenu;
	JTree*			itsTree;				// We don't own this.
	JPoint			itsDownPt;
	JBoolean		itsDownInCell;
	JIndex			itsCurrentDndHereIndex;
	JBoolean		itsDraggingBranch;
	Atom			itsMessageXAtom;
	Atom			itsAddressBookXAtom;

	JXImage*			itsBookIcon;
	JXImage*			itsSelectedBookIcon;
	JXImage*			itsEntryIcon;
	JXImage*			itsSelectedEntryIcon;
	JXImage*			itsEMailIcon;
	JXImage*			itsNameIcon;
	JXImage*			itsMailboxIcon;
	JXImage*			itsCommentIcon;

	JBoolean			itsIsBookDnd;
	JBoolean			itsOutsideRoot;
	JBoolean			itsWaitingToEdit;
	JCursorIndex		itsDNDCursor;

	JXInputField*		itsStringInputField;

	DragType			itsDragType;

private:

	SelectionType	GetSelectionType();
	EditType		GetEditType();

	JBoolean	GetFirstWritableBook(GAddressBookTreeNode** book);
	JBoolean	GetCurrentBook(GAddressBookTreeNode** book);

	void		UpdateAddressBookMenu();
	void		HandleAddressBookMenu(const JIndex index);

	void		NewMailFromAddress(const JIndex index);
	void		NewNickName(JNamedTreeNode* parent, const JPtrArray<JString>& addresses);
	JBoolean	NewBook(JString* book);

	void		AddSelectedToBook(JNamedTreeNode* book, const JBoolean dndmove);
	void		AddEntryToBook(JNamedTreeNode* entry, JNamedTreeNode* book, const JBoolean dndmove);
	void		AddAddressToEntry(JNamedTreeNode* node, const JString& address);

	void		AddFile(const JString& path, const JString& file);
	void		AddFile(const JCharacter* fullname);
	void		AddDirectory(const JCharacter* path);

	void		DeleteSelected();

	JIndex		ClosestSelection(const JPoint& pt, const JIndex index, const JXNamedTreeListWidget::NodePart part);

	JBoolean	OkToDND();

	JBoolean	SelectedHaveSameParent();
	JBoolean	SelectedInSameBook();
	JBoolean	SelectedInSameEntry();
	JBoolean	GetCurrentEntryCell(JPoint* cell);

	void		RemoveSelected();

	// not allowed

	GAddressBookTreeWidget(const GAddressBookTreeWidget& source);
	const GAddressBookTreeWidget& operator=(const GAddressBookTreeWidget& source);

};


/******************************************************************************
 <INLINE_NAME> (note if protected or private)

 *****************************************************************************/

// Define inline function or group of tightly related functions


#endif
