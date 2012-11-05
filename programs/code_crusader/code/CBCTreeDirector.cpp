/******************************************************************************
 CBCTreeDirector.cpp

	BASE CLASS = CBTreeDirector

	Copyright � 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBCTreeDirector.h"
#include "CBCTree.h"
#include "CBEditCPPMacroDialog.h"
#include "CBProjectDocument.h"
#include "cbHelpText.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <jAssert.h>

#include "jcc_c_tree_window.xpm"

static const JCharacter* kWindowTitleSuffix = " C++ Inheritance";

// Tree menu

static const JCharacter* kTreeMenuTitleStr = "Tree";
static const JCharacter* kTreeMenuStr =
	"    Configure C preprocessor...                            %i" kCBConfigureCTreeCPPAction
	"  | Add classes...                                         %i" kCBEditSearchPathsAction
	"  | Update                 %k Meta-U                       %i" kCBUpdateClassTreeAction
	"  | Minimize MI link lengths now                           %i" kCBMinimizeMILinkLengthAction
	"%l| Open source            %k Left-dbl-click or Return     %i" kCBOpenSelectedFilesAction
	"  | Open header            %k Middle-dbl-click or Meta-Tab %i" kCBOpenComplFilesAction
	"  | Open function list     %k Right-dbl-click              %i" kCBOpenClassFnListAction
	"%l| Create derived class...                                %i" kCBCreateDerivedClassAction
	"%l| Collapse               %k Meta-<                       %i" kCBCollapseClassesAction
	"  | Expand                 %k Meta->                       %i" kCBExpandClassesAction
	"  | Expand all                                             %i" kCBExpandAllClassesAction
	"  | Show lone classes   %b                                 %i" kCBShowLoneClassAction
	"  | Show lone structs   %b                                 %i" kCBShowLoneCStructAction
	"  | Show enums          %b                                 %i" kCBShowCEnumAction
	"%l| Select parents                                         %i" kCBSelectParentClassAction
	"  | Select descendants                                     %i" kCBSelectDescendantClassAction
	"  | Copy selected names    %k Meta-C                       %i" kCBCopyClassNameAction
	"%l| Find function...       %k Meta-F                       %i" kCBFindFunctionAction
	"  | Close function windows                                 %i" kCBCloseAllClassFnListAction;

enum
{
	kEditCPPMacrosCmd = 1, kEditSearchPathsCmd, kUpdateCurrentCmd,
	kForceMinMILinksCmd,
	kTreeOpenSourceCmd, kTreeOpenHeaderCmd, kTreeOpenFnListCmd,
	kCreateDerivedClassCmd,
	kTreeCollapseCmd, kTreeExpandCmd, kTreeExpandAllCmd,
	kTreeShowLoneClassesCmd, kTreeShowLoneStructsCmd, kTreeShowEnumsCmd,
	kTreeSelParentsCmd, kTreeSelDescendantsCmd, kCopySelNamesCmd,
	kFindFnCmd, kCloseFnWindCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCTreeDirector::CBCTreeDirector
	(
	CBProjectDocument* supervisor
	)
	:
	CBTreeDirector(supervisor, NewCTree, kWindowTitleSuffix, kCBCTreeHelpName,
				   jcc_c_tree_window, kTreeMenuTitleStr, kTreeMenuStr,
				   "CBCTreeDirector",
				   kCBCTreeToolBarID, InitCTreeToolBar)
{
	CBCTreeDirectorX();
}

CBCTreeDirector::CBCTreeDirector
	(
	istream&			projInput,
	const JFileVersion	projVers,
	istream*			setInput,
	const JFileVersion	setVers,
	istream*			symInput,
	const JFileVersion	symVers,
	CBProjectDocument*	supervisor,
	const JBoolean		subProject,
	CBDirList*			dirList
	)
	:
	CBTreeDirector(projInput, projVers, setInput, setVers, symInput, symVers,
				   supervisor, subProject, StreamInCTree,
				   kWindowTitleSuffix, kCBCTreeHelpName, jcc_c_tree_window,
				   kTreeMenuTitleStr, kTreeMenuStr, "CBCTreeDirector",
				   kCBCTreeToolBarID, InitCTreeToolBar,
				   dirList, kJTrue)
{
	CBCTreeDirectorX();
}

// private

void
CBCTreeDirector::CBCTreeDirectorX()
{
	itsCTree = dynamic_cast<CBCTree*>(GetTree());
	assert( itsCTree != NULL );

	itsEditCPPDialog = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCTreeDirector::~CBCTreeDirector()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBCTreeDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditCPPDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful() &&
			itsEditCPPDialog->UpdateMacros(itsCTree->GetCPreprocessor()))
			{
			itsCTree->NextUpdateMustReparseAll();
			(GetProjectDoc())->UpdateSymbolDatabase();
			}
		itsEditCPPDialog = NULL;
		}

	else
		{
		CBTreeDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateTreeMenu (virtual protected)

 ******************************************************************************/

