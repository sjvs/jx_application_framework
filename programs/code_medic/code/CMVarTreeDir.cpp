/******************************************************************************
 CMVarTreeDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright � 2001-04 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "CMVarTreeDir.h"
#include "CMVarTreeWidget.h"
#include "CMVarNode.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include "cmHelpText.h"
#include "cmActionDefs.h"

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXInputField.h>
#include <JXTextButton.h>
#include <JXHelpManager.h>
#include <JXWDMenu.h>
#include <JXImage.h>

#include <JTree.h>
#include <JNamedTreeList.h>
#include <jAssert.h>

static const JCharacter* kWindowTitleSuffix = " Variables";

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    Open source file... %k Meta-O %i" kCMOpenSourceFileAction
	"%l| Close               %k Meta-W %i" kJXCloseWindowAction
	"  | Quit                %k Meta-Q %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kCloseWindowCmd,
	kQuitCmd
};

// Actions menu

static const JCharacter* kActionMenuTitleStr = "Actions";
static const JCharacter* kActionMenuStr =
	"    New expression      %k Meta-N       %i" kCMNewExpressionAction
	"  | Remove expression   %k Backspace.   %i" kCMRemoveExpressionAction
	"%l| Display as 1D array %k Meta-Shift-A %i" kCMDisplay1DArrayAction
	"  | Plot as 1D array                    %i" kCMPlot1DArrayAction
	"  | Display as 2D array                 %i" kCMDisplay2DArrayAction
	"%l| Watch expression                    %i" kCMWatchVarValueAction
	"  | Watch expression location           %i" kCMWatchVarLocationAction
	"%l| Examine memory                      %i" kCMExamineMemoryAction
	"  | Disassemble memory                  %i" kCMDisasmMemoryAction;

enum
{
	kAddVarCmd = 1,
	kDelVarCmd,
	kDisplay1DArrayCmd,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd
};

// Windows menu

static const JCharacter* kWindowsMenuTitleStr = "Windows";

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About"
	"%l| Table of Contents"
	"  | Overview"
	"  | This window %k F1"
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

CMVarTreeDir::CMVarTreeDir
	(
	CMCommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMVarTreeDir::~CMVarTreeDir()
{
	(CMGetPrefsManager())->SaveWindowSize(kVarTreeWindSizeID, GetWindow());
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CMVarTreeDir::Activate()
{
	JXWindowDirector::Activate();
	itsWidget->ShouldUpdate(kJTrue);
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

JBoolean
CMVarTreeDir::Deactivate()
{
	itsWidget->ShouldUpdate(kJFalse);
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "medic_variables_window.xpm"

#include "medic_show_1d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_memory.xpm"
#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
CMVarTreeDir::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 450,500, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle(kWindowTitleSuffix);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(kJTrue);
	window->SetWMClass(CMGetWMClassInstance(), CMGetVariableWindowClass());
	(CMGetPrefsManager())->GetWindowSize(kVarTreeWindSizeID, window);

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, medic_variables_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	CMVarNode* root = itsLink->CreateVarNode();
	assert( root != NULL );
	itsTree = new JTree(root);
	assert( itsTree != NULL );
	JNamedTreeList* treeList = new JNamedTreeList(itsTree);
	assert( treeList != NULL );

	itsWidget =
		new CMVarTreeWidget(itsCommandDir, kJTrue, menuBar, itsTree, treeList,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != NULL);
	itsWidget->FitToEnclosure();

	// menus

	itsFileMenu = menuBar->PrependTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionMenu = menuBar->AppendTextMenu(kActionMenuTitleStr);
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "CMVarTreeDir");
	ListenTo(itsActionMenu);

	itsActionMenu->SetItemImage(kDisplay1DArrayCmd, medic_show_1d_array);
	itsActionMenu->SetItemImage(kPlot1DArrayCmd,    medic_show_2d_plot);
	itsActionMenu->SetItemImage(kDisplay2DArrayCmd, medic_show_2d_array);
	itsActionMenu->SetItemImage(kExamineMemCmd,     medic_show_memory);

	JXWDMenu* wdMenu =
		new JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != NULL );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMVarTreeDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CMVarTreeDir::UpdateWindowTitle
	(
	const JCharacter* binaryName
	)
{
	JString title = binaryName;
	title += kWindowTitleSuffix;
	(GetWindow())->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CMVarTreeDir::GetName()
	const
{
	return JGetString("WindowsMenuText::CMVarTreeDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
CMVarTreeDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetVariablesIcon();
	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMVarTreeDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionMenu();
		}
	else if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleActionMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsLink && message.Is(CMLink::kSymbolsLoaded))
		{
		const CMLink::SymbolsLoaded* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != NULL );
		UpdateWindowTitle(info->GetProgramName());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMVarTreeDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		(itsTree->GetRoot())->DeleteAllChildren();

		CMVarNode* root = itsLink->CreateVarNode();
		assert( root != NULL );
		itsTree->SetRoot(root);
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CMVarTreeDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !(GetWindow())->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CMVarTreeDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
		{
		itsCommandDir->OpenSourceFiles();
		}

	else if (index == kCloseWindowCmd)
		{
		Deactivate();
		}
	else if (index == kQuitCmd)
		{
		(JXGetApplication())->Quit();
		}
}

/******************************************************************************
 UpdateActionMenu

 ******************************************************************************/

