/******************************************************************************
 CBEditMacroDialog.h

	Interface for the CBEditMacroDialog class

	Copyright � 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEditMacroDialog
#define _H_CBEditMacroDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JPrefObject.h>
#include "CBPrefsManager.h"		// need definition of MacroSetInfo

class JXTextButton;
class JXVertPartition;
class CBMacroSetTable;
class CBCharActionTable;
class CBMacroTable;

class CBEditMacroDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBEditMacroDialog(JArray<CBPrefsManager::MacroSetInfo>* macroList,
					  const JIndex initialSelection,
					  const JIndex firstUnusedID);

	virtual ~CBEditMacroDialog();

	JBoolean	ContentsValid() const;
	JBoolean	GetCurrentMacroSetName(JString* name) const;

	JArray<CBPrefsManager::MacroSetInfo>*
		GetMacroList(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	CBMacroSetTable*	itsMacroSetTable;
	CBCharActionTable*	itsActionTable;
	CBMacroTable*		itsMacroTable;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin actionLayout


// end actionLayout

// begin macroLayout


// end macroLayout

// begin macroSetLayout


// end macroSetLayout

private:

	void	BuildWindow(JArray<CBPrefsManager::MacroSetInfo>* macroList,
						const JIndex initialSelection,
						const JIndex firstUnusedID);

	// not allowed

	CBEditMacroDialog(const CBEditMacroDialog& source);
	const CBEditMacroDialog& operator=(const CBEditMacroDialog& source);
};

#endif