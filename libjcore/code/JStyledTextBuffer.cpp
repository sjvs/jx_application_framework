/******************************************************************************
 JStyledTextBuffer.cpp

	Class to manage styled text.

	Only public functions are required to call NewUndo(), and only if the
	action to be performed changes the text or styles.  Protected and
	private functions must *not* call NewUndo(), because several
	manipulations may be required to perform one user command, and only the
	user command as a whole is undoable.  (Otherwise, the user may get
	confused.)

	Because of this convention, public functions that affect undo should
	only be a public interface to a private function.  The public function
	calls NewUndo() and then calls the private function.  The private
	function does all the work, but doesn't modify the undo information.
	This allows other private functions to use the routine (private
	version) without modifying the undo information.

	In order to guarantee that the TextChanged message means "text has
	already changed", NewUndo() must be called -after- making the
	modification.  (even though the Undo object has to be created before
	the modifications)

	TextSet is different from TextChanged because documents will typically
	use only the latter for setting their "unsaved" flag.

	Functionality yet to be implemented:

		Search & replace for text and styles combined

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JStyledTextBuffer.h>
#include <JTEUndoTyping.h>
#include <JTEUndoPaste.h>
#include <JTEUndoMove.h>
#include <JTEUndoStyle.h>
#include <JTEUndoTabShift.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <JListUtil.h>
#include <JRunArrayIterator.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <JInterpolate.h>
#include <JLatentPG.h>
#include <JMinMax.h>
#include <jTextUtil.h>
#include <jASCIIConstants.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

const JFileVersion kCurrentPrivateFormatVersion = 1;

const JSize kDefaultMaxUndoCount = 100;

const JSize kUNIXLineWidth    = 75;
const JSize kUNIXTabCharCount = 8;

inline JBoolean
isWhitespace
	(
	const JUtf8Character& c
	)
{
	return JI2B( c == ' ' || c == '\t' );
}

// JBroadcaster message types

const JUtf8Byte* JStyledTextBuffer::kTextSet     = "TextSet::JStyledTextBuffer";
const JUtf8Byte* JStyledTextBuffer::kTextChanged = "TextChanged::JStyledTextBuffer";
const JUtf8Byte* JStyledTextBuffer::kWillBeBusy  = "WillBeBusy::JStyledTextBuffer";

/******************************************************************************
 Constructor

	We don't provide a constructor that accepts text because we
	can't call RecalcAll() due to pure virtual functions.

	*** Derived classes must call RecalcAll().

 ******************************************************************************/

