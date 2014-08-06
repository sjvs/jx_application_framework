/******************************************************************************
 GMessageHeader.cc

	BASE CLASS = <NONE>

	Copyright � 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include <GMessageHeader.h>
#include "GMMIMEParser.h"

#include <gMailUtils.h>

#include <JOrderedSetUtil.h>
#include <JPtrArray-JString.h>
#include <JRegex.h>
#include <jStreamUtil.h>

#include <ctype.h>
#include <time.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GMessageHeader::GMessageHeader()
{
	itsMessageStatus	= kNew;
	itsIsMIME			= kJFalse;
	itsHasAttachment	= kJFalse;
	itsReplyStatus		= kNoReply;
	itsMIMEParser		= NULL;
	itsSubjectAdjusted	= kJFalse;
	itsReplyToAdjusted	= kJFalse;
	itsCCAdjusted		= kJFalse;
	itsToAdjusted		= kJFalse;
	itsHasBaseSubject	= kJFalse;

	// these need to be initialized in case there is no Date header
	itsYear				= 0;
	itsDay				= 0;

}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMessageHeader::~GMessageHeader()
{
	delete itsMIMEParser;
}

/******************************************************************************
 SetHeader

 *****************************************************************************/

static const JRegex yearRegex("[[:space:]]+([0-9]+)$");
static const JRegex timeRegex1("([0-9][0-9]:[0-9][0-9]:[0-9][0-9])[[:space:]]+([0-9]+)");
static const JRegex timeRegex2("([0-9][0-9]:[0-9][0-9])[[:space:]]+([0-9]+)");
static const JRegex dateRegex("(Mon|Tue|Wed|Thu|Fri|Sat|Sun)[[:space:]]+(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)[[:space:]]+([0-9]+)");

void
GMessageHeader::SetHeader
	(
	const JString& header
	)
{
	itsHeader = header;
	itsHeader.TrimWhitespace();
	return;
	JArray<JIndexRange> subList;
	JBoolean matched = yearRegex.Match(itsHeader, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(2);
		JString year = itsHeader.GetSubstring(sRange);
		year.ConvertToUInt(&itsYear);
		}

	matched = timeRegex1.Match(itsHeader, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(2);
		itsTime = itsHeader.GetSubstring(sRange);
		if (itsYear == 0)
			{
			sRange = subList.GetElement(3);
			JString year = itsHeader.GetSubstring(sRange);
			year.ConvertToUInt(&itsYear);
			}
		}
	else
		{
		matched = timeRegex2.Match(itsHeader, &subList);
		if (matched)
			{
			JIndexRange sRange = subList.GetElement(2);
			itsTime = itsHeader.GetSubstring(sRange);
			if (itsYear == 0)
				{
				sRange = subList.GetElement(3);
				JString year = itsHeader.GetSubstring(sRange);
				year.ConvertToUInt(&itsYear);
				}
			}
		}
	matched = dateRegex.Match(itsHeader, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(2);
		JString dow	= itsHeader.GetSubstring(sRange);
		sRange		= subList.GetElement(3);
		itsMonth	= itsHeader.GetSubstring(sRange);
		sRange = subList.GetElement(4);
		JString day = itsHeader.GetSubstring(sRange);
		day.ConvertToUInt(&itsDay);
		itsShortDate	= itsMonth + " " + day;

		time_t now;
		struct tm *local_time;
		time(&now);
		local_time = localtime(&now);
		if (itsYear != (JIndex)(local_time->tm_year + 1900))
			{
			JString year	= JString(itsYear, JString::kBase10);
			itsShortDate += " " + year;
			}

		itsDate			= dow + ", " + day + " " + itsMonth + " " + JString(itsYear) + " " + itsTime;
		}
}

/******************************************************************************
 SetFrom

 *****************************************************************************/

