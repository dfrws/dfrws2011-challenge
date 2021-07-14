/**
 *	@file	PageClassifier.h
 *	@date	2011/06/25
 *	@author	Jungheum Park

 *	@brief	YAFFS Page Classifier
 */

/**
	@mainpage	YAFFS Page Classifier

	@section	MODIFYINFO	History

		- Jungheum Park / 2011.06.25
			- Initial Version
 */


#pragma once
#include "atlcoll.h"
#include "Utils/Utils.h"

#define MB	1024*1024
#define safe_free(p) { if(p) { free(p); (p) = NULL; } }


class CPageClassifier
{
public:
	CPageClassifier(void);
	CPageClassifier(UINT, UINT);

	~CPageClassifier(void);


public:
	UINT pageSizeYaffs;
	UINT pageSizeSQLite;

	UINT htmlCounts;
	UINT xmlCounts;

	CUtils utils;

public:
	// Public Function
	UINT RunPageClassifier(CString srcPath1, CString srtPath2, CString dstPath);


private:
	// Chunk Header Classification
	UINT MetaPageFilter(CString srcPath, CString dstPath);
	BOOL IsMetaPage(PBYTE buffer, UINT pageSize);


private:
	// Hash-based Classification
	UINT HashFilterSelf(CString srcPath, CString dstPath);


private:
	// Statistical-based Classification
	UINT StatisticallyBasedClassifier(CString srcPath1, CString srtPath2, CString dstPath);
	

private:
	// Non-random page Classification
	UINT NonRandomPageClassifier(CString srcPath1, CString srtPath2, CString dstPath);
	UINT SQLitePageClassifier(CString srcPath1, CString srtPath2, CString dstPath);
	UINT OtherPageClassifier(CString srcPath1, CString srtPath2, CString dstPath);
	UINT ClassifyPage(PBYTE buffer, UINT readSize, UINT pageSignature, CString dstPath);
	BOOL ClassifySQLitePage(PBYTE buffer, UINT pageSize, UINT pageSignature, CString dstPath);
	UINT VerifyPageSignature(PBYTE buffer, UINT pageSize);

private:
	// Random page Classification
	UINT RandomPageClassifier(CString srcPath1, CString srtPath2, CString dstPath);
};