JStyledTextBuffer::JStyledTextBuffer
	(
	const JFontManager*	fontManager,
	JColormap*			colormap,
	const JBoolean		pasteStyledText
	)
	:
	itsPasteStyledTextFlag(pasteStyledText),

	itsFontMgr(fontManager),
	itsDefFont(fontManager->GetDefaultFont()),
	itsColormap(colormap)
{
	itsStyles = jnew JRunArray<JFont>;
	assert( itsStyles != NULL );

	itsUndo                    = NULL;
	itsUndoList                = NULL;
	itsFirstRedoIndex          = 1;
	itsLastSaveRedoIndex       = itsFirstRedoIndex;
	itsUndoState               = kIdle;
	itsMaxUndoCount            = kDefaultMaxUndoCount;
	itsTabToSpacesFlag         = kJFalse;
	itsBcastAllTextChangedFlag = kJFalse;

	itsCRMLineWidth     = kUNIXLineWidth;
	itsCRMTabCharCount  = kUNIXTabCharCount;
	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;

	itsCharInWordFn = DefaultIsCharacterInWord;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JStyledTextBuffer::JStyledTextBuffer
	(
	const JStyledTextBuffer& source
	)
	:
	JBroadcaster(source),

	itsBuffer( source.itsBuffer ),
	itsPasteStyledTextFlag( source.itsPasteStyledTextFlag ),

	itsFontMgr( source.itsFontMgr ),
	itsDefFont( source.itsDefFont ),
	itsColormap( source.itsColormap )
{
	itsStyles = jnew JRunArray<JFont>(*(source.itsStyles));
	assert( itsStyles != NULL );

	itsUndo                    = NULL;
	itsUndoList                = NULL;
	itsFirstRedoIndex          = 1;
	itsLastSaveRedoIndex       = itsFirstRedoIndex;
	itsUndoState               = kIdle;
	itsMaxUndoCount            = source.itsMaxUndoCount;
	itsTabToSpacesFlag         = source.itsTabToSpacesFlag;
	itsBcastAllTextChangedFlag = source.itsBcastAllTextChangedFlag;

	itsCRMLineWidth     = source.itsCRMLineWidth;
	itsCRMTabCharCount  = source.itsCRMTabCharCount;

	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;
	if (source.itsCRMRuleList != NULL)
		{
		SetCRMRuleList(source.itsCRMRuleList, source.itsOwnsCRMRulesFlag);
		}

	itsCharInWordFn = source.itsCharInWordFn;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStyledTextBuffer::~JStyledTextBuffer()
{
	jdelete itsStyles;
	jdelete itsUndo;
	jdelete itsUndoList;

	ClearCRMRuleList();
}

/******************************************************************************
 SetText

	Returns kJFalse if illegal characters had to be removed.

	Clears undo history.

	style can safely be NULL or itsStyles.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::SetText
	(
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	ClearUndo();
	itsBuffer = text;

	JBoolean cleaned = kJFalse;
	if (style != NULL)
		{
		assert( itsBuffer.GetCharacterCount() == style->GetElementCount() );
		*itsStyles = *style;
		cleaned    = RemoveIllegalChars(&itsBuffer, itsStyles);
		}
	else
		{
		cleaned = RemoveIllegalChars(&itsBuffer);

		itsStyles->RemoveAll();
		if (!itsBuffer.IsEmpty())
			{
			itsStyles->AppendElements(itsDefFont, itsBuffer.GetCharacterCount());
			}
		}

	Broadcast(TextSet());
	return !cleaned;
}

/******************************************************************************
 ReadPlainText

	If acceptBinaryFile == kJTrue, returns kJFalse if illegal characters had to
	be removed.  If acceptBinaryFile == kJFalse, returns kJFalse without loading
	the file if the file contains illegal characters.

	We read directly into itsBuffer to avoid making two copies of the
	file's data.  (The file could be very large.)

	Clears undo history.

 ******************************************************************************/

static const JUtf8Byte  kUNIXNewlineChar      = '\n';
static const JUtf8Byte* kMacintoshNewline     = "\r";
static const JUtf8Byte  kMacintoshNewlineChar = '\r';
static const JUtf8Byte* kDOSNewline           = "\r\n";
static const JUtf8Byte  k1stDOSNewlineChar    = '\r';

JBoolean
JStyledTextBuffer::ReadPlainText
	(
	const JString&		fileName,
	PlainTextFormat*	format,
	const JBoolean		acceptBinaryFile
	)
{
	JReadFile(fileName, &itsBuffer);

	JIndex i;
	if (ContainsIllegalChars(itsBuffer))
		{
		if (!acceptBinaryFile)
			{
			return kJFalse;
			}

		// It is probably a binary file, so we shouldn't mess with it.

		*format = kUNIXText;
		}

	else if (itsBuffer.Contains(kDOSNewline))
		{
		*format = kDOSText;

		Broadcast(WillBeBusy());

		JSize byteCount;
		JGetFileLength(fileName, &byteCount);

		JLatentPG pg(100);
		pg.FixedLengthProcessBeginning(byteCount,
				JGetString("ConvertFromDOS::JStyledTextBuffer"), kJFalse, kJFalse);

		// Converting itsBuffer in memory is more than 100 times slower,
		// but we have to normalize the bytes after reading, so we have to
		// allocate a new block.

		JUtf8Byte* buffer = jnew JUtf8Byte[ byteCount ];
		assert( buffer != NULL );

		std::ifstream input(fileName.GetBytes());
		JIndex i = 0;
		while (1)
			{
			const JUtf8Byte c = input.get();
			if (input.eof() || input.fail())
				{
				break;
				}

			if (c != k1stDOSNewlineChar)
				{
				buffer[i] = c;
				i++;
				}
			else
				{
				byteCount--;
				pg.IncrementProgress(i - pg.GetCurrentStepCount());
				}
			}
		input.close();

		itsBuffer.Set(buffer, byteCount);
		jdelete [] buffer;

		pg.ProcessFinished();
		}

	else if (itsBuffer.Contains(kMacintoshNewline))
		{
		*format = kMacintoshText;

		ConvertFromMacintoshNewlinetoUNIXNewline(&itsBuffer);
		}

	else
		{
		*format = kUNIXText;
		}

	return SetText(itsBuffer, NULL);
}

/******************************************************************************
 ConvertFromMacintoshNewlinetoUNIXNewline (private static)

	Efficient hack to replace bytes in place.

 ******************************************************************************/

void
JStyledTextBuffer::ConvertFromMacintoshNewlinetoUNIXNewline
	(
	JString* buffer
	)
{
	JUtf8Byte* s = const_cast<JUtf8Byte*>(buffer->GetBytes());
	while (*s > 0)
		{
		if (*s == kMacintoshNewlineChar)
			{
			*s = kUNIXNewlineChar;
			}
		*s++;
		}
}

/******************************************************************************
 WritePlainText

 ******************************************************************************/

void
JStyledTextBuffer::WritePlainText
	(
	const JString&			fileName,
	const PlainTextFormat	format
	)
	const
{
	std::ofstream output(fileName.GetBytes());
	WritePlainText(output, format);
}

void
JStyledTextBuffer::WritePlainText
	(
	std::ostream&			output,
	const PlainTextFormat	format
	)
	const
{
	if (format == kUNIXText)
		{
		itsBuffer.Print(output);
		return;
		}

	const JUtf8Byte* newlineStr = NULL;
	if (format == kDOSText)
		{
		newlineStr = kDOSNewline;
		}
	else if (format == kMacintoshText)
		{
		newlineStr = kMacintoshNewline;
		}
	assert( newlineStr != NULL );

	const JUtf8Byte* buffer = itsBuffer.GetBytes();
	const JSize byteCount   = itsBuffer.GetByteCount();
	JIndex start            = 0;
	for (JIndex i=0; i<byteCount; i++)
		{
		if (buffer[i] == kUNIXNewlineChar)
			{
			if (start < i)
				{
				output.write(buffer + start, i - start);
				}
			output << newlineStr;
			start = i+1;
			}
		}

	if (start < byteCount)
		{
		output.write(buffer + start, byteCount - start);
		}
}

/******************************************************************************
 ReadPrivateFormat

	See WritePrivateFormat() for version information.

	Clears undo history.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::ReadPrivateFormat
	(
	std::istream& input
	)
{
	Broadcast(WillBeBusy());

	JString text;
	JRunArray<JFont> style;
	if (ReadPrivateFormat(input, this, &text, &style))
		{
		SetText(text, &style);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReadPrivateFormat (static protected)

	See WritePrivateFormat() for version information.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::ReadPrivateFormat
	(
	std::istream&				input,
	const JStyledTextBuffer*	te,
	JString*					text,
	JRunArray<JFont>*			style
	)
{
	text->Clear();
	style->RemoveAll();

	// version

	JFileVersion vers;
	input >> vers;

	if (vers > kCurrentPrivateFormatVersion)
		{
		return kJFalse;
		}

	// text

	JSize textLength;
	input >> textLength;
	input.ignore(1);
	if (textLength > 0)
		{
		text->Read(input, textLength);
		}

	// list of font names

	JSize fontCount;
	input >> fontCount;

	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=fontCount; i++)
		{
		JString* name = jnew JString;
		assert( name != NULL );
		input >> *name;
		fontList.Append(name);
		}

	// list of rgb values

	JSize colorCount;
	input >> colorCount;

	JArray<JColorIndex> colorList;

	JRGB color;
	for (JIndex i=1; i<=colorCount; i++)
		{
		input >> color;
		colorList.AppendElement(te->itsColormap->GetColor(color));
		}

	// styles

	JSize runCount;
	input >> runCount;

	const JFontManager* fontMgr = te->GetFontManager();

	JSize size;
	JFontStyle fStyle;
	for (JIndex i=1; i<=runCount; i++)
		{
		JSize charCount;
		input >> charCount;

		JIndex fontIndex;
		input >> fontIndex;

		input >> size;
		input >> fStyle.bold >> fStyle.italic >> fStyle.strike;
		input >> fStyle.underlineCount;

		JIndex colorIndex;
		input >> colorIndex;
		fStyle.color = colorList.GetElement(colorIndex);

		style->AppendElements(
			fontMgr->GetFont(*(fontList.GetElement(fontIndex)), size, fStyle),
			charCount);
		}

	return kJTrue;
}

/******************************************************************************
 WritePrivateFormat

 ******************************************************************************/

void
JStyledTextBuffer::WritePrivateFormat
	(
	std::ostream& output
	)
	const
{
	if (!IsEmpty())
		{
		WritePrivateFormat(output, itsColormap, kCurrentPrivateFormatVersion,
						   itsBuffer, *itsStyles,
						   JCharacterRange(1, itsBuffer.GetCharacterCount()),
						   JUtf8ByteRange(1, itsBuffer.GetByteCount()));
		}
	else
		{
		output << kCurrentPrivateFormatVersion;
		output << " 0 0 0 0";
		}
}

/******************************************************************************
 WritePrivateFormat (static)

	We have to be able to write each version because this is what we
	put on the clipboard.

	version 1:  initial version.

 ******************************************************************************/

void
JStyledTextBuffer::WritePrivateFormat
	(
	std::ostream&			output,
	const JColormap*		colormap,
	const JFileVersion		vers,
	const JString&			text,
	const JRunArray<JFont>&	style,
	const JCharacterRange&	charRange
	)
{
	assert( !charRange.IsEmpty() );
	assert( text.RangeValid(charRange) );

	JUtf8ByteRange byteRange;
	if (charRange.first == 1 && charRange.last == text.GetCharacterCount())
		{
		byteRange.Set(1, text.GetCharacterCount());
		}
	else
		{
		JStringIterator iter(text);
		byteRange = CharToByteRange(charRange, &iter);
		}

	WritePrivateFormat(output, colormap, vers, text, style, charRange, byteRange);
}

// static private

void
JStyledTextBuffer::WritePrivateFormat
	(
	std::ostream&			output,
	const JColormap*		colormap,
	const JFileVersion		vers,
	const JString&			text,
	const JRunArray<JFont>&	style,
	const JCharacterRange&	charRange,
	const JUtf8ByteRange&	byteRange
	)
{
	assert( vers <= kCurrentPrivateFormatVersion );

	// write version

	output << vers;

	// write text efficiently

	output << ' ' << byteRange.GetCount() << ' ';
	output.write(text.GetRawBytes() + byteRange.first - 1, byteRange.GetCount());

	// build lists of font names and colors

	JSize styleRunCount = 0;
	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	fontList.SetCompareFunction(JCompareStringsCaseSensitive);
	JArray<JColorIndex> colorList;
	colorList.SetCompareFunction(JCompareIndices);

	JIndex startRunIndex, startFirstInRun;
	JBoolean found = style.FindRun(charRange.first, &startRunIndex, &startFirstInRun);
	assert( found );

	JIndex i          = charRange.first;
	JIndex runIndex   = startRunIndex;
	JIndex firstInRun = startFirstInRun;
	do
		{
		const JFont& f   = style.GetRunDataRef(runIndex);
		JString fontName = f.GetName();
		const JIndex fontIndex =
			fontList.SearchSorted1(&fontName, JListT::kAnyMatch, &found);
		if (!found)
			{
			fontList.InsertAtIndex(fontIndex, fontName);
			}

		const JColorIndex color = f.GetStyle().color;
		const JIndex colorIndex =
			colorList.SearchSorted1(color, JListT::kAnyMatch, &found);
		if (!found)
			{
			colorList.InsertElementAtIndex(colorIndex, color);
			}

		i += style.GetRunLength(runIndex) - (i - firstInRun);
		runIndex++;
		firstInRun = i;
		styleRunCount++;
		}
		while (i <= charRange.last);

	// write list of font names

	const JSize fontCount = fontList.GetElementCount();
	output << ' ' << fontCount;

	for (i=1; i<=fontCount; i++)
		{
		output << ' ' << *(fontList.GetElement(i));
		}

	// write list of rgb values

	const JSize colorCount = colorList.GetElementCount();
	output << ' ' << colorCount;

	for (i=1; i<=colorCount; i++)
		{
		output << ' ' << colormap->GetRGB(colorList.GetElement(i));
		}

	// write styles

	output << ' ' << styleRunCount;

	i          = charRange.first;
	runIndex   = startRunIndex;
	firstInRun = startFirstInRun;
	do
		{
		JSize charCount = style.GetRunLength(runIndex) - (i - firstInRun);
		if (charRange.last < i + charCount - 1)
			{
			charCount = charRange.last - i + 1;
			}

		const JFont& f   = style.GetRunDataRef(runIndex);
		JString fontName = f.GetName();

		JIndex fontIndex;
		found = fontList.SearchSorted(&fontName, JListT::kAnyMatch, &fontIndex);
		assert( found );

		const JFontStyle& fStyle = f.GetStyle();

		JIndex colorIndex;
		found = colorList.SearchSorted(fStyle.color, JListT::kAnyMatch, &colorIndex);
		assert( found );

		output << ' ' << charCount;
		output << ' ' << fontIndex;
		output << ' ' << f.GetSize();
		output << ' ' << fStyle.bold << fStyle.italic << fStyle.strike;
		output << ' ' << fStyle.underlineCount;
		output << ' ' << colorIndex;

		i += charCount;
		runIndex++;
		firstInRun = i;
		}
		while (i <= charRange.last);
}

/******************************************************************************
 Search and replace

	We only support regular expressions because there is no advantage in
	optimizing for literal strings.

 ******************************************************************************/

/******************************************************************************
 SearchForward

	Look for the next match beyond the given position.

 ******************************************************************************/

JStringMatch
JStyledTextBuffer::SearchForward
	(
	const TextIndex&	startIndex,
	const JRegex&		regex,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	TextIndex i = startIndex;

	*wrapped = kJFalse;
	if (i.charIndex > itsBuffer.GetCharacterCount() && wrapSearch)
		{
		i.charIndex = 1;
		i.byteIndex = 1;
		*wrapped    = kJTrue;
		}
	else if (i.charIndex > itsBuffer.GetCharacterCount())
		{
		return JStringMatch(itsBuffer);
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, i.charIndex, i.byteIndex);

	while (1)
		{
		if (iter.Next(regex))
			{
			const JStringMatch m = iter.GetLastMatch();
			if (!entireWord || IsEntireWord(itsBuffer, m))
				{
				return m;
				}
			}

		if (iter.AtEnd() && wrapSearch && !(*wrapped) && startIndex.charIndex > 1)
			{
			iter.MoveTo(kJIteratorStartAtBeginning, 0);
			*wrapped  = kJTrue;
			}
		else if (iter.AtEnd() || (*wrapped && iter.GetNextCharacterIndex() >= startIndex.charIndex))
			{
			break;
			}
		}

	return JStringMatch(itsBuffer);
}

/******************************************************************************
 SearchBackward

	Look for the match before the given position.

 ******************************************************************************/

JStringMatch
JStyledTextBuffer::SearchBackward
	(
	const TextIndex&	startIndex,
	const JRegex&		regex,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	TextIndex i = startIndex;

	*wrapped = kJFalse;
	if (i.charIndex == 1 && wrapSearch)
		{
		i.charIndex = itsBuffer.GetCharacterCount();
		i.byteIndex = itsBuffer.GetByteCount();
		*wrapped  = kJTrue;
		}
	else if (i.charIndex == 1)
		{
		return JStringMatch(itsBuffer);
		}

	JStringIterator iter(itsBuffer);

	if (i.charIndex > itsBuffer.GetCharacterCount())
		{
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		}
	else
		{
		iter.UnsafeMoveTo(kJIteratorStartBefore, i.charIndex, i.byteIndex);
		}

	while (1)
		{
		if (iter.Prev(regex))
			{
			const JStringMatch m = iter.GetLastMatch();
			if (!entireWord || IsEntireWord(itsBuffer, m))
				{
				return m;
				}
			}

		if (iter.AtBeginning() && wrapSearch && !(*wrapped) &&
			startIndex.charIndex < itsBuffer.GetCharacterCount())
			{
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			*wrapped = kJTrue;
			}
		else if (iter.AtBeginning() ||
				  (*wrapped && iter.GetPrevCharacterIndex() <= startIndex.charIndex))
			{
			break;
			}
		}

	return JStringMatch(itsBuffer);
}

/******************************************************************************
 ReplaceRange

	Replace the specified range with the given replace text.

	Does not handle undo.

 ******************************************************************************/

void
JStyledTextBuffer::ReplaceRange
	(
	JStringIterator*	buffer,
	JRunArray<JFont>*	styles,
	const JStringMatch&	match,
	const JString&		replaceStr,
	const JBoolean		replaceIsRegex,
	JInterpolate&		interpolator,
	const JBoolean		preserveCase
	)
{
	JString replaceText;
	if (replaceIsRegex)
		{
		replaceText = interpolator.Interpolate(replaceStr, match);
		}
	else
		{
		replaceText = replaceStr;
		}

	if (preserveCase)
		{
		replaceText.MatchCase(buffer->GetString().GetRawBytes(), match.GetUtf8ByteRange());
		}

	DeleteText(buffer, TextCount(match.GetCharacterCount(), match.GetByteCount()));
	InsertText(buffer, styles, replaceText, NULL, NULL);
}

/******************************************************************************
 IsEntireWord

	Return kJTrue if the given character range is a single, complete word.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::IsEntireWord
	(
	const JString&		buffer,
	const TextRange&	range
	)
	const
{
	const JString s(buffer, kJFalse);

	JStringIterator iter(s);
	JUtf8Character c;

	if (range.charRange.first > 1)
		{
		iter.UnsafeMoveTo(kJIteratorStartBefore, range.charRange.first, range.byteRange.first);
		if (iter.Prev(&c) && IsCharacterInWord(c))
			{
			return kJFalse;
			}
		}

	if (range.charRange.last < buffer.GetCharacterCount())
		{
		iter.UnsafeMoveTo(kJIteratorStartAfter, range.charRange.last, range.byteRange.last);
		if (iter.Next(&c) && IsCharacterInWord(c))
			{
			return kJFalse;
			}
		}

	iter.UnsafeMoveTo(kJIteratorStartBefore, range.charRange.first, range.byteRange.first);
	while (iter.Next(&c))
		{
		if (!IsCharacterInWord(c))
			{
			return kJFalse;
			}

		if (iter.GetNextCharacterIndex() > range.charRange.last)
			{
			break;
			}
		}

	return kJTrue;
}

/******************************************************************************
 SearchForward

	Look for the next match beyond the given position.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::SearchForward
	(
	const FontMatch&	match,
	const TextIndex&	start,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped,
	TextRange*			range
	)
{
	JIndex startIndex = start.charIndex;

	const JSize runCount = itsStyles->GetRunCount();
	JIndex endRun        = runCount;

	*wrapped = kJFalse;
	range->SetToNothing();
	while (1)
		{
		JIndex runIndex, firstIndexInRun;
		if (itsStyles->FindRun(startIndex, &runIndex, &firstIndexInRun))
			{
			const JIndex origStartRun = runIndex;
			JIndex startRun           = runIndex;
			if (startIndex > firstIndexInRun)
				{
				firstIndexInRun += itsStyles->GetRunLength(startRun);
				startRun++;
				}

			for (JIndex i=startRun; i<=endRun; i++)
				{
				const JFont& f        = itsStyles->GetRunDataRef(i);
				const JSize runLength = itsStyles->GetRunLength(i);
				if (match.Match(f))
					{
					range->charRange.SetFirstAndCount(firstIndexInRun, runLength);

					if (wrapped)
						{
						range->byteRange = JString::CharacterToUtf8ByteRange(itsBuffer.GetRawBytes(), range->charRange);
						}
					else	// optimize from where we started
						{
						range->byteRange.SetFirstAndCount(
							start.byteIndex,
							JString::CountBytes(
								itsBuffer.GetRawBytes() + start.byteIndex - 1,
								range->charRange.GetCount()));
						}

					return kJTrue;
					}

				firstIndexInRun += runLength;
				}

			if (wrapSearch && !(*wrapped) && origStartRun > 1)
				{
				startIndex = 1;
				endRun     = origStartRun-1;
				*wrapped   = kJTrue;
				}
			else
				{
				break;
				}
			}

		else if (wrapSearch && !(*wrapped))
			{
			startIndex = 1;
			*wrapped   = kJTrue;
			}
		else
			{
			break;
			}
		}

	return kJFalse;
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::SearchBackward
	(
	const FontMatch&	match,
	const TextIndex&	start,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped,
	TextRange*			range
	)
{
	JIndex startIndex = start.charIndex;

	JIndex endRun = 1;

	*wrapped = kJFalse;
	range->SetToNothing();
	while (1)
		{
		JIndex runIndex, firstIndexInRun;
		if (itsStyles->FindRun(startIndex, &runIndex, &firstIndexInRun))
			{
			const JIndex origStartRun = runIndex;
			JIndex startRun           = runIndex;
			if (startIndex < firstIndexInRun + itsStyles->GetRunLength(startRun)-1)
				{
				startRun--;
				if (startRun > 0)
					{
					firstIndexInRun -= itsStyles->GetRunLength(startRun);
					}
				}

			for (JIndex i=startRun; i>=endRun; i--)
				{
				const JFont& f = itsStyles->GetRunDataRef(i);
				if (match.Match(f))
					{
					range->charRange.SetFirstAndCount(firstIndexInRun, itsStyles->GetRunLength(i));

					if (wrapped)
						{
						range->byteRange = JString::CharacterToUtf8ByteRange(itsBuffer.GetRawBytes(), range->charRange);
						}
					else	// optimize from where we started
						{
						JSize byteCount;
						const JBoolean ok =
							JString::CountBytesBackward(
								itsBuffer.GetRawBytes(), start.byteIndex - 1,
								range->charRange.GetCount(), &byteCount);
						assert(ok);

						range->byteRange.Set(start.byteIndex - byteCount, start.byteIndex - 1);
						}

					return kJTrue;
					}

				if (i > endRun)
					{
					firstIndexInRun -= itsStyles->GetRunLength(i-1);
					}
				}

			if (wrapSearch && !(*wrapped) && origStartRun < itsStyles->GetRunCount())
				{
				startIndex = itsStyles->GetElementCount();
				endRun     = origStartRun+1;
				*wrapped   = kJTrue;
				}
			else
				{
				break;
				}
			}

		else if (wrapSearch && !(*wrapped))
			{
			startIndex = itsStyles->GetElementCount();
			*wrapped   = kJTrue;
			}
		else
			{
			break;
			}
		}

	return kJFalse;
}

/******************************************************************************
 FontMatch destructor

 ******************************************************************************/

JStyledTextBuffer::FontMatch::~FontMatch()
{
}

/******************************************************************************
 Set font

	Returns kJTrue if anything actually changed

	Does not handle undo

 ******************************************************************************/

JBoolean
JStyledTextBuffer::SetFontName
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JString&	name,
	const JBoolean	clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	JFont f1 = itsFontMgr->GetDefaultFont(), f2 = itsFontMgr->GetDefaultFont();
	JBoolean changed = kJFalse;
	JRunArrayIterator<JFont> iter(itsStyles, kJIteratorStartBefore, startIndex);
	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		const JBoolean ok = iter.Next(&f1);
		assert( ok );

		f2 = f1;
		f2.SetName(name);
		if (f2 != f1)
			{
			iter.SetPrev(f2);
			changed = kJTrue;
			}
		}

	return changed;
}