void
GMessageHeader::SetFrom
	(
	const JString& from
	)
{
	itsFrom = from;
	DecodeMIMEHeader(&itsFrom);
	JPtrArray<JString> names(JPtrArrayT::kForgetAll);
	GGetRealNames(itsFrom, names);
	if (names.GetElementCount() > 0)
		{
		itsFromNames = *(names.NthElement(1));
		for (JSize i = 2; i <= names.GetElementCount(); i++)
			{
			itsFromNames += ", " + *(names.NthElement(i));
			}
		}
	else
		{
		itsFromNames = from;
		}
	names.DeleteAll();
}

/******************************************************************************
 SetAddress

 *****************************************************************************/

void
GMessageHeader::SetAddress
	(
	const JString& address
	)
{
	itsAddress = address;
}

/******************************************************************************
 SetSubject

 *****************************************************************************/

void
GMessageHeader::SetSubject
	(
	const JString& subject
	)
{
	itsSubject = subject;
	itsSubject.TrimWhitespace();
}

/******************************************************************************
 SetDate

 *****************************************************************************/

static const JRegex monthRegex1("^[a-zA-Z]+,[[:space:]]+([0-9]+)[[:space:]]+([a-zA-Z]+)");
static const JRegex monthRegex2("^([0-9]+)[[:space:]]+([a-zA-Z]+)");

void
GMessageHeader::SetDate
	(
	const JString& date
	)
{
	itsDate = date;
	itsDate.TrimWhitespace();
	const JSize count	= itsDate.GetLength();
	if (count == 0)
		{
		return;
		}
	JIndex findex;
	JIndex current	= 0;
	JString day;
	// dow and day of month
	if (itsDate.LocateSubstring(" ", &findex) && findex > 1)
		{
		// check for day of week (Mon|Monday|Tue...)
		if (itsDate.GetCharacter(findex - 1) == ',')
			{
			// ignore dow for now
			findex ++;
			while (findex <= itsDate.GetLength() &&
				   itsDate.GetCharacter(findex) == ' ')
				{
				findex++;
				}
			current	= findex;
			if (itsDate.LocateNextSubstring(" ", &findex) && 
				findex < count &&
				current < findex)
				{
				day	= itsDate.GetSubstring(current, findex - 1);
				}
			}
		else
			{
			day	= itsDate.GetSubstring(1, findex - 1);
			}
		if (!day.IsEmpty())
			{
			if (day.GetFirstCharacter() == '0')
				{
				day.RemoveSubstring(1,1);
				}
			}
		day.ConvertToUInt(&itsDay);

		findex ++;
		current	= findex;
		}
	else
		{
		return;
		}

	if (itsDate.LocateNextSubstring(" ", &findex) && findex > current)
		{
		itsMonth	= itsDate.GetSubstring(current, findex - 1);
		findex ++;
		current	= findex;
		}
	else
		{
		return;
		}

	if (itsDate.LocateNextSubstring(" ", &findex) && findex > current)
		{
		JString year	= itsDate.GetSubstring(current, findex - 1);
		year.ConvertToUInt(&itsYear);
		findex ++;
		current	= findex;
		}
	else
		{
		return;
		}

	if (itsDate.LocateNextSubstring(" ", &findex) && findex > current)
		{
		itsTime	= itsDate.GetSubstring(current, findex - 1);
		findex ++;
		current	= findex;
		}
	else
		{
		return;
		}

	itsShortDate = itsMonth + " " + day;
	time_t now;
	struct tm *local_time;
	time(&now);
	local_time = localtime(&now);
	if (itsYear != (JIndex)(local_time->tm_year + 1900))
		{
		JString year	= JString(itsYear, JString::kBase10);
		itsShortDate += " " + year;
		}

	return;			
	
/*	JArray<JIndexRange> subList;
	JString year	= JString(itsYear, JString::kBase10);
	JBoolean matched = monthRegex1.Match(itsDate, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(3);
		itsMonth = itsDate.GetSubstring(sRange);
		sRange = subList.GetElement(2);
		JString day = itsDate.GetSubstring(sRange);
		if (!day.IsEmpty())
			{
			if (day.GetFirstCharacter() == '0')
				{
				day.RemoveSubstring(1,1);
				}
			}
		day.ConvertToUInt(&itsDay);
		itsShortDate = itsMonth + " " + day;
		time_t now;
		struct tm *local_time;
		time(&now);
		local_time = localtime(&now);
		if (itsYear != (JIndex)(local_time->tm_year + 1900))
			{
			JString year	= JString(itsYear, JString::kBase10);
			itsShortDate += " " + year;
			}

		return;
		}
	matched = monthRegex2.Match(itsDate, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(3);
		itsMonth = itsDate.GetSubstring(sRange);
		sRange = subList.GetElement(2);
		JString day = itsDate.GetSubstring(sRange);
		if (!day.IsEmpty())
			{
			if (day.GetFirstCharacter() == '0')
				{
				day.RemoveSubstring(1,1);
				}
			}
		day.ConvertToUInt(&itsDay);
		itsShortDate = itsMonth + " " + day;
		time_t now;
		struct tm *local_time;
		time(&now);
		local_time = localtime(&now);
		if (itsYear != (JIndex)(local_time->tm_year + 1900))
			{
			JString year	= JString(itsYear, JString::kBase10);
			itsShortDate += " " + year;
			}
		return;
		}
	else
		{
		itsShortDate = itsDate;
		}*/
}

