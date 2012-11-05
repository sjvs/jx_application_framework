/******************************************************************************
 CBSymbolUpdatePG.h

	Copyright � 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSymbolUpdatePG
#define _H_CBSymbolUpdatePG

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JProgressDisplay.h>

class CBSymbolUpdatePG : public JProgressDisplay
{
public:

	CBSymbolUpdatePG(ostream& link, const JSize scaleFactor);

	virtual ~CBSymbolUpdatePG();

	virtual JBoolean	IncrementProgress(const JCharacter* message = NULL);
	virtual JBoolean	IncrementProgress(const JSize delta);
	virtual JBoolean	IncrementProgress(const JCharacter* message,
										  const JSize delta);
	virtual void		ProcessFinished();
	virtual void		DisplayBusyCursor();

	void	SetScaleFactor(const JSize scaleFactor);

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JCharacter* message, const JBoolean allowCancel,
									 const JBoolean allowBackground);

	virtual JBoolean	CheckForCancel();

private:

	ostream&	itsLink;
	JSize		itsScaleFactor;

private:

	// not allowed

	CBSymbolUpdatePG(const CBSymbolUpdatePG& source);
	const CBSymbolUpdatePG& operator=(const CBSymbolUpdatePG& source);
};


/******************************************************************************
 SetScaleFactor

 ******************************************************************************/

inline void
CBSymbolUpdatePG::SetScaleFactor
	(
	const JSize scaleFactor
	)
{
	itsScaleFactor = scaleFactor;
}

#endif