JBoolean
JStyledTextBuffer::SetFontSize
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JSize		size,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   size
	#define GetElementName GetSize()
	#define SetElementName SetSize
	#include <JSTBSetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JStyledTextBuffer::SetFontBold
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	bold,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   bold
	#define GetElementName GetStyle().bold
	#define SetElementName SetBold
	#include <JSTBSetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JStyledTextBuffer::SetFontItalic
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	italic,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   italic
	#define GetElementName GetStyle().italic
	#define SetElementName SetItalic
	#include <JSTBSetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JStyledTextBuffer::SetFontUnderline
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JSize		count,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   count
	#define GetElementName GetStyle().underlineCount
	#define SetElementName SetUnderlineCount
	#include <JSTBSetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JStyledTextBuffer::SetFontStrike
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	strike,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName   strike
	#define GetElementName GetStyle().strike
	#define SetElementName SetStrike
	#include <JSTBSetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JStyledTextBuffer::SetFontColor
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JColorIndex	color,
	const JBoolean		clearUndo

	)
{
	#define LocalVarName   color
	#define GetElementName GetStyle().color
	#define SetElementName SetColor
	#include <JSTBSetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

JBoolean
JStyledTextBuffer::SetFontStyle
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JFontStyle&	style,
	const JBoolean		clearUndo
	)
{
	#define LocalVarName   style
	#define GetElementName GetStyle()
	#define SetElementName SetStyle
	#include <JSTBSetFont.th>
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JStyledTextBuffer::SetFont
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JFont&		f,
	const JBoolean		clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	if (endIndex > startIndex)
		{
		const JSize charCount = endIndex - startIndex + 1;
		itsStyles->SetNextElements(startIndex, charCount, f);
		}
	else
		{
		assert( startIndex == endIndex );

		itsStyles->SetElement(startIndex, f);
		}
}

void
JStyledTextBuffer::SetFont
	(
	const JIndex			startIndex,
	const JRunArray<JFont>&	fontList,
	const JBoolean			clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	JFont f = itsFontMgr->GetDefaultFont();
	JRunArrayIterator<JFont> fIter(fontList);
	JRunArrayIterator<JFont> sIter(itsStyles, kJIteratorStartBefore, startIndex);

	while (fIter.Next(&f) && sIter.SetNext(f))
		{
		sIter.SkipNext();
		}
}

/******************************************************************************
 SetAllFontNameAndSize

	This function is useful for unstyled text editors that allow the user
	to change the font and size.

	It preserves the styles, in case they are controlled by the program.
	(e.g. context sensitive hilighting)

	You can choose whether or not to throw out all Undo information.
	Unstyled text editors can usually preserve Undo, since they will not
	allow the user to modify styles.  (We explicitly ask for this because
	it is too easy to forget about the issue.)

 ******************************************************************************/

void
JStyledTextBuffer::SetAllFontNameAndSize
	(
	const JString&	name,
	const JSize		size,
	const JBoolean	clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	const JSize runCount = itsStyles->GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		JFont f = itsStyles->GetRunData(i);
		f.Set(name, size, f.GetStyle());
		itsStyles->SetRunData(i, f);
		}

	if (itsUndoList != NULL)
		{
		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=1; i<=undoCount; i++)
			{
			(itsUndoList->GetElement(i))->SetFont(name, size);
			}
		}
	else if (itsUndo != NULL)
		{
		itsUndo->SetFont(name, size);
		}

	itsDefFont.Set(name, size, itsDefFont.GetStyle());
}

/******************************************************************************
 Copy

	Returns kJTrue if there was anything to copy.  style can be NULL.
	If function returns kJFalse, text and style are not modified.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::Copy
	(
	const TextRange&	range,
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	if (!range.IsEmpty())
		{
		text->Set(itsBuffer.GetRawBytes(), range.byteRange);

		if (style != NULL)
			{
			style->RemoveAll();
			style->AppendSlice(*itsStyles, range.charRange);

			assert( style->GetElementCount() == text->GetCharacterCount() );
			}

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Paste

	style can be NULL

	Handles undo

 ******************************************************************************/

void
JStyledTextBuffer::Paste
	(
	const TextRange&		range,
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	assert( style == NULL || style->GetElementCount() == text.GetCharacterCount() );

	JBoolean isNew;
	JTEUndoPaste* newUndo = GetPasteUndo(range, &isNew);
	assert( newUndo != NULL );

	const TextCount pasteCount = PrivatePaste(range, text, style);
	newUndo->SetCount(pasteCount);

	NewUndo(newUndo, isNew);
}

/******************************************************************************
 PrivatePaste (protected)

	Returns number of characters that were actually inserted.

	style can be NULL.

 ******************************************************************************/

JStyledTextBuffer::TextCount
JStyledTextBuffer::PrivatePaste
	(
	const TextRange&		range,
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	if (!range.IsEmpty())
		{
		DeleteText(range);
		}

	const TextIndex i(range.charRange.first, range.byteRange.first);
	if (itsPasteStyledTextFlag)
		{
		return InsertText(i, text, style);
		}
	else
		{
		return InsertText(i, text);
		}
}

/******************************************************************************
 InsertText (protected)

	Returns number of characters / bytes that were actually inserted.

	style can be NULL.

	In second version, iterator must be positioned at insertion index.

 ******************************************************************************/

