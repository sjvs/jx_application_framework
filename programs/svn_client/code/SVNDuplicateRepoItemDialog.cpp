/******************************************************************************
 SVNDuplicateRepoItemDialog.cpp

	BASE CLASS = JXGetStringDialog

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "SVNDuplicateRepoItemDialog.h"
#include "SVNRepoTreeNode.h"
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNDuplicateRepoItemDialog::SVNDuplicateRepoItemDialog
	(
	JXDirector*			supervisor,
	const JString&		windowTitle,
	const JString&		prompt,
	const JString&		initialName,
	SVNRepoTreeNode*	srcNode
	)
	:
	JXGetStringDialog(supervisor, windowTitle, prompt, initialName),
	itsSrcNode(srcNode)
{
	assert( itsSrcNode != nullptr );

	GetInputField()->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNDuplicateRepoItemDialog::~SVNDuplicateRepoItemDialog()
{
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
SVNDuplicateRepoItemDialog::OKToDeactivate()
{
	if (!JXGetStringDialog::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	const JString& name = GetString();
	JNamedTreeNode* node;
	if ((itsSrcNode->GetNamedParent())->FindNamedChild(name, &node))
		{
		JGetUserNotification()->ReportError(JGetString("NameUsed::SVNDuplicateRepoItemDialog"));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}
