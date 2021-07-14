/**
 *	@file	SQLiteRecover.h
 *	@date	2011/06/17
 *	@author	Sangjun Jeon (heros86@korea.ac.kr)

 *	@brief	SQLite Recover
 */

/**
	@mainpage	SQLite Recover

	@section	MODIFYINFO	History

		- Sangjun Jeon / 2011.06.17
			- Initial Version
		- Jungheum Park / 2011.06.27
			- Add & Modify some functions for supporting DFRWS Challenges
 */

#pragma once
#include "SQLiteDataStructure.h"
#include "RecordPageExtractor.h"

// SQLiteRecover

class SQLiteRecover
{

public:
	SQLiteRecover();
	virtual ~SQLiteRecover();


public:
	UINT	EncodedType;
	UINT	PageSize;

	ARRTableData  arrTableData;
	

public:
	UINT GetDataFromPages(CString PageFileName, UINT PageSize_parm, UINT EncodedType_parm);
};


