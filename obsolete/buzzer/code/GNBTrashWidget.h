/******************************************************************************
 GNBTrashWidget.h

	Interface for the GNBTrashWidget class

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GNBTrashWidget
#define _H_GNBTrashWidget

#include <GNBTreeWidgetBase.h>

#include <JFileArray.h>

class JXMenuBar;
class JNamedTreeList;
class JTree;
class JXTextMenu;
class JXToolBar;
class JNamedTreeNode;

class GNBTrashWidget : public GNBTreeWidgetBase
{
public:

	static GNBTrashWidget* Create(GNBTreeDir* dir, JTree* tree, 
		JNamedTreeList* treeList, JXMenuBar* menubar, JXToolBar* toolbar,
		JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		const HSizingOption hSizing, const VSizingOption vSizing,
		const JCoordinate x, const JCoordinate y,
		const JCoordinate w, const JCoordinate h);
	virtual ~GNBTrashWidget();

	void	FlushDiscards();

protected:

	GNBTrashWidget(GNBTreeDir* dir, JTree* tree, 
		JNamedTreeList* treeList, JXMenuBar* menubar, JXToolBar* toolbar,
		JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		const HSizingOption hSizing, const VSizingOption vSizing,
		const JCoordinate x, const JCoordinate y,
		const JCoordinate w, const JCoordinate h);
	virtual void Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu*		itsNBMenu;
	JXToolBar*		itsToolBar;

private:

	void 		UpdateNBMenu();
	void 		HandleNBMenu(const JIndex index);

	// not allowed

	GNBTrashWidget(const GNBTrashWidget& source);
	const GNBTrashWidget& operator=(const GNBTrashWidget& source);

};

#endif
