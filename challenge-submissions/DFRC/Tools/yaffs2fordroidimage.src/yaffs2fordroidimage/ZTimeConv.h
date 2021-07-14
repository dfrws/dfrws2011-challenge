//////////////////////////////////////////////////////////////////////////////
//
// Class    : CZTimeConv
// Filename : ZTimeConv.h
// Version  : 1.3 ( 18-Jun-2008 )
//
// Copyright: 2009 - Mathias Svensson
// Author   : Mathias Svensson
// E-Mail   : ms@result42.com
//
// This is a Time convertor class, that let you convert to<->from a lot of time formats
//
// All rights reserved.
//
// You are free to use/modify this code but leave this header intact.
// This class is public domain so you are free to use it in any of
// your applications (Freeware,Shareware,Commercial). All I ask is
// that you let me know so that if you have a real winner I can
// brag to my buddies that some of my code is in your app. 
//
// Warranty.
//
// No Warranty of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// bugs@result42.com
//
//////////////////////////////////////////////////////////////////////////////
//  2008-06-18  - Made it work for VS2003 and lower
//  2007-10-20  - Splittet up time32_t and time64_t so both can be used from WIN64
//  2007-01-26	- Change name from UnixTime to Time_t
//  2007-03-01	- Added LocalTime_tToUTCTime_t
//  2007-03-01  - Change Time_tToLocalTime_t to Time_tToLocalRelTime_t
//  2007-03-01	- Added new Time_tToLocalTime_t
//

// Information about different timestamps
//
// http://blogs.msdn.com/oldnewthing/archive/2005/10/28/486194.aspx

/*
  Example of usage

  void myfunc(const FILETIME& ft)
  {
    __time32_t timeT=0;
    CZTimeConv::ToTime_t(&timeT, &ft);

    SYSTEMTIME st;
    CZTimeConv::ToTime_t(&st, &timeT);
  }

  //////////////////////////////////////////////////////////////////////////
  // Functions list
	//
  static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const FILETIME* pFileTime )
  static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const __time32_t dwTime_t )
  static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const __time64_t dwTime_t )
  static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const double variantTime )
  static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const WORD dosDate , const WORD dosTime )

  static BOOL ToLocalFileTime( FILETIME* pLocalFileTime , const FILETIME* pFileTime )
  static BOOL ToLocalFileTime( FILETIME* pLocalFileTime , const SYSTEMTIME* st )
  static BOOL ToLocalRelFileTime( FILETIME* pLocalFileTime , const FILETIME* pFileTime )
  static BOOL FromFileTimeLocal( FILETIME* pFileTime  ,const FILETIME* pLocalFileTime  )
  static BOOL FromFileTimeLocalRel( FILETIME* pFileTime  ,const FILETIME* pLocalFileTime  )
  static BOOL ToFileTime( FILETIME* pFileTime , const SYSTEMTIME* pSystemTime )
  static BOOL ToFileTime( FILETIME* pFileTime , const __time32_t Time_t )
  static BOOL ToFileTime( FILETIME* pFileTime , const __time64_t Time_t )
  static BOOL ToFileTime( FILETIME* pFileTime , const double variantTime )
  static BOOL ToFileTime( FILETIME* pFileTime , const WORD dosDate , const WORD dosTime )
  static __int64  FileTimeToINT64(FILETIME* ft)
  static void INT64ToFileTime(FILETIME* ft , __int64 nValue)

  static BOOL ToDosDateTime( DWORD* pDateTime , const __time32_t dwTime_t )
  static BOOL ToDosDateTime( WORD* pDate , WORD* pTime , const FILETIME* pFileTime )
  static BOOL ToDosDateTime( DWORD* pDosDateTime , const SYSTEMTIME* st )
  static BOOL ToDosDateTime( WORD* pDate , WORD* pTime , const SYSTEMTIME* st )
  static void DosTimeDWORD2WORD( WORD* pDate , WORD* pTime , DWORD dosDateTime )
  static void DosTimeWORD2DWORD( DWORD* pDosDateTime , WORD Date , WORD Time )
  static BOOL ToDosDateTime( DWORD* pDateTime , const FILETIME* pFileTime )

  static BOOL ToTime_t( __time32_t* pdwTime_t , const FILETIME* pFileTime )
  static BOOL ToTime_t( __time64_t* pdwTime_t , const FILETIME* pFileTime )
  static BOOL ToTime_t( __time32_t* pdwTime_t , const SYSTEMTIME* pSystemTime )
  static BOOL ToTime_t( __time64_t* pdwTime_t , const SYSTEMTIME* pSystemTime )
  static BOOL ToTime_t( __time32_t* pdwTime_t , const double variantTime )
  static BOOL ToTime_t( __time32_t* pdwTime_t , const DWORD dosDateTime )
  static BOOL ToTime_t( __time32_t* pdwTime_t , const WORD dosDate , const WORD dosTime )
  static BOOL ToTime_t( __time64_t* pdwTime_t , const DWORD dosDateTime )
  static BOOL ToTime_t( __time64_t* pdwTime_t , const WORD dosDate , const WORD dosTime )

  static __time32_t Time_tToLocalTime_t( const __time32_t Time_t )
  static __time64_t Time_tToLocalTime_t( const __time64_t Time_t )
  static __time32_t Time_tToLocalRelTime_t( const __time32_t Time_t )
  static __time64_t Time_tToLocalRelTime_t( const __time64_t Time_t )
  static __time32_t LocalTime_tToUTCTime_t( const __time32_t Time_t )
  static __time64_t LocalTime_tToUTCTime_t( const __time64_t Time_t )
*/

