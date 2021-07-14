/**
 *	@file	RecordPageExtractor.cpp
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

#include "stdafx.h"
#include "RecordPageExtractor.h"

// RecordPageExtractor

RecordPageExtractor::RecordPageExtractor()
{
	EncodedType = ENCODED_UTF8;				//Default Encoding Type == UTF8
	this->PageSize = 0x400;
}

RecordPageExtractor::RecordPageExtractor(UINT32 PageSize, UINT EncodedType)
{
	this->EncodedType = EncodedType;				//Default Encoding Type == UTF8
	this->PageSize = PageSize;
}

RecordPageExtractor::~RecordPageExtractor()
{
}

/********************
* External Functions
*********************/

void RecordPageExtractor::GetNormalRecordSet(UINT8 *Buff)
{
	GetNormalRecordOffsets(Buff);
	UINT CellLength = 0;

	for(int idx = 0 ; idx < NormalRecordOffsetArr.GetCount() ; idx++)
	{
		UINT16 RemainedBuffSize = PageSize - NormalRecordOffsetArr.GetAt(idx);
		if( NormalRecordOffsetArr.GetAt(idx) < PageSize )									
		{
			CTypeArray TypeArray;
			CRowArray  RowArray;

			CellLength = NormalRecordParsing(&Buff[NormalRecordOffsetArr.GetAt(idx)], &RowArray, &TypeArray, RemainedBuffSize);

			Types_InPage.Add(TypeArray);
			NormalTable_InPage.Add(RowArray);
		}
	}
}

/********************
* Internal Functions
*********************/

UINT RecordPageExtractor::CalcValriableLengthInteger(UINT8 *Buff, UINT64 *Result)
{
	BOOL FirstBitFlag = 0;
	UINT Index = 0;
	UINT64 Temp = 0;

	*Result = 0;

	while(true)
	{
		Temp = 0;

		FirstBitFlag = (Buff[Index] & 0x80) >> 7 ;	// Buff[Index] & 10000000 , 그리고 오른쪽 shift 7
		Temp = Buff[Index] & 0x7F;					// 0XXXXXXX , 1XXXXXXX 0XXXXXXX 등 처리
		*Result = *Result | Temp;

		Index++;

		if(FirstBitFlag == 0 || Index == 9)
			break;

		*Result = (*Result << 7);
	}

	return Index;
}

