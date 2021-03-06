/******************************************************************************
 GDBAnalyzeCore.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBAnalyzeCore.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jFileUtil.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kFindProgramID = "FindProgram::GDBAnalyzeCore";

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBAnalyzeCore::GDBAnalyzeCore
	(
	const JCharacter* cmd
	)
	:
	CMCommand(cmd, kJTrue, kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBAnalyzeCore::~GDBAnalyzeCore()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex programNamePattern = "Core was generated by `([^\n]+)'";

void
GDBAnalyzeCore::HandleSuccess
	(
	const JString& data
	)
{
	CMLink* link = CMGetLink();

	JString programName;
	JArray<JIndexRange> matchList;
	if (programNamePattern.Match(data, &matchList))
		{
		programName = data.GetSubstring(matchList.GetElement(2));
		programName.TrimWhitespace();
		}
	else
		{
		link->Log("GDBAnalyzeCore failed to match");
		}

	JString coreFullName;
	if (!programName.IsEmpty() && link->GetCore(&coreFullName))
		{
		JString programFullName = programName;
		JString path, coreName;
		if (JIsRelativePath(programFullName))
			{
			JSplitPathAndName(coreFullName, &path, &coreName);
			programFullName = JCombinePathAndName(path, programFullName);
			}

		JBoolean found = kJFalse;
		while (1)
			{
			if (JFileExecutable(programFullName))
				{
				found = kJTrue;
				break;
				}

			// try stripping off last word

			JIndex spaceIndex;
			if (!programFullName.LocateLastSubstring(" ", &spaceIndex))
				{
				break;
				}
			programFullName.RemoveSubstring(spaceIndex, programFullName.GetLength());
			}

		// if all else fails, ask user

		if (!found)
			{
			const JCharacter* map[] =
				{
				"name", programName.GetCString()
				};
			const JString instr = JGetString(kFindProgramID, map, sizeof(map));
			found = (JGetChooseSaveFile())->ChooseFile("Name of program", instr,
													   &programFullName);
			}

		// must load core after program so shared libs get loaded
		// and source file gets displayed

		if (found)
			{
			link->SetProgram(programFullName);
			link->SetCore(coreFullName);
			}
		}
}
