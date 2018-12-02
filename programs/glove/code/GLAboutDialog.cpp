/******************************************************************************
 GLAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "GLAboutDialog.h"

#include "glove_icon.xpm"
#include "nps.xpm"

#include <JXApplication.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXDisplay.h>
#include <JXHelpManager.h>

#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

GLAboutDialog::GLAboutDialog
	(
	JXDirector*		supervisor,
	const JString&	prevVersStr
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsIsUpgradeFlag = kJFalse;

	BuildWindow(prevVersStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLAboutDialog::~GLAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLAboutDialog::BuildWindow
	(
	const JString& prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,180, JString::empty);
	assert( window != nullptr );

	JXImageWidget* gloveIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,15, 50,50);
	assert( gloveIcon != nullptr );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::GLAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
	assert( textWidget != nullptr );

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::GLAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::GLAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
	assert( itsHelpButton != nullptr );

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::GLAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
	assert( itsCreditsButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLAboutDialog"));
	SetButtons(okButton, nullptr);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	const JUtf8Byte* map1[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	JString text = JGetString("GLDescription", map1, sizeof(map1));

	if (!prevVersStr.IsEmpty())
		{
		const JUtf8Byte* map[] =
			{
			"vers", prevVersStr.GetBytes()
			};
		text += JGetString("UpgradeNotice::GLAboutDialog");
		(JGetStringManager())->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString("ChangeButtonLabel::GLAboutDialog"));
		itsIsUpgradeFlag = kJTrue;
		}
	textWidget->GetText()->SetText(text);

	JXImage* image = jnew JXImage(GetDisplay(), JXPM(glove_icon));
	assert(image != nullptr);
	gloveIcon->SetImage(image, kJTrue);

	// NPS icon

	image = jnew JXImage(GetDisplay(), nps);
	assert( image != nullptr );
	npsIcon->SetImage(image, kJTrue);

	const JSize bdh = textWidget->GetBoundsHeight();
	const JSize aph = textWidget->GetApertureHeight();
	if (bdh > aph)
		{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
		}
}

/******************************************************************************
 Receive (protected)

	Handle itsHelpButton.

 ******************************************************************************/

void
GLAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowChangeLog();
			}
		else
			{
			(JXGetHelpManager())->ShowTOC();
			}
		EndDialog(kJFalse);
		}
	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowCredits();
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
