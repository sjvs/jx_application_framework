/******************************************************************************
 JTextUserNotification.cpp

	Class for notifying the user of something via the console.

	Copyright (C) 1994-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JTextUserNotification.h>
#include <jCommandLine.h>
#include <jGlobals.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTextUserNotification::JTextUserNotification()
	:
	JUserNotification()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTextUserNotification::~JTextUserNotification()
{
}

/******************************************************************************
 DisplayMessage

 ******************************************************************************/

void
JTextUserNotification::DisplayMessage
	(
	const JCharacter* message
	)
{
	if (!IsSilent())
		{
		std::cout << std::endl;
		std::cout << message;
		JWaitForReturn();
		}
}

/******************************************************************************
 ReportError

 ******************************************************************************/

void
JTextUserNotification::ReportError
	(
	const JCharacter* message
	)
{
	if (!IsSilent())
		{
		std::cout << std::endl;
		std::cout << "Error: " << message;
		JWaitForReturn();
		}
}

/******************************************************************************
 AskUserYes

	Display the message (asking for a y/n answer) and wait for a response.
	If the user answers anything but no, we take it as a yes.

 ******************************************************************************/

JBoolean
JTextUserNotification::AskUserYes
	(
	const JCharacter* message
	)
{
	std::cout << std::endl;
	std::cout << message << " (y/n) ";

	char c;
	std::cin >> c;
	JInputFinished();

	return JConvertToBoolean( c != 'n' && c != 'N' );
}

/******************************************************************************
 AskUserNo

	Display the message (asking for a y/n answer) and wait for a response.
	If the user answers anything but yes, we take it as a no.

 ******************************************************************************/

JBoolean
JTextUserNotification::AskUserNo
	(
	const JCharacter* message
	)
{
	std::cout << std::endl;
	std::cout << message << " (y/n) ";

	char c;
	std::cin >> c;
	JInputFinished();

	return JConvertToBoolean( c == 'y' || c == 'Y' );
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

JUserNotification::CloseAction
JTextUserNotification::OKToClose
	(
	const JCharacter* message
	)
{
	std::cout << std::endl;
	std::cout << message << " [(S)ave, (D)on't save, (C)ancel] ";

	char c;
	std::cin >> c;
	JInputFinished();

	if (c == 's' || c == 'S')
		{
		return kSaveData;
		}
	else if (c == 'd' || c == 'D')
		{
		return kDiscardData;
		}
	else
		{
		return kDontClose;
		}
}

/******************************************************************************
 AcceptLicense

	Display the license (from "LICENSE") and wait for a response.
	If the user answers anything but yes, we take it as a no.

 ******************************************************************************/

JBoolean
JTextUserNotification::AcceptLicense()
{
	return AskUserNo(JGetString("LICENSE"));
}
