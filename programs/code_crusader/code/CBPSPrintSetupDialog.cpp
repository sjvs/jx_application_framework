/******************************************************************************
 CBPSPrintSetupDialog.cpp

	BASE CLASS = JXPSPrintSetupDialog

	Copyright � 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBPSPrintSetupDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXIntegerInput.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXFontSizeMenu.h>
#include <JString.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBPSPrintSetupDialog*
CBPSPrintSetupDialog::Create
	(
	const JXPSPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					collate,
	const JBoolean					bw,
	const JSize						fontSize,
	const JBoolean					printHeader
	)
{
	CBPSPrintSetupDialog* dlog = new CBPSPrintSetupDialog;
	assert( dlog != NULL );
	dlog->BuildWindow(dest, printCmd, fileName, collate, bw, fontSize, printHeader);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPSPrintSetupDialog::CBPSPrintSetupDialog()
	:
	JXPSPrintSetupDialog()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPSPrintSetupDialog::~CBPSPrintSetupDialog()
{
}

/******************************************************************************
 CBGetSettings

 ******************************************************************************/

void
CBPSPrintSetupDialog::CBGetSettings
	(
	JSize*		fontSize,
	JBoolean*	printHeader
	)
	const
{
	*fontSize    = itsFontSizeMenu->GetFontSize();
	*printHeader = itsPrintHeaderCB->IsChecked();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBPSPrintSetupDialog::BuildWindow
	(
	const JXPSPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					collate,
	const JBoolean					bw,
	const JSize						fontSize,
	const JBoolean					printHeader
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,300, "");
	assert( window != NULL );

	JXStaticText* printCmdLabel =
		new JXStaticText(JGetString("printCmdLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 100,19);
	assert( printCmdLabel != NULL );
	printCmdLabel->SetToLabel();

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,270, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,270, 70,20);
	assert( cancelButton != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXRadioGroup* destination =
		new JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
	assert( destination != NULL );

	JXTextRadioButton* obj2_JXLayout =
		new JXTextRadioButton(1, JGetString("obj2_JXLayout::CBPSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetShortcuts(JGetString("obj2_JXLayout::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextRadioButton* obj3_JXLayout =
		new JXTextRadioButton(2, JGetString("obj3_JXLayout::CBPSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetShortcuts(JGetString("obj3_JXLayout::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXInputField* printCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 110,70, 240,20);
	assert( printCmdInput != NULL );

	JXTextButton* chooseFileButton =
		new JXTextButton(JGetString("chooseFileButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,70, 80,20);
	assert( chooseFileButton != NULL );

	JXIntegerInput* copyCount =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 170,110, 40,20);
	assert( copyCount != NULL );

	JXTextCheckbox* bwCheckbox =
		new JXTextCheckbox(JGetString("bwCheckbox::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,190, 150,20);
	assert( bwCheckbox != NULL );

	JXIntegerInput* firstPageIndex =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,150, 40,20);
	assert( firstPageIndex != NULL );

	JXIntegerInput* lastPageIndex =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,150, 40,20);
	assert( lastPageIndex != NULL );

	JXTextCheckbox* printAllCB =
		new JXTextCheckbox(JGetString("printAllCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,150, 120,20);
	assert( printAllCB != NULL );

	JXStaticText* firstPageIndexLabel =
		new JXStaticText(JGetString("firstPageIndexLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,150, 70,20);
	assert( firstPageIndexLabel != NULL );
	firstPageIndexLabel->SetToLabel();

	JXStaticText* lastPageIndexLabel =
		new JXStaticText(JGetString("lastPageIndexLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,150, 20,20);
	assert( lastPageIndexLabel != NULL );
	lastPageIndexLabel->SetToLabel();

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 55,110, 115,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	JXTextCheckbox* collateCB =
		new JXTextCheckbox(JGetString("collateCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 240,110, 70,20);
	assert( collateCB != NULL );

	itsPrintHeaderCB =
		new JXTextCheckbox(JGetString("itsPrintHeaderCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,190, 130,20);
	assert( itsPrintHeaderCB != NULL );
	itsPrintHeaderCB->SetShortcuts(JGetString("itsPrintHeaderCB::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	itsFontSizeMenu =
		new JXFontSizeMenu(JGetMonospaceFontName(), "Font size:", window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,220, 170,30);
	assert( itsFontSizeMenu != NULL );

// end JXLayout

	SetObjects(okButton, cancelButton, destination, dest,
			   printCmdLabel, printCmdInput, printCmd,
			   chooseFileButton, fileName, copyCount,
			   collateCB, collate, bwCheckbox, bw,
			   printAllCB, firstPageIndexLabel,
			   firstPageIndex, lastPageIndexLabel, lastPageIndex);

	itsFontSizeMenu->SetFontSize(fontSize);
	itsFontSizeMenu->SetToPopupChoice();

	itsPrintHeaderCB->SetState(printHeader);
}
