#ifndef _H_CBHTMLScanner
#define _H_CBHTMLScanner

/******************************************************************************
 CBHTMLScanner.h

	Copyright (C) 2001-2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBHTMLScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPHPFlexLexer
#include <FlexLexer.h>
#endif

#include <JPtrArray-JString.h>
#include <JPtrStack.h>

class CBHTMLScanner : public CBPHPFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBHTMLStyler

	enum TokenType
	{
		kEOF = 258,

		kHTMLEmptyTag,
		kHTMLUnterminatedTag,
		kHTMLInvalidNamedCharacter,
		kUnterminatedString,
		kUnterminatedComment,
		kUnterminatedPHPVariable,
		kUnterminatedJSRegex,
		kJavaBadCharConst,
		kPHPBadInt,
		kBadHex,
		kIllegalChar,

		kWhitespace,		// must be the one before the first item in type table

		// HTML

		kHTMLText,
		kHTMLTag,
		kHTMLScript,
		kHTMLNamedCharacter,
		kHTMLComment,
		kCDATABlock,

		// Mustache

		kMustacheCommand,

		// PHP

		kPHPStartEnd,
		kPHPID,
		kPHPVariable,
		kPHPReservedKeyword,
		kPHPBuiltInDataType,

		kPHPOperator,
		kPHPDelimiter,

		kPHPSingleQuoteString,
		kPHPDoubleQuoteString,
		kPHPHereDocString,
		kPHPExecString,

		// JSP/Java

		kJSPStartEnd,
		kJSPComment,

		kJavaID,
		kJavaReservedKeyword,
		kJavaBuiltInDataType,

		kJavaOperator,
		kJavaDelimiter,

		kJavaString,
		kJavaCharConst,

		// JavaScript

		kJSID,
		kJSReservedKeyword,

		kJSOperator,
		kJSDelimiter,

		kJSString,
		kJSTemplateString,
		kJSRegexSearch,

		// shared

		kFloat,
		kDecimalInt,
		kHexInt,
		kPHPOctalInt,

		kComment,
		kDocCommentHTMLTag,
		kDocCommentSpecialTag,

		kError			// place holder for CBHTMLStyler
	};

	struct Token
	{
		TokenType		type;
		JIndexRange		range;
		const JString*	language;			// if type == kHTMLScript
		JIndexRange		docCommentRange;	// preceding comment range for DOC comment tags

		Token()
			:
			type(kEOF), range(), language(NULL)
			{ };

		Token(const TokenType t, const JIndexRange& r, const JString* l)
			:
			type(t), range(r), language(l)
			{ };
	};

public:

	CBHTMLScanner();

	virtual ~CBHTMLScanner();

	void	BeginScan(std::istream& input, const yy_state_type startState);
	Token	NextToken();		// written by flex

protected:

	yy_state_type	GetCurrentLexerState() const;
	JBoolean		InTagState() const;

	virtual const JString&	GetScannedText() const = 0;

private:

	JBoolean		itsResetFlag;
	yy_state_type	itsStartState;
	JIndexRange		itsCurrentRange;
	JString			itsScriptLanguage;
	JString			itsPHPHereDocTag;
	JBoolean		itsProbableJSOperatorFlag;	// kTrue if / is most likely operator instead of regex

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);
	Token	ScriptToken();
	Token	DocToken(const TokenType type);

	JBoolean	IsScript(JString* language) const;
	JBoolean	InTagState(const int state) const;

	// not allowed

	CBHTMLScanner(const CBHTMLScanner& source);
	const CBHTMLScanner& operator=(const CBHTMLScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBHTMLScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBHTMLScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBHTMLScanner::Token
CBHTMLScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange, NULL);
}

/******************************************************************************
 ScriptToken (private)

 *****************************************************************************/

inline CBHTMLScanner::Token
CBHTMLScanner::ScriptToken()
{
	return Token(kHTMLScript, itsCurrentRange, &itsScriptLanguage);
}

/******************************************************************************
 DocToken (private)

 *****************************************************************************/

inline CBHTMLScanner::Token
CBHTMLScanner::DocToken
	(
	const TokenType type
	)
{
	Token t;
	t.docCommentRange = itsCurrentRange;	// save preceding comment range

	StartToken();							// tag is separate token
	t.type  = type;
	t.range = itsCurrentRange;

	// prepare for continuation of comment (StartToken() with yyleng==0)
	itsCurrentRange.Set(itsCurrentRange.last+1, itsCurrentRange.last);

	return t;
}

/******************************************************************************
 Comparisons

	These compare the type and *range*, not the contents, since ranges do not
	know where they came from.  Still useful when the tokens come from the same
	string.

 *****************************************************************************/

inline int
operator==
	(
	const CBHTMLScanner::Token& t1,
	const CBHTMLScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range == t2.range || t1.type == CBHTMLScanner::kEOF));
}

inline int
operator!=
	(
	const CBHTMLScanner::Token& t1,
	const CBHTMLScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
