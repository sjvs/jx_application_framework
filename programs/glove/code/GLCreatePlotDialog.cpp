/******************************************************************************
 GLCreatePlotDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLCreatePlotDialog.h"
#include "GLDataDocument.h"
#include "GLRaggedFloatTableData.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXStaticText.h>

#include <JPtrArray.h>
#include <JString.h>
#include <JUserNotification.h>

#include <jGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLCreatePlotDialog::GLCreatePlotDialog
	(
	GLDataDocument* supervisor, 
	GLRaggedFloatTableData* data,
	const JIndex startX, 
	const JIndex startY,
	const JIndex startXErr,
	const JIndex startYErr
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsTableDir = supervisor;
	BuildWindow(data, startX, startXErr, startY, startYErr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLCreatePlotDialog::~GLCreatePlotDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLCreatePlotDialog::BuildWindow
	(
	GLRaggedFloatTableData* data,
	const JIndex startX, 
	const JIndex startY,
	const JIndex startXErr, 
	const JIndex startYErr
	)
{
	
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 370,160, JString::empty);
	assert( window != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 280,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLCreatePlotDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 180,130, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::GLCreatePlotDialog::shortcuts::JXLayout"));

	itsLabelInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 115,10, 200,20);
	assert( itsLabelInput != nullptr );

	JXStaticText* labelLabel =
		jnew JXStaticText(JGetString("labelLabel::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 65,10, 50,20);
	assert( labelLabel != nullptr );
	labelLabel->SetToLabel();

	itsXMenu =
		jnew JXTextMenu(JGetString("itsXMenu::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 160,30);
	assert( itsXMenu != nullptr );

	itsXErrMenu =
		jnew JXTextMenu(JGetString("itsXErrMenu::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,80, 160,30);
	assert( itsXErrMenu != nullptr );

	itsYErrMenu =
		jnew JXTextMenu(JGetString("itsYErrMenu::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,80, 160,30);
	assert( itsYErrMenu != nullptr );

	itsYMenu =
		jnew JXTextMenu(JGetString("itsYMenu::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 160,30);
	assert( itsYMenu != nullptr );

	itsPlotsMenu =
		jnew JXTextMenu(JGetString("itsPlotsMenu::GLCreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,120, 130,30);
	assert( itsPlotsMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLCreatePlotDialog"));
	SetButtons(okButton, cancelButton);
	
	itsXErrMenu->AppendItem(JGetString("NoneItemLabel::GLCreatePlotDialog"));
	itsYErrMenu->AppendItem(JGetString("NoneItemLabel::GLCreatePlotDialog"));
	
	const JSize count = data->GetDataColCount();
	for (JUInt64 i=1; i<=count; i++)
		{
		JString str(i);
		str.Prepend(JGetString("ColumnTitlePrefix::GLColByRangeDialog")); 
		itsXMenu->AppendItem(str);
		itsXErrMenu->AppendItem(str);
		itsYMenu->AppendItem(str);
		itsYErrMenu->AppendItem(str);
		}

	
	itsStartX = startX;
	if (startX == 0)
		{
		itsStartX = 1;
		}
		
	itsStartXErr = startXErr + 1;
		
	itsStartY = startY;
	if (startY == 0)
		{
		itsStartY = 1;
		}
		
	itsStartYErr = startYErr + 1;
	
	JPtrArray<JString> names(JPtrArrayT::kDeleteAll);
	itsTableDir->GetPlotNames(names);
	
	itsPlotsMenu->AppendItem(JGetString("NewPlotItemLabel::GLCreatePlotDialog"));
	
	const JSize strCount = names.GetElementCount();
	
	for (JSize i = 1; i <= strCount; i++)
		{
		itsPlotsMenu->AppendItem(*(names.GetElement(i)));
		}

	itsPlotsMenu->ShowSeparatorAfter(1, kJTrue);

	itsPlotIndex = 1;
	
	itsXMenu->SetToPopupChoice(kJTrue, itsStartX);
	itsXErrMenu->SetToPopupChoice(kJTrue, itsStartXErr);
	itsYMenu->SetToPopupChoice(kJTrue, itsStartY);
	itsYErrMenu->SetToPopupChoice(kJTrue, itsStartYErr);
	itsPlotsMenu->SetToPopupChoice(kJTrue, itsPlotIndex);
	
	itsXMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsXErrMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYErrMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPlotsMenu->SetUpdateAction(JXMenu::kDisableNone);
	
	ListenTo(itsXMenu);
	ListenTo(itsXErrMenu);
	ListenTo(itsYMenu);
	ListenTo(itsYErrMenu);
	ListenTo(itsPlotsMenu);

	itsLabelInput->GetText()->SetText(JGetString("DefaultLabel::GLCreatePlotDialog"));
}

/******************************************************************************
 GetColumns 

 ******************************************************************************/

void
GLCreatePlotDialog::GetColumns
	(
	JIndex* startX, 
	JIndex* startXErr, 
	JIndex* startY,
	JIndex* startYErr
	)
{
	*startX = itsStartX;
	*startY = itsStartY;
	*startXErr = itsStartXErr - 1;
	*startYErr = itsStartYErr - 1;
}

/******************************************************************************
 GetColumns 

 ******************************************************************************/

void
GLCreatePlotDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsXMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartX = selection->GetIndex();
		}
		
	else if (sender == itsXErrMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartXErr = selection->GetIndex();
		}
		
	else if (sender == itsYMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartY = selection->GetIndex();
		}
		
	else if (sender == itsYErrMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartYErr = selection->GetIndex();
		}
		
	else if (sender == itsPlotsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsPlotIndex = selection->GetIndex();
		}
		
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetPlotIndex 

 ******************************************************************************/

JBoolean 
GLCreatePlotDialog::GetPlotIndex
	(
	JIndex* index
	)
{
	if (itsPlotIndex == 1)
		{
		return kJFalse;
		}
		
	*index = itsPlotIndex - 1;
	return kJTrue;
}

/******************************************************************************
 GetPlotIndex 

 ******************************************************************************/

const JString& 
GLCreatePlotDialog::GetLabel()
{
	return itsLabelInput->GetText()->GetText();
}	

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

JBoolean 
GLCreatePlotDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}
	if (GetLabel().IsEmpty())
		{
		JGetUserNotification()->ReportError(JGetString("SpecifyCurveLabel::GLCreatePlotDialog"));
		return kJFalse;
		}
	return kJTrue;
}