JStyledTextBuffer::TextCount
JStyledTextBuffer::InsertText
	(
	const TextIndex&		index,
	const JString&			text,
	const JRunArray<JFont>*	style,		// can be NULL
	const JFont*			defaultFont	// can be NULL
	)
{
	JStringIterator iter(&itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	return InsertText(&iter, itsStyles, text, style, defaultFont);
}

JStyledTextBuffer::TextCount
JStyledTextBuffer::InsertText
	(
	JStringIterator*		targetText,
	JRunArray<JFont>*		targetStyle,
	const JString&			text,
	const JRunArray<JFont>*	style,			// can be NULL
	const JFont*			defaultFont		// can be NULL
	)
{
	if (text.IsEmpty())
		{
		return TextCount();
		}

	JString* cleanText           = NULL;
	JRunArray<JFont>* cleanStyle = NULL;

	JBoolean okToInsert;
	const JBoolean allocated =
		CleanText(text, style, &cleanText, &cleanStyle, &okToInsert);

	if (!okToInsert)
		{
		return TextCount();
		}

	if (!allocated)
		{
		cleanText  = const_cast<JString*>(&text);
		cleanStyle = const_cast<JRunArray<JFont>*>(style);
		}

	// insert the text

	const JIndex charIndex =
		targetText->GetString().IsEmpty() ? 1 :
		targetText->AtEnd() ? targetText->GetString().GetCharacterCount() :
		targetText->GetNextCharacterIndex();

	if (cleanStyle != NULL)
		{
		targetStyle->InsertElementsAtIndex(charIndex, *cleanStyle, 1, cleanStyle->GetElementCount());
		}
	else
		{
		targetStyle->InsertElementsAtIndex(
			charIndex,
			defaultFont != NULL ? *defaultFont : CalcInsertionFont(*targetText, *targetStyle),
			cleanText->GetCharacterCount());
		}

	// modify targetText only after calling CalcInsertionFont()

	targetText->Insert(*cleanText);

	const TextCount result(cleanText->GetCharacterCount(), cleanText->GetByteCount());

	if (allocated)
		{
		jdelete cleanText;
		jdelete cleanStyle;
		}

	return result;
}

/******************************************************************************
 CleanText (protected)

	Removes illegal characters, converts to UNIX newline format, lets
	derived class perform additional filtering.  Returns true if the text
	was modified.

	*okToInsert is true if derived class filtering accepted the text.

	style can be NULL.

 ******************************************************************************/

#define COPY_FOR_CLEAN_TEXT \
	if (*cleanText == NULL) \
		{ \
		*cleanText = jnew JString(text); \
		assert( *cleanText != NULL ); \
		if (style != NULL) \
			{ \
			*cleanStyle = jnew JRunArray<JFont>(*style); \
			assert( *cleanStyle != NULL ); \
			} \
		}

JBoolean
JStyledTextBuffer::CleanText
	(
	const JString&			text,
	const JRunArray<JFont>*	style,	// can be NULL
	JString**				cleanText,
	JRunArray<JFont>**		cleanStyle,
	JBoolean*				okToInsert
	)
{
	if (text.IsEmpty())
		{
		return kJFalse;
		}

	assert( style == NULL || text.GetCharacterCount() == style->GetElementCount() );

	*cleanText  = NULL;
	*cleanStyle = NULL;

	// remove illegal characters

	if (ContainsIllegalChars(text))
		{
		COPY_FOR_CLEAN_TEXT

		RemoveIllegalChars(*cleanText, *cleanStyle);
		}

	// convert from DOS format -- deleting is n^2, so we copy instead
	// (not using Split because the text could be very large)

	if ((*cleanText != NULL && (**cleanText).Contains(kDOSNewline)) ||
		(*cleanText == NULL && text.Contains(kDOSNewline)))
		{
		COPY_FOR_CLEAN_TEXT

		JString tmpText;
		tmpText.SetBlockSize((**cleanText).GetByteCount()+256);

		JRunArray<JFont> tmpStyle;
		if (*cleanStyle != NULL)
			{
			tmpStyle.SetBlockSize((**cleanStyle).GetRunCount()+16);
			}

		JStringIterator iter(*cleanText);
		JUtf8Character c;

		JBoolean done = kJFalse;
		iter.BeginMatch();
		while (!done)
			{
			done = !iter.Next("\r");	// ensures that we append trailing text

			const JStringMatch& m = iter.FinishMatch();
			if (!m.IsEmpty())
				{
				tmpText.Append(m.GetString());
				if (*cleanStyle != NULL)
					{
					tmpStyle.AppendSlice(**cleanStyle, m.GetCharacterRange());
					}
				}
			iter.BeginMatch();
			}

		**cleanText = tmpText;
		if (*cleanStyle != NULL)
			{
			**cleanStyle = tmpStyle;
			}
		}

	// convert from Macintosh format

	else if ((*cleanText != NULL && (**cleanText).Contains(kMacintoshNewline)) ||
			 (*cleanText == NULL && text.Contains(kMacintoshNewline)))
		{
		COPY_FOR_CLEAN_TEXT

		ConvertFromMacintoshNewlinetoUNIXNewline(*cleanText);
		}

	// allow derived classes to make additional changes
	// (last so we don't pass anything illegal to FilterText())

	*okToInsert = kJTrue;
	if (NeedsToFilterText(*cleanText != NULL ? **cleanText : text))
		{
		COPY_FOR_CLEAN_TEXT

		*okToInsert = FilterText(*cleanText, *cleanStyle);
		}

	return JI2B( *cleanText != NULL );
}

/******************************************************************************
 ContainsIllegalChars (static)

	Returns kJTrue if the given text contains characters that we will not
	accept:  00-08, 0B, 0E-1F, 7F

	We accept form feed (0C) because PrintPlainText() converts it to space.

	We accept all characters above 0x7F because they provide useful
	(though hopelessly system dependent) extensions to the character set.

 ******************************************************************************/

static const JRegex illegalCharRegex = "[\\0\x01-\x08\x0B\x0E-\x1F\x7F]+";

JBoolean
JStyledTextBuffer::ContainsIllegalChars
	(
	const JString&	text
	)
{
	return illegalCharRegex.Match(text);
}

/******************************************************************************
 RemoveIllegalChars (static)

	Returns kJTrue if we had to remove any characters that
	ContainsIllegalChars() would flag.

	style can be NULL or empty.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::RemoveIllegalChars
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	assert( style == NULL || style->IsEmpty() ||
			style->GetElementCount() == text->GetCharacterCount() );

	JBoolean changed = kJFalse;

	JStringIterator iter(text);
	while (iter.Next(illegalCharRegex))
		{
		const JStringMatch& m = iter.GetLastMatch();

		if (style != NULL)
			{
			style->RemoveElements(m.GetCharacterRange());
			}
		iter.RemoveLastMatch();		// invalidates m
		changed = kJTrue;
		}

	return changed;
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return kJTrue if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::NeedsToFilterText
	(
	const JString& text
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return kJFalse if the text cannot be used at all.

	*** Note that style may be NULL or empty if the data was plain text.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	return kJTrue;
}

/******************************************************************************
 DeleteText (protected)

 ******************************************************************************/

void
JStyledTextBuffer::DeleteText
	(
	const TextRange& range
	)
{
	JStringIterator iter(&itsBuffer);

	iter.UnsafeMoveTo(kJIteratorStartBefore, range.charRange.first, range.byteRange.first);
	DeleteText(&iter, range.GetCount());
}

void
JStyledTextBuffer::DeleteText
	(
	JStringIterator*	iter,
	const TextCount&	count
	)
{
	itsStyles->RemoveNextElements(iter->GetNextCharacterIndex(), count.charCount);

	iter->BeginMatch();
	iter->UnsafeMoveTo(
		kJIteratorStartAfter,
		iter->GetNextCharacterIndex() + count.charCount - 1,
		iter->GetNextByteIndex()      + count.byteCount - 1);
	iter->FinishMatch();
	iter->RemoveLastMatch();
}

/******************************************************************************
 BackwardDelete

	Delete characters preceding the insertion caret.

	Handles undo

 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::BackwardDelete
	(
	const TextIndex&	lineStart,
	const TextIndex&	caretIndex,
	const JBoolean		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	if (caretIndex.charIndex <= 1)
		{
		return TextIndex(1,1);
		}

	JStringIterator iter(&itsBuffer);

	if (caretIndex.charIndex > itsBuffer.GetCharacterCount())
		{
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		}
	else
		{
		iter.UnsafeMoveTo(kJIteratorStartBefore, caretIndex.charIndex, caretIndex.byteIndex);
		}
	iter.BeginMatch();

	JUtf8Character c;
	if (deleteToTabStop && iter.Prev(&c, kJFalse) && isWhitespace(c))
		{
		const JIndex textColumn = GetColumnForChar(lineStart, caretIndex);
		if ((textColumn-1) % itsCRMTabCharCount == 0)
			{
			JIndex deleteCount = 0;
			while (deleteCount < itsCRMTabCharCount && iter.Prev(&c))
				{
				if (c == ' ')
					{
					deleteCount++;
					}
				else if (c == '\t')
					{
					iter.Next(&c);
					deleteCount += CRMGetTabWidth(
						GetColumnForChar(lineStart, TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex())));
					iter.Prev(&c);
					}
				else	// normal delete when close to text
					{
					if (caretIndex.charIndex > itsBuffer.GetCharacterCount())
						{
						iter.MoveTo(kJIteratorStartAtEnd, 0);
						}
					else
						{
						iter.UnsafeMoveTo(kJIteratorStartBefore, caretIndex.charIndex, caretIndex.byteIndex);
						}
					iter.Prev(&c);
					break;
					}
				}
			}
		else			// normal delete when close to text
			{
			iter.Prev(&c);
			}
		}
	else
		{
		iter.Prev(&c);
		}

	const JStringMatch& match = iter.FinishMatch();

	if (returnText != NULL)
		{
		returnText->Set(match.GetString());
		}
	if (returnStyle != NULL)
		{
		returnStyle->RemoveAll();
		returnStyle->AppendSlice(*itsStyles, match.GetCharacterRange());
		}

	JBoolean isNew;
	JTEUndoTyping* typingUndo = GetTypingUndo(caretIndex, &isNew);
	typingUndo->HandleDelete(match);

	itsStyles->RemoveElements(match.GetCharacterRange());
	iter.RemoveLastMatch();		// invalidates match

	typingUndo->Activate();
	NewUndo(typingUndo, isNew);

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 ForwardDelete

	Delete characters following the insertion caret.

	Handles undo

 ******************************************************************************/

void
JStyledTextBuffer::ForwardDelete
	(
	const TextIndex&	lineStart,
	const TextIndex&	caretIndex,
	const JBoolean		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	if (caretIndex.charIndex > itsBuffer.GetCharacterCount())
		{
		return;
		}

	JStringIterator iter(&itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, caretIndex.charIndex, caretIndex.byteIndex);
	iter.BeginMatch();

	JUtf8Character c;
	if (deleteToTabStop && iter.Next(&c, kJFalse) && isWhitespace(c))
		{
		const JIndex textColumn = GetColumnForChar(lineStart, caretIndex);
		if ((textColumn-1) % itsCRMTabCharCount == 0)
			{
			JIndex deleteCount = 0;
			while (deleteCount < itsCRMTabCharCount && iter.Next(&c))
				{
				if (c == '\t')
					{
					break;
					}
				else if (c != ' ')	// normal delete when close to text
					{
					iter.UnsafeMoveTo(kJIteratorStartBefore, caretIndex.charIndex, caretIndex.byteIndex);
					iter.Next(&c);
					break;
					}

				deleteCount++;
				}
			}
		else						// normal delete when close to text
			{
			iter.Next(&c);
			}
		}
	else
		{
		iter.Next(&c);
		}

	const JStringMatch& match = iter.FinishMatch();

	if (returnText != NULL)
		{
		returnText->Set(match.GetString());
		}
	if (returnStyle != NULL)
		{
		returnStyle->RemoveAll();
		returnStyle->AppendSlice(*itsStyles, match.GetCharacterRange());
		}

	JBoolean isNew;
	JTEUndoTyping* typingUndo = GetTypingUndo(caretIndex, &isNew);
	typingUndo->HandleFwdDelete(match);

	itsStyles->RemoveElements(match.GetCharacterRange());

	iter.RemoveLastMatch();		// invalidates match

	typingUndo->Activate();
	NewUndo(typingUndo, isNew);
}

/******************************************************************************
 Outdent

	Remove tabs from the beginning of every line within the given range.
	The first line is assumed to start at the beginning of the range.

	Handles undo

 ******************************************************************************/

void
JStyledTextBuffer::Outdent
	(
	const TextRange&	origRange,
	const JSize			tabCount,
	const JBoolean		force
	)
{
	const TextIndex pEnd = GetParagraphEnd(TextIndex(origRange.charRange.last, origRange.byteRange.last));

	const TextRange range(JCharacterRange(origRange.charRange.first, pEnd.charIndex),
						  JUtf8ByteRange(origRange.byteRange.first, pEnd.byteIndex));

	// check that there are enough tabs at the start of every selected line,
	// ignoring lines created by wrapping

	JBoolean sufficientWS      = kJTrue;
	JSize prefixSpaceCount     = 0;		// min # of spaces at start of line
	JBoolean firstNonemptyLine = kJTrue;

	JStringIterator iter(&itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, range.charRange.first, range.byteRange.first);
	do
		{
		JUtf8Character c;
		if ((!iter.Prev(&c, kJFalse) || c == '\n') &&
			iter.Next(&c, kJFalse) && c != '\n')
			{
			for (JIndex j=1; j<=tabCount; j++)
				{
				// accept itsCRMTabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (iter.Next(&c) && c == ' ' &&
					   spaceCount < itsCRMTabCharCount)
					{
					spaceCount++;
					}

				if (iter.AtEnd())
					{
					break;	// last line contains only whitespace
					}
				if (j == 1 && (spaceCount < prefixSpaceCount || firstNonemptyLine))
					{
					prefixSpaceCount  = spaceCount;
					firstNonemptyLine = kJFalse;
					}
				if (spaceCount >= itsCRMTabCharCount)
					{
					continue;
					}

				if (c != '\t' && c != '\n')
					{
					sufficientWS = kJFalse;
					break;
					}
				if (c == '\n')
					{
					break;	// line is blank or contains only whitespace
					}
				}
			}
		}
		while (iter.Next("\n") && range.charRange.Contains(iter.GetNextCharacterIndex()));

	// If same number of spaces at front of every line, remove those even if
	// not enough to shift by one tab.

	JSize tabCharCount = itsCRMTabCharCount;
	if (!sufficientWS && prefixSpaceCount > 0 && tabCount == 1)
		{
		tabCharCount = prefixSpaceCount;
		}
	else if (force)
		{
		sufficientWS = kJTrue;
		}
	else if (!sufficientWS)
		{
		return;
		}

	JBoolean isNew;
	JTEUndoTabShift* undo = GetTabShiftUndo(range, &isNew);

	iter.UnsafeMoveTo(kJIteratorStartBefore, range.charRange.first, range.byteRange.first);

	JCharacterRange cr = range.charRange;

	JSize deleteCount = 0;	// only deleting single-byte characters
	do
		{
		JUtf8Character c;
		if ((!iter.Prev(&c, kJFalse) || c == '\n') &&
			iter.Next(&c, kJFalse) && c != '\n')
			{
			for (JIndex j=1; j<=tabCount; j++)
				{
				// The deletion point stays in same place (charIndex) and
				// simply eats characters.

				// accept tabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (iter.Next(&c, kJFalse) && c == ' ' &&
					   spaceCount < tabCharCount)
					{
					spaceCount++;
					DeleteText(&iter, TextCount(1,1));
					deleteCount++;
					cr.last--;
					}

				if (iter.AtEnd() ||	// last line contains only whitespace
					!sufficientWS)	// only remove tabCharCount spaces at front of every line
					{
					break;
					}

				if (spaceCount >= tabCharCount)
					{
					continue;	// removed equivalent of one tab
					}

				if (c != '\t')
					{
					break;
					}
				DeleteText(&iter, TextCount(1,1));
				deleteCount++;
				cr.last--;
				}
			}
		}
		while (iter.Next("\n") && cr.Contains(iter.GetNextCharacterIndex()));

	undo->SetCount(TextCount(cr.GetCount(), range.byteRange.GetCount() - deleteCount));

	NewUndo(undo, isNew);
}