/******************************************************************************
 SetReplyTo

 *****************************************************************************/

void
GMessageHeader::SetReplyTo
	(
	const JString& replyto
	)
{
	itsReplyTo = replyto;
}

/******************************************************************************
 SetCC

 *****************************************************************************/

void
GMessageHeader::SetCC
	(
	const JString& cc
	)
{
	itsCC = cc;
}

/******************************************************************************
 SetTo

 *****************************************************************************/

void
GMessageHeader::SetTo
	(
	const JString& to
	)
{
	itsTo = to;
	itsTo.TrimWhitespace();
}

/******************************************************************************
 SetHeaderStart

 *****************************************************************************/

void
GMessageHeader::SetHeaderStart
	(
	const JIndex start
	)
{
	itsHeaderStart = start;
}

/******************************************************************************
 SetHeaderEnd

 *****************************************************************************/

void
GMessageHeader::SetHeaderEnd
	(
	const JIndex end
	)
{
	itsHeaderEnd = end;
}

/******************************************************************************
 SetMessageEnd

 *****************************************************************************/

void
GMessageHeader::SetMessageEnd
	(
	const JIndex end
	)
{
	itsMessageEnd = end;
}

/******************************************************************************
 SetMessageStatus

 *****************************************************************************/

void
GMessageHeader::SetMessageStatus
	(
	const MessageStatus status
	)
{
	itsMessageStatus = status;
}

/******************************************************************************
 SetIsMIME (public)

 ******************************************************************************/


void
GMessageHeader::SetIsMIME
	(
	const JBoolean mime
	)
{
	itsIsMIME	= mime;
}

/******************************************************************************
 SetHasAttachment (public)

 ******************************************************************************/

void
GMessageHeader::SetHasAttachment
	(
	const JBoolean attachment
	)
{
	itsHasAttachment	= attachment;
}

/******************************************************************************
 SetReplyStatus (public)

 ******************************************************************************/

void
GMessageHeader::SetReplyStatus
	(
	const ReplyStatus status
	)
{
	itsReplyStatus	= status;
}

/******************************************************************************
 SetMIMEParser (public)

 ******************************************************************************/

void
GMessageHeader::SetMIMEParser
	(
	GMMIMEParser* parser
	)
{
	itsMIMEParser	= parser;
}

/******************************************************************************
 SetAccount

 *****************************************************************************/

