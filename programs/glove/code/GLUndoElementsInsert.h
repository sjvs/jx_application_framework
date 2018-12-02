/******************************************************************************
 GLUndoElementsInsert.h

	Interface for the GLUndoElementsInsert class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementsInsert
#define _H_GLUndoElementsInsert

#include <GLUndoElementsBase.h>
#include <JPoint.h>
#include <JPtrArray.h>

class GLRaggedFloatTable;
class GLRaggedFloatTableData;

class GLUndoElementsInsert : public GLUndoElementsBase
{
public:

	GLUndoElementsInsert(GLRaggedFloatTable* table, const JPoint& start,
						 const JPoint& end, 
						 const GLUndoElementsBase::UndoType type);

	virtual ~GLUndoElementsInsert();

	virtual void	Undo();

private:

private:

	// not allowed

	GLUndoElementsInsert(const GLUndoElementsInsert& source);
	const GLUndoElementsInsert& operator=(const GLUndoElementsInsert& source);
};

#endif
