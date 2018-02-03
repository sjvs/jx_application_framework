/******************************************************************************
 JUtf8ByteRange.h

	Light-weight class for type safety.

	Copyright (C) 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JUtf8ByteRange
#define _H_JUtf8ByteRange

#include <JIndexRange.h>

class JUtf8ByteRange : public JIndexRange
{
public:

	JUtf8ByteRange()
		:
		JIndexRange()
	{ };

	JUtf8ByteRange
		(
		const JIndex firstIndex,
		const JIndex lastIndex
		)
		:
		JIndexRange(firstIndex, lastIndex)
	{ };
};

#endif