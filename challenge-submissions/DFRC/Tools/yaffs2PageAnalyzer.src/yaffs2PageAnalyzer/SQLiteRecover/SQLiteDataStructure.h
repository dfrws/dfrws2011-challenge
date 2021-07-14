/**
 *	@file	SQLiteDataStructure.h
 *	@date	2011/06/17
 *	@author	Sangjun Jeon (heros86@korea.ac.kr)

 *	@brief	SQLite Data Structure
 */

/**
	@mainpage	SQLite Data Structure

	@section	MODIFYINFO	History

		- Sangjun Jeon / 2011.06.17
			- Initial Version
		- Jungheum Park / 2011.06.27
			- Add & Modify some codes for supporting DFRWS Challenges
 */

#pragma once
#include "..\stdafx.h"

//UserDefine
////////////

#define ENCODED_UTF8			1
#define ENCODED_LITTLE_UTF16	2
#define ENCODED_BIG_UTF16		3

#define TYPE_TEXT				0
#define TYPE_INTEGER0			1
#define TYPE_INTEGER1			2
#define TYPE_INTEGER2			3
#define TYPE_INTEGER3			4
#define TYPE_INTEGER4			5
#define TYPE_INTEGER6			6
#define TYPE_INTEGER8			7
#define TYPE_NUMERIC			8
#define TYPE_BLOB				9
#define TYPE_NULL				10

#define MAX_QUERY_LEN			4096*2
#define MAX_FIELD_LEN			256
#define MAX_TABLE_NUM			2000

#define safe_free(p) { if(p) { free(p); (p) = NULL; } }

const UINT8 createtable_utf16_big_upper[24]={0,'c',0,'r',0,'e',0,'a',0,'t',0,'e',0,0x20,0,'t',0,'a',0,'b',0,'l',0,'e'};
const UINT8 createtable_utf16_big_lower[24]={0,'C',0,'R',0,'E',0,'A',0,'T',0,'E',0,0x20,0,'T',0,'A',0,'B',0,'L',0,'E'};
const UINT8 createtable_utf16_little_upper[24]={'c',0,'r',0,'e',0,'a',0,'t',0,'e',0,0x20,0,'t',0,'a',0,'b',0,'l',0,'e',0};
const UINT8 createtable_utf16_little_lower[24]={'C',0,'R',0,'E',0,'A',0,'T',0,'E',0,0x20,0,'T',0,'A',0,'B',0,'L',0,'E',0};

enum PROCESSRESULT{PROCESS_SUCCESS, PROCESS_FAIL};
enum DB_ERR{DB_ERROR, SELECTQUERY_ERROR, NO_DATA_ERROR};

#define big_2bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[1] << 0) + \
	((UINT64)((UINT8 *)(x))[0] << 8)) )

#define big_3bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[2] << 0) + \
	((UINT64)((UINT8 *)(x))[1] << 8) + \
	((UINT64)((UINT8 *)(x))[0] << 16)) )

#define big_4bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[3] << 0) + \
	((UINT64)((UINT8 *)(x))[2] << 8) + \
	((UINT64)((UINT8 *)(x))[1] << 16) + \
	((UINT64)((UINT8 *)(x))[0] << 24)) )

#define big_5bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[4] << 0) + \
	((UINT64)((UINT8 *)(x))[3] << 8) + \
	((UINT64)((UINT8 *)(x))[2] << 16) + \
	((UINT64)((UINT8 *)(x))[1] << 24) + \
	((UINT64)((UINT8 *)(x))[0] << 32)) )

#define big_6bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[5] << 0) + \
	((UINT64)((UINT8 *)(x))[4] << 8) + \
	((UINT64)((UINT8 *)(x))[3] << 16) + \
	((UINT64)((UINT8 *)(x))[2] << 24) + \
	((UINT64)((UINT8 *)(x))[1] << 32) + \
	((UINT64)((UINT8 *)(x))[0] << 40)) )

#define big_7bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[6] <<  0) + \
	((UINT64)((UINT8 *)(x))[5] << 8) + \
	((UINT64)((UINT8 *)(x))[4] << 16) + \
	((UINT64)((UINT8 *)(x))[3] << 24) + \
	((UINT64)((UINT8 *)(x))[2] << 32) + \
	((UINT64)((UINT8 *)(x))[1] << 40) + \
	((UINT64)((UINT8 *)(x))[0] << 48)) )