/******************************************************************************
 Indent

	Insert tabs at the beginning of every line within the given range.
	The first line is assumed to start at the beginning of the range.

	Handles undo

 ******************************************************************************/

void
JStyledTextBuffer::Indent
	(
	const TextRange&	origRange,
	const JSize			tabCount
	)
{
	const TextIndex pEnd = GetParagraphEnd(TextIndex(origRange.charRange.last, origRange.byteRange.last));

	const TextRange range(JCharacterRange(origRange.charRange.first, pEnd.charIndex),
						  JUtf8ByteRange(origRange.byteRange.first, pEnd.byteIndex));


	JBoolean isNew;
	JTEUndoTabShift* undo = GetTabShiftUndo(range, &isNew);

	JString tabs;
	if (itsTabToSpacesFlag)
		{
		const JSize spaceCount = tabCount*itsCRMTabCharCount;
		for (JIndex i=1; i<=spaceCount; i++)
			{
			tabs.Append(" ");
			}
		}
	else
		{
		for (JIndex i=1; i<=tabCount; i++)
			{
			tabs.Append("\t");
			}
		}

	JStringIterator iter(&itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, range.charRange.first, range.byteRange.first);

	JCharacterRange cr = range.charRange;

	JSize insertCount = 0;	// only inserting single-byte characters
	do
		{
		JUtf8Character c;
		if ((!iter.Prev(&c, kJFalse) || c == '\n') &&
			iter.Next(&c, kJFalse) && c != '\n')
			{
			const JSize count = InsertText(&iter, itsStyles, tabs, NULL, NULL).charCount;
			insertCount      += count;
			cr.last          += count;
			}
		}
		while (iter.Next("\n") && cr.Contains(iter.GetNextCharacterIndex()));

	undo->SetCount(TextCount(cr.GetCount(), range.byteRange.GetCount() + insertCount));

	NewUndo(undo, isNew);
}

/******************************************************************************
 MoveText

	Handles undo

 ******************************************************************************/

JBoolean
JStyledTextBuffer::MoveText
	(
	const TextRange&	srcRange,
	const TextIndex&	origDestIndex,
	const JBoolean		copy
	)
{
	if (!copy &&
		(srcRange.charRange.first <= origDestIndex.charIndex &&
		 origDestIndex.charIndex <= srcRange.charRange.last + 1))
		{
		return kJFalse;
		}

	JString text;
	JRunArray<JFont> styles;
	const JBoolean ok = Copy(srcRange, &text, &styles);
	assert( ok );

	TextIndex destIndex = origDestIndex;

	JTEUndoBase* undo = NULL;
	JBoolean isNew;
	if (copy)
		{
		undo = GetPasteUndo(TextRange(
			JCharacterRange(destIndex.charIndex, 0),
			JUtf8ByteRange(destIndex.byteIndex, 0)), &isNew);
		}
	else	// move
		{
		TextIndex srcIndex(srcRange.charRange.first, srcRange.byteRange.first);
		if (destIndex.charIndex > srcRange.charRange.first)
			{
			destIndex.charIndex -= text.GetCharacterCount();
			destIndex.byteIndex -= text.GetByteCount();
			}
		else
			{
			assert( destIndex.charIndex < srcRange.charRange.first );
			srcIndex.charIndex += text.GetCharacterCount();
			srcIndex.byteIndex += text.GetByteCount();
			}

		undo = GetMoveUndo(srcIndex, destIndex, srcRange.GetCount(), &isNew);

		DeleteText(srcRange);
//****		Recalc(srcRange.charRange.first, 1, kJTrue, kJFalse);
		}
	assert( undo != NULL );

	const TextCount insertCount = InsertText(destIndex, text, &styles);
	undo->SetCount(insertCount);

	NewUndo(undo, isNew);
	return kJTrue;
}

/******************************************************************************
 CleanWhitespace

	Clean up the indentation whitespace and strip trailing whitespace in
	the specified range.

	Returns the range of the resulting text.

	Handles undo

 ******************************************************************************/

#define CLEAN_WS_ALIGNMENT \
	iter.SkipPrev(); \
	const JStringMatch& m = iter.FinishMatch(); \
	style.RemoveElements(m.GetCharacterRange()); \
	iter.RemoveLastMatch(); // invalidates m