void
CBCTreeDirector::UpdateTreeMenu()
{
	JXTextMenu* treeMenu = GetTreeMenu();

	treeMenu->EnableItem(kEditCPPMacrosCmd);
	treeMenu->EnableItem(kEditSearchPathsCmd);
	treeMenu->EnableItem(kUpdateCurrentCmd);

	treeMenu->EnableItem(kTreeShowLoneClassesCmd);
	treeMenu->EnableItem(kTreeShowLoneStructsCmd);
	treeMenu->EnableItem(kTreeShowEnumsCmd);

	if (!itsCTree->IsEmpty())
		{
		treeMenu->EnableItem(kFindFnCmd);
		treeMenu->EnableItem(kTreeExpandAllCmd);

		if (itsCTree->NeedsMinimizeMILinks())
			{
			treeMenu->EnableItem(kForceMinMILinksCmd);
			}
		}

	if (itsCTree->WillShowLoneClasses())
		{
		treeMenu->CheckItem(kTreeShowLoneClassesCmd);
		}
	if (itsCTree->WillShowLoneStructs())
		{
		treeMenu->CheckItem(kTreeShowLoneStructsCmd);
		}
	if (itsCTree->WillShowEnums())
		{
		treeMenu->CheckItem(kTreeShowEnumsCmd);
		}

	JBoolean hasSelection, canCollapse, canExpand;
	itsCTree->GetMenuInfo(&hasSelection, &canCollapse, &canExpand);
	if (hasSelection)
		{
		treeMenu->EnableItem(kTreeOpenSourceCmd);
		treeMenu->EnableItem(kTreeOpenHeaderCmd);
		treeMenu->EnableItem(kTreeOpenFnListCmd);
		treeMenu->EnableItem(kCreateDerivedClassCmd);
		treeMenu->EnableItem(kTreeSelParentsCmd);
		treeMenu->EnableItem(kTreeSelDescendantsCmd);
		treeMenu->EnableItem(kCopySelNamesCmd);
		}
	if (canCollapse)
		{
		treeMenu->EnableItem(kTreeCollapseCmd);
		}
	if (canExpand)
		{
		treeMenu->EnableItem(kTreeExpandCmd);
		}

	if (HasFunctionBrowsers())
		{
		treeMenu->EnableItem(kCloseFnWindCmd);
		}
}

/******************************************************************************
 HandleTreeMenu (virtual protected)

 ******************************************************************************/