UINT16 RecordPageExtractor::NormalRecordParsing(UINT8 *Buff, CRowArray *RowArray, CTypeArray *TypeArray, UINT16 RemainedBuffSize)
{
	UINT16 CellLength = 0;
	UINT16 CurrOffset = 0;

	/*	Cell Header 해석
	**	Cell Size				: Variable Len Integer		
	**	Row ID					: Variable Len Integer
	**	Data Length Area Size	: Big Endian 2Byte Interger	(DataLenAreaSize Length(=2) + DatalenArea Length)
	**	Data Length Array		: 0			= NULL
								: N(N=1~4)	= N Byte					Signed Integer
								: 5			= 6 Byte					Signed Integer
								: 6			= 8 Byte					Signed Integer
								: 7			= 8 Byte					IEEE Float
								: 8-11		= Reserved Area
								: N > 12	= Variable Length Integer	BLOB 
								: N > 13	= Variable Length Integer	TEXT 
	*/

	CellHeader stCellHeader;

	//Get Cell Size
	if( 9 <= RemainedBuffSize )									
	{
		stCellHeader.CellSize.Length = CalcValriableLengthInteger(Buff, &stCellHeader.CellSize.Value);
	}
	else
		return false;

	//Get RowID
	CurrOffset += stCellHeader.CellSize.Length;
	if( CurrOffset + 9 <= RemainedBuffSize )
	{
		stCellHeader.RowID.Length = CalcValriableLengthInteger(&Buff[CurrOffset], &stCellHeader.RowID.Value);
	}
	else
		return false;

	//Get Data Length Area Size
	CurrOffset += stCellHeader.RowID.Length;
	if( CurrOffset + 1 <= RemainedBuffSize )
	{
		stCellHeader.DataLenAreaSize = little_2bytetoint(&Buff[CurrOffset]);
	}
	else
		return false;

	//Get Data Array
	CurrOffset += 2;
	UINT16 ReadSize = 2;

	while(true)
	{
		if(ReadSize >= stCellHeader.DataLenAreaSize)
			break;

		if( CurrOffset > RemainedBuffSize )									
		{
			return false;
		}

		if(Buff[CurrOffset] <= 11)		// Integer (<= 11)
		{
			stCellHeader.DataLenArr.Add(Buff[CurrOffset]);
			CurrOffset += 1;
			ReadSize += 1;
		}
		else							// BLOB or TEXT (>= 12)
		{
			VarLenInt VarIntTemp;
			VarIntTemp.Length = CalcValriableLengthInteger(&Buff[CurrOffset], &VarIntTemp.Value);

			stCellHeader.DataLenArr.Add(VarIntTemp.Value);
			CurrOffset += VarIntTemp.Length;
			ReadSize += VarIntTemp.Length;
		}
	}


	CString Data;
	Data.Format(_T("%d"), stCellHeader.RowID);
	RowArray->Add(Data);


	//Get Data
	for(int idx = 0 ; idx < stCellHeader.DataLenArr.GetCount() ; idx++)
	{
		UINT64 CurrDataLengthValue = stCellHeader.DataLenArr.GetAt(idx);
		
		if( CurrDataLengthValue == 0 )							// : 0			= NULL
		{
			Data = _T("NULL");

			TypeArray->Add(TYPE_NULL);
		}

		if( CurrDataLengthValue == 1 )							// : N(N=1~4)	= N Byte					Signed Integer
		{
			if( CurrOffset + 1 <= RemainedBuffSize )	
			{
				INT64 INTData = Buff[CurrOffset];
				Data.Format(_T("%lld"),INTData);
			}
			else 
				return false;
			
			CurrOffset += 1;
	
			TypeArray->Add(TYPE_INTEGER1);
		}

		if( CurrDataLengthValue == 2 )							
		{
			if( CurrOffset + 2 <= RemainedBuffSize )									
			{
				INT64 INTData = big_2bytetoint(&Buff[CurrOffset]);
				Data.Format(_T("%lld"),INTData);
			}
			else
				return false;

			CurrOffset += 2;

			TypeArray->Add(TYPE_INTEGER2);
		}

		if( CurrDataLengthValue == 3 )
		{
			if( CurrOffset + 3 <= RemainedBuffSize )
			{
				INT64 INTData = big_3bytetoint(&Buff[CurrOffset]);
				Data.Format(_T("%lld"),INTData);
			}
			else
				return false;

			CurrOffset += 3;
		
			TypeArray->Add(TYPE_INTEGER3);
		}

		if( CurrDataLengthValue == 4 )
		{
			if( CurrOffset + 4 <= RemainedBuffSize )									
			{
				INT64 INTData = big_4bytetoint(&Buff[CurrOffset]);
				Data.Format(_T("%lld"),INTData);
			}
			else
				return false;

			CurrOffset += 4;
		
			TypeArray->Add(TYPE_INTEGER4);
		}

		if( CurrDataLengthValue == 5 )							// : 5			= 6 Byte					Signed Integer
		{
			if( CurrOffset + 6 <= RemainedBuffSize )									
			{
				INT64 INTData = big_6bytetoint(&Buff[CurrOffset]);
				Data.Format(_T("%lld"),INTData);
			}
			else
				return false;

			CurrOffset += 6;
		
			TypeArray->Add(TYPE_INTEGER6);
		}

		if( CurrDataLengthValue == 6 )							// : 6			= 8 Byte					Signed Integer
		{
			if( CurrOffset + 8 <= RemainedBuffSize )									
			{
				INT64 INTData = big_8bytetoint(&Buff[CurrOffset]);
				Data.Format(_T("%lld"),INTData);
			}
			else
				return false;
			
			CurrOffset += 8;
		
			TypeArray->Add(TYPE_INTEGER8);
		}

		if( CurrDataLengthValue == 7 )							// : 7			= 8 Byte					IEEE Float
		{
			if( CurrOffset + 8 <= RemainedBuffSize )									
			{
				INT64 INTData = big_8bytetoint(&Buff[CurrOffset]);
				Data.Format(_T("%llf"),INTData);
			}
			else
				return false;

			CurrOffset += 8;
		
			TypeArray->Add(TYPE_NUMERIC);
		}

		if( CurrDataLengthValue == 8 )							// : 8			= 0 Byte					0
		{
			INT64 INTData = 0;
			Data.Format(_T("%lld"),INTData);

			CurrOffset += 0;
		
			TypeArray->Add(TYPE_INTEGER0);
		}

		if( CurrDataLengthValue == 9 )							// : 9			= 0	 Byte					1
		{
			INT64 INTData = 1;
			Data.Format(_T("%lld"),INTData);

			CurrOffset += 0;
			
			TypeArray->Add(TYPE_INTEGER0);
		}

		if( CurrDataLengthValue >= 12 )							// : N > 12	= Variable Length Integer	BLOB 
		{
			UINT64 DataLen = 0;

			if((CurrDataLengthValue % 2) == 0)					// BLOB : Data Size = (N-12)/2
			{
				DataLen = (CurrDataLengthValue - 12) / 2 ;
				
				TypeArray->Add(TYPE_BLOB);
			}
			else if((CurrDataLengthValue % 2) == 1)				// TEXT : Data Size = (N-13)/2
			{
				DataLen = (CurrDataLengthValue - 13) / 2 ;

				if(DataLen > RemainedBuffSize)					
					return false;

				char *szMultiByte;
				szMultiByte = (char *)malloc((size_t)DataLen+512);
				ZeroMemory(szMultiByte, (size_t)DataLen+1);

				if( CurrOffset + DataLen <= RemainedBuffSize )									
					memcpy(szMultiByte, &Buff[CurrOffset], (size_t)DataLen);
				else
				{
					safe_free(szMultiByte);
					return false;
				}
				
				if(EncodedType == ENCODED_UTF8)
				{
					CString parm_src_string/* =szMultiByte*/;
					
					int temp_length = MultiByteToWideChar(CP_UTF8, 0, szMultiByte/*parm_src_string*/, -1, NULL, 0);
					
					if(temp_length > 0)
					{
						BSTR unicode_str = SysAllocStringLen(NULL, temp_length);

						MultiByteToWideChar(CP_UTF8, 0, szMultiByte/*parm_src_string*/, -1, unicode_str, temp_length);
						parm_src_string = unicode_str;

						Data=parm_src_string;
						SysFreeString(unicode_str);
					} 
					else 
						parm_src_string.Empty();
				}
				else if(EncodedType == ENCODED_LITTLE_UTF16 || EncodedType == ENCODED_BIG_UTF16)
				{
					char *multistring = CovertUTF16toMutil(szMultiByte, (size_t)DataLen);

					CString parm_src_string/* =szMultiByte*/;
					int temp_length = MultiByteToWideChar(CP_ACP, 0, multistring/*parm_src_string*/, -1, NULL, 0);
					
					if(temp_length > 0)
					{
						BSTR unicode_str = SysAllocStringLen(NULL, temp_length);

						MultiByteToWideChar(CP_ACP, 0, multistring/*parm_src_string*/, -1, unicode_str, temp_length);
						parm_src_string = unicode_str;

						Data=parm_src_string;
						SysFreeString(unicode_str);
					}
					else 
						parm_src_string.Empty();

					safe_free(multistring);
				}

				safe_free(szMultiByte);

				CurrOffset += (UINT16)DataLen;

				TypeArray->Add(TYPE_TEXT);
			}
		}

		RowArray->Add(Data);
	}

	return CellLength;
}

