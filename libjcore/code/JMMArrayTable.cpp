/******************************************************************************
 JMMArrayTable.cpp

	A JMMTable implemented with JArrays.

	BASE CLASS = JMMTable

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include <JMMArrayTable.h>
#include <JMMRecord.h>
#include <jAssert.h>

	const JSize blockSize = 5000;

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMArrayTable::JMMArrayTable
	(
	JMemoryManager* manager,
	const JBoolean  recordDelete
	)
	:
	JMMTable(manager),
	itsAllocatedTable(NULL),
	itsAllocatedBytes(0),
	itsDeletedTable(NULL),
	itsDeletedCount(0)
{
	itsAllocatedTable = jnew JArray<JMMRecord>(blockSize);
	assert(itsAllocatedTable != NULL);

	if (recordDelete)
		{
		itsDeletedTable = jnew JArray<JMMRecord>(blockSize);
		assert(itsDeletedTable != NULL);
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMMArrayTable::~JMMArrayTable()
{
	jdelete itsAllocatedTable;
	itsAllocatedTable = NULL;

	jdelete itsDeletedTable;
	itsDeletedTable = NULL;
}

/******************************************************************************
 GetAllocatedCount (virtual)

 *****************************************************************************/

JSize
JMMArrayTable::GetAllocatedCount() const
{
	return itsAllocatedTable->GetElementCount();
}

/******************************************************************************
 GetAllocatedBytes (virtual)

 *****************************************************************************/

JSize
JMMArrayTable::GetAllocatedBytes() const
{
	return itsAllocatedBytes;
}

/******************************************************************************
 GetDeletedCount (virtual)

	Returns zero if the table is not recording deletions.

 *****************************************************************************/

JSize
JMMArrayTable::GetDeletedCount() const
{
	if (itsDeletedTable != NULL)
		{
		return itsDeletedTable->GetElementCount();
		}
	else
		{
		return itsDeletedCount;
		}
}

/******************************************************************************
 GetTotalCount (virtual)

 *****************************************************************************/

JSize
JMMArrayTable::GetTotalCount() const
{
	return GetAllocatedCount() + GetDeletedCount();
}

/******************************************************************************
 PrintAllocated (virtual)

 *****************************************************************************/

void
JMMArrayTable::PrintAllocated
	(
	const JBoolean printInternal // = kJFalse
	)
	const
{
	std::cout << "\nAllocated block statistics:" << std::endl;

	std::cout << "\nAllocated user memory:" << std::endl;

	const JSize count = itsAllocatedTable->GetElementCount();
	for (JIndex i=1;i<=count;i++)
		{
		const JMMRecord thisRecord = itsAllocatedTable->GetElement(i);
		if ( !thisRecord.IsManagerMemory() )
			{
			PrintAllocatedRecord(thisRecord);
			}
		}

	if (printInternal)
		{
		std::cout << "\nThe following blocks are probably owned by the memory manager"
			 << "\nand *should* still be allocated--please report all cases of user"
			 << "\nallocated memory showing up on this list!" << std::endl;

		for (JIndex i=1;i<=count;i++)
			{
			const JMMRecord thisRecord = itsAllocatedTable->GetElement(i);
			if ( thisRecord.IsManagerMemory() )
				{
				PrintAllocatedRecord(thisRecord);
				}
			}
		}
}

/******************************************************************************
 StreamAllocatedForDebug (virtual)

 *****************************************************************************/

void
JMMArrayTable::StreamAllocatedForDebug
	(
	std::ostream&							output,
	const JMemoryManager::RecordFilter&	filter
	)
	const
{
	const JSize count = itsAllocatedTable->GetElementCount();
	for (JIndex i=1;i<=count;i++)
		{
		const JMMRecord thisRecord = itsAllocatedTable->GetElement(i);
		if (filter.Match(thisRecord))
			{
			output << ' ' << kJTrue;
			output << ' ';
			thisRecord.StreamForDebug(output);
			}
		}

	output << ' ' << kJFalse;
}

/******************************************************************************
 StreamAllocationSizeHistogram (virtual)

 *****************************************************************************/