void
CBCTreeDirector::HandleTreeMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(GetProjectDoc());

	if (index == kEditCPPMacrosCmd)
		{
		assert( itsEditCPPDialog == NULL );
		itsEditCPPDialog =
			new CBEditCPPMacroDialog(this, *(itsCTree->GetCPreprocessor()));
		assert( itsEditCPPDialog != NULL );
		itsEditCPPDialog->BeginDialog();
		ListenTo(itsEditCPPDialog);
		}
	else if (index == kEditSearchPathsCmd)
		{
		(GetProjectDoc())->EditSearchPaths(this);
		}
	else if (index == kUpdateCurrentCmd)
		{
		(GetProjectDoc())->UpdateSymbolDatabase();
		}
	else if (index == kForceMinMILinksCmd)
		{
		itsCTree->ForceMinimizeMILinks();
		}

	else if (index == kTreeOpenSourceCmd)
		{
		itsCTree->ViewSelectedSources();
		}
	else if (index == kTreeOpenHeaderCmd)
		{
		itsCTree->ViewSelectedHeaders();
		}
	else if (index == kTreeOpenFnListCmd)
		{
		itsCTree->ViewSelectedFunctionLists();
		}

	else if (index == kCreateDerivedClassCmd)
		{
		itsCTree->DeriveFromSelected();
		}

	else if (index == kTreeCollapseCmd)
		{
		itsCTree->CollapseExpandSelectedClasses(kJTrue);
		}
	else if (index == kTreeExpandCmd)
		{
		itsCTree->CollapseExpandSelectedClasses(kJFalse);
		}
	else if (index == kTreeExpandAllCmd)
		{
		itsCTree->ExpandAllClasses();
		}
	else if (index == kTreeShowLoneClassesCmd)
		{
		itsCTree->ToggleShowLoneClasses();
		}
	else if (index == kTreeShowLoneStructsCmd)
		{
		itsCTree->ToggleShowLoneStructs();
		}
	else if (index == kTreeShowEnumsCmd)
		{
		itsCTree->ToggleShowEnums();
		}

	else if (index == kTreeSelParentsCmd)
		{
		itsCTree->SelectParents();
		}
	else if (index == kTreeSelDescendantsCmd)
		{
		itsCTree->SelectDescendants();
		}
	else if (index == kCopySelNamesCmd)
		{
		itsCTree->CopySelectedClassNames();
		}

	else if (index == kFindFnCmd)
		{
		AskForFunctionToFind();
		}
	else if (index == kCloseFnWindCmd)
		{
		CloseFunctionBrowsers();
		}
}

/******************************************************************************
 NewCTree (static private)

 ******************************************************************************/

CBTree*
CBCTreeDirector::NewCTree
	(
	CBTreeDirector*	director,
	const JSize		marginWidth
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	CBCTreeDirector* cTreeDir = static_cast<CBCTreeDirector*>(director);
	assert( cTreeDir != NULL );

	CBCTree* tree = new CBCTree(cTreeDir, marginWidth);
	assert( tree != NULL );
	return tree;
}

/******************************************************************************
 StreamInCTree (static private)

 ******************************************************************************/

CBTree*
CBCTreeDirector::StreamInCTree
	(
	istream&			projInput,
	const JFileVersion	projVers,
	istream*			setInput,
	const JFileVersion	setVers,
	istream*			symInput,
	const JFileVersion	symVers,
	CBTreeDirector*		director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
{
	// dynamic_cast<> doesn't work because object is not fully constructed

	CBCTreeDirector* cTreeDir = static_cast<CBCTreeDirector*>(director);
	assert( cTreeDir != NULL );

	CBCTree* tree = new CBCTree(projInput, projVers,
								setInput, setVers, symInput, symVers,
								cTreeDir, marginWidth, dirList);
	assert( tree != NULL );
	return tree;
}

/******************************************************************************
 InitCTreeToolBar (static private)

 ******************************************************************************/

void
CBCTreeDirector::InitCTreeToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu*	treeMenu
	)
{
	toolBar->AppendButton(treeMenu, kEditSearchPathsCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(treeMenu, kTreeCollapseCmd);
	toolBar->AppendButton(treeMenu, kTreeExpandCmd);
}