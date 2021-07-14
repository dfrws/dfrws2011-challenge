/**
 *	@file	PageAnalyzer.cpp
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


#include "StdAfx.h"
#include "PageAnalyzer.h"
#include "SQLiteRecover/SQLiteRecover.h"
#include "SQLiteRecover/SQLiteDataStructure.h"


CPageAnalyzer::CPageAnalyzer(void)
{
	pageSizeYaffs = 2048;
	pageSizeSQLite = 1024;
}

CPageAnalyzer::CPageAnalyzer(UINT yaffs, UINT sqlite)
{
	pageSizeYaffs = yaffs;
	pageSizeSQLite = sqlite;
}


CPageAnalyzer::~CPageAnalyzer(void)
{
}

BOOL CPageAnalyzer::RunPageAnalyzer(CString srcPath1, CString srcPath2, CString dstPath)
{
	CString strPath;

	// SQLite Page Analyzer
	SQLitePageAnalyzer(srcPath1, srcPath2, dstPath);

	return TRUE;
}

UINT CPageAnalyzer::SQLitePageAnalyzer(CString srcPath1, CString srcPath2, CString dstPath)
{
	CString strPath;

	// 01.SQLiteHeaderPages.bin
	strPath.Format(_T("%s\\SQLite\\01.SQLiteHeaderPages.bin"), srcPath1);
	
	if(PathFileExists(strPath))
	{
		printf("\n START : SQLite Header Page Analysis.");
		SQLiteHeaderPages(strPath, srcPath2, dstPath);
		printf("\n COMPLETE : SQLite Header Page Analysis.");
	}

	// 02.TableLeafPages.bin
	strPath.Format(_T("%s\\SQLite\\02.TableLeafPages.bin"), srcPath1);

	if(PathFileExists(strPath))
	{
		printf("\n START : SQLite Table Leaf Page Analysis.");
		SQLiteTableLeafPages(strPath, srcPath2, dstPath);
		printf("\n COMPLETE : SQLite Table Leaf Page Analysis.");
	}

	return TRUE;
}

BOOL CPageAnalyzer::SQLiteHeaderPages(CString srcPath1, CString srcPath2, CString dstPath)
{
	// Make output directory
	SHCreateDirectoryEx(NULL, dstPath, NULL);

	CFile srcFile;
	if(srcFile.Open(srcPath1, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	BYTE *buffer = (PBYTE)malloc(pageSizeYaffs);
	UINT totalPageCount = 0;
	UINT classifiedPageCount = 0;
	UINT pageSignature = 0;

	UINT   readSize = 0;
	UINT64 totalSize = 0;
	UINT64 srcFileSize = srcFile.GetLength();

	ARRSQLiteHeader arrSH;

	while(srcFileSize > totalSize)
	{
		if(srcFileSize-totalSize < pageSizeYaffs)
			readSize = (UINT)(srcFileSize-totalSize);
		else
			readSize = pageSizeYaffs;

		if(srcFile.Read(buffer, readSize) != readSize)
		{
			break;
		}

		// Analyze SQLite header pages & Export the result to CSV
		SQLiteHeaderParser(buffer, readSize, &arrSH);

		ZeroMemory(buffer, pageSizeYaffs);

		totalSize += readSize;
		totalPageCount++;
	}

	safe_free(buffer);
	srcFile.Close();

	// Export the result to CSV
	dstPath += _T("\\SQLiteHeaders\\");
	SHCreateDirectoryEx(NULL, dstPath, NULL);

	dstPath += _T("SQLiteHeaderPages.csv");

	CFile exportFile;
	if(exportFile.Open(dstPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		return FALSE;
	}

	USHORT nUniSig = 0xfeff;
	exportFile.Write(&nUniSig, 2);

	utils.WriteUnicode(&exportFile, _T("pageSize"), FALSE);
	utils.WriteUnicode(&exportFile, _T("changeCount"), FALSE);
	utils.WriteUnicode(&exportFile, _T("firstFreeListPageNum"), FALSE);
	utils.WriteUnicode(&exportFile, _T("totalFreeListPages"), FALSE);
	utils.WriteUnicode(&exportFile, _T("schemaCookie"), FALSE);
	utils.WriteUnicode(&exportFile, _T("largestRootBtreePage"), FALSE);
	utils.WriteUnicode(&exportFile, _T("encoding"), FALSE);
	utils.WriteUnicode(&exportFile, _T("firstPageHeader"), FALSE);
	utils.WriteUnicode(&exportFile, _T("fileSize"), TRUE);

	CString strTemp;

	for(int i = 0; i < arrSH.GetSize(); i++)
	{
		strTemp.Format(_T("%d"), arrSH[i].pageSize);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		strTemp.Format(_T("%d"), arrSH[i].changeCount);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		strTemp.Format(_T("%d"), arrSH[i].firstFreeListPageNum);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		strTemp.Format(_T("%d"), arrSH[i].totalFreeListPages);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		strTemp.Format(_T("%d"), arrSH[i].schemaCookie);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		strTemp.Format(_T("%d"), arrSH[i].largestRootBtreePage);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		switch(arrSH[i].encoding)
		{
		case 1:
			strTemp = _T("UTF-8");
			break;			
		case 2:
			strTemp = _T("UTF-16le");
			break;			
		case 3:
			strTemp = _T("UTF-16be");
			break;			
		}

		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		strTemp.Format(_T("%02x  %d  %d  %d  %d"), arrSH[i].pageHeader.pageFlag, arrSH[i].pageHeader.firstFreeBlockOffset, arrSH[i].pageHeader.cellNumber,
			arrSH[i].pageHeader.firstCellOffset, arrSH[i].pageHeader.fragmentedFreeBytes);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

		strTemp.Format(_T("%d"), arrSH[i].fileSize);
		utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), TRUE);
	}

	exportFile.Close();
	return TRUE;
}

BOOL CPageAnalyzer::SQLiteHeaderParser(PBYTE buffer, UINT pageSize, ARRSQLiteHeader *arrSH)
{
	SQLiteHeader tSH;
	UINT currOffset = 0;
	UINT pageCount = 0;


	currOffset = 16;
	tSH.pageSize = big_2bytetoint(&buffer[currOffset]);

	currOffset = 24;
	tSH.changeCount = big_4bytetoint(&buffer[currOffset]);

	currOffset = 32;
	tSH.firstFreeListPageNum = big_4bytetoint(&buffer[currOffset]);

	currOffset = 36;
	tSH.totalFreeListPages = big_4bytetoint(&buffer[currOffset]);

	currOffset = 40;
	tSH.schemaCookie = big_4bytetoint(&buffer[currOffset]);

	currOffset = 52;
	tSH.largestRootBtreePage = big_4bytetoint(&buffer[currOffset]);

	currOffset = 56;
	tSH.encoding = big_4bytetoint(&buffer[currOffset]);

	// Offset 100 ~ 
	currOffset = 100;
	tSH.pageHeader.pageFlag = buffer[currOffset];

	currOffset = 101;
	tSH.pageHeader.firstFreeBlockOffset = big_2bytetoint(&buffer[currOffset]);

	currOffset = 103;
	tSH.pageHeader.cellNumber = big_2bytetoint(&buffer[currOffset]);

	currOffset = 105;
	tSH.pageHeader.firstCellOffset = big_2bytetoint(&buffer[currOffset]);

	currOffset = 107;
	tSH.pageHeader.fragmentedFreeBytes = big_2bytetoint(&buffer[currOffset]);


	// Calculate file size from Pointer Map page
	if(buffer[pageSize/2] == 0x01)	// A b-tree root page. The page number should be zero.
	{
		// Each 5-byte ptrmap entry consists of one byte of "page type" information followed by a 4-byte big-endian page number.
		currOffset = pageSize/2;

		while(buffer[currOffset] != 0x00)
		{
			pageCount++;

			currOffset += 5;

			if(currOffset >= pageSize)
				break;
		}
	}

	tSH.fileSize = (pageCount+2)*pageSize;

	arrSH->Add(tSH);
	return TRUE;
}

BOOL CPageAnalyzer::SQLiteTableLeafPages(CString srcPath1, CString srcPath2, CString dstPath)
{
	SQLiteRecover recoveryModule;
	recoveryModule.GetDataFromPages(srcPath1, pageSizeSQLite, ENCODED_UTF8);

	if(recoveryModule.arrTableData.GetSize() == 0)
		return FALSE;

	dstPath += _T("\\SQLiteRecords\\");
	SHCreateDirectoryEx(NULL, dstPath, NULL);

	// Export the result to CSV
	CString dstPathBak = dstPath;
	CString fileName;
	CStringA fileNameA;
	TableData tTableData;

	for(int index = 0; index < recoveryModule.arrTableData.GetSize(); index++)
	{
		tTableData = recoveryModule.arrTableData[index];

		CString strTemp;
		strTemp.Format(_T("%d"), tTableData.pageOffset);

		// Normal Records
		for(int i = 0; i < tTableData.NorRowArray->GetSize(); i++)
		{
			fileName = _T("");

			// CSV file naming : Concatenete all types
			for(int j = 0; j < tTableData.FieldArray->GetAt(i).GetSize(); j++)
			{
				Field field = tTableData.FieldArray->GetAt(i).GetAt(j);

				fileName.AppendFormat(_T("%X"), field.length);
			}

			fileNameA = utils.DoSHA1((LPBYTE)(LPCTSTR)fileName, fileName.GetLength()*2);

			fileName = fileNameA;
			fileName += _T(".csv");


			// Open or Create a CSV file
			dstPath = dstPathBak;
			dstPath += fileName;

			CFile exportFile;
			if(PathFileExists(dstPath))
			{
				if(exportFile.Open(dstPath, CFile::modeReadWrite) == FALSE)
				{
					return FALSE;
				}
			}
			else
			{
				if(exportFile.Open(dstPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
				{
					return FALSE;
				}

				USHORT nUniSig = 0xfeff;
				exportFile.Write(&nUniSig, 2);

				// Columns
				utils.WriteUnicode(&exportFile, utils.StringFilter(_T("PageOffset")), FALSE);
				utils.WriteUnicode(&exportFile, utils.StringFilter(_T("RowID")), FALSE);

				for(int j = 0; j < tTableData.FieldArray->GetAt(i).GetSize(); j++)
				{
					Field field = tTableData.FieldArray->GetAt(i).GetAt(j);

					if(j == tTableData.FieldArray->GetAt(i).GetSize()-1)
					{
						utils.WriteUnicode(&exportFile, utils.StringFilter(field.type), TRUE);
					}
					else
					{
						utils.WriteUnicode(&exportFile, utils.StringFilter(field.type), FALSE);
					}
				}
			}

			exportFile.SeekToEnd();

			CRowArray rowArr;
			rowArr = tTableData.NorRowArray->GetAt(i);

			if(rowArr.GetSize() > 0)
				utils.WriteUnicode(&exportFile, utils.StringFilter(strTemp), FALSE);

			for(int rowIdx = 0; rowIdx < rowArr.GetSize(); rowIdx++)
			{// Array of CStrings

				CString strRowItem;
				strRowItem = rowArr[rowIdx];

				if(rowIdx != 0)
				{
					if(tTableData.FieldArray->GetAt(i).GetSize() >= rowIdx)
					{
						Field field = tTableData.FieldArray->GetAt(i).GetAt(rowIdx-1);

						if(field.length == 6 && strRowItem != "NULL")
						{// UNIX Numeric
							CString strTime;

							time_t epoch = (time_t)_wtof(strRowItem);

							// New York [UTC -5] Daylight Saving Time +1 hour = [UTC -4] for DFRWS Challenges
							strTime = utils.SYSTEMTIMEtoCString(utils.Time_tToSYSTEMTIME(epoch/1000, -4));	

							strRowItem = strTime;
						}
					}
				}

				if(rowIdx == rowArr.GetSize()-1)
				{
					utils.WriteUnicode(&exportFile, utils.StringFilter(strRowItem), TRUE);
				}
				else
				{
					utils.WriteUnicode(&exportFile, utils.StringFilter(strRowItem), FALSE);
				}
			}

			exportFile.Close();
		}
	}

	return TRUE;
}