void
JMMArrayTable::StreamAllocationSizeHistogram
	(
	std::ostream& output
	)
	const
{
	JSize histo[ JMemoryManager::kHistogramSlotCount ];
	bzero(histo, sizeof(histo));

	const JSize count = itsAllocatedTable->GetElementCount();
	for (JIndex i=1;i<=count;i++)
		{
		AddToHistogram(itsAllocatedTable->GetElement(i), histo);
		}

	StreamHistogram(output, histo);
}

/******************************************************************************
 _CancelRecordDeallocated (virtual)

 *****************************************************************************/

void
JMMArrayTable::_CancelRecordDeallocated()
{
	if (itsDeletedTable != NULL)
		{
		itsDeletedCount = itsDeletedTable->GetElementCount();

		jdelete itsDeletedTable;
		itsDeletedTable = NULL;
		}
}

/******************************************************************************
 _AddNewRecord (virtual protected)

 *****************************************************************************/

void
JMMArrayTable::_AddNewRecord
	(
	const JMMRecord& record,
	const JBoolean   checkDoubleAllocation
	)
{
	JSize index = 0;

	if (checkDoubleAllocation)
		{
		index = FindAllocatedBlock( record.GetAddress() );
		}

	if (index == 0)
		{
		// Append because new allocations tend to be free'd the fastest
		itsAllocatedTable->AppendElement(record);
		}
	else
		{
		JMMRecord thisRecord = itsAllocatedTable->GetElement(index);
		itsAllocatedBytes   -= thisRecord.GetSize();

		NotifyMultipleAllocation(record, thisRecord);

		// Might as well trust malloc--the table should never have duplicate
		// entries!
		itsAllocatedTable->SetElement(index, record);
		}

	itsAllocatedBytes += record.GetSize();
}

/******************************************************************************
 _SetRecordDeleted

 *****************************************************************************/

JBoolean
JMMArrayTable::_SetRecordDeleted
	(
	JMMRecord*        record,
	const void*       block,
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	JSize index = FindAllocatedBlock(block);

	if (index != 0)
		{
		JMMRecord thisRecord = itsAllocatedTable->GetElement(index);
		thisRecord.SetDeleteLocation(file, line, isArray);
		itsAllocatedBytes -= thisRecord.GetSize();

		if (!thisRecord.ArrayNew() && isArray)
			{
			NotifyObjectDeletedAsArray(thisRecord);
			}
		else if (thisRecord.ArrayNew() && !isArray)
			{
			NotifyArrayDeletedAsObject(thisRecord);
			}

		itsAllocatedTable->RemoveElement(index);
		if (itsDeletedTable != NULL)
			{
			itsDeletedTable->AppendElement(thisRecord);
			}
		else
			{
			itsDeletedCount++;
			}

		*record = thisRecord;
		return kJTrue;
		}
	else
		{
		// Because the array is searched backwards, if it finds a block it
		// will be the most recent deallocation at that address
		index = FindDeletedBlock(block);
		if (index == 0)
			{
			NotifyUnallocatedDeletion(file, line, isArray);
			}
		else
			{
			JMMRecord thisRecord = itsDeletedTable->GetElement(index);

			NotifyMultipleDeletion(thisRecord, file, line, isArray);
			}

		return kJFalse;
		}
}

/******************************************************************************
 FindAllocatedBlock (private)

	Returns list index, or zero on failure.

 *****************************************************************************/

JSize
JMMArrayTable::FindAllocatedBlock
	(
	const void* block
	)
	const
{
	JSize allocatedCount = itsAllocatedTable->GetElementCount();
	for (JSize i=allocatedCount;i>=1;i--)
		{
		const JMMRecord thisRecord = itsAllocatedTable->GetElement(i);
		if (thisRecord.GetAddress() == block)
			{
			return i;
			}
		}

	return 0;
}

/******************************************************************************
 FindDeletedBlock (private)

	Returns list index, or zero on failure.

 *****************************************************************************/

JSize
JMMArrayTable::FindDeletedBlock
	(
	const void* block
	)
	const
{
	if (itsDeletedTable != NULL)
		{
		JSize deletedCount = itsDeletedTable->GetElementCount();
		for (JSize i=deletedCount;i>=1;i--)
			{
			const JMMRecord thisRecord = itsDeletedTable->GetElement(i);
			if (thisRecord.GetAddress() == block)
				{
				return i;
				}
			}
		}

	return 0;
}