void
CMVarTreeDir::UpdateActionMenu()
{
	itsActionMenu->EnableItem(kAddVarCmd);

	if (itsWidget->HasSelection())
		{
		itsActionMenu->EnableItem(kDelVarCmd);
		itsActionMenu->EnableItem(kDisplay1DArrayCmd);
		itsActionMenu->EnableItem(kPlot1DArrayCmd);
		itsActionMenu->EnableItem(kDisplay2DArrayCmd);
		itsActionMenu->EnableItem(kWatchVarCmd);
		itsActionMenu->EnableItem(kWatchLocCmd);
		}

	if (itsLink->GetFeature(CMLink::kExamineMemory))
		{
		itsActionMenu->EnableItem(kExamineMemCmd);
		}
	if (itsLink->GetFeature(CMLink::kDisassembleMemory))
		{
		itsActionMenu->EnableItem(kDisassembleMemCmd);
		}
}

/******************************************************************************
 HandleActionMenu

 ******************************************************************************/

void
CMVarTreeDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kAddVarCmd)
		{
		itsWidget->NewExpression();
		}
	else if (index == kDelVarCmd)
		{
		itsWidget->RemoveSelection();
		}

	else if (index == kDisplay1DArrayCmd)
		{
		itsWidget->Display1DArray();
		}
	else if (index == kPlot1DArrayCmd)
		{
		itsWidget->Plot1DArray();
		}
	else if (index == kDisplay2DArrayCmd)
		{
		itsWidget->Display2DArray();
		}

	else if (index == kWatchVarCmd)
		{
		itsWidget->WatchExpression();
		}
	else if (index == kWatchLocCmd)
		{
		itsWidget->WatchLocation();
		}

	else if (index == kExamineMemCmd)
		{
		itsWidget->ExamineMemory(CMMemoryDir::kHexByte);
		}
	else if (index == kDisassembleMemCmd)
		{
		itsWidget->ExamineMemory(CMMemoryDir::kAsm);
		}
}

/******************************************************************************
 DisplayExpression

 ******************************************************************************/

void
CMVarTreeDir::DisplayExpression
	(
	const JCharacter* expr
	)
{
	itsWidget->DisplayExpression(expr);
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
CMVarTreeDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		(CMGetApplication())->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMVarTreeHelpName);
		}
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMCreditsName);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CMVarTreeDir::ReadSetup
	(
	istream&			input,
	const JFileVersion	vers
	)
{
	itsWidget->ReadSetup(input, vers);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CMVarTreeDir::WriteSetup
	(
	ostream& output
	)
	const
{
	itsWidget->WriteSetup(output);
}