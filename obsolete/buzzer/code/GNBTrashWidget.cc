/******************************************************************************
 GNBTrashWidget.cc

	<Description>

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <GNBTrashWidget.h>
#include <GNBTreeDir.h>
#include <GNBCategoryNode.h>
#include <GNBNoteNode.h>
#include <GNBGlobals.h>
#include "GNBApp.h"

#include <JTree.h>
#include <JNamedTreeList.h>
#include <JNamedTreeNode.h>

#include <JXToolBar.h>

#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JString.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXDirector.h>
#include <JXSelectionManager.h>
#include <JXUserNotification.h>
#include <JXApplication.h>
#include <JXDNDManager.h>
#include <JXChooseSaveFile.h>
#include <JXImage.h>
#include <JXDisplay.h>
#include <JXColormap.h>

#include <JFontManager.h>
#include <JTableSelection.h>
#include <JTableSelectionIterator.h>
#include <JOrderedSetIterator.h>
#include <JDirInfo.h>
#include <JDirEntry.h>
#include <JError.h>

#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jASCIIConstants.h>
#include <jStreamUtil.h>
#include <jErrno.h>
#include <jMath.h>

#include <X11/keysym.h>
#include <strstream>
#include <iostream.h>
#include <fstream.h>
#include <jAssert.h>

static const JCharacter* kGNBMenuTitleStr = "Discards";
static const JCharacter* kGNBMenuStr = 
	"    Edit tool bar... %i EditToolBar::Notebook"
	"%l| Flush discards   %i FlushDiscards::Notebook"
	"%l| Close %k Meta-W  %i CloseTrash::Notebook"
	"  | Quit %k Meta-Q   %i Quite::Notebook";

enum
{
	kEditToolBarCmd = 1,
	kFlushCmd,
	kCloseCmd,
	kQuitCmd
};

static const JIndex kGNBTrashTreePrefsIndex = 2;

/******************************************************************************
 Constructor

 *****************************************************************************/

GNBTrashWidget*
GNBTrashWidget::Create
	(
	GNBTreeDir* 		dir, 
	JTree*				tree,
	JNamedTreeList* 	treeList, 
	JXMenuBar* 			menuBar,
	JXToolBar* 			toolBar,
	JXScrollbarSet* 	scrollbarSet, 
	JXContainer* 		enclosure,
	const HSizingOption	hSizing, 
	const VSizingOption vSizing,
	const JCoordinate 	x, 
	const JCoordinate 	y,
	const JCoordinate 	w, 
	const JCoordinate 	h
	)
{
	GNBTrashWidget* widget = 
		new GNBTrashWidget(dir, tree, treeList, menuBar, toolBar,
   					  scrollbarSet,	enclosure, hSizing, vSizing, x, y, w, h);
   	assert(widget != NULL);

	widget->itsToolBar->LoadPrefs();

	if (widget->itsToolBar->IsEmpty())
		{
		widget->itsToolBar->AppendButton(widget->itsNBMenu, kFlushCmd);
		widget->itsToolBar->AppendButton(widget->itsNBMenu, kCloseCmd);
		}

	widget->ReadSetup();
	
	return widget;
}

GNBTrashWidget::GNBTrashWidget
	(
	GNBTreeDir* 		dir, 
	JTree*				tree,
	JNamedTreeList* 	treeList, 
	JXMenuBar* 			menuBar,
	JXToolBar* 			toolBar,
	JXScrollbarSet* 	scrollbarSet, 
	JXContainer* 		enclosure,
	const HSizingOption	hSizing, 
	const VSizingOption vSizing,
	const JCoordinate 	x, 
	const JCoordinate 	y,
	const JCoordinate 	w, 
	const JCoordinate 	h
	)
	:
   GNBTreeWidgetBase(kGNBTrashTreePrefsIndex, dir, tree, treeList, 
   					  scrollbarSet,	enclosure, hSizing, vSizing, x, y, w, h),
	itsToolBar(toolBar)
{
	itsNBMenu = menuBar->AppendTextMenu(kGNBMenuTitleStr);
	itsNBMenu->SetMenuItems(kGNBMenuStr);
	itsNBMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsNBMenu);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GNBTrashWidget::~GNBTrashWidget()
{
}

/******************************************************************************
 Receive (virtual protected)


 ******************************************************************************/

void
GNBTrashWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNBMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleNBMenu(selection->GetIndex());
		}
	else if (sender == itsNBMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateNBMenu();
		}
	else
		{
		GNBTreeWidgetBase::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateNBMenu 


 ******************************************************************************/

void
GNBTrashWidget::UpdateNBMenu()
{
	itsNBMenu->EnableItem(kFlushCmd);
	itsNBMenu->EnableItem(kCloseCmd);
	itsNBMenu->EnableItem(kQuitCmd);
}

/******************************************************************************
 HandleNBMenu 


 ******************************************************************************/

void
GNBTrashWidget::HandleNBMenu
	(
	const JIndex index
	)
{
	if (index == kFlushCmd)
		{
		FlushDiscards();
		}
	else if (index == kCloseCmd)
		{
		GetWindow()->GetDirector()->Deactivate();
		}
	else if (index == kQuitCmd)
		{
		GNBGetApplication()->Quit();
		}
}

/******************************************************************************
 FlushDiscards (public)

 ******************************************************************************/

void
GNBTrashWidget::FlushDiscards()
{
	JTreeNode* base = GetTreeList()->GetTree()->GetRoot();
	GNBBaseNode* node = dynamic_cast<GNBBaseNode*>(base);
	assert(node != NULL);
	node->DiscardBranch();
	SelectedFlushed();
}
