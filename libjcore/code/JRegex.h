/******************************************************************************
 JRegex.h

	Interface for the JRegex class.

	Copyright � 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JRegex
#define _H_JRegex

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JString.h"
#include "JArray.h"
#include "JError.h"
#include "regex.h"

class JInterpolate;

//#define JRE_ALLOC_CHECK
//#define JRE_PRINT_COMPILE_ERRORS

class JRegex
{
public:

	JRegex();
	JRegex(const JCharacter* pattern);
	JRegex(const JCharacter* pattern, const JSize length);

	virtual ~JRegex();

	JRegex(const JRegex& source);
	const JRegex& operator=(const JRegex& source);

// Setting and examining patterns

	JError SetPattern(const JString& pattern);
	JError SetPattern(const JCharacter* pattern, const JSize length);
	JError SetPattern(const JCharacter* pattern);

	void SetPatternOrDie(const JCharacter* pattern);
	void SetPatternOrDie(const JCharacter* pattern, const JSize length);
	void SetPatternOrDie(const JString& pattern);

	void           GetPattern(const JCharacter** pattern, JSize* length) const;
	const JString& GetPattern() const;

	// JAFL 5/11/98
	static JBoolean	NeedsBackslashToBeLiteral(const JCharacter c);
	// JAFL 2/18/06
	static JString	BackslashForLiteral(const JCharacter* text);

// Pattern-related settings and statistics

	JBoolean ContainsNULL() const;
	JSize    NULLCount() const;

	JSize    	GetSubCount() const;
	JBoolean	GetSubexpressionIndex(const JCharacter* name, JIndex* index) const;
	JBoolean	GetSubexpression(const JCharacter* str, const JCharacter* name,
								 const JArray<JIndexRange>& matchList, JString* s) const;

/******************************************************************************
 Match and friends

	possible future interface extensions included in comments

 *****************************************************************************/

// These versions simply determine whether there is a match

	// Return one match, yes/no
	JBoolean Match(const JCharacter* str) const;
	JBoolean MatchFrom(const JCharacter* str, const JIndex index) const;
	JBoolean MatchAfter(const JCharacter* str, const JIndexRange& range) const;
	JBoolean MatchWithin(const JCharacter* str, const JIndexRange& range) const;

	// Count all matches
	JSize    MatchAll(const JCharacter* str) const;

// These versions return the overall match ranges

	// First match
	JBoolean Match(const JCharacter* str, JIndexRange* match) const;
	JBoolean MatchFrom(const JCharacter* str, const JIndex index, JIndexRange* match) const;
	JBoolean MatchAfter(const JCharacter* str, const JIndexRange& range, JIndexRange* match) const;
	JBoolean MatchWithin(const JCharacter* str, const JIndexRange& range, JIndexRange* match) const;

	// Last match
	JSize    MatchLast(const JCharacter* str, JIndexRange* match) const;
//	JSize    MatchLastFrom(const JCharacter* str, const JIndex index, JIndexRange* match) const;
//	JSize    MatchLastAfter(const JCharacter* str, const JIndexRange& range, JIndexRange* match) const;
	JSize    MatchLastWithin(const JCharacter* str, const JIndexRange& range, JIndexRange* match) const;

	// All Matches
	JSize    MatchAll(const JCharacter* str, JArray<JIndexRange>* matchList) const;
//	JSize    MatchAllFrom(const JCharacter* str, const JIndex index, JArray<JIndexRange>* matchList) const;
//	JSize    MatchAllAfter(const JCharacter* str, const JIndexRange& range, JArray<JIndexRange>* matchList) const;
	JSize    MatchAllWithin(const JCharacter* str, const JIndexRange& range, JArray<JIndexRange>* matchList) const;

// These versions return a list of subexpression matches from the first match

	// First match
	JBoolean Match(const JCharacter* str, JArray<JIndexRange>* subMatchList) const;
	JBoolean MatchFrom(const JCharacter* str, const JIndex index, JArray<JIndexRange>* subMatchList) const;
	JBoolean MatchAfter(const JCharacter* str, const JIndexRange& range, JArray<JIndexRange>* subMatchList) const;
	JBoolean MatchWithin(const JCharacter* str, const JIndexRange& range, JArray<JIndexRange>* subMatchList) const;

	// Last match
//	JSize    MatchLast(const JCharacter* str, JArray<JIndexRange>* subMatchList) const;
//	JSize    MatchLastFrom(const JCharacter* str, const JIndex index, JArray<JIndexRange>* subMatchList) const;
//	JSize    MatchLastAfter(const JCharacter* str, const JIndexRange& range, JArray<JIndexRange>* subMatchList) const;
	JSize    MatchLastWithin(const JCharacter* str, const JIndexRange& range, JArray<JIndexRange>* subMatchList) const;

// Search backwards

	JBoolean MatchBackward(const JCharacter* str, const JIndex index, JIndexRange* match) const;
	JSize    MatchBackward(const JCharacter* str, const JIndex index, JArray<JIndexRange>* matchList) const;

// Split--a sort of inverse Match that can be very useful.

