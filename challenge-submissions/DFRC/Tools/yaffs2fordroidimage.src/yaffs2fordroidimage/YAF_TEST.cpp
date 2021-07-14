/*
License: DFRC@KU

Copyright (c) 2011, Jewan Bang (jwbang@korea.ac.kr),
DFRC(Digital Forensic Research Center). All rights reserved.

Refer to AUTHORS for acknowledgements.

Redistribution and use in source and binary forms, with or without modification, 
are permitted for any non-commercial purpose provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- All advertising materials mentioning features or use of this software
  must acknowledge the contribution by people stated in the acknowledgements.

You may not use or distribute this Software or any derivative works in any form 
for commercial purposes. Examples of commercial purposes would be running business 
operations, licensing, leasing, or selling the Software, distributing the Software 
for use with commercial products, using the Software in the creation or use of 
commercial products or any other activity which purpose is to procure a commercial 
gain to you or others. 
  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL JUERGEN GALL AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#include "stdafx.h"
#include "YAF_TEST.h"
#include "ZTimeConv.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;
using namespace std;

	CArray<YAFFSSTRUCT, YAFFSSTRUCT> m_Array;
	BOOL GetYaffs2Files( CString CurrentFolderPath, unsigned int Odject_Id );
	BOOL ConstructFileChunk( CString CurrentFolderPath, YAFFSSTRUCT m_data);

	CString CurrentPath;
	CFile g_fp;
void DODADODA()
{

	m_Array.RemoveAll();

	CFile fp;
	ULONGLONG nOffset = 0;
	ULONGLONG totalsize = 0;

	if(!fp.Open(m_path, CFile::modeRead, NULL))
		return;

	totalsize = fp.GetLength();

	fp.SeekToBegin();

	TAG TAG_BUF;
	TAG_EXT TAG_EXT_BUF;
	CHUNKHEADER CHUNK_BUF;

	//printf("OFFSET, BLOCK ID, OBJECT ID, CHUNK ID, OBJECT TYPE, PARENT OBJECT ID, FILE SIZE, OBJECT TYPE, Deleted,FILE NAME,MODIFIED,ACCESSED,META MODIFIED\n");			
	
	while(1)
	{
		CString LastAccessTime;
		CString LastModifiedTime;
		CString LastMetaModifiedTime;

		memset(&CHUNK_BUF, 0x00, sizeof(CHUNK_BUF));
		memset(&TAG_EXT_BUF, 0x00, sizeof(TAG_EXT_BUF));
		memset(&TAG_BUF, 0x00, sizeof(TAG_BUF));

		int sdgsdg = sizeof(CHUNK_BUF);
		YAFFSSTRUCT tempStruct;
		tempStruct.OFFSET = fp.GetPosition();
		//printf("%d, ", fp.GetPosition());

		fp.Read( &CHUNK_BUF , 296 );

		fp.Seek( DEFAULT_CHUNKSIZE-296, CFile::current );

		fp.Read( &TAG_EXT_BUF , 64 );

		TAG_EXT_BUF.OBJECTTYPE = TAG_EXT_BUF.OBJECTID >> EXTRA_OBJECT_TYPE_SHIFT;
		TAG_EXT_BUF.OBJECTID &= ~EXTRA_OBJECT_TYPE_MASK;

		TAG_EXT_BUF.ExtendedTagFlag = TAG_EXT_BUF.PARENTOBJECTID >> EXTRA_OBJECT_TYPE_SHIFT;
		TAG_EXT_BUF.PARENTOBJECTID &= ~EXTRA_OBJECT_TYPE_MASK;	

		if ( TAG_EXT_BUF.ExtendedTagFlag != 0x08 )
		{
			memset( &TAG_BUF, 0x00, 64 );
			fp.Seek( -64, CFile::current );
			fp.Read( &TAG_BUF , 64 );

			TAG_BUF.OBJECTTYPE = TAG_BUF.OBJECTID >> EXTRA_OBJECT_TYPE_SHIFT;
			TAG_BUF.OBJECTID &= ~EXTRA_OBJECT_TYPE_MASK;

			nOffset = fp.GetPosition();

			//BLOCK ID, OBJECT ID, CHUNK ID, OBJECT TYPE, PARENT OBJECT ID, FILE SIZE, OBJECT TYPE, FILE NAME
			tempStruct.BLOCK_ID = TAG_BUF.BLOCKID;
			tempStruct.OBJECT_ID =  TAG_BUF.OBJECTID;
			tempStruct.CHUNK_ID = TAG_BUF.CHUNKID;
			//printf("%d,%d,%d,-,-,-,-,-,-,-,-,-\n", TAG_BUF.BLOCKID, TAG_BUF.OBJECTID, TAG_BUF.CHUNKID  );			

			USES_CONVERSION;
	
		} else if ( TAG_EXT_BUF.ExtendedTagFlag == 0x08 )
		{


			if ( CHUNK_BUF.OBJECTTYPE < 7 )
			{	//BLOCK ID, OBJECT ID, CHUNK ID, OBJECT TYPE, PARENT OBJECT ID, FILE SIZE, OBJECT TYPE, UNKOWN, FILE NAME

				CZTimeConv tz;
				SYSTEMTIME pLastModifiedTime, pLastMetaModifiedTime, pLastAccessTime;

				tz.ToSystemTime( &pLastModifiedTime,	(CHUNK_BUF.LastModifiedTime)  );
				tz.ToSystemTime( &pLastMetaModifiedTime,  (CHUNK_BUF.LastMetaModifiedTime)  );
				tz.ToSystemTime( &pLastAccessTime,  (CHUNK_BUF.LastAccessTime)  );

				///tm	*a1, *a2, *a3;
				//a1 = localtime(&(CHUNK_BUF.LastModifiedTime));
			//	a2 = localtime(&(CHUNK_BUF.LastAccessTime));
				//a3 = localtime(&(CHUNK_BUF.LastMetaModifiedTime));

				LastModifiedTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),pLastModifiedTime.wYear,pLastModifiedTime.wMonth	,pLastModifiedTime.wDay,pLastModifiedTime.wHour,pLastModifiedTime.wMinute,pLastModifiedTime.wSecond);
				LastAccessTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),pLastMetaModifiedTime.wYear,pLastMetaModifiedTime.wMonth	,pLastMetaModifiedTime.wDay,pLastMetaModifiedTime.wHour,pLastMetaModifiedTime.wMinute,pLastMetaModifiedTime.wSecond);
				LastMetaModifiedTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),pLastMetaModifiedTime.wYear,pLastMetaModifiedTime.wMonth	,pLastMetaModifiedTime.wDay,pLastMetaModifiedTime.wHour,pLastMetaModifiedTime.wMinute,pLastMetaModifiedTime.wSecond);

		unsigned char PARENTOBJECTID[3]; // 부모 아이디
		unsigned char ExtendedTagFlag;	     // Extended Tag Flag 0x80

				tempStruct.BLOCK_ID = TAG_EXT_BUF.BLOCKID;
				tempStruct.OBJECT_ID =  TAG_EXT_BUF.OBJECTID;
				tempStruct.OBJECT_TYPE_IN_TAG = TAG_EXT_BUF.OBJECTTYPE;
				tempStruct.PARENT_OBJECT_ID = TAG_EXT_BUF.PARENTOBJECTID;
				tempStruct.FILE_SIZE = TAG_EXT_BUF.FILESIZE;
				tempStruct.OBJECT_TYPE_IN_CHUNK = CHUNK_BUF.OBJECTTYPE;
				tempStruct.UNKNOWN = TAG_EXT_BUF.UNKNOWN01;
				tempStruct.FILE_NAME = CHUNK_BUF.FILENAME;

				tempStruct.MODIFIED = LastModifiedTime;
				tempStruct.ACCESSED = LastAccessTime;
				tempStruct.META_MODIFIED = LastMetaModifiedTime;



				/*printf("%d,%d,0,%X,%d,%d,%X,%d,%s,%s,%s,%s\n", 

				TAG_EXT_BUF.BLOCKID, 
				
				TAG_EXT_BUF.OBJECTID, 
				
				TAG_EXT_BUF.OBJECTTYPE, 
				
				TAG_EXT_BUF.PARENTOBJECTID,
				
				TAG_EXT_BUF.FILESIZE, 
				
				CHUNK_BUF.OBJECTTYPE, 

				TAG_EXT_BUF.UNKNOWN01,
				
				CHUNK_BUF.FILENAME,
				
				(LastModifiedTime.GetBuffer()),

				(LastAccessTime.GetBuffer()),

				(LastMetaModifiedTime.GetBuffer())
				
				);			*/
			}
			else
			{	//BLOCK ID, OBJECT ID, CHUNK ID, OBJECT TYPE, PARENT OBJECT ID, FILE SIZE, OBJECT TYPE, UNKOWN, FILE NAME
				
				tempStruct.BLOCK_ID = TAG_EXT_BUF.BLOCKID;
				tempStruct.OBJECT_ID =  TAG_EXT_BUF.OBJECTID;
				tempStruct.OBJECT_TYPE_IN_TAG = TAG_EXT_BUF.OBJECTTYPE;
				tempStruct.PARENT_OBJECT_ID = TAG_EXT_BUF.PARENTOBJECTID;
				tempStruct.FILE_SIZE = TAG_EXT_BUF.FILESIZE;
				tempStruct.OBJECT_TYPE_IN_CHUNK = CHUNK_BUF.OBJECTTYPE;
				tempStruct.UNKNOWN = TAG_EXT_BUF.UNKNOWN01;
			

				/*printf("%d,%d,-,%X,%d,%d,%X,%d,-,-,-,-\n", 
				
				TAG_EXT_BUF.BLOCKID, 
				
				TAG_EXT_BUF.OBJECTID, 
				
				TAG_EXT_BUF.OBJECTTYPE, 
				
				TAG_EXT_BUF.PARENTOBJECTID,
				
				TAG_EXT_BUF.FILESIZE, 
				
				CHUNK_BUF.OBJECTTYPE,  

				TAG_EXT_BUF.UNKNOWN01
				
				);*/


			}
		} else
			;


		//		CString writeline;
			//	writeline.Format( _T("%d,%s,%s,%s,%s\n"), tempStruct.OBJECT_TYPE_IN_CHUNK, tempStruct.FILE_NAME, LastModifiedTime, LastAccessTime, LastMetaModifiedTime );
			//				_tprintf(writeline);

		totalsize = totalsize - 2048 - 64;

		memset(&TAG_EXT_BUF, 0x00, sizeof(TAG_EXT_BUF) );
		memset(&CHUNK_BUF, 0x00, sizeof(CHUNK_BUF) );


		m_Array.Add(tempStruct);
		//printf("%d, %d, %d, %d, %d, %d, %d\n", tag1.chunk_id, tag1.serial_number, tag1.n_bytes, tag1.obj_id, tag1.ecc, tag1.deleted, tag1.unused_stuff);

		if ( totalsize == 0 )
			break;
	}
