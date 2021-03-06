/******************************************************************************
 TestTextEditor.cpp

	BASE CLASS = JXTextEditor

	Written by John Lindal.

 ******************************************************************************/

#include "TestTextEditor.h"
#include <JXTextMenu.h>
#include <JXPSPrinter.h>
#include <JString.h>
#include <jAssert.h>

// Edit menu

static const JCharacter* kEditMenuStr =
	"    Auto-indent    %b"
	"%l| Undo depth 1   %r"
	"  | Undo depth 2   %r"
	"  | Undo depth 3   %r"
	"  | Undo depth 100 %r";

static const JSize kUndoDepth[] = { 1, 2, 3, 100 };
const JSize kUndoDepthCount     = sizeof(kUndoDepth)/sizeof(JSize);

/******************************************************************************
 Constructor

 ******************************************************************************/

TestTextEditor::TestTextEditor
	(
	const JBoolean		editable,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextEditor(menuBar, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
	if (!editable)
		{
		SetType(kSelectableText);
		}

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != NULL );
	SetPSPrinter(itsPrinter);

	// adjust the edit menu

	JXTextMenu* editMenu;
	const JBoolean ok = GetEditMenu(&editMenu);
	assert( ok );
	const JSize editCount = editMenu->GetItemCount();
	editMenu->ShowSeparatorAfter(editCount);

	itsAutoIndentCmdIndex     = editCount + 1;
	itsFirstUndoDepthCmdIndex = itsAutoIndentCmdIndex + 1;
	editMenu->AppendMenuItems(kEditMenuStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestTextEditor::~TestTextEditor()
{
	jdelete itsPrinter;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
TestTextEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* editMenu;
	const JBoolean ok = GetEditMenu(&editMenu);
	assert( ok );

	if (sender == editMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCustomEditMenuItems();
		}
	else if (sender == editMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		if (HandleCustomEditMenuItems(selection->GetIndex()))
			{
			return;
			}
		}

	JXTextEditor::Receive(sender, message);
}

/******************************************************************************
 UpdateCustomEditMenuItems (private)

 ******************************************************************************/

void
TestTextEditor::UpdateCustomEditMenuItems()
{
	JXTextMenu* editMenu;
	const JBoolean ok = GetEditMenu(&editMenu);
	assert( ok );

	editMenu->EnableItem(itsAutoIndentCmdIndex);
	if (WillAutoIndent())
		{
		editMenu->CheckItem(itsAutoIndentCmdIndex);
		}

	const JSize undoDepth = GetUndoDepth();
	for (JIndex i=0; i<kUndoDepthCount; i++)
		{
		const JIndex itemIndex = itsFirstUndoDepthCmdIndex + i;
		editMenu->EnableItem(itemIndex);
		if (undoDepth == kUndoDepth[i])
			{
			editMenu->CheckItem(itemIndex);
			}
		}
}

/******************************************************************************
 HandleCustomEditMenuItems (private)

	Returns kJTrue if it is one of our items.

 ******************************************************************************/

JBoolean
TestTextEditor::HandleCustomEditMenuItems
	(
	const JIndex index
	)
{
	if (index == itsAutoIndentCmdIndex)
		{
		ShouldAutoIndent(!WillAutoIndent());
		return kJTrue;
		}
	else if (index >= itsFirstUndoDepthCmdIndex)
		{
		SetUndoDepth(kUndoDepth[ index - itsFirstUndoDepthCmdIndex ]);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
