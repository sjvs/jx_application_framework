#ifndef _H_JMMHashTable
#define _H_JMMHashTable

/******************************************************************************
 JMMHashTable.h

	Interface for the JMMHashTable class.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "JMMTable.h"
#include "JHashTable.h"

class JMMHashTable : public JMMTable
{
public:

	JMMHashTable(JMemoryManager* manager, const JBoolean recordDelete);
	virtual ~JMMHashTable();

	virtual JSize GetAllocatedCount() const;
	virtual JSize GetAllocatedBytes() const;
	virtual JSize GetDeletedCount() const;
	virtual JSize GetTotalCount() const;

	virtual void PrintAllocated(const JBoolean printInternal = kJFalse) const;
	virtual void StreamAllocatedForDebug(std::ostream& output, const JMemoryManager::RecordFilter& filter) const;
	virtual void StreamAllocationSizeHistogram(std::ostream& output) const;

protected:

	virtual void _CancelRecordDeallocated();

	virtual void _AddNewRecord(const JMMRecord& record,
									   const JBoolean checkDoubleAllocation);

	virtual JBoolean _SetRecordDeleted(JMMRecord* record, const void* block,
									   const JUtf8Byte* file, const JUInt32 line,
									   const JBoolean isArray);

private:

	JHashTable<JMMRecord>*	itsAllocatedTable;
	JSize					itsAllocatedBytes;
	JHashTable<JMMRecord>*	itsDeletedTable;
	JSize					itsDeletedCount;	// used if itsDeletedTable==nullptr

private:

	// not allowed

	JMMHashTable(const JMMHashTable& source);
	const JMMHashTable& operator=(const JMMHashTable& source);
};

#endif