/*
	CArray <unsigned int,unsigned int> m_OffsetArray;

	unsigned int largeoffset = 0;
	unsigned int largeBlckid = 0;
	unsigned int largeChnkid = 0;
	unsigned int largeObjeid = 0;

	unsigned int HeaderofChunk = 0;

	int asassd = m_Array.GetCount();
	for ( int i = 0 ; i < m_Array.GetCount() ; i ++ )
	{
		if ( m_Array.GetAt(i).FILE_NAME == _T("KakaoTalk.db") )
			largeBlckid = IsBig( largeBlckid, m_Array.GetAt(i).BLOCK_ID );
	}
	for ( int i = 0 ; i < m_Array.GetCount() ; i ++ )
	{
		if ( ( m_Array.GetAt(i).FILE_NAME == _T("KakaoTalk.db") ) && ( m_Array.GetAt(i).BLOCK_ID == largeBlckid ) )
			largeoffset = IsBig( largeoffset, m_Array.GetAt(i).OFFSET );
	}
	for ( int i = 0 ; i < m_Array.GetCount() ; i ++ )
	{
		if ( ( m_Array.GetAt(i).FILE_NAME == _T("KakaoTalk.db") ) && ( m_Array.GetAt(i).BLOCK_ID == largeBlckid ) && ( m_Array.GetAt(i).OFFSET == largeoffset ) )
			HeaderofChunk = i;
	}
	unsigned int NumOfChunk = 0;

	if ( HeaderofChunk != 0 )
		NumOfChunk = m_Array.GetAt(HeaderofChunk).FILE_SIZE / 2048;

	if ( NumOfChunk != 0 )
	{
		for ( int i = 0 ; i < NumOfChunk ; i++ )
		{


		}
	}

*/  
	
	fp.Close();
	
	CurrentPath = ExtractPath;

	if (!g_fp.Open(m_path, CFile::modeRead, NULL))
		return;

	GetYaffs2Files( CurrentPath, 1 ); // ROOT == 0

	g_fp.Close();
	
	return;
}

