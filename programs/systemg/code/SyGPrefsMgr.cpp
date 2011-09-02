/******************************************************************************
 SyGPrefsMgr.cpp

	BASE CLASS = public JXPrefsManager

	Copyright � 1998 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <SyGStdInc.h>
#include "SyGPrefsMgr.h"
#include "SyGEditPrefsDialog.h"
#include "SyGViewManPageDialog.h"
#include "SyGFileVersions.h"
#include "SyGGlobals.h"
#include <JXChooseSaveFile.h>
#include <jDirUtil.h>
#include <jAssert.h>

// JBroadcaster messages

const JCharacter* SyGPrefsMgr::kPrefsChanged = "PrefsChanged::SyGPrefsMgr";

/******************************************************************************
 Constructor

 *****************************************************************************/

SyGPrefsMgr::SyGPrefsMgr
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	itsDialog = NULL;
	*isNew    = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kSgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

SyGPrefsMgr::~SyGPrefsMgr()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
SyGPrefsMgr::SaveAllBeforeDestruct()
{
	SetData(kSProgramVersionID, SyGGetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 GetSystemGVersionStr

 ******************************************************************************/

JString
SyGPrefsMgr::GetSystemGVersionStr()
	const
{
	std::string data;
	if (GetData(kSProgramVersionID, &data))
		{
		return JString(data);
		}
	else
		{
		return JString("< 0.4.0");		// didn't exist before this version
		}
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
SyGPrefsMgr::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		const JBoolean prefs[kSyGTreePrefCount] =
			{kJFalse,kJFalse,kJFalse,kJFalse,kJFalse,kJFalse,kJFalse};
		SetTreePreferences(prefs);
		SaveFilterStatePref(kJFalse);
		}

	if (currentVersion < 5)
		{
		std::string data;
		if (GetData(3, &data))
			{
			std::ostringstream newData;
			newData << 0 << ' ' << data.c_str();
			SetData(3, newData);
			}
		}

	if (currentVersion < 6)
		{
		RemoveData(13);
		RemoveData(14);
		RemoveData(15);
		RemoveData(20);

		DelShouldDelete(kJFalse);
		}

	if (currentVersion < 7)
		{
		RemoveData(6);
		RemoveData(8);
		RemoveData(9);
		RemoveData(16);
		RemoveData(17);
		}

	if (currentVersion < 8)
		{
		JString cmd;
		std::string data;
		if (GetData(11, &data))
			{
			std::istringstream input(data);
			input >> cmd;
			(SyGGetApplication())->SetTerminalCommand(cmd);
			RemoveData(11);
			}
		if (GetData(12, &data))
			{
			std::istringstream input(data);
			input >> cmd;
			SyGViewManPageDialog::SetViewManPageCommand(cmd);
			RemoveData(12);
			}
		}

	if (currentVersion < 10)
		{
		ShouldOpenNewWindows(kJTrue);
		}

	if (currentVersion < 11)
		{
		ShouldSaveFolderPrefs(kJTrue);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGPrefsMgr::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			UpdatePrefs();
			}
		itsDialog = NULL;
		}
	else
		{
		JXPrefsManager::Receive(sender, message);
		}
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
SyGPrefsMgr::EditPrefs()
{
	assert( itsDialog == NULL );

	const JString manCmd        = (SyGGetManPageDialog())->GetViewManPageCommand();
	const JString termCmd       = (SyGGetApplication())->GetTerminalCommand();
	const JString gitStatusCmd  = (SyGGetApplication())->GetGitStatusCommand();
	const JString gitHistoryCmd = (SyGGetApplication())->GetGitHistoryCommand();
	const JString coCmd         = (SyGGetApplication())->GetPostCheckoutCommand();

	itsDialog = new SyGEditPrefsDialog(termCmd, manCmd, gitStatusCmd, gitHistoryCmd,
									   coCmd, DelWillDelete(), WillOpenNewWindows(),
									   WillSaveFolderPrefs());
	assert(itsDialog != NULL);

	itsDialog->BeginDialog();
	ListenTo(itsDialog);
}

/******************************************************************************
 UpdatePrefs

 ******************************************************************************/

void
SyGPrefsMgr::UpdatePrefs()
{
	assert( itsDialog != NULL );

	JString manCmd, termCmd, gitStatusCmd, gitHistoryCmd, postCheckoutCmd;
	JBoolean del, newWindows, perFolderPrefs;
	itsDialog->GetPrefs(&termCmd, &manCmd, &gitStatusCmd, &gitHistoryCmd,
						&postCheckoutCmd, &del, &newWindows, &perFolderPrefs);

	(SyGGetApplication())->SetTerminalCommand(termCmd);
	(SyGGetApplication())->SetGitStatusCommand(gitStatusCmd);
	(SyGGetApplication())->SetGitHistoryCommand(gitHistoryCmd);
	(SyGGetApplication())->SetPostCheckoutCommand(postCheckoutCmd);
	(SyGGetManPageDialog())->SetViewManPageCommand(manCmd);
	DelShouldDelete(del);
	ShouldOpenNewWindows(newWindows);
	ShouldSaveFolderPrefs(perFolderPrefs);

	Broadcast(PrefsChanged());
}

/******************************************************************************
 TreePreferences

 ******************************************************************************/

void
SyGPrefsMgr::SetTreePreferences
	(
	const JBoolean prefs[]
	)
{
	std::ostringstream data;
	for (JIndex i = 0; i < kSyGTreePrefCount; i++)
		{
		data << prefs[i];
		}

	SetData(kSTreeOptionsID, data);
}

void
SyGPrefsMgr::GetTreePreferences
	(
	JBoolean prefs[]
	)
	const
{
	std::string data;
	const JBoolean ok = GetData(kSTreeOptionsID, &data);
	assert( ok );

	std::istringstream dataStream(data);
	for (JIndex i = 0; i < kSyGTreePrefCount; i++)
		{
		dataStream >> prefs[i];
		}
}

/******************************************************************************
 DefaultWindowSize

 ******************************************************************************/

JBoolean
SyGPrefsMgr::GetDefaultWindowSize
	(
	JSize* w,
	JSize* h
	)
	const
{
	std::string data;
	if (GetData(kSDefaultWindowSizeID, &data))
		{
		std::istringstream dataStream(data);
		dataStream >> *w;
		dataStream >> *h;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

void
SyGPrefsMgr::SaveDefaultWindowSize
	(
	const JSize w,
	const JSize h
	)
{
	std::ostringstream data;
	data << w << ' ' << h;
	SetData(kSDefaultWindowSizeID, data);
}

/******************************************************************************
 FilterStatePref

 ******************************************************************************/

void
SyGPrefsMgr::SaveFilterStatePref
	(
	const JBoolean show
	)
{
	std::ostringstream data;
	data << show << ' ';
	SetData(kSFilterVisibleID, data);
}

JBoolean
SyGPrefsMgr::GetFilterStatePref()
	const
{
	JBoolean show = kJFalse;
	if (IDValid(kSFilterVisibleID))
		{
		std::string data;
		GetData(kSFilterVisibleID, &data);
		std::istringstream dataStream(data);
		dataStream >> show;
		}
	return show;
}

/******************************************************************************
 Program State

 ******************************************************************************/

JBoolean
SyGPrefsMgr::RestoreProgramState
	(
	JPtrArray<JString>* children
	)
{
	if (IDValid(kSChildWindowListID))
		{
		std::string data;
		GetData(kSChildWindowListID, &data);
		std::istringstream dataStream(data);
		dataStream >> *children;

		RemoveData(kSChildWindowListID);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

void
SyGPrefsMgr::SaveProgramState
	(
	const JPtrArray<JString>& children
	)
{
	std::ostringstream data;
	data << children;
	SetData(kSChildWindowListID, data);
}

/******************************************************************************
 Delete key should delete files

 ******************************************************************************/

JBoolean
SyGPrefsMgr::DelWillDelete()
	const
{
	std::string data;
	GetData(kSDelDeleteID, &data);
	std::istringstream dataStream(data);
	JBoolean del;
	dataStream >> del;
	return del;
}

void
SyGPrefsMgr::DelShouldDelete
	(
	const JBoolean del
	)
{
	std::ostringstream data;
	data << del;
	SetData(kSDelDeleteID, data);
}

/******************************************************************************
 Open new windows

 ******************************************************************************/

JBoolean
SyGPrefsMgr::WillOpenNewWindows()
	const
{
	std::string data;
	GetData(kSNewWindowsID, &data);
	std::istringstream dataStream(data);
	JBoolean newWindows;
	dataStream >> newWindows;
	return newWindows;
}

void
SyGPrefsMgr::ShouldOpenNewWindows
	(
	const JBoolean newWindows
	)
{
	std::ostringstream data;
	data << newWindows;
	SetData(kSNewWindowsID, data);
}

/******************************************************************************
 Per folder prefs

 ******************************************************************************/

JBoolean
SyGPrefsMgr::WillSaveFolderPrefs()
	const
{
	std::string data;
	GetData(kSPerFolderPrefsID, &data);
	std::istringstream dataStream(data);
	JBoolean perFolder;
	dataStream >> perFolder;
	return perFolder;
}

void
SyGPrefsMgr::ShouldSaveFolderPrefs
	(
	const JBoolean perFolder
	)
{
	std::ostringstream data;
	data << perFolder;
	SetData(kSPerFolderPrefsID, data);
}
