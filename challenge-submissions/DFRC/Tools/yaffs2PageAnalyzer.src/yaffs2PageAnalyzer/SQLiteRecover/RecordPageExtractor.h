/**
 *	@file	RecordPageExtractor.h
 *	@date	2011/06/17
 *	@author	Sangjun Jeon (heros86@korea.ac.kr)

 *	@brief	SQLite Record Page Extractor
 */

/**
	@mainpage	SQLite Record Page Extractor

	@section	MODIFYINFO	History

		- Sangjun Jeon / 2011.06.17
			- Initial Version
		- Jungheum Park / 2011.06.27
			- Add & Modify some functions for supporting DFRWS Challenges
 */

#pragma once
#include "SQLiteDataStructure.h"

// RecordPageExtractor

class RecordPageExtractor
{
public:
	RecordPageExtractor();
	RecordPageExtractor(UINT32 PageSize, UINT EncodedType);
	virtual ~RecordPageExtractor();

	/*
	*	External Variables
	*/
	CArray<CRowArray, CRowArray&>	NormalTable_InPage;
	CArray<CTypeArray, CTypeArray&>	Types_InPage;

	/*
	*	External Functions
	*/
	void GetNormalRecordSet(UINT8 *Buff);
	void GetDeletedRecordSet(UINT8 *Buff);

private:
	/*
	*	Internal Variables
	*/
	UINT EncodedType;
	CArray <UINT16, UINT16> NormalRecordOffsetArr;		
	UINT32 PageSize;

	/*
	*	Internal Functions
	*/
	UINT CalcValriableLengthInteger(UINT8 *Buff, UINT64 *Result);
	
	UINT16 NormalRecordParsing(UINT8 *Buff, CRowArray *RowArray, CTypeArray *TypeArray, UINT16 RemainedBuffSize);	
	
	char* CovertUTF16toMutil(char *UTF16str, int buflen);				

	UINT GetNormalRecordOffsets(UINT8 *Buff);
};