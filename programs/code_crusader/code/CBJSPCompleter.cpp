/******************************************************************************
 CBJSPCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBJSPCompleter.h"
#include "CBHTMLCompleter.h"
#include "CBJavaScriptCompleter.h"
#include "CBJavaScriptStyler.h"
#include "CBHTMLStyler.h"
#include <ctype.h>
#include <jAssert.h>

CBJSPCompleter* CBJSPCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	// remember to update CBJavaCompleter

	// keywords

	"abstract", "boolean", "break", "byte", "byvalue", "case", "cast",
	"catch", "char", "class", "const", "continue", "default", "do",
	"double", "else", "enum", "extends", "false", "final", "finally", "float",
	"for", "future", "generic", "goto", "if", "implements", "import",
	"inner", "instanceof", "int", "interface", "long", "native", "new",
	"null", "operator", "outer", "package", "private", "protected",
	"public", "rest", "return", "short", "static", "super", "switch",
	"synchronized", "this", "throw", "throws", "transient", "true",
	"try", "var", "void", "volatile", "while",

	// methods

	"clone", "equals", "finalize", "getClass", "hashCode", "notify",
	"notifyAll", "toString", "wait",

	// lang package

	"Appendable", "Boolean", "Byte", "Character", "CharSequence",
	"Class", "ClassLoader", "Cloneable", "Comparable", "Compiler",
	"Double", "Enum", "Float", "InheritableThreadLocal", "Integer",
	"Iterable", "Long", "Math", "Number", "Object", "Package",
	"Process", "ProcessBuilder", "Readable", "Runnable", "Runtime",
	"RuntimePermission", "SecurityManager", "Short", "StackTraceElement",
	"StrictMath", "String", "StringBuffer", "StringBuilder",
	"System", "Thread", "ThreadGroup", "ThreadLocal", "Throwable", "Void",

	// util package

	"AbstractCollection", "AbstractList", "AbstractMap", "AbstractQueue",
	"AbstractSequentialList", "AbstractSet", "ArrayList", "Arrays", "BitSet",
	"Calendar", "Collections", "Comparator", "Currency", "Date", "Dictionary",
	"EnumMap", "EnumSet", "Enumeration", "EventListenerProxy", "EventObject",
	"FormattableFlags", "Formatter", "GregorianCalendar", "HashMap", "HashSet",
	"Hashtable", "IdentityHashMap", "Iterator", "LinkedHashMap", "LinkedHashSet",
	"LinkedList", "List", "ListIterator", "ListResourceBundle", "Locale",
	"Map", "Map.Entry", "Observable", "Observer", "PriorityQueue", "Properties",
	"PropertyPermission", "PropertyResourceBundle", "Queue", "Random",
	"RandomAccess", "ResourceBundle", "Scanner", "Set", "SimpleTimeZone",
	"SortedMap", "SortedSet", "Stack", "StringTokenizer", "Timer", "TimerTask",
	"TimeZone", "TreeMap", "TreeSet", "UUID", "Vector", "WeakHashMap"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBJSPCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBJSPCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBJSPCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBJSPCompleter::CBJSPCompleter()
	:
	CBStringCompleter(kCBJSPLang, kKeywordCount, kKeywordList, kJTrue)
{
	UpdateWordList();	// include HTML and JavaScript
	ListenTo(CBHTMLStyler::Instance());
	ListenTo(CBJavaScriptStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJSPCompleter::~CBJSPCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBJSPCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || (includeNS && c == '.'));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBJSPCompleter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == CBHTMLStyler::Instance() ||
		 sender == CBJavaScriptStyler::Instance()) &&
		message.Is(CBStylerBase::kWordListChanged))
		{
		UpdateWordList();
		}
	else
		{
		CBStringCompleter::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateWordList (virtual protected)

 ******************************************************************************/

void
CBJSPCompleter::UpdateWordList()
{
	CBStringCompleter::UpdateWordList();

	// include HTML words

	const JCharacter** htmlWordList;
	JSize count = CBHTMLCompleter::GetDefaultWordList(&htmlWordList);
	for (JIndex i=0; i<count; i++)
		{
		Add(htmlWordList[i]);
		}

	CopyWordsFromStyler(CBHTMLStyler::Instance());

	// include JavaScript words

	const JCharacter** jsWordList;
	count = CBJavaScriptCompleter::GetDefaultWordList(&jsWordList);
	for (JIndex i=0; i<count; i++)
		{
		Add(jsWordList[i]);
		}

	CopyWordsFromStyler(CBJavaScriptStyler::Instance());
	CopySymbolsForLanguage(kCBJavaScriptLang);
}