#ifndef _CZTimeConv_
#define _CZTimeConv_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VS2003 and lower
#if _MSC_VER < 1400
	#ifndef _TIME32_T_DEFINED
		typedef _W64 long __time32_t;   /* 32-bit time value */
	#define _TIME32_T_DEFINED
	#endif
#endif
// Some conversions are included even do they are included in the Windows API. but I want to have all my
// time conversion function at the same place. 

class CZTimeConv
{
public:

	// To SystemTime

	static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const FILETIME* pFileTime )
	{
		return ::FileTimeToSystemTime( pFileTime, pSystemTime );
	}

	static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const __time32_t dwTime_t )
	{
		FILETIME ft;
		if( !ToFileTime( &ft , dwTime_t ) )
			return FALSE;

		if( !ToSystemTime( pSystemTime , &ft) )
			return FALSE;

		return TRUE;
	}

	static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const __time64_t dwTime_t )
	{
		FILETIME ft;
		if( !ToFileTime( &ft , dwTime_t ) )
			return FALSE;

		if( !ToSystemTime( pSystemTime , &ft) )
			return FALSE;

		return TRUE;
	}

	static BOOL ToSystemTime( SYSTEMTIME* /*pSystemTime*/ , const double /*variantTime*/ )
	{
		return FALSE;
	}
	static BOOL ToSystemTime( SYSTEMTIME* pSystemTime , const WORD dosDate , const WORD dosTime )
	{
		FILETIME ft;
		if( !ToFileTime( &ft , dosDate , dosTime ) )
			return FALSE;

		return ToSystemTime( pSystemTime , &ft );
	}

	// To FileTime
	static BOOL ToLocalFileTime( FILETIME* pLocalFileTime , const FILETIME* pFileTime )
	{
		return ::FileTimeToLocalFileTime( pFileTime , pLocalFileTime );
	}
	static BOOL ToLocalFileTime( FILETIME* pLocalFileTime , const SYSTEMTIME* st )
	{
		FILETIME ft;
		if( ToFileTime( &ft , st ) )
		{
			return ::FileTimeToLocalFileTime( &ft , pLocalFileTime );
		}
		return FALSE;
	}
	static BOOL ToLocalRelFileTime( FILETIME* pLocalFileTime , const FILETIME* pFileTime )
	{
		SYSTEMTIME stUTC;
		SYSTEMTIME stLocal;
		if( !FileTimeToSystemTime(pFileTime, &stUTC) )
			return FALSE;
		if( !SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal) )
			return FALSE;
		if( !ToFileTime( pLocalFileTime , &stLocal ) )
			return FALSE;
		return TRUE;
	}

	static BOOL FromFileTimeLocal( FILETIME* pFileTime  ,const FILETIME* pLocalFileTime  )
	{
		if( !::LocalFileTimeToFileTime( pLocalFileTime , pFileTime ) )
			return FALSE;
		return TRUE;
	}

	static BOOL FromFileTimeLocalRel( FILETIME* pFileTime  ,const FILETIME* pLocalFileTime  )
	{
		SYSTEMTIME stUTC;
		SYSTEMTIME stLocal;

		if( !FileTimeToSystemTime(pLocalFileTime, &stLocal) )
			return FALSE;

		if( !TzSpecificLocalTimeToSystemTime(NULL, &stLocal , &stUTC ) )
			return FALSE;

		if( !ToFileTime( pFileTime , &stUTC ) )
			return FALSE;
		return TRUE;
	}


	static BOOL ToFileTime( FILETIME* pFileTime , const SYSTEMTIME* pSystemTime )
	{
		return SystemTimeToFileTime( pSystemTime , pFileTime );
	}

	static BOOL ToFileTime( FILETIME* pFileTime , const __time32_t Time_t )
	{
		LONGLONG ll = Int32x32To64(Time_t, 10000000) + 116444736000000000;
		pFileTime->dwLowDateTime = (DWORD)ll;
		pFileTime->dwHighDateTime = (DWORD)(ll >> 32);
		return TRUE;
	}

  static BOOL ToFileTime( FILETIME* pFileTime , const __time64_t Time_t )
  {
    // Convert sec to Nano sec.. Multi by 10 Mil, then add the difference between 1601 and 1970
    LONGLONG ll = (Time_t * 10000000) + 116444736000000000;
    pFileTime->dwLowDateTime = (DWORD)ll;
    pFileTime->dwHighDateTime = (DWORD)(ll >> 32);
    return TRUE;
  }

	static BOOL ToFileTime( FILETIME* pFileTime , const double variantTime )
	{
		SYSTEMTIME systime;

		if( VariantTimeToSystemTime( variantTime , &systime ) == FALSE )
			return FALSE;

		return ToFileTime( pFileTime , &systime );
	}

	static BOOL ToFileTime( FILETIME* pFileTime , const WORD dosDate , const WORD dosTime )
	{
		return DosDateTimeToFileTime( dosDate , dosTime , pFileTime );
	}
	
  static __int64  FileTimeToINT64(FILETIME* ft)
  {
    ASSERT(ft);
    ULARGE_INTEGER ul;
    ul.HighPart = ft->dwHighDateTime;
    ul.LowPart = ft->dwLowDateTime;
    return ul.QuadPart;
  }

  static void INT64ToFileTime(FILETIME* ft , __int64 nValue)
  {
    ASSERT(ft);
    ULARGE_INTEGER ul;
    ul.QuadPart = nValue;
    ft->dwHighDateTime = ul.HighPart;
    ft->dwLowDateTime = ul.LowPart;
  }
	//////////////////////////////////////////////////////////////////////////

	// To Dos Time
	static BOOL ToDosDateTime( DWORD* pDateTime , const __time32_t dwTime_t )
	{
		FILETIME ft;
		if( !ToFileTime( &ft , dwTime_t ) )
			return FALSE;
		return ToDosDateTime( pDateTime , &ft );
	}
	static BOOL ToDosDateTime( WORD* pDate , WORD* pTime , const FILETIME* pFileTime )
	{
		return ::FileTimeToDosDateTime( pFileTime , pDate , pTime );
	}

	static BOOL ToDosDateTime( DWORD* pDosDateTime , const SYSTEMTIME* st )
	{
		WORD Date;
		WORD Time;

		if( !ToDosDateTime( &Date , &Time , st ) )
			return FALSE;

		DosTimeWORD2DWORD( pDosDateTime , Date , Time );
		return TRUE;
	}
	static BOOL ToDosDateTime( WORD* pDate , WORD* pTime , const SYSTEMTIME* st )
	{
		FILETIME ft;
		if( !ToFileTime( &ft , st) )
			return FALSE;


		if( !ToDosDateTime( pDate , pTime , &ft ) )
			return FALSE;

		return TRUE;
	}



	static void DosTimeDWORD2WORD( WORD* pDate , WORD* pTime , DWORD dosDateTime )
	{
		*pDate = HIWORD( dosDateTime );
		*pTime = LOWORD( dosDateTime );
	}

	static void DosTimeWORD2DWORD( DWORD* pDosDateTime , WORD Date , WORD Time )
	{
		*pDosDateTime = MAKELONG( Time , Date );
	}

	static BOOL ToDosDateTime( DWORD* pDateTime , const FILETIME* pFileTime )
	{
		WORD Date=0;
		WORD Time=0;
		if( CZTimeConv::ToDosDateTime( &Date , &Time , pFileTime ) )
		{
			*pDateTime = MAKELONG( Time , Date );
			return TRUE;
		}
		return FALSE;
	}
	// To UnixTime
	static BOOL ToTime_t( __time32_t* pdwTime_t , const FILETIME* pFileTime )
	{
    if( pdwTime_t == NULL )
      return FALSE;


    // converts 64-bit FILETIME specifying the number of 100-nanosecond
    // intervals which have passed since January 1, 1601.
    LONGLONG t = pFileTime->dwHighDateTime;
    t <<= 32;
    t += (UINT32)pFileTime->dwLowDateTime;
    t -= 116444736000000000L; // difference between 1601 and 1970

    // divide by 10million to convert to seconds
    if (t < 0)
    {
      *pdwTime_t = (DWORD)( -1 - ((-t - 1) / 10000000) );
    }
    else
    {
      *pdwTime_t = (DWORD)(t / 10000000);
    }

    return TRUE;

	}
	
  static BOOL ToTime_t( __time64_t* pdwTime_t , const FILETIME* pFileTime )
  {
    if( pdwTime_t == NULL )
      return FALSE;
      
      
    // converts 64-bit FILETIME specifying the number of 100-nanosecond
    // intervals which have passed since January 1, 1601.
    ULARGE_INTEGER li;
    li.HighPart = pFileTime->dwHighDateTime;
    li.LowPart = pFileTime->dwLowDateTime;
    
    INT64 nSec = li.QuadPart - 116444736000000000L; // difference between 1601 and 1970

    // divide by 10million to convert to seconds
    *pdwTime_t = (DWORD)(nSec / 10000000);
    
    return TRUE;
  }
  
	static BOOL ToTime_t( __time32_t* pdwTime_t , const SYSTEMTIME* pSystemTime )
	{
		FILETIME ft;
		if( !SystemTimeToFileTime( pSystemTime , &ft ) )
			return FALSE;

		return ToTime_t( pdwTime_t , &ft );
		
	}
	
  static BOOL ToTime_t( __time64_t* pdwTime_t , const SYSTEMTIME* pSystemTime )
  {
    FILETIME ft;
    if( !SystemTimeToFileTime( pSystemTime , &ft ) )
      return FALSE;

    return ToTime_t( pdwTime_t , &ft );

  }
  
	static BOOL ToTime_t( __time32_t* /*pdwTime_t*/ , const double /*variantTime*/ )
	{
		return FALSE;
	}

	static BOOL ToTime_t( __time32_t* pdwTime_t , const DWORD dosDateTime )
	{
		return ToTime_t( pdwTime_t , (WORD)(dosDateTime >> 16), (WORD)dosDateTime );
	}

	static BOOL ToTime_t( __time32_t* pdwTime_t , const WORD dosDate , const WORD dosTime )
	{
		FILETIME ft;
		if( ToFileTime( &ft , dosDate, dosTime ) )
  		return ToTime_t( pdwTime_t , &ft );
  	
  	return FALSE;
	}
	static BOOL ToTime_t( __time64_t* pdwTime_t , const DWORD dosDateTime )
	{
		return ToTime_t( pdwTime_t , (WORD)(dosDateTime >> 16), (WORD)dosDateTime );
	}

	static BOOL ToTime_t( __time64_t* pdwTime_t , const WORD dosDate , const WORD dosTime )
	{
		FILETIME ft;
		if( ToFileTime( &ft , dosDate, dosTime ) )
			return ToTime_t( pdwTime_t , &ft );

		return FALSE;
	}

	// UTC time_t to Local time_t
	static __time32_t Time_tToLocalTime_t( const __time32_t Time_t )
	{
		FILETIME ft;
		FILETIME ftLocal;
		if( CZTimeConv::ToFileTime( &ft , Time_t ) )
		{
			if( CZTimeConv::ToLocalFileTime( &ftLocal , &ft ) )
			{
				__time32_t tLocal = 0;
				if( CZTimeConv::ToTime_t( &tLocal , &ftLocal ) )
					return tLocal;
			}
		}
		return 0;
	}
	
  static __time64_t Time_tToLocalTime_t( const __time64_t Time_t )
  {
    FILETIME ft;
    FILETIME ftLocal;
    if( CZTimeConv::ToFileTime( &ft , Time_t ) )
    {
      if( CZTimeConv::ToLocalFileTime( &ftLocal , &ft ) )
      {
        __time64_t tLocal = 0;
        if( CZTimeConv::ToTime_t( &tLocal , &ftLocal ) )
          return tLocal;
      }
    }
    return 0;
  }

    //////////////////////////////////////////////////////////////////////////
	// UTC to local relative time ( if time is a time where dst should be active then adjust time for dst)
	static __time32_t Time_tToLocalRelTime_t( const __time32_t Time_t )
	{
		if( Time_t == -1 )
			return 0;
		register struct tm *stm;
		register __time32_t time;

// VS2005+
#if _MSC_VER >= 1400 
		stm = _gmtime32( &Time_t );
		if( stm )
			time = _mktime32(stm);
		else
			return 0;
#else
// VS2003 and lower
		stm = gmtime( &Time_t );
		if( stm )
			time = mktime(stm);
		else
			return 0;
#endif

		return (2*Time_t-time);
	}
	

