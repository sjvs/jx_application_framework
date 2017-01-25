/******************************************************************************
 GMFilterManager.cc

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMFilterManager.h>
#include <GMFilterDialog.h>
#include <GMFilter.h>
#include <GMFilterAction.h>
#include <GMFilterCondition.h>
#include <GPrefsMgr.h>

#include <gMailUtils.h>

#include <GMGlobals.h>

#include <JUserNotification.h>

#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jProcessUtil.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsVersion	= 0;
const JCharacter* kArrowFilterCookie	= "# Arrow generated procmail filter file";
const JCharacter* kArrowFilterMessage	=
	"# This file was generated by Arrow. Any changes\n"
	"# made to this file will be overwritten if Arrow\n"
	"# ever re-writes it.\n";

const JCharacter* kTypeStrings[] =
	{"^From:", "^To:", "^TO", "^Subject:", "^[Cc][Cc]:",
	 "^ReplyTo:", "^[^:]+:", "B ?? "};

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterManager::GMFilterManager()
	:
	JPrefObject(GGetPrefsMgr(), kGFilterMgrID),
	itsDialog(NULL)
{
	itsFilters	= new JPtrArray<GMFilter>(JPtrArrayT::kForgetAll);
	assert(itsFilters != NULL);

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterManager::~GMFilterManager()
{
	JPrefObject::WritePrefs();
	itsFilters->DeleteAll();
	delete itsFilters;
}

/******************************************************************************
 EditFilters (public)

 ******************************************************************************/

void
GMFilterManager::EditFilters()
{
	if (!OKToOverwrite())
		{
		return;
		}
	if (itsDialog == NULL)
		{
		itsDialog = new GMFilterDialog(*itsFilters);
		assert(itsDialog != NULL);
		ListenTo(itsDialog);
		itsDialog->BeginDialog();
		}
	else
		{
		itsDialog->Activate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMFilterManager::Receive
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
			itsFilters->DeleteAll();
			JPtrArray<GMFilter>* filters	= itsDialog->GetFilters();
			const JSize count = filters->GetElementCount();
			for (JIndex i = 1; i <= count; i++)
				{
				itsFilters->Append(filters->GetElement(i));
				}
			filters->RemoveAll();
			WriteFilterFile();
			}
		itsDialog = NULL;
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
GMFilterManager::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version > kCurrentPrefsVersion)
		{
		return;
		}

	JSize count;
	input >> count;
	for (JIndex i = 1; i <= count; i++)
		{
		GMFilter* filter	= new GMFilter();
		assert(filter != NULL);
		filter->ReadSetup(input);
		itsFilters->Append(filter);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
GMFilterManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion << ' ';
	const JSize count	= itsFilters->GetElementCount();
	output << count << ' ';
	for (JIndex i = 1; i <= count; i++)
		{
		GMFilter* filter = itsFilters->GetElement(i);
		filter->WriteSetup(output);
		}
}

/******************************************************************************
 OKToOverwrite (private)

 ******************************************************************************/

JBoolean
GMFilterManager::OKToOverwrite()
{
	JString file;
	if (!GGetAbsoluteFromHome(GGetPrefsMgr()->GetFilterFile(), &file))
		{
		return kJFalse;
		}

	if (JFileExists(file))
		{
		std::ifstream is(file);
		JString line	= JReadLine(is);
		is.close();
		if (!line.BeginsWith(kArrowFilterCookie))
			{
			JString report =
				"You have a filter file that was not "
				"generated by Arrow. You can change the file "
				"that Arrow writes the filters to by editing the "
				"filter file in the Misc Preferences dialog, or "
				"you can overwrite your current filter file.\n\n"
				"Would you like to overwrite this file?";
			if (JGetUserNotification()->AskUserYes(report))
				{
				JString savefile	= file + ".orig";
				JString cmd			= "cp \"" + file + "\" \"" + savefile + "\"";
				JString errors;
				JError err	= JRunProgram(cmd, &errors);
				if (err.OK())
					{
					JString message	= "Your file " + file + " has been saved to " + savefile + ".";
					JGetUserNotification()->DisplayMessage(message);
					return kJTrue;
					}
				}
			else
				{
				return kJFalse;
				}
			}
		}
	return kJTrue;
}

/******************************************************************************
 WriteFilterFile (private)

 ******************************************************************************/

void
GMFilterManager::WriteFilterFile()
{
	JString file;
	if (!GGetAbsoluteFromHome(GGetPrefsMgr()->GetFilterFile(), &file))
		{
		return;
		}
	std::ofstream os(file);
	if (!os.good())
		{
		JGetUserNotification()->ReportError("There was an error saving your filter file.");
		return;
		}
	os << kArrowFilterCookie << std::endl;
	os << kArrowFilterMessage << std::endl;

	const JSize count	= itsFilters->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMFilter* filter	= itsFilters->GetElement(i);
		WriteFilter(os, filter);
		}
}

/******************************************************************************
 WriteFilter (private)

 ******************************************************************************/

void
GMFilterManager::WriteFilter
	(
	std::ostream&	os,
	GMFilter*	filter
	)
{
	JPtrArray<GMFilterAction>* actions	= filter->GetActions();
	const JSize actionCount	= actions->GetElementCount();
	if (actionCount == 0)
		{
		return;
		}

	os << ":0";

	GMFilterAction* action	= actions->GetElement(1);
	if (action->IsCopying())
		{
		os << " c";
		}

	os << std::endl;

	JPtrArray<GMFilterCondition>* conditions	= filter->GetConditions();
	const JSize condCount	= conditions->GetElementCount();
	for (JIndex i = 1; i <= condCount; i++)
		{
		GMFilterCondition* condition	= conditions->GetElement(i);
		WriteCondition(os, condition);
		}

	if (!action->IsMailbox())
		{
		os << "! ";
		}

	action->GetDestination().Print(os);
	os << std::endl;

	for (JIndex i = 2; i <= actionCount; i++)
		{
		os << std::endl;
		action	= actions->GetElement(i);
		os << ":0 A";
		if (i != actionCount)
			{
			os << "c";
			}
		os << std::endl;
		if (!action->IsMailbox())
			{
			os << "! ";
			}
		action->GetDestination().Print(os);
		os << std::endl;
		}
	os << std::endl;
}

/******************************************************************************
 WriteCondition (private)

 ******************************************************************************/

void
GMFilterManager::WriteCondition
	(
	std::ostream&			os,
	GMFilterCondition*	condition
	)
{
	os << "* ";
	GMFilterCondition::ConditionType type		= condition->GetType();
	GMFilterCondition::ConditionRelation rel	= condition->GetRelation();

	os << kTypeStrings[type - 1];

	if (rel  == GMFilterCondition::kContains &&
		type != GMFilterCondition::kBody)
		{
		os << ".*";
		}
	else if (rel == GMFilterCondition::kIs ||
			 rel == GMFilterCondition::kBeginsWith)
		{
		if (type == GMFilterCondition::kBody)
			{
			os << "^^";
			}
		else
			{
			os << ".";
			}
		}
	else if (rel  == GMFilterCondition::kEndsWith &&
			 type != GMFilterCondition::kBody)
		{
		os << ".*";
		}

	condition->GetPattern().Print(os);

	if (rel == GMFilterCondition::kIs ||
		rel == GMFilterCondition::kEndsWith)
		{
		if (type == GMFilterCondition::kBody)
			{
			os << "$$";
			}
		else
			{
			os << "$";
			}
		}
	os << std::endl;
}