JStyledTextBuffer::TextRange
JStyledTextBuffer::CleanWhitespace
	(
	const TextRange&	origRange,
	const JBoolean		align
	)
{
	const TextIndex i1 = GetParagraphStart(TextIndex(origRange.charRange.first, origRange.byteRange.first));
	const TextIndex i2 = GetParagraphEnd(TextIndex(origRange.charRange.last, origRange.byteRange.last));

	const TextRange range(JCharacterRange(i1.charIndex, i2.charIndex),
						  JUtf8ByteRange(i1.byteIndex, i2.byteIndex));

	JString text;
	JRunArray<JFont> style;
	const JBoolean ok = Copy(range, &text, &style);
	assert( ok );

	// strip trailing whitespace -- first, to clear blank lines

	JStringIterator iter(&text);
	JBoolean keepGoing;
	do
		{
		keepGoing = iter.Next("\n");

		JUtf8Character c;
		JSize count = 0;
		while (iter.Prev(&c, kJFalse) && isWhitespace(c))
			{
			iter.RemovePrev();
			count++;
			}

		if (count > 0)
			{
			style.RemoveNextElements(iter.GetNextCharacterIndex(), count);
			}
		}
		while (keepGoing);

	// clean indenting whitespace

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	JString fill;
	do
		{
		JSize n = 0;
		TextIndex p(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
		JUtf8Character c;

		if (itsTabToSpacesFlag)
			{
			iter.BeginMatch();
			while (iter.Next(&c) && isWhitespace(c))
				{
				if (c == ' ')
					{
					n++;
					if (n >= itsCRMTabCharCount)
						{
						n = 0;
						iter.BeginMatch();
						}
					}
				else	// tab
					{
					const JSize count = itsCRMTabCharCount - n;

					fill.Clear();
					for (JIndex i=1; i<=count; i++)
						{
						fill += " ";
						}

					const JIndex i = iter.GetPrevCharacterIndex();
					style.InsertElementsAtIndex(i, style.GetElement(i), count-1);

					iter.SkipPrev();
					iter.BeginMatch();
					iter.SkipNext();
					iter.FinishMatch();
					iter.ReplaceLastMatch(fill);

					n = 0;
					iter.BeginMatch();
					}
				}

			if (align && 0 < n && n <= itsCRMTabCharCount/2)
				{
				CLEAN_WS_ALIGNMENT
				}
			else if (align && itsCRMTabCharCount/2 < n)
				{
				const JSize count = itsCRMTabCharCount - n;

				fill.Clear();
				for (JIndex i=1; i<=count; i++)
					{
					fill += " ";
					}

				const JIndex i = iter.GetPrevCharacterIndex();
				style.InsertElementsAtIndex(i, style.GetElement(i), count);

				iter.SkipPrev();
				iter.Insert(fill);
				}
			}
		else
			{
			iter.BeginMatch();
			while (iter.Next(&c) && isWhitespace(c))
				{
				if (c == ' ')
					{
					n++;
					if (n >= itsCRMTabCharCount)
						{
						const JStringMatch& m = iter.FinishMatch();
						style.RemoveNextElements(
							iter.GetNextCharacterIndex(), m.GetCharacterCount()-1);

						iter.ReplaceLastMatch("\t");	// invalidates m

						n = 0;
						iter.BeginMatch();
						}
					}
				else	// tab
					{
					iter.SkipPrev();
					const JStringMatch& m = iter.FinishMatch();
					if (!m.IsEmpty())	// remove useless preceding spaces
						{
						style.RemoveElements(m.GetCharacterRange());
						iter.RemoveLastMatch();		// invalidates m
						}
					iter.SkipNext();

					n = 0;
					iter.BeginMatch();
					}
				}

			if (align && 0 < n && n <= itsCRMTabCharCount/2)
				{
				CLEAN_WS_ALIGNMENT
				}
			else if (align && itsCRMTabCharCount/2 < n)
				{
				iter.SkipPrev();
				const JStringMatch& m = iter.FinishMatch();

				const JCharacterRange r = m.GetCharacterRange();
				style.RemoveNextElements(r.first+1, r.GetCount()-1);

				iter.ReplaceLastMatch("\t");	// invalidates m
				}
			}
		}
		while (iter.Next("\n") && !iter.AtEnd());

	// replace selection with cleaned text/style

	Paste(range, text, &style);		// handles undo
	return TextRange(JCharacterRange(range.charRange.first, range.charRange.first + text.GetCharacterCount() - 1),
					 JUtf8ByteRange(range.byteRange.first, range.byteRange.first + text.GetByteCount() - 1));
}

#undef CLEAN_WS_ALIGNMENT

/******************************************************************************
 CleanRightMargin

	If there is a selection, cleans up the right margin of each paragraph
	touched by the selection.  Otherwise, cleans up the right margin of the
	paragraph containing the caret and maintains the caret position.

	If coerce, paragraphs are detected by looking only for blank lines.
	Otherwise, they are detected by blank lines or a change in the line prefix.

	If anything is changed, returns kJTrue and sets *reformatRange to the
	range of reformatted text, excluding the last newline.

	The work done by this function can be changed by calling SetCRMRuleList()
	and overriding the virtual CRM*() functions.

	Handles undo

 ******************************************************************************/

JBoolean
JStyledTextBuffer::CleanRightMargin
	(
	const JBoolean		coerce,
	JCharacterRange*	reformatRange
	)
{
		reformatRange->SetToNothing();
		return kJFalse;
/*
	if (TEIsDragging())
		{
		return kJFalse;
		}

	JBoolean changed = kJFalse;
	JIndexRange origTextRange;
	JString newText;
	JRunArray<JFont> newStyles;
	JIndex newCaretIndex;
	if (itsSelection.IsEmpty())
		{
		changed = PrivateCleanRightMargin(coerce, &origTextRange, &newText,
										  &newStyles, &newCaretIndex);
		}
	else
		{
		const JIndex endChar = GetParagraphEnd(itsSelection.last) - 1;

		JIndexRange range;
		JString text;
		JRunArray<JFont> styles;
		JIndex caretIndex;
		JBoolean first = kJTrue;
		while (1)
			{
			if (PrivateCleanRightMargin(coerce, &range, &text, &styles, &caretIndex))
				{
				origTextRange += range;
				newText       += text;
				newStyles.InsertElementsAtIndex(newStyles.GetElementCount()+1,
												styles, 1, styles.GetElementCount());
				if (range.last < endChar)
					{
					(range.last)++;
					(origTextRange.last)++;
					newText.AppendCharacter('\n');
					newStyles.AppendElement(newStyles.GetLastElement());
					}
				}
			else
				{
				caretIndex = GetInsertionIndex();
				range.Set(GetParagraphStart(caretIndex),
						  GetParagraphEnd(caretIndex));

				assert( newText.IsEmpty() ||
						range.first == origTextRange.last + 1 );

				origTextRange += range;
				newText       += itsBuffer->GetSubstring(range);
				newStyles.InsertElementsAtIndex(newStyles.GetElementCount()+1,
												*itsStyles, range.first, range.GetLength());
				}

			if (first)
				{
				newCaretIndex = caretIndex;
				first         = kJFalse;
				}

			if (range.last < endChar && IndexValid(range.last + 1))
				{
				SetCaretLocation(range.last + 1);
				}
			else
				{
				break;
				}
			}

		changed = kJTrue;
		}

	if (changed)
		{
		SetSelection(origTextRange, kJFalse);
		Paste(newText, &newStyles);
		SetCaretLocation(newCaretIndex);

		reformatRange->SetFirstAndLength(origTextRange.first, newText.GetLength());
		return kJTrue;
		}
	else
		{
		reformatRange->SetToNothing();
		return kJFalse;
		}
*/
}

/******************************************************************************
 SetCRMRuleList

	To find the first line of a paragraph, one searches backward for a blank
	line:  one that matches any CRMRule::first.  The following line is the
	beginning of the paragraph.

	Each following line is part of the paragraph if it is not blank:
	no CRMRule::first matches the entire line, and neither does the
	CRMRule::rest corresponding to the CRMRule::first that matched the first
	line of the paragraph.

	The prefix of the first line is unchanged.  The prefix of the following
	lines is calculated from calling (CRMRule::first)->Replace() on the
	prefix of the first line.

 ******************************************************************************/

void
JStyledTextBuffer::SetCRMRuleList
	(
	CRMRuleList*	ruleList,
	const JBoolean	teOwnsRuleList
	)
{
	ClearCRMRuleList();

	itsCRMRuleList      = ruleList;
	itsOwnsCRMRulesFlag = teOwnsRuleList;
}

/******************************************************************************
 ClearCRMRuleList

	With no other rules, the default is to match [ \t]*

 ******************************************************************************/

void
JStyledTextBuffer::ClearCRMRuleList()
{
	if (itsOwnsCRMRulesFlag && itsCRMRuleList != NULL)
		{
		itsCRMRuleList->DeleteAll();
		jdelete itsCRMRuleList;
		}

	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;
}

/******************************************************************************
 CRMRule functions

 ******************************************************************************/

JStyledTextBuffer::CRMRule::CRMRule
	(
	const JString& firstPattern,
	const JString& restPattern,
	const JString& replacePattern
	)
{
	first = jnew JRegex(firstPattern);
	assert( first != NULL );

	rest = jnew JRegex(restPattern);
	assert( rest != NULL );

	replace = jnew JString(restPattern);
	assert( replace != NULL );
}

/******************************************************************************
 CRMRuleList functions

 ******************************************************************************/

JStyledTextBuffer::CRMRuleList::CRMRuleList
	(
	const CRMRuleList& source
	)
	:
	JArray<CRMRule>()
{
	const JSize count = source.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CRMRule r = source.GetElement(i);
		AppendElement(
			CRMRule((r.first)->GetPattern(), (r.rest)->GetPattern(),
					*r.replace));
		}
}

void
JStyledTextBuffer::CRMRuleList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CRMRule r = GetElement(i);
		jdelete r.first;
		jdelete r.rest;
		jdelete r.replace;
		}

	RemoveAll();
}

/******************************************************************************
 SetCRMLineWidth

 ******************************************************************************/

void
JStyledTextBuffer::SetCRMLineWidth
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMLineWidth = charCount;
}

/******************************************************************************
 SetCRMTabCharCount

 ******************************************************************************/

void
JStyledTextBuffer::SetCRMTabCharCount
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMTabCharCount = charCount;
}

/*******************************************************************************
 CRMGetTabWidth

	Returns the number of spaces to which the tab is equivalent by rounding
	up to the nearest multiple of GetCRMTabCharCount().  The default value
	for this is 8 since this is what all UNIX programs use.

	textColumn starts at 1 at the left margin.

 ******************************************************************************/

JSize
JStyledTextBuffer::CRMGetTabWidth
	(
	const JIndex textColumn
	)
	const
{
	return itsCRMTabCharCount - ((textColumn-1) % itsCRMTabCharCount);
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
JStyledTextBuffer::Undo()
{
	assert( itsUndoState == kIdle );

	JTEUndoBase* undo;
	if (GetCurrentUndo(&undo))
		{
		itsUndoState = kUndo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
}

/******************************************************************************
 Redo

 ******************************************************************************/

void
JStyledTextBuffer::Redo()
{
	assert( itsUndoState == kIdle );

	JTEUndoBase* undo;
	if (GetCurrentRedo(&undo))
		{
		itsUndoState = kRedo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
}

/******************************************************************************
 DeactivateCurrentUndo

 ******************************************************************************/

void
JStyledTextBuffer::DeactivateCurrentUndo()
{
	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo))
		{
		undo->Deactivate();
		}
}

/******************************************************************************
 ClearUndo

	Avoid calling this whenever possible since it throws away -all-
	undo information.

 ******************************************************************************/

void
JStyledTextBuffer::ClearUndo()
{
	jdelete itsUndo;
	itsUndo = NULL;

	if (itsUndoList != NULL)
		{
		itsUndoList->CleanOut();
		}
	itsFirstRedoIndex = 1;
	ClearLastSaveLocation();
}

/******************************************************************************
 UseMultipleUndo

	You probably never need to turn off multiple undo unless you
	are running out of memory.

	If you call this in your constructor, you should call
	SetLastSaveLocation() afterwards.

 ******************************************************************************/

void
JStyledTextBuffer::UseMultipleUndo
	(
	const JBoolean useMultiple
	)
{
	if (useMultiple && itsUndoList == NULL)
		{
		ClearUndo();

		itsUndoList = jnew JPtrArray<JTEUndoBase>(JPtrArrayT::kDeleteAll,
												 itsMaxUndoCount+1);
		assert( itsUndoList != NULL );
		}
	else if (!useMultiple && itsUndoList != NULL)
		{
		ClearUndo();

		jdelete itsUndoList;
		itsUndoList = NULL;
		}
}

/******************************************************************************
 SetUndoDepth

 ******************************************************************************/

void
JStyledTextBuffer::SetUndoDepth
	(
	const JSize maxUndoCount
	)
{
	assert( maxUndoCount > 0 );

	itsMaxUndoCount = maxUndoCount;
	ClearOutdatedUndo();
}

/******************************************************************************
 GetCurrentUndo (protected)

 ******************************************************************************/

JBoolean
JStyledTextBuffer::GetCurrentUndo
	(
	JTEUndoBase** undo
	)
	const
{
	if (itsUndoList != NULL && itsFirstRedoIndex > 1)
		{
		*undo = itsUndoList->GetElement(itsFirstRedoIndex - 1);
		return kJTrue;
		}
	else if (itsUndoList != NULL)
		{
		return kJFalse;
		}
	else
		{
		*undo = itsUndo;
		return JConvertToBoolean( *undo != NULL );
		}
}

/******************************************************************************
 GetCurrentRedo (protected)

 ******************************************************************************/

JBoolean
JStyledTextBuffer::GetCurrentRedo
	(
	JTEUndoBase** redo
	)
	const
{
	if (itsUndoList != NULL && itsFirstRedoIndex <= itsUndoList->GetElementCount())
		{
		*redo = itsUndoList->GetElement(itsFirstRedoIndex);
		return kJTrue;
		}
	else if (itsUndoList != NULL)
		{
		return kJFalse;
		}
	else
		{
		*redo = itsUndo;
		return JConvertToBoolean( *redo != NULL );
		}
}

/******************************************************************************
 NewUndo (protected)

	Register a jnew Undo object.

	itsFirstRedoIndex points to the first redo object in itsUndoList.
	1 <= itsFirstRedoIndex <= itsUndoList->GetElementCount()+1
	Minimum => everything is redo
	Maximum => everything is undo

 ******************************************************************************/

void
JStyledTextBuffer::NewUndo
	(
	JTEUndoBase*	undo,
	const JBoolean	isNew
	)
{
	if (!isNew)
		{
		if (itsBcastAllTextChangedFlag)
			{
			Broadcast(TextChanged());
			}
		return;
		}

	if (itsUndoList != NULL && itsUndoState == kIdle)
		{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
			{
			itsUndoList->DeleteElement(i);
			}

		if (itsLastSaveRedoIndex > 0 &&
			itsFirstRedoIndex < JIndex(itsLastSaveRedoIndex))
			{
			ClearLastSaveLocation();
			}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		ClearOutdatedUndo();
		assert( !itsUndoList->IsEmpty() );
		}

	else if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(kJTrue);
		undo->Deactivate();
		}

	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(kJFalse);
		undo->Deactivate();
		}

	else
		{
		jdelete itsUndo;
		itsUndo = undo;
		}

	Broadcast(TextChanged());
}