#define big_8bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[7] <<  0) + \
	((UINT64)((UINT8 *)(x))[6] <<  8) + \
	((UINT64)((UINT8 *)(x))[5] << 16) + \
	((UINT64)((UINT8 *)(x))[4] << 24) + \
	((UINT64)((UINT8 *)(x))[3] << 32) + \
	((UINT64)((UINT8 *)(x))[2] << 40) + \
	((UINT64)((UINT8 *)(x))[1] << 48) + \
	((UINT64)((UINT8 *)(x))[0] << 56)) )

#define little_2bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[0] << 0) + \
	((UINT64)((UINT8 *)(x))[1] << 8)) )

#define little_3bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[0] << 0) + \
	((UINT64)((UINT8 *)(x))[1] << 8) + \
	((UINT64)((UINT8 *)(x))[2] << 16)) )

#define little_4bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[0] << 0) + \
	((UINT64)((UINT8 *)(x))[1] << 8) + \
	((UINT64)((UINT8 *)(x))[2] << 16) + \
	((UINT64)((UINT8 *)(x))[3] << 24)) )

#define little_5bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[0] << 0) + \
	((UINT64)((UINT8 *)(x))[1] << 8) + \
	((UINT64)((UINT8 *)(x))[2] << 16) + \
	((UINT64)((UINT8 *)(x))[3] << 24) + \
	((UINT64)((UINT8 *)(x))[4] << 32)) )

#define little_6bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[0] << 0) + \
	((UINT64)((UINT8 *)(x))[1] << 8) + \
	((UINT64)((UINT8 *)(x))[2] << 16) + \
	((UINT64)((UINT8 *)(x))[3] << 24) + \
	((UINT64)((UINT8 *)(x))[4] << 32) + \
	((UINT64)((UINT8 *)(x))[5] << 40)) )

#define little_7bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[0] <<  0) + \
	((UINT64)((UINT8 *)(x))[1] << 8) + \
	((UINT64)((UINT8 *)(x))[2] << 16) + \
	((UINT64)((UINT8 *)(x))[3] << 24) + \
	((UINT64)((UINT8 *)(x))[4] << 32) + \
	((UINT64)((UINT8 *)(x))[5] << 40) + \
	((UINT64)((UINT8 *)(x))[6] << 48)) )

#define little_8bytetoint(x)   \
	(UINT64)( ((UINT64) \
	((UINT64)((UINT8 *)(x))[0] <<  0) + \
	((UINT64)((UINT8 *)(x))[1] <<  8) + \
	((UINT64)((UINT8 *)(x))[2] << 16) + \
	((UINT64)((UINT8 *)(x))[3] << 24) + \
	((UINT64)((UINT8 *)(x))[4] << 32) + \
	((UINT64)((UINT8 *)(x))[5] << 40) + \
	((UINT64)((UINT8 *)(x))[6] << 48) + \
	((UINT64)((UINT8 *)(x))[7] << 56)) )


//Data Structure
////////////////
struct VarLenInt
{
	UINT64 Value;
	UINT8 Length;
};

struct CellHeader
{
	VarLenInt	CellSize ;			//CellSize = Real CellSize - (CellSize Length + RowID Length)
	VarLenInt	RowID ;
	UINT16		DataLenAreaSize ;	//DataLenAreaSize = DataLenAreaSize Length(=2) + DatalenArea Length
	CArray <UINT64, UINT64> DataLenArr;
};

struct Field
{
	CString cid;						
	CString name;
	CString type;
	int		length;

	Field()
	{
		cid = _T("");
		name = _T("");
		type = _T("");
		length = 0;
	}
};

class ARRField : public CArray<Field, Field&> {	
public:
	void operator = (const ARRField& fieldArray) {
		RemoveAll();
		Append(fieldArray);
	}
};

typedef CArray<ARRField, ARRField&> ARRFieldArray;



class CRowArray : public CArray<CString, CString> {
public:
	void operator = (const CRowArray& RowArray) {
		RemoveAll();
		Append(RowArray);
	}
};

typedef CArray<CRowArray, CRowArray&> ARRRowArray;


// Type Array
class CTypeArray : public CArray<UINT, UINT> {	
public:
	void operator = (const CTypeArray& TypeArray) {
		RemoveAll();
		Append(TypeArray);
	}
};

typedef CArray<CTypeArray, CTypeArray&> ARRTypeArray;


struct TableData
{
	UINT64		pageOffset;

	CString		TableName;
	CString		RootPageNum;
	UINT		NumOfField;			// Number of Fields
	ARRFieldArray *FieldArray;
	ARRRowArray *NorRowArray;		// Array's Array
	ARRRowArray *DelRowArray;
};

typedef CArray<TableData, TableData&> ARRTableData;