void
GMessageHeader::SetAccount
	(
	const JString& account
	)
{
	itsAccount = account;
	itsAccount.TrimWhitespace();
}

/******************************************************************************
 GetReceived

 ******************************************************************************/

const JString&
GMessageHeader::GetReceived()
	const
{
	return itsReceived;
}

/******************************************************************************
 SetReceived

 There are multiple received lines, so these are appended. This is used for
 matching/comparing headers

 ******************************************************************************/

void
GMessageHeader::SetReceived
	(
	const JString& received
	)
{
	itsReceived += received;
}

/******************************************************************************
 GetHeader

 *****************************************************************************/

const JString&
GMessageHeader::GetHeader()
	const
{
	return itsHeader;
}

/******************************************************************************
 GetFrom

 *****************************************************************************/

const JString&
GMessageHeader::GetFrom()
	const
{
	return itsFrom;
}

/******************************************************************************
 GetFromNames

 *****************************************************************************/

const JString&
GMessageHeader::GetFromNames()
	const
{
	return itsFromNames;
}

/******************************************************************************
 GetTo

 *****************************************************************************/

const JString&
GMessageHeader::GetTo()
	const
{
	if (!itsToAdjusted)
		{
		GMessageHeader* th	= const_cast<GMessageHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsTo));
		th->itsToAdjusted	= kJTrue;
		}
	return itsTo;
}

/******************************************************************************
 GetAddress

 *****************************************************************************/

const JString&
GMessageHeader::GetAddress()
	const
{
	return itsAddress;
}


/******************************************************************************
 GetSubject

 *****************************************************************************/

const JString&
GMessageHeader::GetSubject()
	const
{
	if (!itsSubjectAdjusted)
		{
		GMessageHeader* th	= const_cast<GMessageHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsSubject));
		th->itsSubjectAdjusted	= kJTrue;
		}
	return itsSubject;
}

/******************************************************************************
 GetBaseSubject

 *****************************************************************************/

static JRegex kFixSubjectRegex("^((re|fw|fwd)[:.])+(.*)");

const JString&
GMessageHeader::GetBaseSubject()
{
	if (itsHasBaseSubject)
		{
		return itsBaseSubject;
		}
	itsHasBaseSubject	= kJTrue;
	itsBaseSubject		= GetSubject();
	kFixSubjectRegex.SetCaseSensitive(kJFalse);
	JArray<JIndexRange> subList;
	while (kFixSubjectRegex.Match(itsBaseSubject, &subList))
		{
		itsBaseSubject	= itsBaseSubject.GetSubstring(subList.GetElement(subList.GetElementCount()));
		itsBaseSubject.TrimWhitespace();
		subList.RemoveAll();
		}
	const JSize length	= itsBaseSubject.GetLength();
	JIndex findex		= 1;
	while (findex <= length && 
		   !isalnum(itsBaseSubject.GetCharacter(findex)))
		{
		findex++;
		}
	if (findex > 1 && findex <= length)
		{
		itsBaseSubject	= itsBaseSubject.GetSubstring(findex, length);
		}
	return itsBaseSubject;
}


/******************************************************************************
 GetDate

 *****************************************************************************/

const JString&
GMessageHeader::GetDate()
	const
{
	return itsDate;
}

/******************************************************************************
 GetShortDate

 *****************************************************************************/

const JString&
GMessageHeader::GetShortDate()
	const
{
	return itsShortDate;
}

/******************************************************************************
 GetMonth

 *****************************************************************************/

const JString&
GMessageHeader::GetMonth()
	const
{
	return itsMonth;
}

/******************************************************************************
 GetYear

 *****************************************************************************/

JIndex
GMessageHeader::GetYear()
	const
{
	return itsYear;
}

/******************************************************************************
 GetDay

 *****************************************************************************/

JIndex
GMessageHeader::GetDay()
	const
{
	return itsDay;
}

/******************************************************************************
 GetTime

 *****************************************************************************/

