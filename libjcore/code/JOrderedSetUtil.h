/******************************************************************************
 JOrderedSetUtil.h

	Interface for JOrderedSetUtil.cc

	Copyright (C) 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JOrderedSetUtil
#define _H_JOrderedSetUtil

#include <JOrderedSet.h>

JOrderedSetT::CompareResult JCompareIndices(const JIndex& i, const JIndex& j);
JOrderedSetT::CompareResult JCompareSizes(const JSize& s1, const JSize& s2);
JOrderedSetT::CompareResult JCompareCoordinates(const JCoordinate& x1, const JCoordinate& x2);
JOrderedSetT::CompareResult JCompareUInt64(const JUInt64& i1, const JUInt64& i2);

void		JAdjustIndexAfterInsert(const JIndex firstInsertedIndex, const JSize count,
									JIndex* indexToAdjust);
JBoolean	JAdjustIndexAfterRemove(const JIndex firstRemovedIndex, const JSize count,
									JIndex* indexToAdjust);
void		JAdjustIndexAfterMove(const JIndex origIndex, const JIndex newIndex,
								  JIndex* indexToAdjust);
void		JAdjustIndexAfterSwap(const JIndex swappedIndex1, const JIndex swappedIndex2,
								  JIndex* indexToAdjust);

#endif