char* RecordPageExtractor::CovertUTF16toMutil(char *UTF16str, int buflen)
{
	wchar_t utf16[MAX_QUERY_LEN+512]={0,};

	char *ansi = (PCHAR)malloc(MAX_QUERY_LEN);
	ZeroMemory(ansi, MAX_QUERY_LEN);

	for(int i=0, k=0; i<buflen && i<MAX_QUERY_LEN*2 ; i+=2 ,k++)
	{
		if(EncodedType==ENCODED_BIG_UTF16)
		{
			if( memcmp(createtable_utf16_big_upper,&UTF16str[i+1],24)==0 || memcmp(createtable_utf16_big_lower,&UTF16str[i+1],24)==0 )
			{	
				utf16[k]=UTF16str[i];
				i++;
				k++;
			}

			utf16[k]=big_2bytetoint(&UTF16str[i]);
		}
		else if(EncodedType==ENCODED_LITTLE_UTF16)
		{
			if( memcmp(createtable_utf16_little_upper,&UTF16str[i+1],24)==0 || memcmp(createtable_utf16_little_lower,&UTF16str[i+1],24)==0 )
			{
				utf16[k]=UTF16str[i];
				i++;
				k++;
			}

			utf16[k]=little_2bytetoint(&UTF16str[i]);
		}
	}

	utf16[buflen/2]=NULL;

	int nLen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)utf16, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)utf16, -1, (LPSTR)(ansi), nLen, NULL, NULL);

	return ansi;
}

UINT RecordPageExtractor::GetNormalRecordOffsets(UINT8 *Buff)
{
	NormalRecordOffsetArr.RemoveAll();
	UINT16 NumOfCells = big_2bytetoint(&Buff[3]);						

	for(UINT idx = 0 ; idx < NumOfCells ; idx++)
	{
		if( 8+(idx*2) + 1 < PageSize )									
		{
			UINT16 TempOffset =  big_2bytetoint(&Buff[8 + (idx*2)]);	
			NormalRecordOffsetArr.Add(TempOffset);
		}
	}

	return PROCESS_SUCCESS; 
}