const JString&
GMessageHeader::GetTime()
	const
{
	return itsTime;
}

/******************************************************************************
 GetReplyTo

 *****************************************************************************/

const JString&
GMessageHeader::GetReplyTo()
	const
{
	if (!itsReplyToAdjusted)
		{
		GMessageHeader* th	= const_cast<GMessageHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsReplyTo));
		th->itsReplyToAdjusted	= kJTrue;
		}
	return itsReplyTo;
}

/******************************************************************************
 GetCC

 *****************************************************************************/

const JString&
GMessageHeader::GetCC()
	const
{
	if (!itsCCAdjusted)
		{
		GMessageHeader* th	= const_cast<GMessageHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsCC));
		th->itsCCAdjusted	= kJTrue;
		}
	return itsCC;
}

/******************************************************************************
 GetHeaderStart

 *****************************************************************************/

JIndex
GMessageHeader::GetHeaderStart()
	const
{
	return itsHeaderStart;
}

/******************************************************************************
 GetHeaderEnd

 *****************************************************************************/

JIndex
GMessageHeader::GetHeaderEnd()
	const
{
	return itsHeaderEnd;
}

/******************************************************************************
 GetMessageEnd

 *****************************************************************************/

JIndex
GMessageHeader::GetMessageEnd()
	const
{
	return itsMessageEnd;
}



/******************************************************************************
 GetMessageStatus

 *****************************************************************************/

GMessageHeader::MessageStatus
GMessageHeader::GetMessageStatus()
	const
{
	return itsMessageStatus;
}

/******************************************************************************
 IsMIME (public)

 ******************************************************************************/

JBoolean
GMessageHeader::IsMIME()
	const
{
	return itsIsMIME;
}

/******************************************************************************
 HasAttachment (public)

 ******************************************************************************/

JBoolean
GMessageHeader::HasAttachment()
	const
{
	return itsHasAttachment;
}

/******************************************************************************
 GetReplyStatus (public)

 ******************************************************************************/

GMessageHeader::ReplyStatus
GMessageHeader::GetReplyStatus()
	const
{
	return itsReplyStatus;
}

/******************************************************************************
 GetMIMEParser (public)

 ******************************************************************************/

GMMIMEParser*
GMessageHeader::GetMIMEParser()
{
	return itsMIMEParser;
}

/******************************************************************************
 GetAccount

 *****************************************************************************/

const JString&
GMessageHeader::GetAccount()
	const
{
	return itsAccount;
}

/******************************************************************************
 DecodeMIMEHeader (private)

 ******************************************************************************/

static const JRegex encodedMIMEQRegex("=\\?(ISO|iso)-8859-[1-9]\\?[Q|q]\\?[^?]+\\?=");
static const JRegex encodedMIMEBRegex("=\\?(ISO|iso)-8859-[1-9]\\?[B|b]\\?[^?]+\\?=");

void
GMessageHeader::DecodeMIMEHeader
	(
	JString* header
	)
{
	JArray<JIndexRange> subList;
	JSize count	= encodedMIMEQRegex.MatchAll(*header, &subList);
	JBoolean qType	= kJTrue;
	if (count == 0)
		{
		count	= encodedMIMEBRegex.MatchAll(*header, &subList);
		qType	= kJFalse;
		}
	if (count > 0)
		{
		JString temp;
		JIndex mIndex		= 1;
		const JSize count	= subList.GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			JIndexRange range	= subList.GetElement(i);
//			if ((range.first != mIndex) &&
//				RangeContainsNWS(*header, mIndex, range.first))
//				{
//				temp += header->GetSubstring(mIndex, range.first - 1);
//				}
			if (range.first != mIndex)
				{
				JString trimmed = header->GetSubstring(mIndex, range.first - 1);
				trimmed.TrimWhitespace();
				if (!trimmed.IsEmpty())
					{
					temp += header->GetSubstring(mIndex, range.first - 1);
					}
				}
			temp   += DecodeMIMEWord(qType, header, range);
			mIndex	= range.last + 1;
			}
		if (mIndex < header->GetLength())
			{
			temp   += header->GetSubstring(mIndex, header->GetLength());
			}
		*header = temp;
		}
}

