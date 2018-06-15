/******************************************************************************
 JRunArrayIterator.h

	Interface for JRunArrayIterator class template

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRunArrayIterator
#define _H_JRunArrayIterator

#include "JListIterator.h"

// must forward declare to avoid #include loop
// (We do it here so JRunArray includes JRunArrayIterator for convenience.)
template <class T> class JRunArray;

template <class T>
class JRunArrayIterator : public JListIterator<T>
{
public:

	JRunArrayIterator(const JRunArray<T>& theRunArray,
					  const JIteratorPosition start = kJIteratorStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(JRunArray<T>* theRunArray,
					  const JIteratorPosition start = kJIteratorStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(const JRunArrayIterator<T>& source);

	virtual ~JRunArrayIterator();

	virtual JBoolean	Prev(T* item);
	virtual JBoolean	Next(T* item);

	virtual void		SkipPrev(const JSize count = 1);
	virtual void		SkipNext(const JSize count = 1);

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index);

	// only allowed if constructed from non-const JList<T>*

	virtual JBoolean	SetPrev(const T& data);
	virtual JBoolean	SetNext(const T& data);

	virtual JBoolean	RemovePrev();
	virtual JBoolean	RemoveNext();

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message);

private:

	JRunArray<T>*	itsRunArray;
	JIndex			itsRunIndex;
	JIndex			itsOffsetInRun;

private:

	void	CalcPrevRunInfo(JIndex* runIndex, JIndex* firstInRun) const;
	void	CalcNextRunInfo(JIndex* runIndex, JIndex* firstInRun) const;

	// not allowed

	const JRunArrayIterator<T>& operator=(const JRunArrayIterator<T>& source);
};

#endif