/******************************************************************************
 ReplaceUndo (protected)

 ******************************************************************************/

void
JStyledTextBuffer::ReplaceUndo
	(
	JTEUndoBase* oldUndo,
	JTEUndoBase* newUndo
	)
{
#ifndef NDEBUG

	assert( itsUndoState != kIdle );

	if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 &&
				oldUndo == itsUndoList->GetElement(itsFirstRedoIndex - 1) );
		}
	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() &&
				oldUndo == itsUndoList->GetElement(itsFirstRedoIndex) );
		}
	else
		{
		assert( oldUndo == itsUndo );
		}

#endif

	NewUndo(newUndo, kJTrue);
}

/******************************************************************************
 ClearOutdatedUndo (protected)

 ******************************************************************************/

void
JStyledTextBuffer::ClearOutdatedUndo()
{
	if (itsUndoList != NULL)
		{
		JSize undoCount = itsUndoList->GetElementCount();
		while (undoCount > itsMaxUndoCount)
			{
			itsUndoList->DeleteElement(1);
			undoCount--;
			itsFirstRedoIndex--;
			itsLastSaveRedoIndex--;

			// If we have to throw out redo's, we have to toss everything.

			if (itsFirstRedoIndex == 0)
				{
				ClearUndo();
				break;
				}
			}
		}
}

/******************************************************************************
 GetTypingUndo (protected)

	Return the active JTEUndoTyping object.  If the current undo object is
	not an active JTEUndoTyping object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoTyping*
JStyledTextBuffer::GetTypingUndo
	(
	const JStyledTextBuffer::TextIndex&	start,
	JBoolean*							isNew
	)
{
	JTEUndoTyping* typingUndo = NULL;

	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(typingUndo = dynamic_cast<JTEUndoTyping*>(undo)) != NULL &&
		typingUndo->IsActive() &&
		typingUndo->SameStartIndex(start))
		{
		*isNew = kJFalse;
		return typingUndo;
		}
	else
		{
		typingUndo = jnew JTEUndoTyping(this, start);
		assert( typingUndo != NULL );

		*isNew = kJTrue;
		return typingUndo;
		}
}

/******************************************************************************
 GetStyleUndo (protected)

	Return the active JTEUndoStyle object.  If the current undo object is
	not an active JTEUndoStyle object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoStyle*
JStyledTextBuffer::GetStyleUndo
	(
	const JCharacterRange&	range,
	JBoolean*				isNew
	)
{
	JTEUndoStyle* styleUndo = NULL;

	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(styleUndo = dynamic_cast<JTEUndoStyle*>(undo)) != NULL &&
		styleUndo->IsActive() &&
		styleUndo->SameRange(range))
		{
		*isNew = kJFalse;
		return styleUndo;
		}
	else
		{
		styleUndo = jnew JTEUndoStyle(this, range);
		assert( styleUndo != NULL );

		*isNew = kJTrue;
		return styleUndo;
		}
}

/******************************************************************************
 GetPasteUndo (protected)

	Return a new JTEUndoPaste object, since they can never be reused.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoPaste*
JStyledTextBuffer::GetPasteUndo
	(
	const JStyledTextBuffer::TextRange&	range,
	JBoolean*							isNew
	)
{
	JTEUndoPaste* pasteUndo = jnew JTEUndoPaste(this, range);
	assert( pasteUndo != NULL );

	*isNew = kJTrue;
	return pasteUndo;
}

/******************************************************************************
 GetTabShiftUndo (protected)

	Return the active JTEUndoTabShift object.  If the current undo object is
	not an active JTEUndoTabShift object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoTabShift*
JStyledTextBuffer::GetTabShiftUndo
	(
	const JStyledTextBuffer::TextRange&	range,
	JBoolean*							isNew
	)
{
	JTEUndoTabShift* tabShiftUndo = NULL;

	JTEUndoBase* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(tabShiftUndo = dynamic_cast<JTEUndoTabShift*>(undo)) != NULL &&
		tabShiftUndo->IsActive() &&
		tabShiftUndo->SameStartIndex(range))
		{
		*isNew = kJFalse;
		return tabShiftUndo;
		}
	else
		{
		tabShiftUndo = jnew JTEUndoTabShift(this, range);
		assert( tabShiftUndo != NULL );

		*isNew = kJTrue;
		return tabShiftUndo;
		}
}

/******************************************************************************
 GetMoveUndo (protected)

	Return a new JTEUndoMove object, since they can never be reused.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoMove*
JStyledTextBuffer::GetMoveUndo
	(
	const JStyledTextBuffer::TextIndex&	srcIndex,
	const JStyledTextBuffer::TextIndex&	destIndex,
	const JStyledTextBuffer::TextCount&	count,
	JBoolean*							isNew
	)
{
	JTEUndoMove* moveUndo = jnew JTEUndoMove(this, srcIndex, destIndex, count);
	assert( moveUndo != NULL );

	*isNew = kJTrue;
	return moveUndo;
}

/******************************************************************************
 AutoIndent (protected)

	Insert the "rest" prefix based on the previous line.  If the previous
	line is nothing but whitespace, clear it.

 ******************************************************************************/

void
JStyledTextBuffer::AutoIndent
	(
	JTEUndoTyping* typingUndo
	)
{
#if 0
	assert( itsSelection.IsEmpty() );

	// Move up one line if we are not at the very end of the text.

	JIndex lineIndex = itsCaretLoc.lineIndex;
	if (itsCaretLoc.charIndex <= itsBuffer->GetLength())	// ends with newline
		{
		lineIndex--;
		}

	// Skip past lines that were created by wrapping the text.
	// (This is faster than using GetParagraphStart() because then we would
	//  have to call GetLineForChar(), which is a search.  GetLineStart()
	//  is an array lookup.)

	JIndex firstChar = GetLineStart(lineIndex);
	while (lineIndex > 1 && itsBuffer->GetCharacter(firstChar-1) != '\n')
		{
		lineIndex--;
		firstChar = GetLineStart(lineIndex);
		}

	// Calculate the prefix range for the original line.

	const JIndex lastChar = GetParagraphEnd(firstChar) - 1;
	assert( itsBuffer->GetCharacter(lastChar+1) == '\n' );

	JIndex firstTextChar = firstChar;
	JString linePrefix;
	JSize prefixLength;
	JIndex ruleIndex = 0;
	if (lastChar < firstChar ||
		!CRMGetPrefix(&firstTextChar, lastChar,
					  &linePrefix, &prefixLength, &ruleIndex))
		{
		firstTextChar = lastChar+1;
		if (firstTextChar > firstChar)
			{
			linePrefix = itsBuffer->GetSubstring(firstChar, firstTextChar-1);
			}
		}
	else if (CRMLineMatchesRest(JIndexRange(firstChar, lastChar)))
		{
		// CRMBuildRestPrefix() will complain if we pass in the entire
		// line, so we give it only the whitespace.

		CRMRuleList* savedRuleList = itsCRMRuleList;
		itsCRMRuleList             = NULL;
		firstTextChar              = firstChar;
		ruleIndex                  = 0;
		CRMGetPrefix(&firstTextChar, lastChar,
					 &linePrefix, &prefixLength, &ruleIndex);
		itsCRMRuleList = savedRuleList;
		}

	// Generate the prefix and include it in the undo object.

	if (firstTextChar > firstChar)
		{
		linePrefix   = CRMBuildRestPrefix(linePrefix, ruleIndex, &prefixLength);
		prefixLength = linePrefix.GetLength();

		typingUndo->HandleCharacters(prefixLength);

		const JSize pasteLength = InsertText(itsCaretLoc, linePrefix);
		assert( pasteLength == prefixLength );
		Recalc(itsCaretLoc, prefixLength, kJFalse, kJFalse);

		JIndex newCaretChar = itsCaretLoc.charIndex + prefixLength;

		// check the initial whitespace range

		JIndex lastWSChar = firstChar;
		while (1)
			{
			const JCharacter c = itsBuffer->GetCharacter(lastWSChar);
			if ((c != ' ' && c != '\t') || lastWSChar > lastChar)		// can't use isspace() because '\n' stops us
				{
				lastWSChar--;
				break;
				}
			lastWSChar++;
			}

		// if the line is blank, clear it

		if (lastWSChar >= firstChar && itsBuffer->GetCharacter(lastWSChar+1) == '\n')
			{
			typingUndo->HandleDelete(firstChar, lastWSChar);

			DeleteText(firstChar, lastWSChar);
			Recalc(CaretLocation(firstChar, lineIndex), 1, kJTrue, kJFalse);

			newCaretChar -= lastWSChar - firstChar + 1;
			}

		SetCaretLocation(newCaretChar);

		// re-activate undo after SetCaretLocation()

		typingUndo->Activate();
		}
#endif
}

/******************************************************************************
 InsertSpacesForTab (protected)

	Insert spaces to use up the same amount of space that a tab would use.

 ******************************************************************************/

void
JStyledTextBuffer::InsertSpacesForTab
	(
	const TextIndex& lineStart,
	const TextIndex& caretIndex
	)
{
	JIndex column = GetColumnForChar(lineStart, caretIndex);

	if (caretIndex.charIndex == itsBuffer.GetCharacterCount()+1 && EndsWithNewline())
		{
		column = 1;
		}

	const JSize spaceCount = CRMGetTabWidth(column);

	JString space;
	for (JIndex i=1; i<=spaceCount; i++)
		{
		space.Append(" ");
		}

	Paste(TextRange(
			JCharacterRange(caretIndex.charIndex, 0),
			JUtf8ByteRange(caretIndex.byteIndex, 0)),
		  space);
}

