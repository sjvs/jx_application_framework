/******************************************************************************
 GFGMainDirector.h

	Copyright � 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GFGMainDirector
#define _H_GFGMainDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JPrefObject.h>

#include <JPtrArray-JString.h>

class JXInputField;
class JXPathInput;
class JXTextButton;
class JXStaticText;
class JXTextMenu;
class JXToolBar;

class GFGClass;
class GFGFunctionTable;

class GFGMainDirector : public JXWindowDirector, public JPrefObject
{
public:

public:

	GFGMainDirector(JXDirector* supervisor, const JPtrArray<JString>& files);

	virtual	~GFGMainDirector();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsHelpMenu;

	GFGClass*			itsClass;
	GFGFunctionTable*	itsTable;

// begin JXLayout

	JXInputField* itsClassInput;
	JXTextButton* itsChooseButton;
	JXTextButton* itsGenerateButton;
	JXTextButton* itsHelpButton;
	JXTextButton* itsCancelButton;
	JXPathInput*  itsDirInput;
	JXStaticText* itsBaseClassTxt;
	JXInputField* itsAuthorInput;
	JXTextButton* itsStringsButton;
	JXInputField* itsCopyrightInput;

// end JXLayout

private:

	void 		BuildWindow(const JCharacter* outputPath);
	JBoolean 	Write();

	// not allowed

	GFGMainDirector(const GFGMainDirector& source);
	const GFGMainDirector& operator=(const GFGMainDirector& source);
};

#endif