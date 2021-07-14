/**
 *	@file	Utils.cpp
*	@date	2011/06/07
 *	@author	Jungheum Park

 *	@brief	Utils for Forensic Application
 */

/**
	@mainpage	Utils for Forensic Application

	@section	MODIFYINFO	History

		- Jungheum Park / 2011.06.07
			- Initial Version
 */


#include "StdAfx.h"
#include "Utils.h"
#include "../CistCrypt/sha1.h"

CUtils::CUtils(void)
{
}

CUtils::~CUtils(void)
{
}

CString CUtils::StringFilter(CString strInput)
{
	CString strResult;
	strResult += _T("\"");

	if(strInput != "")
	{
		strInput.Replace(_T("\""), _T("\"\""));
		strResult += strInput;
	}

	strResult += _T("\"");
	strResult.ReleaseBuffer();
	return strResult;
}

void CUtils::WriteUnicode(CFile *file, CString strString, BOOL bNewLine)
{
	if(bNewLine == TRUE)	strString += _T("\n");
	else					strString += _T("\t");

	file->Write(strString.GetBuffer(), strString.GetLength()*2);
}

CStringA CUtils::DoSHA1(PBYTE buffer, UINT bufSize)
{
	SHA1_ALG_INFO sha_ctx;
	BYTE sha1value[SHA1_DIGEST_VALUELEN] = {0,0};
	CStringA srtHash = "";

	SHA1_Init(&sha_ctx);
	SHA1_Update(&sha_ctx, buffer, bufSize);
	SHA1_Final(&sha_ctx, sha1value);

	for(int i = 0; i < SHA1_DIGEST_VALUELEN; i++) 
	{
		char tmpBuf[3] = {0,0};

		_itoa(sha1value[i], (char *)tmpBuf, 16);

		if(strlen(tmpBuf) == 1) 
		{
			tmpBuf[1] = tmpBuf[0];
			tmpBuf[0] = '0';
			tmpBuf[2] = NULL;
		}

		srtHash += tmpBuf;
	}

	srtHash.MakeUpper();
	return srtHash;
}

SYSTEMTIME CUtils::Time_tToSYSTEMTIME(time_t t, int UTC)
{
	const __int64 IN_HOUR = (__int64)10000000*60*60;
	const __int64 IN_MIN  = (__int64)10000000*60;	

	FILETIME	ft;
	SYSTEMTIME	st;
	LONGLONG	ll;

	int GMT_hour = UTC;
	int GMT_min = 0;

	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = ll >> 32;

	LARGE_INTEGER	largeInt;
	memcpy(&largeInt,&ft,sizeof(FILETIME));	// filetime -> large_integer;
	largeInt.QuadPart += (IN_HOUR*GMT_hour) + (IN_MIN*GMT_min);	// UTC Setting
	memcpy(&ft, &largeInt, sizeof(FILETIME)); // large_integer -> filetime

	FileTimeToSystemTime(&ft, &st);
	return st;
}

CString CUtils::SYSTEMTIMEtoCString(SYSTEMTIME st)
{
	CString resString = _T("");
	TCHAR *dayPre[] = {_T("Sun"), _T("Mon"), _T("Tue"), _T("Wed"), _T("Thu"), _T("Fri"), _T("Sat")};

	resString.Format(_T("%d-%02d-%02d  %02d:%02d:%02d  %s"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, dayPre[st.wDayOfWeek%7]);

	return resString;
}
