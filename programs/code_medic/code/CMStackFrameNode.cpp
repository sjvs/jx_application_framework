/******************************************************************************
 CMStackFrameNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright � 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "CMStackFrameNode.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMStackFrameNode::CMStackFrameNode
	(
	JTreeNode*			parent,
	const JUInt64		id,
	const JCharacter*	name,
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
	:
	JNamedTreeNode(NULL, name, kJFalse),
	itsID(id),
	itsFileName(fileName),
	itsLineIndex(lineIndex)
{
	assert( parent != NULL );
	parent->Append(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMStackFrameNode::~CMStackFrameNode()
{
}