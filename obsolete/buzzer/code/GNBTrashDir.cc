/******************************************************************************
 GNBTrashDir.cc

	<Description>

	BASE CLASS = public JXWindowDirector

	Copyright � 1999 by Glenn W. Bach.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

// includes

//Class Header
#include <GNBTrashDir.h>
#include <JTree.h>
#include <GNBRootNode.h>
#include <GNBNoteNode.h>
#include <GNBToDoNode.h>
#include <GNBTrashWidget.h>
#include <JNamedTreeList.h>
#include <GNBGlobals.h>
#include <GNBPrefsMgr.h>
#include <GNBApp.h>

#include <GNBTrashIcon.xpm>

#include <JChooseSaveFile.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXApplication.h>
#include <JXTextButton.h>
#include <JXToolBar.h>
#include <JXDocumentManager.h>
#include <JXDocumentMenu.h>

#include <JString.h>
#include <JFileArray.h>

#include <jAssert.h>

const JCoordinate kCloseButtonWidth 	= 50;
const JCoordinate kCurrentPrefsVersion	= 0;

const JCharacter* kTrashWindowsMenuTitleStr = "Windows";

/******************************************************************************
 Constructor

 *****************************************************************************/

GNBTrashDir::GNBTrashDir
	(
	JXDirector* supervisor
	)
	:
   JXDocument(supervisor),
   itsTreeChanged(kJFalse)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GNBTrashDir::~GNBTrashDir()
{
	FlushDiscards();
	itsWidget->Save();
	delete itsTree;
}

/******************************************************************************
 BuildWindow 


 ******************************************************************************/

void
GNBTrashDir::BuildWindow()
{
	JCoordinate w = 300;
	JCoordinate h = 500;
	JXWindow* window = new JXWindow(this, w,h, "Discards");
    assert( window != NULL );
    SetWindow(window);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
    window->PlaceAsDialogWindow();
    window->SetWMClass(GNBGetWMClassInstance(), GNBGetTrashWindowClass());
    
	JXMenuBar* menuBar =
		new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop, 
			0,0, w - kCloseButtonWidth,kJXDefaultMenuBarHeight);
	assert( menuBar != NULL );

    itsCloseButton =
        new JXTextButton("Close", window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 
                    w - kCloseButtonWidth,0, 
                    kCloseButtonWidth,kJXDefaultMenuBarHeight);
    assert( itsCloseButton != NULL );
    itsCloseButton->SetShortcuts("#W^[");
    ListenTo(itsCloseButton);

	itsToolBar =
		new JXToolBar(GNBGetPrefsMgr(), kGTrashToolBarID,
			itsMenuBar, 300, 200,
			window, JXWidget::kHElastic, JXWidget::kVElastic, 
			0,kJXDefaultMenuBarHeight, w,h - kJXDefaultMenuBarHeight);
	assert( itsToolBar != NULL );

    JSize newHeight = itsToolBar->GetWidgetEnclosure()->GetBoundsHeight();

	h -= kJXDefaultMenuBarHeight;
	
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window, JXWidget::kHElastic,JXWidget::kVElastic,
						   0, kJXDefaultMenuBarHeight,
						   w, newHeight);
	assert( scrollbarSet != NULL );
    
    GNBRootNode* base = new GNBRootNode(NULL, "");
    assert(base != NULL);
    itsTree = new JTree(base);
    assert(itsTree != NULL);
    JNamedTreeList* list = new JNamedTreeList(itsTree);
    
//    itsWidget = 
//    	GNBTrashWidget::Create(itsTree, list, menuBar,
//    		scrollbarSet, scrollbarSet->GetScrollEnclosure(),
//    		JXWidget::kHElastic, JXWidget::kVElastic,
//    		0,0,w,h);
//    assert(itsWidget != NULL);
//	ListenTo(itsWidget);

	JXDocumentMenu* fileListMenu =
		new JXDocumentMenu(kTrashWindowsMenuTitleStr, menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != NULL );
	menuBar->AppendMenu(fileListMenu);
}

/******************************************************************************
 Receive (virtual protected)


 ******************************************************************************/

void
GNBTrashDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsWidget && message.Is(GNBTreeWidgetBase::kDataChanged))
		{
		DataModified();
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}
	else
		{
		JXDocument::Receive(sender, message);
		}
}

/******************************************************************************
 DiscardNode (public)

 ******************************************************************************/

void
GNBTrashDir::DiscardNode
	(
	JTreeNode* node
	)
{
	JTreeNode* base = itsTree->GetRoot();
	base->Append(node);
	GNBBaseNode* bnode = dynamic_cast<GNBBaseNode*>(node);
	assert(bnode != NULL);
	GNBBaseNode::Type type = bnode->GetType();
	if (type == GNBBaseNode::kNoteNode)
		{
		GNBNoteNode* nnode = dynamic_cast<GNBNoteNode*>(node);
		assert(nnode != NULL);
		//GNBGetApplication()->CloseNote(nnode->GetID());
		}
	else if (type == GNBBaseNode::kToDoNode)
		{
		GNBToDoNode* tdnode = dynamic_cast<GNBToDoNode*>(node);
		assert(tdnode != NULL);
		JFAID_t id;
		if (tdnode->GetID(&id))
			{
			//GNBGetApplication()->CloseNote(id);
			}
		}
}

/******************************************************************************
 FlushDiscards (public)

 ******************************************************************************/

void
GNBTrashDir::FlushDiscards()
{
	itsWidget->FlushDiscards();
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

JBoolean
GNBTrashDir::OKToClose()
{
	if (itsWidget->EndEditing())
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 NeedsSave

 ******************************************************************************/

JBoolean
GNBTrashDir::NeedsSave()
	const
{
	return itsTreeChanged;
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
GNBTrashDir::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JBoolean tellApp = kJFalse;
	if (reason == JXDocumentManager::kTimer)
		{
		tellApp = kJTrue;
		}
	itsWidget->Save();
	DataReverted();
	if (tellApp)
		{
		//GNBGetApplication()->NeedsSave();
		}
}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
GNBTrashDir::DiscardChanges()
{

}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JXPM
GNBTrashDir::GetMenuIcon()
	const
{
	return JXPM(kfm_trash);
}

/******************************************************************************
 DataModified (public)

 ******************************************************************************/

void
GNBTrashDir::DataModified()
{
	itsTreeChanged = kJTrue;
}

/******************************************************************************
 DataReverted (public)

 ******************************************************************************/

void
GNBTrashDir::DataReverted()
{
	itsTreeChanged = kJFalse;
}