/******************************************************************************
 RangeContainsNWS (private)

 ******************************************************************************/

JBoolean
GMessageHeader::RangeContainsNWS
	(
	const JString&	text,
	const JIndex	index1,
	const JIndex	index2
	)
{
	for (JIndex i = index1; i <= index2; i++)
		{
		JCharacter c	= text.GetCharacter(i);
		if (!isspace(c) && (c != '\n'))
			{
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 DecodeMIMEWord (private)

  format:		=?iso-8859-1?q?encoded_text?=
  reference:	RFC 2047

 ******************************************************************************/

JString
GMessageHeader::DecodeMIMEWord
	(
	const JBoolean		qType,
	JString*			header,
	const JIndexRange	range
	)
{
	JString temp;
	JIndex findex	= range.first;
	// first ? '=?'
	JBoolean ok		= header->LocateNextSubstring("?", &findex);
	if (!ok)
		{
		return temp;
		}
	findex++;
	// second ? '?Q'
	ok		= header->LocateNextSubstring("?", &findex);
	if (!ok)
		{
		return temp;
		}
	// third ? 'Q?'
	findex++;
	ok		= header->LocateNextSubstring("?", &findex);
	if (!ok || (findex > range.last))
		{
		return temp;
		}
	JIndex endIndex	= findex + 1;
	// final ? '?='
	ok		= header->LocateNextSubstring("?", &endIndex);
	if (!ok || (endIndex > range.last))
		{
		return temp;
		}
	// so the encoded text is between findex and endIndex.
	if (qType)
		{
		JIndex dIndex	= findex + 1;
		while (dIndex < endIndex)
			{
			JCharacter c	= header->GetCharacter(dIndex);
			if (c == '=')
				{
				JString hex	= header->GetSubstring(dIndex + 1, dIndex + 2);
				hex.Prepend("0x");
				JInteger hexVal;
				if (hex.ConvertToInteger(&hexVal))
					{
					c	= (JCharacter)hexVal;
					temp.AppendCharacter(c);
					dIndex += 3;
					}
				}
			else if (c == '_')
				{
				temp.AppendCharacter(' ');
				dIndex++;
				}
			else
				{
				temp.AppendCharacter(c);
				dIndex++;
				}
			}
		}
	else
		{
		if (findex + 1 > header->GetLength())
			{
			return temp;
			}
		if (endIndex - 1 < findex + 1)
			{
			return temp;
			}
		temp = header->GetSubstring(findex + 1, endIndex - 1);
		const std::string s(temp.GetCString(), temp.GetLength());
		std::istringstream is(s);
		std::ostringstream os;
		JDecodeBase64(is, os);
		temp = os.str();
		}
	return temp;
}

/******************************************************************************
 AdjustPosition

 *****************************************************************************/

void
GMessageHeader::AdjustPosition
	(
	const JIndex newposition
	)
{
	int delta = itsHeaderStart - newposition;
	itsHeaderStart = newposition;
	itsHeaderEnd -= delta;
	itsMessageEnd -= delta;
}

/******************************************************************************
 CompareDates

 *****************************************************************************/

JOrderedSetT::CompareResult 	
GMessageHeader::CompareDates
	(
	GMessageHeader* const & h1,
	GMessageHeader* const & h2
	)
{
	int r = h1->GetYear() - h2->GetYear();
	if (r == 0)
		{
		if (h1->GetMonth() == h2->GetMonth())
			{
			r = h1->GetDay() - h2->GetDay();
			}
		else
			{
			JString month1 = h1->GetMonth();
			JString month2 = h2->GetMonth();
			JString months = "JanFebMarAprMayJunJulAugSepOctNovDec";
			JIndex val1;
			JIndex val2;
			if (months.LocateSubstring(month1, &val1) &&
				months.LocateSubstring(month2, &val2))
				{
				r = val1 - val2;
				}
			}
		}

	if (r == 0)
		{
		r = JStringCompare(h1->GetTime(), h2->GetTime(), kJFalse);
		}

	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 CompareFrom

 *****************************************************************************/

JOrderedSetT::CompareResult 	
GMessageHeader::CompareFrom
	(
	GMessageHeader* const & h1,
	GMessageHeader* const & h2
	)
{
	return JCompareStringsCaseInsensitive(&(h1->itsFromNames), &(h2->itsFromNames));
}

/******************************************************************************
 CompareSubject

 *****************************************************************************/

JOrderedSetT::CompareResult 	
GMessageHeader::CompareSubject
	(
	GMessageHeader* const & h1,
	GMessageHeader* const & h2
	)
{
	const int r	=  JStringCompare(h1->GetBaseSubject(), h2->GetBaseSubject(), kJFalse);
	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareDates(h1, h2);
		}
}

/******************************************************************************
 CompareSize

 *****************************************************************************/

JOrderedSetT::CompareResult 	
GMessageHeader::CompareSize
	(
	GMessageHeader* const & h1,
	GMessageHeader* const & h2
	)
{
	JSize s1	= h1->GetMessageEnd() - h1->GetHeaderEnd();
	JSize s2	= h2->GetMessageEnd() - h2->GetHeaderEnd();
	return JCompareSizes(s1, s2);
}


/******************************************************************************
 Stream operators

	The string data is delimited by double quotes:  "this is a string".

	To include double quotes in a string, use \"
	To include a backslash in a string, use \\

	An exception is made if the streams are cin or cout.
	For input, characters are simply read until 'return' is pressed.
	For output, Print() is used.

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	GMessageHeader&	header
	)
{
	JString temp;
	input >> temp;
	header.SetHeader(temp);
	input >> temp;
	header.SetFrom(temp);
	input >> temp;
	header.SetTo(temp);
	input >> temp;
	header.SetAddress(temp);
	input >> temp;
	header.SetSubject(temp);
	input >> temp;
	header.SetDate(temp);
	int tempi;
	input >> tempi;
	header.SetHeaderStart(tempi);
	input >> tempi;
	header.SetHeaderEnd(tempi);
	input >> tempi;
	header.SetMessageEnd(tempi);
	input >> tempi;
	header.SetMessageStatus((GMessageHeader::MessageStatus)tempi);
	input >> tempi;
	header.SetReplyStatus((GMessageHeader::ReplyStatus)tempi);
	JBoolean tempb;
	input >> tempb;
	header.SetIsMIME(tempb);
	input >> tempb;
	header.SetHasAttachment(tempb);
	input >> temp;
	header.SetReceived(temp);
	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const GMessageHeader&	header
	)
{
	output << header.GetHeader();
	output << header.GetFrom();
	output << header.GetTo();
	output << header.GetAddress();
	output << header.GetSubject();
	output << header.GetDate();
	output << ' ';
	output << header.GetHeaderStart();
	output << ' ';
	output << header.GetHeaderEnd();
	output << ' ';
	output << header.GetMessageEnd();
	output << ' ';
	GMessageHeader::MessageStatus status = header.GetMessageStatus();
	if (status == GMessageHeader::kDelete)
		{
		status = GMessageHeader::kRead;
		}
	output << (int)status;
	output << ' ';
	GMessageHeader::ReplyStatus rStatus	= header.GetReplyStatus();
	output << (int)rStatus;
	output << ' ';
	output << header.IsMIME() << ' ';
	output << header.HasAttachment() << ' ';
	output << header.GetReceived() << ' ';
	return output;
}
