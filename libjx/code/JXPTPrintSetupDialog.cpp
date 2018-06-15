/******************************************************************************
 JXPTPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXPTPrintSetupDialog.h"
#include "JXPSPrintSetupDialog.h"
#include "JXAdjustPrintSetupLayoutTask.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXIntegerInput.h"
#include "JXFileInput.h"
#include "JXTextCheckbox.h"
#include "JXRadioGroup.h"
#include "JXTextRadioButton.h"
#include "JXChooseSaveFile.h"
#include "jXGlobals.h"
#include <JString.h>
#include <jAssert.h>

// mappings from dialog window to JPTPrinter options

enum
{
	kPrintToPrinterID = 1,
	kPrintToFileID
};
static const JXPTPrinter::Destination kIndexToDest[] =
{
	JXPTPrinter::kPrintToPrinter,
	JXPTPrinter::kPrintToFile
};
static const JSize kDestCount = sizeof(kIndexToDest)/sizeof(JXPTPrinter::Destination);

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXPTPrintSetupDialog*
JXPTPrintSetupDialog::Create
	(
	const JXPTPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const JBoolean					printLineNumbers
	)
{
	JXPTPrintSetupDialog* dlog = jnew JXPTPrintSetupDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow(dest, printCmd, fileName, printLineNumbers);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPTPrintSetupDialog::JXPTPrintSetupDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPTPrintSetupDialog::~JXPTPrintSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::BuildWindow
	(
	const JXPTPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const JBoolean					printLineNumbers
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 370,220, JString::empty);
	assert( window != nullptr );

	itsPrintCmdLabel =
		jnew JXStaticText(JGetString("itsPrintCmdLabel::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 90,19);
	assert( itsPrintCmdLabel != nullptr );
	itsPrintCmdLabel->SetToLabel();

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,190, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXPTPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,190, 70,20);
	assert( cancelButton != nullptr );

	JXStaticText* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,30, 80,20);
	assert( destinationLabel != nullptr );
	destinationLabel->SetToLabel();

	itsDestination =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 135,20, 139,39);
	assert( itsDestination != nullptr );

	JXTextRadioButton* printerLabel =
		jnew JXTextRadioButton(1, JGetString("printerLabel::JXPTPrintSetupDialog::JXLayout"), itsDestination,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,8, 70,20);
	assert( printerLabel != nullptr );
	printerLabel->SetShortcuts(JGetString("printerLabel::JXPTPrintSetupDialog::shortcuts::JXLayout"));

	JXTextRadioButton* fileLabel =
		jnew JXTextRadioButton(2, JGetString("fileLabel::JXPTPrintSetupDialog::JXLayout"), itsDestination,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 75,8, 50,20);
	assert( fileLabel != nullptr );
	fileLabel->SetShortcuts(JGetString("fileLabel::JXPTPrintSetupDialog::shortcuts::JXLayout"));

	itsPrintCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,70, 250,20);
	assert( itsPrintCmd != nullptr );

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 80,20);
	assert( itsChooseFileButton != nullptr );
	itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::JXPTPrintSetupDialog::shortcuts::JXLayout"));

	itsCopyCount =
		jnew JXIntegerInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,110, 40,20);
	assert( itsCopyCount != nullptr );

	itsFirstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,150, 40,20);
	assert( itsFirstPageIndex != nullptr );

	itsLastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,150, 40,20);
	assert( itsLastPageIndex != nullptr );

	itsPrintAllCB =
		jnew JXTextCheckbox(JGetString("itsPrintAllCB::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,150, 110,20);
	assert( itsPrintAllCB != nullptr );
	itsPrintAllCB->SetShortcuts(JGetString("itsPrintAllCB::JXPTPrintSetupDialog::shortcuts::JXLayout"));

	itsFirstPageIndexLabel =
		jnew JXStaticText(JGetString("itsFirstPageIndexLabel::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,150, 70,20);
	assert( itsFirstPageIndexLabel != nullptr );
	itsFirstPageIndexLabel->SetToLabel();

	itsLastPageIndexLabel =
		jnew JXStaticText(JGetString("itsLastPageIndexLabel::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 270,150, 20,20);
	assert( itsLastPageIndexLabel != nullptr );
	itsLastPageIndexLabel->SetToLabel();

	JXStaticText* countLabel =
		jnew JXStaticText(JGetString("countLabel::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,110, 115,20);
	assert( countLabel != nullptr );
	countLabel->SetToLabel();

	itsPrintLineNumbersCB =
		jnew JXTextCheckbox(JGetString("itsPrintLineNumbersCB::JXPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,110, 130,20);
	assert( itsPrintLineNumbersCB != nullptr );
	itsPrintLineNumbersCB->SetShortcuts(JGetString("itsPrintLineNumbersCB::JXPTPrintSetupDialog::shortcuts::JXLayout"));

// end JXLayout

	SetObjects(okButton, cancelButton, itsDestination, dest,
			   itsPrintCmdLabel, itsPrintCmd, printCmd,
			   itsChooseFileButton, fileName, itsCopyCount,
			   itsPrintAllCB, itsFirstPageIndexLabel,
			   itsFirstPageIndex, itsLastPageIndexLabel, itsLastPageIndex,
			   itsPrintLineNumbersCB, printLineNumbers);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXPTPrintSetupDialog::SetObjects
	(
	JXTextButton*					okButton,
	JXTextButton*					cancelButton,
	JXRadioGroup*					destinationRG,
	const JXPTPrinter::Destination	dest,
	JXStaticText*					printCmdLabel,
	JXInputField*					printCmdInput,
	const JString&					printCmd,
	JXTextButton*					chooseFileButton,
	const JString&					fileName,
	JXIntegerInput*					copyCount,
	JXTextCheckbox*					printAllCB,
	JXStaticText*					firstPageIndexLabel,
	JXIntegerInput*					firstPageIndex,
	JXStaticText*					lastPageIndexLabel,
	JXIntegerInput*					lastPageIndex,
	JXTextCheckbox*					printLineNumbersCB,
	const JBoolean					printLineNumbers
	)
{
	itsPrintButton         = okButton;
	itsDestination         = destinationRG;
	itsPrintCmdLabel       = printCmdLabel;
	itsPrintCmd            = printCmdInput;
	itsChooseFileButton    = chooseFileButton;
	itsCopyCount           = copyCount;
	itsPrintAllCB          = printAllCB;
	itsFirstPageIndexLabel = firstPageIndexLabel;
	itsFirstPageIndex      = firstPageIndex;
	itsLastPageIndexLabel  = lastPageIndexLabel;
	itsLastPageIndex       = lastPageIndex;
	itsPrintLineNumbersCB  = printLineNumbersCB;

	JXWindow* window = itsDestination->GetWindow();
	window->SetTitle(JGetString("WindowTitle::JXPTPrintSetupDialog"));
	SetButtons(okButton, cancelButton);

	const JRect r1 = itsPrintCmd->GetFrame(),
				r2 = itsChooseFileButton->GetFrame();
	itsFileInput =
		jnew JXFileInput(window,
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 r1.left, r2.top, r1.width(), r2.height());
	assert( itsFileInput != nullptr );
	itsFileInput->ShouldAllowInvalidFile();
	itsFileInput->GetText()->SetText(fileName);
	ListenTo(itsFileInput->GetText());

	JXAdjustPrintSetupLayoutTask* task =
		jnew JXAdjustPrintSetupLayoutTask(this, itsPrintCmd, itsChooseFileButton, itsFileInput);
	assert( task != nullptr );
	task->Go();

	itsPrintCmd->GetText()->SetText(printCmd);
	itsPrintCmd->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	ListenTo(itsDestination);
	ListenTo(itsChooseFileButton);
	ListenTo(itsPrintAllCB);

	JBoolean foundDest = kJFalse;
	for (JIndex i=1; i<=kDestCount; i++)
		{
		if (kIndexToDest[i-1] == dest)
			{
			SetDestination(i);
			foundDest = kJTrue;
			break;
			}
		}
	assert( foundDest );

	itsCopyCount->SetValue(1);
	itsCopyCount->SetLowerLimit(1);

	PrintAllPages(kJTrue);

	itsFirstPageIndex->SetLowerLimit(1);
	itsFirstPageIndex->SetIsRequired(kJFalse);

	itsLastPageIndex->SetLowerLimit(1);
	itsLastPageIndex->SetIsRequired(kJFalse);

	itsPrintLineNumbersCB->SetState(printLineNumbers);

	itsChooseFileButton->SetShortcuts(JGetString("ChooseFileShortcut::JXPTPrintSetupDialog"));
	itsPrintAllCB->SetShortcuts(JGetString("PrintAllShortcut::JXPTPrintSetupDialog"));
	itsPrintLineNumbersCB->SetShortcuts(JGetString("PrintLineNumbersShortcut::JXPTPrintSetupDialog"));

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::UpdateDisplay()
{
	itsPrintButton->SetActive(JI2B(
		itsDestination->GetSelectedItem() == kPrintToPrinterID ||
		!itsFileInput->GetText()->IsEmpty()));
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXPTPrintSetupDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	if (itsDestination->GetSelectedItem() == kPrintToFileID)
		{
		return JXPSPrintSetupDialog::OKToDeactivate(itsFileInput->GetText()->GetText());
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXPTPrintSetupDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDestination && message.Is(JXRadioGroup::kSelectionChanged))
		{
		const JXRadioGroup::SelectionChanged* selection =
			dynamic_cast<const JXRadioGroup::SelectionChanged*>(&message);
		assert( selection != nullptr );
		SetDestination(selection->GetID());
		}

	else if (sender == itsChooseFileButton && message.Is(JXButton::kPushed))
		{
		ChooseDestinationFile();
		}
	else if (sender == itsFileInput->GetText() &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
		{
		UpdateDisplay();
		}

	else if (sender == itsPrintAllCB && message.Is(JXCheckbox::kPushed))
		{
		PrintAllPages(itsPrintAllCB->IsChecked());
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 SetDestination (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::SetDestination
	(
	const JIndex id
	)
{
	itsDestination->SelectItem(id);

	if (id == kPrintToPrinterID)
		{
		itsPrintButton->Activate();
		itsPrintCmdLabel->Show();
		itsPrintCmd->Show();
		itsPrintCmd->SetIsRequired(kJTrue);
		itsPrintCmd->Focus();
		itsChooseFileButton->Hide();
		itsFileInput->Hide();
		}
	else
		{
		assert( id == kPrintToFileID );

		itsPrintCmdLabel->Hide();
		itsPrintCmd->Hide();
		itsPrintCmd->SetIsRequired(kJFalse);
		itsChooseFileButton->Show();
		itsFileInput->Show();
		itsFileInput->Focus();

		UpdateDisplay();
		if (itsFileInput->GetText()->IsEmpty())
			{
			ChooseDestinationFile();
			}
		}
}

/******************************************************************************
 PrintAllPages (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::PrintAllPages
	(
	const JBoolean all
	)
{
	itsPrintAllCB->SetState(all);

	if (all)
		{
		if (itsFirstPageIndex->HasFocus() || itsLastPageIndex->HasFocus())
			{
			GetWindow()->KillFocus();		// values become irrelevant
			GetWindow()->SwitchFocusToFirstWidget();
			}
		itsFirstPageIndex->Hide();
		itsLastPageIndex->Hide();
		itsFirstPageIndexLabel->Hide();
		itsLastPageIndexLabel->Hide();
		}
	else
		{
		itsFirstPageIndex->Show();
		itsFirstPageIndex->Focus();
		itsLastPageIndex->Show();
		itsFirstPageIndexLabel->Show();
		itsLastPageIndexLabel->Show();
		}
}

/******************************************************************************
 ChooseDestinationFile (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::ChooseDestinationFile()
{
	if (itsFileInput->SaveFile(JGetString("SaveFilePrompt::JXPTPrintSetupDialog")))
		{
		itsPrintButton->Activate();
		}
	else if (itsFileInput->GetText()->IsEmpty())
		{
		itsPrintButton->Deactivate();
		}
}

/******************************************************************************
 SetParameters

 ******************************************************************************/

