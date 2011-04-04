/******************************************************************************
 JXToolBarEditDir.cpp

	BASE CLASS = public JXDocument

	Copyright � 1998 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JXStdInc.h>
#include <JXToolBarEditDir.h>
#include <JTree.h>
#include <JXToolBarNode.h>
#include <JXToolBarEditWidget.h>
#include <JNamedTreeList.h>

#include <JXToolBar.h>

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXApplication.h>
#include <JXTextButton.h>
#include <JXDocumentManager.h>
#include <JXDocumentMenu.h>
#include <JXImage.h>
#include <JXTextCheckbox.h>
#include <JXAtLeastOneCBGroup.h>

#include <JChooseSaveFile.h>
#include <JFileArray.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kCurrentPrefsVersion	= 1;
const JIndex kTypeCount					= 3;

/******************************************************************************
 Constructor

 *****************************************************************************/

JXToolBarEditDir::JXToolBarEditDir
	(
	JTree*						tree,
	const JBoolean				show,
	const JBoolean				useSmall,
	const JXToolBarButton::Type	type,
	JXDirector*				supervisor
	)
	:
   JXDialogDirector(supervisor, kJTrue),
   itsTree(tree),
   itsTreeChanged(kJFalse)
{
	UseModalPlacement(kJFalse);

	BuildWindow();
	ListenTo(itsTree);

	itsShowToolBarCB->SetState(show);
	itsUseSmallButtonsCB->SetState(useSmall);
	if ((type == JXToolBarButton::kImage) || (type == JXToolBarButton::kBoth))
		{
		itsShowImagesCB->SetState(kJTrue);
		}
	if ((type == JXToolBarButton::kText) || (type == JXToolBarButton::kBoth))
		{
		itsShowTextCB->SetState(kJTrue);
		}

}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXToolBarEditDir::~JXToolBarEditDir()
{
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
JXToolBarEditDir::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 320,430, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,100, 290,290);
	assert( scrollbarSet != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::JXToolBarEditDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,400, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::JXToolBarEditDir::shortcuts::JXLayout"));

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::JXToolBarEditDir::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 200,400, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXToolBarEditDir::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::JXToolBarEditDir::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,10, 270,40);
	assert( obj1_JXLayout != NULL );

	itsShowToolBarCB =
		new JXTextCheckbox(JGetString("itsShowToolBarCB::JXToolBarEditDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 135,20);
	assert( itsShowToolBarCB != NULL );
	itsShowToolBarCB->SetShortcuts(JGetString("itsShowToolBarCB::JXToolBarEditDir::shortcuts::JXLayout"));

	itsUseSmallButtonsCB =
		new JXTextCheckbox(JGetString("itsUseSmallButtonsCB::JXToolBarEditDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 135,20);
	assert( itsUseSmallButtonsCB != NULL );
	itsUseSmallButtonsCB->SetShortcuts(JGetString("itsUseSmallButtonsCB::JXToolBarEditDir::shortcuts::JXLayout"));

	itsShowImagesCB =
		new JXTextCheckbox(JGetString("itsShowImagesCB::JXToolBarEditDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,50, 140,20);
	assert( itsShowImagesCB != NULL );
	itsShowImagesCB->SetShortcuts(JGetString("itsShowImagesCB::JXToolBarEditDir::shortcuts::JXLayout"));

	itsShowTextCB =
		new JXTextCheckbox(JGetString("itsShowTextCB::JXToolBarEditDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,70, 130,20);
	assert( itsShowTextCB != NULL );
	itsShowTextCB->SetShortcuts(JGetString("itsShowTextCB::JXToolBarEditDir::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Configure Toolbar");
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	SetButtons(okButton, cancelButton);

	JXAtLeastOneCBGroup* group =
		new JXAtLeastOneCBGroup(2, itsShowImagesCB, itsShowTextCB);
	assert( group != NULL );

    JNamedTreeList* list = new JNamedTreeList(itsTree);
    assert(list != NULL);

    itsWidget =
    	new JXToolBarEditWidget(list, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
								JXWidget::kHElastic, JXWidget::kVElastic,
								0,0,10,10);
    assert(itsWidget != NULL);
    itsWidget->FitToEnclosure();
    ListenTo(itsWidget);
}

/******************************************************************************
 ReadPrefs

	This uses assert() to check the version number because we cannot use
	a marker to skip the data (since it contains strings), so it is the
	client's responsibility to check whether or not the file can be
	read (via a higher level version number).

 ******************************************************************************/

void
JXToolBarEditDir::ReadSetup
	(
	istream& is
	)
{
	JCoordinate version;
	is >> version;
	assert( version <= kCurrentPrefsVersion );
	GetWindow()->ReadGeometry(is);
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
JXToolBarEditDir::WriteSetup
	(
	ostream& os
	)
{
	os << ' ' << kCurrentPrefsVersion << ' ';
	GetWindow()->WriteGeometry(os);
	os << ' ';
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXToolBarEditDir::Receive
	(
	JBroadcaster*  sender,
	const Message& message
	)
{
	if (sender == itsTree && message.Is(JTree::kNodeChanged))
		{
		itsTreeChanged = kJTrue;
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ShowToolBar (public)

 ******************************************************************************/

JBoolean
JXToolBarEditDir::ShowToolBar()
{
	return itsShowToolBarCB->IsChecked();
}

/******************************************************************************
 UseSmallButtons (public)

 ******************************************************************************/

JBoolean
JXToolBarEditDir::UseSmallButtons()
{
	return itsUseSmallButtonsCB->IsChecked();
}

/******************************************************************************
 GetType (public)

 ******************************************************************************/

JXToolBarButton::Type
JXToolBarEditDir::GetType()
{
	JBoolean images = itsShowImagesCB->IsChecked();
	JBoolean text	= itsShowTextCB->IsChecked();

	if (images && !text)
		{
		return JXToolBarButton::kImage;
		}
	else if (text && !images)
		{
		return JXToolBarButton::kText;
		}
	return JXToolBarButton::kBoth;
}
