/**
 *	@file	PageAnalyzer.h
 *	@date	2011/07/07
 *	@author	Jungheum Park

 *	@brief	YAFFS Page Analyzer
 */

/**
	@mainpage	YAFFS Page Analyzer

	@section	MODIFYINFO	History

		- Jungheum Park / 2011.07.07
			- Initial Version
 */


#pragma once
#include "stdafx.h"
#include "Utils/Utils.h"

typedef struct _SQLitePageHeader
{
	BYTE	pageFlag;				// A flag indicating the b-tree page type
	USHORT	firstFreeBlockOffset;	// Byte offset into the page of the first freeblock 
	USHORT	cellNumber;				// Number of cells on this page 
	USHORT	firstCellOffset;		// Offset to the first byte of the cell content area
	USHORT	fragmentedFreeBytes;	// Number of fragmented free bytes within the cell content area 

} SQLitePageHeader;

typedef struct _SQLiteHeader
{
	USHORT	pageSize;				// [16-17]
	UINT	changeCount;			// [24-27] File change counter. 
	UINT	firstFreeListPageNum;	// [32-35] Page number of the first freelist trunk page. 
	UINT	totalFreeListPages;		// [36-39] Total number of freelist pages. 
	UINT	schemaCookie;			// [40-43] The schema cookie. 
	UINT	largestRootBtreePage;	// [52-55] The page number of the largest root b-tree page 
									//		   when in auto-vacuum or incremental-vacuum modes, or zero otherwise.  
	UINT	encoding;				// [56-59] The database text encoding. A value of 1 means UTF-8. 
									//		   A value of 2 means UTF-16le. A value of 3 means UTF-16be. 

	SQLitePageHeader pageHeader;	// SQLite Page Header

	CString	dbSchema;				// Database schema

	UINT	fileSize;				// from Pointer Map page

} SQLiteHeader;

typedef CArray<SQLiteHeader, SQLiteHeader&>	ARRSQLiteHeader;


class CPageAnalyzer
{
public:
	CPageAnalyzer(void);
	CPageAnalyzer(UINT, UINT);
	~CPageAnalyzer(void);


public:
	UINT pageSizeYaffs;
	UINT pageSizeSQLite;

	CUtils utils;


public:
	// Public Function
	BOOL RunPageAnalyzer(CString srcPath1, CString srcPath2, CString dstPath);


private:
	// SQLite Page Analysis
	UINT SQLitePageAnalyzer(CString srcPath1, CString srcPath2, CString dstPath);
	BOOL SQLiteHeaderPages(CString srcPath1, CString srcPath2, CString dstPath);
	BOOL SQLiteHeaderParser(PBYTE buffer, UINT pageSize, ARRSQLiteHeader *arrSH);
	BOOL SQLiteTableLeafPages(CString srcPath1, CString srcPath2, CString dstPath);
};