/******************************************************************************
 GetWordStart

	Return the index of the first character of the word at the given location.
	This function is required to work for charIndex == 0.

 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::GetWordStart
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty() || index.charIndex <= 1)
		{
		return TextIndex(1,1);
		}

	const JIndex charIndex = JMin(index.charIndex, itsBuffer.GetCharacterCount()),
				 byteIndex = JMin(index.byteIndex, itsBuffer.GetByteCount());

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartAfter, charIndex, byteIndex);

	JUtf8Character c;
	if (iter.Prev(&c, kJFalse) && !IsCharacterInWord(c))
		{
		while (iter.Prev(&c) && !IsCharacterInWord(c))
			{
			// find end of word
			}
		}

	while (iter.Prev(&c) && IsCharacterInWord(c))
		{
		// find start of word
		}

	if (!iter.AtBeginning())
		{
		iter.SkipNext();
		}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetWordEnd

	Return the index of the last character of the word at the given location.
	This function is required to work for charIndex > buffer length.

 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::GetWordEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty())
		{
		return TextIndex(1,1);
		}

	const JSize bufLen = itsBuffer.GetCharacterCount();
	if (index.charIndex >= bufLen)
		{
		return TextIndex(bufLen, itsBuffer.GetByteCount());
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	JUtf8Character c;
	if (iter.Next(&c, kJFalse) && !IsCharacterInWord(c))
		{
		while (iter.Next(&c) && !IsCharacterInWord(c))
			{
			// find start of word
			}
		}

	while (iter.Next(&c) && IsCharacterInWord(c))
		{
		// find end of word
		}

	if (!iter.AtEnd())
		{
		iter.SkipPrev();
		}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 SetCharacterInWordFunction

	Set the function that determines if a character is part of a word.
	The default is JIsAlnum(), which uses [A-Za-z0-9].

 ******************************************************************************/

void
JStyledTextBuffer::SetCharacterInWordFunction
	(
	JCharacterInWordFn f
	)
{
	assert( f != NULL );
	itsCharInWordFn = f;
}

/******************************************************************************
 IsCharacterInWord (protected)

	Returns kJTrue if the given character should be considered part of a word.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::IsCharacterInWord
	(
	const JUtf8Character& c
	)
	const
{
	return itsCharInWordFn(c);
}

/******************************************************************************
 DefaultIsCharacterInWord (static private)

	Returns kJTrue if the given character should be considered part of a word.
	The definition is alphanumeric or apostrophe or underscore.

	This is not a virtual function because one needs to be able to set it
	for simple objects like input fields.

 ******************************************************************************/

JBoolean
JStyledTextBuffer::DefaultIsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return JI2B( c.IsAlnum() || c == '\'' || c == '_' );
}

/******************************************************************************
 GetPartialWordStart

	Return the index of the first character of the partial word at the given
	location.  This function is required to work for charIndex == 0.

	Example:  get_word Get142TheWordABCGood ABCDe
			  ^   ^    ^  ^  ^  ^   ^  ^    ^  ^
 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::GetPartialWordStart
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty() || index.charIndex <= 1)
		{
		return TextIndex(1,1);
		}

	const JIndex charIndex = JMin(index.charIndex, itsBuffer.GetCharacterCount()),
				 byteIndex = JMin(index.byteIndex, itsBuffer.GetByteCount());

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartAfter, charIndex, byteIndex);

	JUtf8Character c;
	if (iter.Prev(&c, kJFalse) && !c.IsAlnum())
		{
		while (iter.Prev(&c) && !c.IsAlnum())
			{
			// find end of word
			}
		}
	else if (!iter.Prev(&c))
		{
		return TextIndex(1,1);
		}

	JUtf8Character prev = c;
	JBoolean foundLower = prev.IsLower();
	while (iter.Prev(&c))
		{
		foundLower = JI2B(foundLower || c.IsLower());
		if (!c.IsAlnum() ||
			(prev.IsUpper() && c.IsLower()) ||
			(prev.IsUpper() && c.IsUpper() && foundLower) ||
			(prev.IsAlpha() && c.IsDigit()) ||
			(prev.IsDigit() && c.IsAlpha()))
			{
			iter.SkipNext();
			break;
			}

		prev = c;
		}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetPartialWordEnd

	Return the index of the last character of the partial word at the given
	location.  This function is required to work for charIndex > buffer length.

	Example:  get_word Get142TheWordABCGood
				^    ^   ^  ^  ^   ^  ^   ^
 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::GetPartialWordEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty())
		{
		return TextIndex(1,1);
		}

	const JSize bufLen = itsBuffer.GetCharacterCount();
	if (index.charIndex >= bufLen)
		{
		return TextIndex(bufLen, itsBuffer.GetByteCount());
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	JUtf8Character c;
	if (iter.Next(&c, kJFalse) && !c.IsAlnum())
		{
		while (iter.Next(&c) && !c.IsAlnum())
			{
			// find start of word
			}
		}
	else if (!iter.Next(&c))
		{
		return TextIndex(1,1);
		}

	JUtf8Character prev = c, c2;
	while (iter.Next(&c))
		{
		if (!c.IsAlnum() ||
			(prev.IsLower() && c.IsUpper()) ||
			(prev.IsAlpha() && c.IsDigit()) ||
			(prev.IsDigit() && c.IsAlpha()) ||
			(iter.Next(&c2, kJFalse) &&
			 prev.IsUpper() && c.IsUpper() && c2.IsLower()))
			{
			iter.SkipPrev();
			break;
			}

		prev = c;
		}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetParagraphStart

	Return the index of the first character in the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex == 0.

 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::GetParagraphStart
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty() || index.charIndex <= 1)
		{
		return TextIndex(1,1);
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	JUtf8Character c;
	while (iter.Prev(&c) && c != '\n')
		{
		// find start of paragraph
		}

	if (!iter.AtBeginning())
		{
		iter.SkipNext();	// skip over newline
		}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetParagraphEnd

	Return the index of the newline that ends the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex > buffer length.

 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::GetParagraphEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsBuffer.IsEmpty())
		{
		return TextIndex(1,1);
		}

	const JSize bufLen = itsBuffer.GetCharacterCount();
	if (index.charIndex >= bufLen)
		{
		return TextIndex(bufLen, itsBuffer.GetByteCount());
		}

	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	JUtf8Character c;
	while (iter.Next(&c) && c != '\n')
		{
		// find end of paragraph
		}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetColumnForChar

	Returns the column that the specified character is in, given the start
	of the line.  If the caret is at the far left, it is column 1.

	Since this is only useful with monospace fonts, the CRM tab width is
	used to calculate the column when tabs are encountered, by calling
	CRMGetTabWidth().

 ******************************************************************************/

JIndex
JStyledTextBuffer::GetColumnForChar
	(
	const TextIndex& lineStart,
	const TextIndex& location
	)
	const
{
	if (location.charIndex > itsBuffer.GetCharacterCount() && EndsWithNewline())
		{
		return 1;
		}

	// create separate object on which to iterate, without copying
	const JString s(itsBuffer.GetBytes(), JUtf8ByteRange(lineStart.byteIndex, location.byteIndex-1), kJFalse);
	JStringIterator iter(s);

	JIndex col = 1;
	JUtf8Character c;
	while (iter.Next(&c))
		{
		col += (c == '\t' ? CRMGetTabWidth(col) : 1);
		}

	return col;
}

/******************************************************************************
 AdjustTextIndex

	Optimized by starting JStringIterator at known TextIndex.

 ******************************************************************************/

JStyledTextBuffer::TextIndex
JStyledTextBuffer::AdjustTextIndex
	(
	const TextIndex&	index,
	const JInteger		charDelta
	)
	const
{
	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	if (charDelta > 0)
		{
		iter.SkipNext(charDelta);
		}
	else
		{
		iter.SkipPrev(-charDelta);
		}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 CharToByteRange (protected)

	Optimized by starting JStringIterator at start of line, computed by
	using binary search.

	lineStart can be NULL

 ******************************************************************************/

JUtf8ByteRange
JStyledTextBuffer::CharToByteRange
	(
	const TextIndex*		lineStart,
	const JCharacterRange&	charRange
	)
	const
{
	assert( !charRange.IsEmpty() );
	assert( itsBuffer.RangeValid(charRange) );

	JStringIterator iter(itsBuffer);
	if (lineStart != NULL)
		{
		iter.UnsafeMoveTo(kJIteratorStartBefore, lineStart->charIndex, lineStart->byteIndex);
		}

	return CharToByteRange(charRange, &iter);
}

/******************************************************************************
 CharToByteRange (static private)

	If JStringIterator is not AtBeginning(), assumes position is optimized.
	Otherwise, optimizes by starting JStringIterator at end closest to
	range.

 ******************************************************************************/

JUtf8ByteRange
JStyledTextBuffer::CharToByteRange
	(
	const JCharacterRange&	charRange,
	JStringIterator*		iter
	)
{
	if (iter->AtBeginning())
		{
		const JString& s = iter->GetString();
		const JSize d1   = charRange.first;
		const JSize d2   = s.GetCharacterCount() - charRange.last;
		if (d2 < d1)
			{
			iter->MoveTo(kJIteratorStartAtEnd, 0);
			}
		}

	JIndex i1, i2;
	if (iter->AtEnd())
		{
		iter->MoveTo(kJIteratorStartAfter, charRange.last);
		i2 = iter->GetPrevByteIndex();
		iter->MoveTo(kJIteratorStartBefore, charRange.first);
		i1 = iter->GetNextByteIndex();
		}
	else
		{
		iter->MoveTo(kJIteratorStartBefore, charRange.first);
		i1 = iter->GetNextByteIndex();
		iter->MoveTo(kJIteratorStartAfter, charRange.last);
		i2 = iter->GetPrevByteIndex();
		}

	return JUtf8ByteRange(i1, i2);
}

/******************************************************************************
 CalcInsertionFont

	Returns the font to use when inserting at the specified point.

 ******************************************************************************/

JFont
JStyledTextBuffer::CalcInsertionFont
	(
	const TextIndex& index
	)
	const
{
	JStringIterator iter(itsBuffer);
	iter.UnsafeMoveTo(kJIteratorStartBefore, index.charIndex, index.byteIndex);

	return CalcInsertionFont(iter, *itsStyles);
}

// protected

JFont
JStyledTextBuffer::CalcInsertionFont
	(
	JStringIterator&		buffer,
	const JRunArray<JFont>&	styles
	)
	const
{
	JUtf8Character c;
	if (buffer.Prev(&c, kJFalse) && c == '\n')
		{
		return styles.GetElement(buffer.GetNextCharacterIndex());
		}
	else if (!buffer.AtBeginning())
		{
		return styles.GetElement(buffer.GetPrevCharacterIndex());
		}
	else if (!styles.IsEmpty())
		{
		return styles.GetElement(1);
		}
	else
		{
		return itsDefFont;
		}
}

/******************************************************************************
 CompareCharacterIndices (static)

 ******************************************************************************/

JListT::CompareResult
JStyledTextBuffer::CompareCharacterIndices
	(
	const TextIndex& i,
	const TextIndex& j
	)
{
	return JCompareIndices(i.charIndex, j.charIndex);
}

/******************************************************************************
 CompareByteIndices (static)

 ******************************************************************************/

JListT::CompareResult
JStyledTextBuffer::CompareByteIndices
	(
	const TextIndex& i,
	const TextIndex& j
	)
{
	return JCompareIndices(i.byteIndex, j.byteIndex);
}