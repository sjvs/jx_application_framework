/******************************************************************************
 GNBPrefsMgr.h

	Interface for the GNBPrefsMgr class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GNBPrefsMgr
#define _H_GNBPrefsMgr

#include <JXPrefsManager.h>
#include <JPtrArray.h>
#include <JString.h>
#include <JFontStyle.h>

class JXPTPrinter;

enum
{
	kGTreeDirPrefs = 1,
	kGProgramVersionID,
	kGNotesDatabaseFileID,
	kCBDefFontID,
	kGTreeToolBarID,
	kGEditorToolBarID,
	kGTrashToolBarID,
	kPTPrinterSetupID,
	kGHelpSetupID,
	kGPTAltPrinterSetupID,
	kGProgramStateID
};

class GNBPrefsMgr : public JXPrefsManager
{
public:

	GNBPrefsMgr(JBoolean*	isNew);
	virtual ~GNBPrefsMgr();

	JString 	GetNotebookVersionStr() const;

	JBoolean	GetNotesDatabaseFile(JString* filename);
	void		SetNotesDatabaseFile(const JString& filename);

	void		GetDefaultFont(JString* name, JSize* size) const;
	void		SetDefaultFont(const JCharacter* name, const JSize size);

	void		WritePrinterSetup(JXPTPrinter* printer);
	void		ReadPrinterSetup(JXPTPrinter* printer);

	JBoolean	RestoreProgramState();
	void		SaveProgramState();
	void		ForgetProgramState();

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	Receive(JBroadcaster* sender, const Message& message);


private:

	void	EditPrefs();

	// not allowed

	GNBPrefsMgr(const GNBPrefsMgr& source);
	const GNBPrefsMgr& operator=(const GNBPrefsMgr& source);

};

#endif
