/******************************************************************************
 JX2DCursorMarkTableDir.h

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX2DCursorMarkTableDir
#define _H_JX2DCursorMarkTableDir

#include <JXWindowDirector.h>

class J2DPlotWidget;
class JXTextButton;
class JX2DCursorMarkTable;

class JX2DCursorMarkTableDir : public JXWindowDirector
{
public:

	JX2DCursorMarkTableDir(JXWindowDirector* supervisor, J2DPlotWidget* plot);

	virtual ~JX2DCursorMarkTableDir();

	JX2DCursorMarkTable*	GetTable();

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

private:

	JX2DCursorMarkTable*	itsTable;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(J2DPlotWidget* plot);

	// not allowed

	JX2DCursorMarkTableDir(const JX2DCursorMarkTableDir& source);
	const JX2DCursorMarkTableDir& operator=(const JX2DCursorMarkTableDir& source);
};

/******************************************************************************
 GetTable

 ******************************************************************************/

inline JX2DCursorMarkTable*
JX2DCursorMarkTableDir::GetTable()
{
	return itsTable;
}

#endif