//	JSize Split(const JCharacter* str, JArray<JIndexRange>* splitList) const;

/******************************************************************************
 Replace and friends

 *****************************************************************************/

	JString InterpolateMatches(const JCharacter* sourceString, const JArray<JIndexRange>& matchList) const;

	void    Replace(JString* str, const JArray<JIndexRange>& matchList,
					JIndexRange* newRange) const;
	void    Replace(JString* str, const JIndexRange& oldRange,
					JIndexRange* newRange) const;

// Setting and testing options

	void     SetCaseSensitive(const JBoolean yesNo = kJTrue);
	JBoolean IsCaseSensitive() const;
	void     SetSingleLine(const JBoolean yesNo = kJTrue);
	JBoolean IsSingleLine() const;
	void     SetLineBegin(const JBoolean yesNo = kJTrue);
	JBoolean IsLineBegin() const;
	void     SetLineEnd(const JBoolean yesNo = kJTrue);
	JBoolean IsLineEnd() const;
	void     SetLiteralReplace(const JBoolean yesNo = kJTrue);
	JBoolean IsLiteralReplace() const;
	void     SetMatchCase(const JBoolean yesNo = kJTrue);
	JBoolean IsMatchCase() const;

	JError SetReplacePattern(const JCharacter* pattern, JIndexRange* errRange = NULL);
	JError SetReplacePattern(const JString& pattern, JIndexRange* errRange = NULL);

	JError SetReplacePattern(const JCharacter* pattern, const JSize length,
							 JIndexRange* errRange = NULL);

	const JString& GetReplacePattern() const;

	JError RestoreDefaults();

	// Direct access to the internal escape and match substitution engines,
	// in case you wish to customize them.

	JInterpolate* GetMatchInterpolator()   const;

private:

	enum PatternState
	{
		kEmpty,         // Do not have a pattern, itsRegex is freed
		kCannotCompile, // Have one, but it can't be compiled, itsRegex is freed
		kRecompile,     // A parameter changed, recompile before next match, itsRegex is allocated
		kReady          // and rarin' to go, obviously itsRegex is allocated
	};

	JString itsPattern;
	JSize   itsNULLCount;

	pcre*	itsRegex;

	int itsCFlags;
	int itsEFlags;

	JString*      itsReplacePattern;

	JInterpolate* itsInterpolator;

	#ifdef JRE_ALLOC_CHECK
	int numRegexAlloc;
	#endif

	// Data which could be stored in less than 32 bits and so are collected
	// together in case JRegex's memory usage is optimized someday

	PatternState itsState; // 2 bits

	JBoolean itsLiteralReplaceFlag;
	JBoolean itsMatchCaseFlag;

	// Static data

	static const JString theSpecialCharList;	// JAFL 5/11/98

private:

	void SetReplaceRegex() const;

	void Allocate();
	void CopyPatternRegex(const JRegex& source);
	void CopyPattern(const JCharacter* pattern, const JSize length);
	void CopyPattern(const JRegex& source);

	void SetCompileOption(const int option, const JBoolean setClear);
	void SetExecuteOption(const int option, const JBoolean setClear);
	void RawSetOption(int* flags, const int option, const JBoolean setClear);

	JBoolean RawGetOption(const int flags, const int option) const;

	void CompileOrDie();

	// The basic regex library functions, translated
	JError   RegComp();
	JBoolean RegExec(const JCharacter* str, const JSize offset, const JSize length,
					 JIndexRange* matchRange, JArray<JIndexRange>* matchList) const;
	void     RegFree();

public:

	// JError stuff

	static const JCharacter* kError; // Break up into real types when we need them

	class JRegexError : public JError
	{
		friend class JRegex;

	protected:

		JRegexError(const JCharacter* type, const JCharacter* message,
					const JIndex index)
			:
			JError(type, message, kJTrue),	// Always copy message!
			itsIndex(index)
		{ };

	public:

		JBoolean
		GetErrorIndex
			(
			JIndex* index
			)
			const
		{
			*index = itsIndex;
			return JI2B(itsIndex > 0);
		};

	private:

		JIndex	itsIndex;
	};
};


/******************************************************************************
 RawGetOption (private)

 *****************************************************************************/

inline JBoolean
JRegex::RawGetOption
	(
	const int flags,
	const int option
	)
	const
{
	return JConvertToBoolean(flags & option);
}

/******************************************************************************
 NeedsBackslashToBeLiteral (static)

	JAFL 5/11/98

	Returns kJTrue if the given character needs to be backslashed
	in order to be treated as a literal by the regex compiler.

 *****************************************************************************/

inline JBoolean
JRegex::NeedsBackslashToBeLiteral
	(
	const JCharacter c
	)
{
	const JCharacter s[] = { c, '\0' };
	return theSpecialCharList.Contains(s);
}

/******************************************************************************
 GetPattern

	Returns the pattern currently set.  This is a reference to the Regex's
	internal pattern buffer, it will point to garbage after the next
	SetPattern.  As usual, use JString to make your own copy.

	If there is no pattern currently set, GetPattern returns the empty string.

 *****************************************************************************/

