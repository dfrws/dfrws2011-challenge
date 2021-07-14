/**
 *	@file	PageClassifier.cpp
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



#include "StdAfx.h"
#include "PageClassifier.h"
#include "FileCarving/FileCarving.h"
#include "RandomTest/RTest.h"
#include "SQLiteRecover/SQLiteDataStructure.h"


CPageClassifier::CPageClassifier(void)
{
	pageSizeYaffs = 2048;
	pageSizeSQLite = 1024;
	htmlCounts = 0;
	xmlCounts = 0;
}

CPageClassifier::CPageClassifier(UINT yaffs, UINT sqlite)
{
	pageSizeYaffs = yaffs;
	pageSizeSQLite = sqlite;
	htmlCounts = 0;
	xmlCounts = 0;
}

CPageClassifier::~CPageClassifier(void)
{
}

UINT CPageClassifier::RunPageClassifier(CString srcPath1, CString srtPath2, CString dstPath)
{
	CString strPath;

	// 1. Chunk Header Classification
	if(PathFileExists(srcPath1))
	{
		printf("\n START : Chunk Header Classification.");
		MetaPageFilter(srcPath1, dstPath);
		printf("\n COMPLETE : Chunk Header Classification.");
	}

	// 2. Hash-based Classification
	strPath.Format(_T("%s\\CHUNK_HEADER_FILTER\\ClassifiedPagesA.bin"), dstPath);

	if(PathFileExists(strPath))
	{
		printf("\n START : Hash-based Classification.");
		HashFilterSelf(strPath, dstPath);
		printf("\n COMPLETE : Hash-based Classification.");
	}
	
	// 3. Statistically-Based Classification
	
	strPath.Format(_T("%s\\HASH_FILTER\\ClassifiedPagesB.bin"), dstPath);

	if(PathFileExists(strPath))
	{
		printf("\n START : Statistically-Based Classification.");
		StatisticallyBasedClassifier(strPath, srtPath2, dstPath);
		printf("\n COMPLETE : Statistically-Based Classification.");
	}

	// 4. Non-Random Page Classification
	strPath.Format(_T("%s\\RANDOMNESS_TEST\\NonRandomPages.bin"), dstPath);

	if(PathFileExists(strPath))
	{
		printf("\n START : Non-Random Page Classification.");
		NonRandomPageClassifier(strPath, srtPath2, dstPath);
		printf("\n COMPLETE : Non-Random Page Classification.");
	}

	return TRUE;
}

UINT CPageClassifier::MetaPageFilter(CString srcPath, CString dstPath)
{
	// Make output directory
	dstPath += _T("\\CHUNK_HEADER_FILTER\\");
	SHCreateDirectoryEx(NULL, dstPath, NULL);
	
	CString strMeta;
	strMeta.Format(_T("%s\\ChunkHeaderPages.bin"), dstPath);

	CString strNonMeta;
	strNonMeta.Format(_T("%s\\ClassifiedPagesA.bin"), dstPath);


	CFile srcFile;
	if(srcFile.Open(srcPath, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	CFile dstFile1;
	if(dstFile1.Open(strMeta, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		srcFile.Close();
		return FALSE;
	}

	CFile dstFile2;
	if(dstFile2.Open(strNonMeta, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		srcFile.Close();
		dstFile1.Close();
		return FALSE;
	}


	// Filtering algorithm 1 : filtering yaff2 meta page 
	BYTE *buffer = (PBYTE)malloc(pageSizeYaffs);
	UINT totalPageCount = 0;
	UINT metaPageCount = 0;


	UINT   readSize = 0;
	UINT64 totalSize = 0;
	UINT64 srcFileSize = srcFile.GetLength();

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

		// verify yaff2 meta page
		if(IsMetaPage(buffer, readSize) == TRUE)
		{
			dstFile1.Write((PBYTE)buffer, readSize);
			metaPageCount++;
		}
		else
		{
			dstFile2.Write((PBYTE)buffer, readSize);
		}

		ZeroMemory(buffer, pageSizeYaffs);

		totalSize += readSize;
		totalPageCount++;
	}

	//printf("\n Total number of pages: %d\n", totalPageCount);
	//printf("\n Total number of meta pages: %d\n", metaPageCount);

	safe_free(buffer);
	srcFile.Close();
	dstFile1.Close();
	dstFile2.Close();
	return TRUE;
}

BOOL CPageClassifier::IsMetaPage(PBYTE buffer, UINT pageSize)
{
	// 128 ~ 144 = 0x00
	BYTE byXor = 0;

	for(int i = 0; i < 16; i++)
	{
		byXor = buffer[128+i] ^ byXor;
	}

	if(byXor != 0x00)
		return FALSE;


	// 1024 ~ 2048 = 0xFF
	byXor = 0;

	for(UINT i = 0; i < pageSize/2; i++)
	{
		if(buffer[pageSize/2+i] != 0xFF)
			return FALSE;
	}

	return TRUE;
}

UINT CPageClassifier::HashFilterSelf(CString srcPath, CString dstPath)
{
	// Make output directory
	dstPath += _T("\\HASH_FILTER\\");
	SHCreateDirectoryEx(NULL, dstPath, NULL);
	dstPath += _T("\\ClassifiedPagesB.bin");

	CFile srcFile;
	if(srcFile.Open(srcPath, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	CFile dstFile;
	if(dstFile.Open(dstPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		srcFile.Close();
		return FALSE;
	}

	ATL::CAtlMap<CAtlStringA, CStringA> hashMap;

	CStringA srtHash;
	CStringA strValue;

	BYTE *buffer = (PBYTE)malloc(pageSizeYaffs);
	UINT totalPageCount = 0;
	UINT duplicatePageCount = 0;

	UINT   readSize = 0;
	UINT64 totalSize = 0;
	UINT64 srcFileSize = srcFile.GetLength();

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

		srtHash = utils.DoSHA1(buffer, pageSizeYaffs);

		// Insert hash value into AtlMap
		if(hashMap.Lookup(srtHash, strValue))
		{// duplicate page does exist
			duplicatePageCount++;
		}
		else
		{
			hashMap.SetAt(srtHash, srtHash);
			// Write the page to dstFile
			dstFile.Write(buffer, readSize);
		}

		ZeroMemory(buffer, pageSizeYaffs);

		totalSize += readSize;
		totalPageCount++;
	}

	//printf("\n Total number of pages: %d\n", totalPageCount);
	//printf("\n Total number of duplicate pages: %d\n", duplicatePageCount);

	safe_free(buffer);
	srcFile.Close();
	dstFile.Close();
	return TRUE;
}

UINT CPageClassifier::StatisticallyBasedClassifier(CString srcPath1, CString srtPath2, CString dstPath)
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

		// Verify page signature
		int pageSignature = VerifyPageSignature(buffer, readSize);

		if(pageSignature == FORMAT_JPEG_EXIF || pageSignature == FORMAT_JPEG_JFIF 
			|| pageSignature == FORMAT_GIF || pageSignature == FORMAT_PNG || pageSignature == FORMAT_ZIP)
		{
			pageSignature = RANDOM_BLOCK;
			classifiedPageCount++;
		}
		else
		{
			// Randomness Test
			pageSignature = Randomness_Test((PCHAR)buffer, readSize);

			if(pageSignature == RANDOM_BLOCK)
			{
				classifiedPageCount++;
			}
		}

		ClassifyPage(buffer, readSize, pageSignature, dstPath);

		ZeroMemory(buffer, pageSizeYaffs);

		totalSize += readSize;
		totalPageCount++;
	}

	//printf("\n Total number of pages: %d\n", totalPageCount);
	//printf("\n Total number of classified random pages: %d\n", classifiedPageCount);

	safe_free(buffer);
	srcFile.Close();
	return TRUE;
}

UINT CPageClassifier::NonRandomPageClassifier( CString srcPath1, CString srtPath2, CString dstPath )
{
	SQLitePageClassifier(srcPath1, srtPath2, dstPath);

	OtherPageClassifier(srcPath1, srtPath2, dstPath);

	return TRUE;
}

UINT CPageClassifier::SQLitePageClassifier( CString srcPath1, CString srtPath2, CString dstPath )
{
	// Make output directory
	SHCreateDirectoryEx(NULL, dstPath, NULL);

	CFile srcFile;
	if(srcFile.Open(srcPath1, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	int	 ret = 0;
	BYTE *buffer = (PBYTE)malloc(pageSizeYaffs);
	UINT totalPageCount = 0;
	UINT classifiedPageCount1 = 0;
	UINT classifiedPageCount2 = 0;
	UINT classifiedPageCount3 = 0;
	UINT classifiedPageCount4 = 0;
	UINT pageSignature = 0;

	UINT   readSize = 0;
	UINT64 totalSize = 0;
	UINT64 srcFileSize = srcFile.GetLength();

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

		for(UINT i = 0; i < readSize/pageSizeSQLite; i++)
		{
			// Verify page signature
			pageSignature = VerifyPageSignature(buffer+(i*pageSizeSQLite), pageSizeSQLite);

			if(pageSignature == FORMAT_SQLITE)
			{
				if(i == 0)
				{
					pageSignature = ClassifySQLitePage(buffer, pageSizeYaffs, pageSignature, dstPath);
					if(pageSignature != FORMAT_UNKNOWN)
					{
						classifiedPageCount1++;
						break;
					}
				}
				else
				{
					pageSignature = FORMAT_UNKNOWN;
				}
			}
			else if(pageSignature >= FORMAT_SQLITE_TL_PAGE && pageSignature <= FORMAT_SQLITE_II_PAGE)
			{
				pageSignature = ClassifySQLitePage(buffer+(i*pageSizeSQLite), pageSizeSQLite, pageSignature, dstPath);
				if(pageSignature != FORMAT_UNKNOWN)
				{
					classifiedPageCount2++;
					continue;
				}
			}
			else
			{
				pageSignature = FORMAT_UNKNOWN;
			}

			if(i == 0)
			{
				pageSignature = FORMAT_SQLITE_EXCEPT;
				ClassifyPage(buffer, readSize, pageSignature, dstPath);
				classifiedPageCount3++;
				break;
			}
			else if(i != 0 && pageSignature == FORMAT_UNKNOWN)
			{// PAGE SLACK AREA OR OVERFLOW PAGE
				pageSignature = FORMAT_SQLITE_SLACK_PAGE;
				ClassifySQLitePage(buffer+(i*pageSizeSQLite), pageSizeSQLite, pageSignature, dstPath);
				classifiedPageCount4++;
				break;
			}
		}

		ZeroMemory(buffer, pageSizeYaffs);

		totalSize += readSize;
		totalPageCount++;
	}

	//printf("\n Total number of pages: %d\n", totalPageCount);
	//printf("\n Total number of classified SQLite pages: %d, %d, %d, %d\n", classifiedPageCount1, classifiedPageCount2, classifiedPageCount4, classifiedPageCount3);

	safe_free(buffer);
	srcFile.Close();
	return TRUE;
}

UINT CPageClassifier::OtherPageClassifier( CString srcPath1, CString srtPath2, CString dstPath )
{
	// Make output directory
	SHCreateDirectoryEx(NULL, dstPath, NULL);

	CFile srcFile;
	if(srcFile.Open(srcPath1, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	int	 ret = 0;
	BYTE *buffer = (PBYTE)malloc(pageSizeYaffs);
	UINT totalPageCount = 0;
	UINT classifiedPageCount = 0;
	UINT pageSignature = 0;

	UINT   readSize = 0;
	UINT64 totalSize = 0;
	UINT64 srcFileSize = srcFile.GetLength();

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

		// Verify page signature
		int pageSignature = VerifyPageSignature(buffer, readSize);
		if(pageSignature != FORMAT_UNKNOWN)
		{
			if(pageSignature < FORMAT_SQLITE)
			{
				ret = ClassifyPage(buffer, readSize, pageSignature, dstPath);
				if(ret != FORMAT_UNKNOWN && ret != FALSE)
				{
					classifiedPageCount++;
				}
			}
		}
		else
		{// For Strings
			ClassifyPage(buffer, readSize, pageSignature, dstPath);
		}

		ZeroMemory(buffer, pageSizeYaffs);

		totalSize += readSize;
		totalPageCount++;
	}

	//printf("\n Total number of pages: %d\n", totalPageCount);
	//printf("\n Total number of classified non-random pages: %d\n", classifiedPageCount);

	safe_free(buffer);
	srcFile.Close();
	return TRUE;
}

UINT CPageClassifier::ClassifyPage( PBYTE buffer, UINT readSize, UINT pageSignature, CString dstPath )
{
	if(pageSignature == FORMAT_HTML || pageSignature == FORMAT_XML)
	{
		dstPath += _T("\\HTML-XML\\");
		SHCreateDirectoryEx(NULL, dstPath, NULL);

		if(pageSignature == FORMAT_HTML)
		{
			htmlCounts++;
			dstPath.AppendFormat( _T("01.HTML%03d.htm"), htmlCounts);
		}
		else if(pageSignature == FORMAT_XML)
		{
			xmlCounts++;
			dstPath.AppendFormat( _T("02.XML%03d.xml"), xmlCounts);
		}
	}
	else if(pageSignature == RANDOM_BLOCK)
	{
		dstPath += _T("\\RANDOMNESS_TEST\\");
		SHCreateDirectoryEx(NULL, dstPath, NULL);
		dstPath.AppendFormat( _T("RandomPages.bin"));
	}
	else if(pageSignature == NON_RANDOM_BLOCK)
	{
		dstPath += _T("\\RANDOMNESS_TEST\\");
		SHCreateDirectoryEx(NULL, dstPath, NULL);
		dstPath.AppendFormat( _T("NonRandomPages.bin"));
	}
	else if(pageSignature == FORMAT_SQLITE_EXCEPT)
	{
		dstPath += _T("\\RANDOMNESS_TEST\\");
		SHCreateDirectoryEx(NULL, dstPath, NULL);
		dstPath.AppendFormat( _T("NonRandomPagesExceptSQLitePages.bin"));
	}
	else
	{
		dstPath += _T("\\UNKNOWN\\");
		SHCreateDirectoryEx(NULL, dstPath, NULL);
		dstPath.AppendFormat( _T("UnknownPages.bin"));
	}

	CFile dstFile;

	if(PathFileExists(dstPath))
	{
		if(dstFile.Open(dstPath, CFile::modeReadWrite) == FALSE)
		{
			return FALSE;
		}
	}
	else
	{
		if(dstFile.Open(dstPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
		{
			return FALSE;
		}
	}

	dstFile.SeekToEnd();

	dstFile.Write(buffer, readSize);

	dstFile.Close();

	return TRUE;
}

BOOL CPageClassifier::ClassifySQLitePage( PBYTE buffer, UINT pageSize, UINT pageSignature, CString dstPath )
{
	if(pageSignature >= FORMAT_SQLITE_TL_PAGE && pageSignature <= FORMAT_SQLITE_II_PAGE)
	{
		USHORT offset = 0;
		USHORT cellNumber = 0;

		// 1~2 : Byte offset into the page of the first free bloc
		offset = big_2bytetoint(&buffer[1]);

		// 3~4 : Number of cells on this page 
		cellNumber = big_2bytetoint(&buffer[3]);

		if(cellNumber > 0x00FF)
		{
			return FORMAT_UNKNOWN;
		}

		// 5~6 : Offset to the first byte of the cell content area.
		offset = big_2bytetoint(&buffer[5]);

		if(offset > pageSizeYaffs)
		{// page size = 1024 (0x400)
			return FORMAT_UNKNOWN;
		}
	}

	if(pageSignature == FORMAT_SQLITE_TI_PAGE || pageSignature == FORMAT_SQLITE_II_PAGE)
	{// Internal page
		USHORT offset = 0;
		USHORT cellNumber = 0;

		// 3~4 : Number of cells on this page 
		cellNumber = big_2bytetoint(&buffer[3]);

		// 12~13 : Byte offset of the first record
		offset = big_2bytetoint(&buffer[12]);

		if(offset > pageSizeYaffs)
		{// page size = 1024 (0x400)
			return FORMAT_UNKNOWN;
		}

		if(pageSignature == FORMAT_SQLITE_II_PAGE)
		{// 0x02 : Index Internal
			UINT pageNumber = 0;

			// 1~4 : Parent Page Numer
			pageNumber = big_4bytetoint(&buffer[1]);

			if(pageNumber == 0)
				return FORMAT_UNKNOWN;
		}
		else
		{// 0x05 : Table Internal
			if(cellNumber == 0 || offset == 0)
			{// Free page로 table leaf만 모으기 위함
				return FORMAT_UNKNOWN;
			}
		}
	}

	if(pageSignature == FORMAT_SQLITE_TL_PAGE || pageSignature == FORMAT_SQLITE_IL_PAGE)
	{// Leaf page
		USHORT offset = 0;
		USHORT cellNumber = 0;

		// 3~4 : Number of cells on this page 
		cellNumber = big_2bytetoint(&buffer[3]);

		// 8~9 : Byte offset of the first record
		offset = big_2bytetoint(&buffer[8]);

		if(offset > pageSizeYaffs)
		{// page size = 1024 (0x400)
			return FORMAT_UNKNOWN;
		}

		if(cellNumber == 0 || offset == 0)
		{// Free page로 table leaf만 모으기 위함
			if(pageSignature == FORMAT_SQLITE_TL_PAGE)
			{
				pageSignature = FORMAT_SQLITE_FREE_PAGE;
			}
			else
			{
				return FORMAT_UNKNOWN;
			}
		}
	}

	if(pageSignature == FORMAT_SQLITE)
	{// Main Header page

		// 16~17 : page size
		USHORT pageSize = 0;

		pageSize = big_2bytetoint(&buffer[16]);

		if(pageSize != 1024)
		{
			//Sleep(1);
		}
	}

	//--------------------------------------------
	// Classifiy SQLite pages

	dstPath += _T("\\SQLite\\");
	SHCreateDirectoryEx(NULL, dstPath, NULL);

	if(pageSignature == FORMAT_SQLITE)
	{
		dstPath += _T("01.SQLiteHeaderPages.bin");
	}
	else if(pageSignature == FORMAT_SQLITE_TL_PAGE)
	{
		dstPath += _T("02.TableLeafPages.bin");
	}
	else if(pageSignature == FORMAT_SQLITE_TI_PAGE)
	{
		dstPath += _T("03.TableInternalPages.bin");
	}
	else if(pageSignature == FORMAT_SQLITE_IL_PAGE)
	{
		dstPath += _T("04.IndexLeafPages.bin");
	}
	else if(pageSignature == FORMAT_SQLITE_II_PAGE)
	{
		dstPath += _T("05.IndexInternalPages.bin");
	}
	else if(pageSignature == FORMAT_SQLITE_SLACK_PAGE)
	{
		dstPath += _T("06.SlackOrOverflowPages.bin");
	}
	else if(pageSignature == FORMAT_SQLITE_FREE_PAGE)
	{
		dstPath += _T("07.FreePages.bin");
	}


	CFile dstFile;

	if(PathFileExists(dstPath))
	{
		if(dstFile.Open(dstPath, CFile::modeReadWrite) == FALSE)
		{
			return FALSE;
		}
	}
	else
	{
		if(dstFile.Open(dstPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
		{
			return FALSE;
		}
	}

	dstFile.SeekToEnd();

	dstFile.Write(buffer, pageSize);

	dstFile.Close();

	return pageSignature;
}

UINT CPageClassifier::VerifyPageSignature(PBYTE pvBuffer, UINT pageSize)
{
	if(!memcmp(ZIP_sigHEADER, pvBuffer, sizeof(ZIP_sigHEADER))) 
	{
		return FORMAT_ZIP;
	}

	if(!memcmp(JPEG_EXIF_sigHEADER, pvBuffer, sizeof(JPEG_EXIF_sigHEADER)))
	{
		return FORMAT_JPEG_EXIF;
	}

	if(!memcmp(JPEG_JFIF_sigHEADER, pvBuffer, sizeof(JPEG_JFIF_sigHEADER))) 
	{
		return FORMAT_JPEG_JFIF;
	}

	if( !memcmp(GIF_sigHEADER1, pvBuffer, sizeof(GIF_sigHEADER1)) || 
		!memcmp(GIF_sigHEADER2, pvBuffer, sizeof(GIF_sigHEADER2))) 
	{
		return FORMAT_GIF;
	}

	if(!memcmp(PNG_sigHEADER, pvBuffer, sizeof(PNG_sigHEADER)))
	{
		return FORMAT_PNG;
	}

	if(!memcmp(PDF_sigHEADER, pvBuffer, sizeof(PDF_sigHEADER))) 
	{
		return FORMAT_PDF;
	}

	if (!memcmp(HTML_sigHEADER1, pvBuffer, strlen(HTML_sigHEADER1)) ||
		!memcmp(HTML_sigHEADER2, pvBuffer, strlen(HTML_sigHEADER2)) ||
		!memcmp(HTML_sigHEADER3, pvBuffer, strlen(HTML_sigHEADER3)) ||
		!memcmp(HTML_sigHEADER4, pvBuffer, strlen(HTML_sigHEADER4))) 
	{
		return FORMAT_HTML;
	}

	if(!memcmp(XML_sigHEADER, pvBuffer, strlen(XML_sigHEADER))) 
	{
		return FORMAT_XML;
	}

	//---------------------------
	// SQLite
	if(!memcmp(SQLITE_sigHeader, pvBuffer, sizeof(SQLITE_sigHeader))) 
	{
		return FORMAT_SQLITE;
	}

	const BYTE	SQLITE_II_PAGE_sigHEADER[1] = {0x02}; // "SQLite Index B-Tree Internal Page"
	if(!memcmp(SQLITE_II_PAGE_sigHEADER, pvBuffer, sizeof(SQLITE_II_PAGE_sigHEADER))) 
	{
		return FORMAT_SQLITE_II_PAGE;
	}

	const BYTE	SQLITE_IL_PAGE_sigHEADER[1] = {0x0A}; // "SQLite Index B-Tree Leaf Page"
	if(!memcmp(SQLITE_IL_PAGE_sigHEADER, pvBuffer, sizeof(SQLITE_IL_PAGE_sigHEADER))) 
	{
		return FORMAT_SQLITE_IL_PAGE;
	}

	const BYTE	SQLITE_TI_PAGE_sigHEADER[1] = {0x05}; // "SQLite Table B-Tree Internal Page"
	if(!memcmp(SQLITE_TI_PAGE_sigHEADER, pvBuffer, sizeof(SQLITE_TI_PAGE_sigHEADER))) 
	{
		return FORMAT_SQLITE_TI_PAGE;
	}

	const BYTE	SQLITE_TL_PAGE_sigHEADER[1] = {0x0D}; // "SQLite Table B-Tree Leaf Page"
	if(!memcmp(SQLITE_TL_PAGE_sigHEADER, pvBuffer, sizeof(SQLITE_TL_PAGE_sigHEADER))) 
	{
		return FORMAT_SQLITE_TL_PAGE;
	}

	return FORMAT_UNKNOWN;
}


UINT CPageClassifier::RandomPageClassifier( CString srcPath1, CString srtPath2, CString dstPath )
{
	return TRUE;
}

