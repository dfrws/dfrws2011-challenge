/**
 *	@file	Utils.h
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

#pragma once
#include "../StdAfx.h"

class CUtils
{
public:
	CUtils(void);
	~CUtils(void);

public:
	CString StringFilter(CString strInput);

	void WriteUnicode(CFile *file, CString strString, BOOL bNewLine);

	CStringA DoSHA1(PBYTE buffer, UINT bufSize);
	
	SYSTEMTIME Time_tToSYSTEMTIME(time_t t, int UTC);

	CString SYSTEMTIMEtoCString(SYSTEMTIME st);
};