BOOL GetYaffs2Files( CString CurrentFolderPath, unsigned int Odject_Id )
{
	CArray <unsigned int,unsigned int> m_OffsetArray;
	CArray <CString, CString> m_NameArray;
	CArray <YAFFSSTRUCT, YAFFSSTRUCT> TempArray;
	YAFFSSTRUCT sfds;
	unsigned int largeoffset = 0;
	unsigned int largeBlckid = 0;
	unsigned int largeChnkid = 0;
	unsigned int largeObjeid = 0;
	unsigned int HeaderofChunk = 0;

	// #1 Object ID
	int m_NameArray_GetCount = 0;
	for ( int i = 0 ; i < m_Array.GetCount() ; i ++ ) 
	{
		if ( m_Array.GetAt(i).PARENT_OBJECT_ID == Odject_Id ) 
		{
			sfds = m_Array.GetAt(i);
			BOOL IsSame = FALSE;
			for ( int x = 0 ; x < m_NameArray_GetCount ; x++ ) 
			{	
				if ( !m_Array.GetAt(i).FILE_NAME.IsEmpty() )
				{
					if ( m_NameArray.GetAt(x) == m_Array.GetAt(i).FILE_NAME )
					{
						sfds = m_Array.GetAt(i);
						IsSame = TRUE;
					}
				}
			}
			if ( !IsSame )
			{
				if ( !m_Array.GetAt(i).FILE_NAME.IsEmpty() )
				{
					YAFFSSTRUCT asdgsgs = m_Array.GetAt(i);
					m_NameArray.Add(m_Array.GetAt(i).FILE_NAME);
					m_NameArray_GetCount++;
				}
			}
		}
	}
	// #1 

	
	//for ( int i = 0 ; i < m_NameArray.GetCount() ; i++ )
	//{
	//	wcout << m_NameArray.GetAt(i).GetBuffer() << endl;

	//}
	//int asdfasdgdgd = m_NameArray.GetCount();

    // #2 Chunk Header
	for ( int i = 0 ; i < m_NameArray.GetCount() ; i++ )
	{
		unsigned int largeoffset = 0;
		unsigned int largeBlckid = 0;
		unsigned int largeChnkid = 0;
		unsigned int largeObjeid = 0;
		unsigned int HeaderofChunk = 0;

		// #2-1 
		for ( int y = 0 ; y < m_Array.GetCount() ; y++ ) {
			if ( (m_Array.GetAt(y).FILE_NAME == m_NameArray.GetAt(i)) && (m_Array.GetAt(y).PARENT_OBJECT_ID == Odject_Id))
				largeBlckid = IsBig( largeBlckid, m_Array.GetAt(y).BLOCK_ID );
		}
		// #2-2 
		for ( int z = 0 ; z < m_Array.GetCount() ; z++ )
		{
			if ( ( m_Array.GetAt(z).FILE_NAME == m_NameArray.GetAt(i) ) && (m_Array.GetAt(z).PARENT_OBJECT_ID == Odject_Id) && ( m_Array.GetAt(z).BLOCK_ID == largeBlckid ) )
				largeoffset = IsBig( largeoffset, m_Array.GetAt(z).OFFSET );
		}
		// #2-3 
		for ( int x = 0 ; x < m_Array.GetCount() ; x++ )
		{
			if ( ( m_Array.GetAt(x).FILE_NAME == m_NameArray.GetAt(i) ) && (m_Array.GetAt(x).PARENT_OBJECT_ID == Odject_Id) && ( m_Array.GetAt(x).BLOCK_ID == largeBlckid ) && ( m_Array.GetAt(x).OFFSET == largeoffset ) )
				HeaderofChunk = x;
		}

		if ( HeaderofChunk != 0 )
			HeaderofChunk;
		
		// #2-4 
		YAFFSSTRUCT tempStruct = m_Array.GetAt(HeaderofChunk);

		//m_Array.GetAt(HeaderofChunk).OBJECT_ID = 0;
		//tempStruct.CHUNK_ID = 0;

		// #2-4-1 파일
		if ( tempStruct.OBJECT_TYPE_IN_TAG == YAFFS_OBJECT_TYPE_FILE )					// #2-4-1 
		{
			if ( !tempStruct.FILE_NAME.IsEmpty() )
			{
				wcout << _T("Extracting : ") << tempStruct.FILE_NAME.GetBuffer() << endl;
				ConstructFileChunk( CurrentFolderPath, tempStruct);
			}
		} else if ( tempStruct.OBJECT_TYPE_IN_TAG == YAFFS_OBJECT_TYPE_DIRECTORY )		// #2-4-2 
		{
			if ( tempStruct.OBJECT_ID == 447 || tempStruct.OBJECT_ID == 490 )
				int c =0;

			if ( PathFileExists(CurrentFolderPath) ) {

				//wcout << _T("DIRECTORY,") << tempStruct.FILE_NAME.GetBuffer() << endl;

				CreateFolder(CurrentFolderPath + _T("\\") + tempStruct.FILE_NAME );
				GetYaffs2Files( CurrentFolderPath + _T("\\") + tempStruct.FILE_NAME, tempStruct.OBJECT_ID );
				wcout << _T("Extracting : ") << CurrentFolderPath + _T("\\") + tempStruct.FILE_NAME << endl;
			}

		} else if ( tempStruct.OBJECT_TYPE_IN_TAG == YAFFS_OBJECT_TYPE_SYMLINK )		// #2-4-3 
		{
	;//			wcout << _T("Extracting : ") << tempStruct.FILE_NAME.GetBuffer() << endl;
		} else if ( tempStruct.OBJECT_TYPE_IN_TAG == YAFFS_OBJECT_TYPE_HARDLINK )		// #2-4-4 
		{
			//wcout << _T("HARDLINK,") << tempStruct.FILE_NAME.GetBuffer() << endl;
		} else if ( tempStruct.OBJECT_TYPE_IN_TAG == YAFFS_OBJECT_TYPE_SPECIAL )		// #2-4-5 Special
		{
			if ( !tempStruct.FILE_NAME.IsEmpty() )
			{
				wcout << _T("Extracting : ") << tempStruct.FILE_NAME.GetBuffer() << endl;
				ConstructFileChunk( CurrentFolderPath, tempStruct);
			}
		} else if ( tempStruct.OBJECT_TYPE_IN_TAG == YAFFS_OBJECT_TYPE_UNKNOWN )		// #2-4-6 Unknown
		{
			if ( !tempStruct.FILE_NAME.IsEmpty() )
			{
				//wcout << _T("FILE,") << tempStruct.FILE_NAME.GetBuffer() << endl;
				ConstructFileChunk( CurrentFolderPath, tempStruct);
			}
		} else
		{
			if ( !tempStruct.FILE_NAME.IsEmpty() )
			{
				//wcout << _T("FILE,") << tempStruct.FILE_NAME.GetBuffer() << endl;
				ConstructFileChunk( CurrentFolderPath, tempStruct);
			}

			;//wcout << _T("ELSE,") << tempStruct.FILE_NAME.GetBuffer() << endl;
		}
	}
	return TRUE;
}
BOOL ConstructFileChunk(CString CurrentFolderPath, YAFFSSTRUCT m_data)
{
	CArray<YAFFSSTRUCT, YAFFSSTRUCT> m_TempArray;
	unsigned int TotalFileSize = m_data.FILE_SIZE;
	unsigned int CurrentFileSize = m_data.FILE_SIZE;
	unsigned int DivideSize = 0;
	
	BYTE BUF[2048];

	CFile fp;

	fp.Open( CurrentFolderPath + _T("\\") + m_data.FILE_NAME , CFile::modeCreate|CFile::modeWrite );

	if ( TotalFileSize == 0 )
	{
		fp.Close();
		return FALSE;
	}
	
	for ( int i = 0 ; i < m_Array.GetCount() ; i++ ) 
	{
		if ( m_Array.GetAt(i).OBJECT_ID == m_data.OBJECT_ID )
			m_TempArray.Add( m_Array.GetAt(i) );
	}

	DivideSize = TotalFileSize / DEFAULT_CHUNKSIZE;

	if ( (TotalFileSize != 0) && (DivideSize == 0) )
		DivideSize++;
	else
		DivideSize += 1;

	for ( int s = 0 ; s < DivideSize ; s++ )
	{
		int m_TempArray_GetCount = m_TempArray.GetCount();
		int check_CHUNK_ID = s+1;
		unsigned int largeoffset = 0;
		unsigned int largeBlckid = 0;
		unsigned int largeChnkid = 0;
		unsigned int largeObjeid = 0;
		for ( int i = 0 ; i < m_TempArray.GetCount() ; i++ ) {
			if ( ( m_TempArray.GetAt(i).FILE_NAME.IsEmpty() ) && ( m_TempArray.GetAt(i).CHUNK_ID == s+1 ) )
				largeBlckid = IsBig( largeBlckid, m_TempArray.GetAt(i).BLOCK_ID );
		}
		for ( int i = 0 ; i < m_TempArray.GetCount() ; i++ ) {
			if ( ( m_TempArray.GetAt(i).FILE_NAME.IsEmpty() ) && ( m_TempArray.GetAt(i).CHUNK_ID == s+1 ) && ( m_TempArray.GetAt(i).BLOCK_ID == largeBlckid ) )
				largeoffset = IsBig( largeoffset, m_TempArray.GetAt(i).OFFSET );
		}
		for ( int i = 0 ; i < m_TempArray.GetCount() ; i++ ) 
		{
			if ( ( m_TempArray.GetAt(i).FILE_NAME.IsEmpty() ) && ( m_TempArray.GetAt(i).CHUNK_ID == s+1 ) && ( m_TempArray.GetAt(i).BLOCK_ID == largeBlckid ) && ( m_TempArray.GetAt(i).OFFSET == largeoffset ) )
			{
				if ( CurrentFileSize >= DEFAULT_CHUNKSIZE )
				{
					memset( BUF, 0x00, DEFAULT_CHUNKSIZE );

					g_fp.Seek( m_TempArray.GetAt(i).OFFSET , CFile::begin );

					g_fp.Read( BUF, DEFAULT_CHUNKSIZE );
					fp.Write( BUF, DEFAULT_CHUNKSIZE );
					CurrentFileSize = CurrentFileSize - DEFAULT_CHUNKSIZE;

					
				} else if ( CurrentFileSize < DEFAULT_CHUNKSIZE )
				{
					memset( BUF, 0x00, DEFAULT_CHUNKSIZE );

					g_fp.Seek( m_TempArray.GetAt(i).OFFSET , CFile::begin );

					g_fp.Read( BUF, CurrentFileSize );
					fp.Write( BUF, CurrentFileSize );

					CurrentFileSize = 0;

					
				}
			}
		}
	}
	fp.Close();
	return FALSE;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
	
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			
			_tprintf(_T("err\n"));
			nRetCode = 1;
		}
		else
		{
			_tprintf(_T("=======================================================\n"));
			_tprintf(_T("Droid flash memory dump reconstructor v0.5\n"));
			_tprintf(_T("Digital Forensic Research Center, Korea University.\n"));
			_tprintf(_T("=======================================================\n"));
			if ( argc != 3 )
			{
				_tprintf( _T("Usage : yaffs2fordroidimage.exe <imagefilefullpath> <outputpathfullpath>\n") );
			_tprintf( _T("e.g., > yaffs2fordroidimage C:\\mtd7.dd C:\\output\n") );
			return 0;
			}

			m_path.Format(_T("%s"), argv[1]);
			ExtractPath.Format(_T("%s"), argv[2]);
			
			

			DODADODA();
		}
	}
	else
	{
		_tprintf(_T("err\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

unsigned int IsBig(unsigned int a, unsigned int b)
{
	if ( a > b )
		return a;
	else if ( b > a )
		return b;
	else
		return a;
}

bool CreateFolder(CString FolderPath)
{
	CString ParentPath;

	if (FolderPath.IsEmpty ())
		return FALSE;

	if (SHPathPrepareForWrite(NULL, NULL, FolderPath, SHPPFW_DEFAULT)!=S_OK)
		return FALSE;

	return TRUE;
}