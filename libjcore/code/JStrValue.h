#ifndef _H_JStrValue
#define _H_JStrValue

/******************************************************************************
 JStrValue.h

	A convenient struct for use with the most common type of JHashTable, one
	with string keys.  JHashTable< JStrValue<V> > is a hash table with a string
	key and an arbitrary value.

	Copyright � 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <jTypes.h>

template <class V>
class JStrValue
{
public:

	const JCharacter* key;
	V                 value;

	JStrValue();
	JStrValue(const JCharacter* k, const V& v);
};

#include <JStrValue.tmpl>

#endif
