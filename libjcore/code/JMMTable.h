#ifndef _H_JMMTable
#define _H_JMMTable

/******************************************************************************
 JMMTable.h

	Interface for the JMMTable class.

	Copyright � 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/


#include <JMemoryManager.h>

	class JMMRecord;

class JMMTable
{
public:

	JMMTable(JMemoryManager* manager);
	virtual ~JMMTable();

	void AddNewRecord(const JMMRecord& record,
	                  const JBoolean checkDoubleAllocation);

	JBoolean SetRecordDeleted(JMMRecord* record, const void* block,
	                          const JCharacter* file, const JUInt32 line,
	                          const JBoolean isArray);

	virtual void CancelRecordDeallocated();

	virtual JSize GetAllocatedCount() const = 0;
	virtual JSize GetAllocatedBytes() const = 0;
	virtual JSize GetDeletedCount() const = 0;
	virtual JSize GetTotalCount() const = 0;

	virtual void PrintAllocated(const JBoolean printInternal = kJFalse) const = 0;
	virtual void StreamAllocatedForDebug(ostream& output, const JMemoryManager::RecordFilter& filter) const = 0;
	virtual void StreamAllocationSizeHistogram(ostream& output) const = 0;

protected:

	virtual void _CancelRecordDeallocated() = 0;

	virtual void _AddNewRecord(const JMMRecord& record,
	                                   const JBoolean checkDoubleAllocation) = 0;

	virtual JBoolean _SetRecordDeleted(JMMRecord* record, const void* block,
	                                   const JCharacter* file, const JUInt32 line,
	                                   const JBoolean isArray) = 0;

	void BeginRecursiveBlock();
	void EndRecursiveBlock();

	// Message notification

	void NotifyObjectDeletedAsArray(const JMMRecord& record);
	void NotifyArrayDeletedAsObject(const JMMRecord& record);

	void NotifyUnallocatedDeletion(const JCharacter* file, const JUInt32 line,
	                               const JBoolean isArray);
	void NotifyMultipleDeletion(const JMMRecord& firstRecord, const JCharacter* file,
	                            const JUInt32 line, const JBoolean isArray);

	void NotifyMultipleAllocation(const JMMRecord& thisRecord,
	                              const JMMRecord& firstRecord);

	void PrintAllocatedRecord(const JMMRecord& record) const;
	void AddToHistogram(const JMMRecord& record, JSize histo[JMemoryManager::kHistogramSlotCount]) const;
	void StreamHistogram(ostream& output, const JSize histo[JMemoryManager::kHistogramSlotCount]) const;

private:

	JMemoryManager* const itsManager;

private:

	// not allowed

	JMMTable(const JMMTable& source);
	const JMMTable& operator=(const JMMTable& source);
};

#endif