JBoolean
JXPTPrintSetupDialog::SetParameters
	(
	JXPTPrinter* p
	)
	const
{
	const JXPTPrinter::Destination newDest =
		kIndexToDest[ itsDestination->GetSelectedItem()-1 ];

	JBoolean changed = JI2B(
		newDest                            != p->GetDestination() ||
		itsPrintCmd->GetText()->GetText()  != p->GetPrintCmd()    ||
		itsFileInput->GetText()->GetText() != p->GetFileName());

	JString fullName;
	itsFileInput->GetFile(&fullName);

	p->SetDestination(newDest, itsPrintCmd->GetText()->GetText(), fullName);

	JInteger copyCount;
	const JBoolean ok = itsCopyCount->GetValue(&copyCount);
	assert( ok );
	p->SetCopyCount(copyCount);

	const JBoolean printAll = itsPrintAllCB->IsChecked();
	p->PrintAllPages();

	if (!printAll)
		{
		JInteger p1, p2;
		const JBoolean ok1 = itsFirstPageIndex->GetValue(&p1);
		const JBoolean ok2 = itsLastPageIndex->GetValue(&p2);
		if (ok1 && ok2)
			{
			p->SetFirstPageToPrint(JMin(p1, p2));
			p->SetLastPageToPrint(JMax(p1, p2));
			}
		else
			{
			if (ok1)
				{
				p->SetFirstPageToPrint(p1);
				}
			if (ok2)
				{
				p->SetLastPageToPrint(p2);
				}
			}
		}

	changed = JI2B(changed ||
		itsPrintLineNumbersCB->IsChecked() != p->WillPrintLineNumbers());

	p->ShouldPrintLineNumbers(itsPrintLineNumbersCB->IsChecked());

	return changed;
}
