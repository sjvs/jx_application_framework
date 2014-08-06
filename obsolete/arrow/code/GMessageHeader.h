/******************************************************************************
 GMessageHeader.h

	Interface for the GMessageHeader class.

	Copyright � 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GMessageHeader
#define _H_GMessageHeader

#include <JString.h>

class GMMIMEParser;

class GMessageHeader
{
public:

	friend istream& operator>>(istream&, GMessageHeader&);
	friend ostream& operator<<(ostream&, const GMessageHeader&);

public:

	enum MessageStatus
		{
		kNew = 1,
		kRead,
		kUrgent,
		kImportant,
		kDelete
		};

	enum ReplyStatus
		{
		kNoReply = 1,
		kReplied,
		kRepliedSender,
		kRepliedAll,
		kForwarded,
		kRedirected,
		};

public:

	GMessageHeader();
	virtual ~GMessageHeader();

	void SetHeader(const JString& header);
	void SetFrom(const JString& from);
	void SetAddress(const JString& address);
	void SetSubject(const JString& subject);
	void SetDate(const JString& date);
	void SetReplyTo(const JString& replyto);
	void SetCC(const JString& cc);
	void SetTo(const JString& to);
	void SetHeaderStart(const JIndex start);
	void SetHeaderEnd(const JIndex end);
	void SetMessageEnd(const JIndex end);
	void SetMessageStatus(const MessageStatus status);
	void SetIsMIME(const JBoolean mime);
	void SetHasAttachment(const JBoolean attachment);
	void SetReplyStatus(const ReplyStatus status);
	void SetMIMEParser(GMMIMEParser* parser);
	void SetAccount(const JString& account);
	void SetReceived(const JString& received);

	const JString&	GetHeader() const;
	const JString&	GetFrom() const;
	const JString&	GetFromNames() const;
	const JString&	GetAddress() const;
	const JString&	GetSubject() const;
	const JString&	GetBaseSubject();
	const JString&	GetDate() const;
	const JString&	GetShortDate() const;
	const JString&	GetMonth() const;
	JIndex			GetYear() const;
	JIndex			GetDay() const;
	const JString&	GetTime() const;
	const JString&	GetReplyTo() const;
	const JString&	GetCC() const;
	const JString&	GetTo() const;
	JIndex			GetHeaderStart() const;
	JIndex			GetHeaderEnd() const;
	JIndex			GetMessageEnd() const;
	MessageStatus	GetMessageStatus() const;
	JBoolean		IsMIME() const;
	JBoolean		HasAttachment() const;
	ReplyStatus		GetReplyStatus() const;
	GMMIMEParser*	GetMIMEParser();
	const JString&	GetAccount() const;
	const JString&	GetReceived() const;

	void			AdjustPosition(const JIndex newposition);

	static JOrderedSetT::CompareResult
		CompareDates(GMessageHeader* const & h1, GMessageHeader* const & h2);

	static JOrderedSetT::CompareResult
		CompareFrom(GMessageHeader* const & h1, GMessageHeader* const & h2);

	static JOrderedSetT::CompareResult
		CompareSubject(GMessageHeader* const & h1, GMessageHeader* const & h2);

	static JOrderedSetT::CompareResult
		CompareSize(GMessageHeader* const & h1, GMessageHeader* const & h2);

private:

	JString		itsHeader;
	JString		itsFrom;
	JString		itsFromNames;
	JString		itsAddress;
	JString		itsSubject;
	JString		itsDate;
	JString		itsShortDate;
	JString		itsReplyTo;
	JString		itsCC;
	JString		itsTo;
	JIndex			itsHeaderStart;
	JIndex			itsHeaderEnd;
	JIndex			itsMessageEnd;
	JIndex			itsYear;
	JString			itsMonth;
	JIndex			itsDay;
	JString			itsTime;
	MessageStatus	itsMessageStatus;
	JBoolean		itsIsMIME;
	JBoolean		itsHasAttachment;
	ReplyStatus		itsReplyStatus;
	GMMIMEParser*	itsMIMEParser;
	JBoolean		itsSubjectAdjusted;
	JBoolean		itsReplyToAdjusted;
	JBoolean		itsCCAdjusted;
	JBoolean		itsToAdjusted;
	JString			itsAccount;
	JString			itsReceived;
	JBoolean		itsHasBaseSubject;
	JString			itsBaseSubject;

private:

	void		DecodeMIMEHeader(JString* header);
	JString		DecodeMIMEWord(const JBoolean qType, JString* header, const JIndexRange range);
	JBoolean	RangeContainsNWS(const JString& text, const JIndex index1, const JIndex index2);

	// not allowed

	GMessageHeader(const GMessageHeader& source);
	const GMessageHeader& operator=(const GMessageHeader& source);
};

#endif