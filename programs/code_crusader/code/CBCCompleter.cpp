/******************************************************************************
 CBCCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBCCompleter* CBCCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	// C

	"auto", "break", "case", "char", "const", "continue", "default", "do",
	"double", "else", "enum", "extern", "float", "for", "goto", "if",
	"inline", "int", "long", "register", "restrict", "return", "short",
	"signed", "sizeof", "static", "struct", "switch", "typedef", "union",
	"unsigned", "void", "volatile", "while",

	// C++

	"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
	"atomic_commit", "atomic_noexcept", "bitand", "bitor", "bool", "catch",
	"char16_t", "char32_t", "class", "compl", "concept", "constexpr",
	"const_cast", "decltype", "delete", "dynamic_cast", "explicit",
	"export", "false", "final", "friend", "module", "mutable", "namespace",
	"new", "noexcept", "not", "not_eq", "nullptr", "operator", "or",
	"or_eq", "overload", "override", "private", "protected", "public",
	"reinterpret_cast", "requires", "static_assert", "static_cast",
	"template", "this", "thread_local", "throw", "true", "try", "typeid",
	"typename", "using", "virtual", "wchar_t", "xor", "xor_eq",

	// preprocessor

	"include", "include_next", "define", "undef", "if", "ifdef", "ifndef",
	"else", "elif", "endif", "line", "pragma", "error"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBCCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBCCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBCCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBCCompleter::CBCCompleter()
	:
	CBStringCompleter(kCBCLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCCompleter::~CBCCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBCCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || (includeNS && c == ':'));
}

/******************************************************************************
 GetDefaultWordList (static)

	Must be static because may be called before object is created.

 ******************************************************************************/

JSize
CBCCompleter::GetDefaultWordList
	(
	const JCharacter*** list
	)
{
	*list = kKeywordList;
	return kKeywordCount;
}
