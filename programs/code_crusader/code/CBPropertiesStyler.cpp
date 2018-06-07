/******************************************************************************
 CBPropertiesStyler.cpp

	Helper object for CBTextEditor that displays .properties with styles to
	hilight comments, etc.

	BASE CLASS = CBStylerBase, CBPropertiesScanner

	Copyright (C) 2015 by John Lindal.

 ******************************************************************************/

#include "CBPropertiesStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JRegex.h>
#include <JXColorManager.h>
#include <jAssert.h>

CBPropertiesStyler* CBPropertiesStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JCharacter* kTypeNames[] =
{
	"Key or Value",
	"Assignment",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit INI Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBPropertiesStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBPropertiesStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPropertiesStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPropertiesStyler::CBPropertiesStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBPropertiesStyleID, kCBPropertiesFT),
	CBPropertiesScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JXColorManager* colormap = GetColormap();

	SetTypeStyle(kComment - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));

	SetTypeStyle(kError   - kWhitespace, JFontStyle(colormap->GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPropertiesStyler::~CBPropertiesStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBPropertiesStyler::Scan
	(
	std::istream&		input,
	const TokenExtra&	initData
	)
{
	BeginScan(input);

	const JString& text = GetText();

	JBoolean keepGoing;
	Token token;
	JFontStyle style;
	do
		{
		token = NextToken();
		if (token.type == kEOF)
			{
			break;
			}

		// save token starts

		if (token.type == kAssignment)
			{
			SaveTokenStart(TokenExtra());
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kComment)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type < kWhitespace)
			{
			style = GetTypeStyle(kError - kWhitespace);
			}
		else if (token.type > kError)	// misc
			{
			if (!GetWordStyle(text.GetSubstring(token.range), &style))
				{
				style = GetDefaultFont().GetStyle();
				}
			}
		else
			{
			style = GetStyle(typeIndex, text.GetSubstring(token.range));
			}

		keepGoing = SetStyle(token.range, style);
		}
		while (keepGoing);
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBPropertiesStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
}