// VS2005+
#if _MSC_VER >= 1400 
	static __time64_t Time_tToLocalRelTime_t( const __time64_t Time_t )
	{
		if( Time_t == -1 )
			return 0;
		register struct tm *stm;
		register __time64_t time;

		stm = _gmtime64( &Time_t );
		if( stm )
			time = _mktime64(stm);
		else 
			return 0;

		return (2*Time_t - time);
	}

#endif
	//////////////////////////////////////////////////////////////////////////

	// Local Time_t to UTC time_t
	static __time32_t LocalTime_tToUTCTime_t( const __time32_t Time_t )
	{
		// the lazy way
		FILETIME ft;
		__time32_t tUTC;
		if( CZTimeConv::ToFileTime( &ft , Time_t ) )
		{
			CZTimeConv::FromFileTimeLocal( &ft , &ft );
			CZTimeConv::ToTime_t( &tUTC , &ft );
			return tUTC;
		}
		return 0;
	}
	
  static __time64_t LocalTime_tToUTCTime_t( const __time64_t Time_t )
  {
    // the lazy way
    FILETIME ft;
    __time64_t tUTC;
    if( CZTimeConv::ToFileTime( &ft , Time_t ) )
    {
      CZTimeConv::FromFileTimeLocal( &ft , &ft );
      CZTimeConv::ToTime_t( &tUTC , &ft );
      return tUTC;
    }
    return 0;
  }
};

#endif