inline const JString&
JRegex::GetPattern() const
{
	return itsPattern;
}


inline void
JRegex::GetPattern
	(
	const JCharacter** pattern,
	JSize*             length
	)
	const
{
	*pattern = itsPattern;
	*length = itsPattern.GetLength();
}

/******************************************************************************
 ContainsNULL

 *****************************************************************************/

inline JBoolean
JRegex::ContainsNULL() const
{
	return JConvertToBoolean(itsNULLCount != 0);
}

/******************************************************************************
 NULLCount

 *****************************************************************************/

inline JSize
JRegex::NULLCount() const
{
	return itsNULLCount;
}

/******************************************************************************
 CaseSensitive

	Controls whether matches will be case sensitive (the default) or case
	insensitive.

	Performance note: changing this option can cause a recompile before the
	next match.

 *****************************************************************************/

inline void
JRegex::SetCaseSensitive
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetCompileOption(PCRE_CASELESS, !yesNo);
}

inline JBoolean
JRegex::IsCaseSensitive() const
{
	return !RawGetOption(itsCFlags, PCRE_CASELESS);
}

/******************************************************************************
 SingleLine

	Controls whether the entire string is considered a single line or whether
	newlines are considered to indicate line boundaries (the default).  This
	option is independent of SetLineBegin() and SetLineEnd().

	Performance note: changing this option can cause a recompile before the
	next match.

 *****************************************************************************/

inline void
JRegex::SetSingleLine
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetCompileOption(PCRE_MULTILINE, !yesNo);
	SetCompileOption(PCRE_DOTALL, yesNo);
}

inline JBoolean
JRegex::IsSingleLine() const
{
	return !RawGetOption(itsCFlags, PCRE_MULTILINE);
}

/******************************************************************************
 LineBegin

	Controls whether the beginning of the string to be matched is considered
	to begin a line for purposes of matching '^'.  Default is for '^' to match
	at the beginning of the string.  This option is independent of
	SetSingleLine().

 *****************************************************************************/

inline void
JRegex::SetLineBegin
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetExecuteOption(PCRE_NOTBOL, !yesNo);
}

inline JBoolean
JRegex::IsLineBegin() const
{
	return !RawGetOption(itsEFlags, PCRE_NOTBOL);
}

/******************************************************************************
 LineEnd

	Controls whether the end of the string to be matched is considered to end a
	line for purposes of matching '$'.  Default is for '$' to match at the end
	of the string.  This option is independent of SetSingleLine().

 *****************************************************************************/

inline void
JRegex::SetLineEnd
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetExecuteOption(PCRE_NOTEOL, !yesNo);
}

inline JBoolean
JRegex::IsLineEnd() const
{
	return !RawGetOption(itsEFlags, PCRE_NOTEOL);
}

/******************************************************************************
 LiteralReplace

	Controls whether one of the forms of Replace() does pattern substitution on
	the replacement pattern before interpolation (for the other two forms
	replace pattern substitution is controlled by their arguments).  The default
	setting is kJFalse, so that replacement is done before interpolation, but the
	default argument is kJTrue, so that SetLiteralReplace() changes to no pattern
	replacement.

 *****************************************************************************/

inline void
JRegex::SetLiteralReplace
	(
	const JBoolean yesNo // = kJTrue
	)
{
	itsLiteralReplaceFlag = yesNo;
}

inline JBoolean
JRegex::IsLiteralReplace() const
{
	return itsLiteralReplaceFlag;
}

/******************************************************************************
 MatchCase

	Controls whether Replace (and InterpolateMatches) attempts to adjust
	the case of the replacement string to match that of the string matched.
	Default is kJFalse, so case is not adjusted.  If set to kJTrue, the first
	character of the replacement string will match the first character of
	the match.  In addition, if the rest of the match (after the first
	character) is entirely one case or the other, the rest of the
	replacement string (after the first character) will be coerced to that
	case.

 *****************************************************************************/

inline void
JRegex::SetMatchCase
	(
	const JBoolean yesNo // = kJTrue
	)
{
	itsMatchCaseFlag = yesNo;
}

inline JBoolean
JRegex::IsMatchCase() const
{
	return itsMatchCaseFlag;
}

/******************************************************************************
 SetReplacePattern

 *****************************************************************************/

// Creating JStrings is clunky, but convenient for now.

inline JError
JRegex::SetReplacePattern
	(
	const JCharacter* pattern,
	JIndexRange*      errRange
	)
{
	return SetReplacePattern( JString(pattern), errRange);
}


inline JError
JRegex::SetReplacePattern
	(
	const JCharacter* pattern,
	const JSize       length,
	JIndexRange*      errRange
	)
{
	return SetReplacePattern( JString(pattern, length), errRange);
}

/******************************************************************************
 GetReplacePattern

	Returns the current replace pattern.  The default (if no previous replace
	pattern was set) is the empty string, "".

 *****************************************************************************/

inline const JString&
JRegex::GetReplacePattern() const
{
	return *itsReplacePattern;
}

#endif
