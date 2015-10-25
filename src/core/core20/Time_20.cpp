//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a Core Service C++ Library.
// Copyright (C) 2002-2008 RISS Ltd.
// All rights reserved.
// 
// This source code or binary libary can be used, distributed or modified
// with or without modification, are permitted including commercial applications.
// -------------- 
//*******************************************************************************
#include <core_i.h>
#include "StdAfx.h"

#pragma message("automatic link to SHLWAPI.LIB")
#pragma comment(lib, "SHLWAPI.LIB")


// DESCRIPTION:
//  Format the specified FILETIME to output in a human readable format
// PARAMETERS:
//  pszFormatTime: [out] Retrives the formatted string time.
//  sFileTime: [in] the UTC-based file time to be converted into a local file time.
//  wDate: input the date (if zero ignore).
//  dwOption: [in] The Options.
//   0:YYYY/MM/DD HH:MM:SS
//   1:MM/DD/YYYY HH:MM:SS
//   2:YYYYMMDDHHMMSS


//////////////////////////////////////////////////////////////////////////
// Format the specified FILETIME to output in a human readable format without using the C run time.
// sFileTime is the UTC-based file time to be converted into a local file time. 
//////////////////////////////////////////////////////////////////////////
DWORD CTimeInfo_i::FormatTime(LPTSTR pszFormatTime, FILETIME sFileTime, DWORD dwOption)
{
	WORD Date = 0, Time = 0;
	pszFormatTime[0] = _T('\0');

  //
  // The MS-DOS date format can represent only dates between 1/1/1980 and 12/31/2107; 
  // this conversion fails if the input file time is outside this range. 
  //
  if (FileTimeToLocalFileTime(&sFileTime, &sFileTime) &&
			FileTimeToDosDateTime(&sFileTime, &Date, &Time))
	{
    if(dwOption == 0)
    {
		  wsprintf(pszFormatTime, _T("%04d/%02d/%02d %02d:%02d:%02d"),
					  (Date / 512) + 1980, (Date / 32) & 15, Date & 31,
					  (Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
    }
    else if(dwOption == 1)
    {
		  wsprintf(pszFormatTime, _T("%02d/%02d/%04d %02d:%02d:%02d"),
					  (Date / 32) & 15,   Date & 31 , (Date / 512) + 1980,
					  (Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
    }
    else if(dwOption == 2)
    {
		  wsprintf(pszFormatTime, _T("%04d%02d%02d%02d%02d%02d"),
					  (Date / 512) + 1980,(Date / 32) & 15, Date & 31 ,
					  (Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
    }

    return ERR_OK_I;
	}
  
  return ERR_WIN32_ERROR_I;
}

//////////////////////////////////////////////////////////////////////////
// GetCurrentTime
//////////////////////////////////////////////////////////////////////////
DWORD CTimeInfo_i::GetCurTime(LPTSTR pszCurrentFormatTime, DWORD dwOption)
{
  U32 ret = ERR_OK_I;

	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
  
	ret = FormatTime(pszCurrentFormatTime, CurrentTime, dwOption);
  
  return ret;
}

DWORD CTimeInfo_i::GetCurrentDateTime(DWORD& dwDate, DWORD &dwTime)
{
  WORD wDate = 0, wTime = 0;
  FILETIME sFileTime = {0, };
  SYSTEMTIME st = {0,};
	GetSystemTimeAsFileTime(&sFileTime); 
  FileTimeToSystemTime(&sFileTime, &st);
  
  //
  // The MS-DOS date format can represent only dates between 1/1/1980 and 12/31/2107; 
  // this conversion fails if the input file time is outside this range. 
  //
  if (FileTimeToLocalFileTime(&sFileTime, &sFileTime) &&
			FileTimeToDosDateTime(&sFileTime, &wDate, &wTime))
	{
    dwTime = st.wMilliseconds<<16 | wTime;
    dwDate = wDate;
    return ERR_OK_I;
  } 
  else
  {
    return ERR_WIN32_ERROR_I;
  }
}

DWORD CTimeInfo_i::ConvertDateTime(TCHAR* pszFormatDate, TCHAR* pszFormatTime, 
                                   DWORD dwDate, DWORD dwTime,
                                   bool bMilliseconds, DWORD dwOption)
{
  WORD wDate           = (WORD) dwDate;
  WORD wTime           = (WORD) dwTime;
  DWORD dwMilliseconds = ((WORD)dwTime) >> 16;
 
  if(dwOption == 0)
  {
    if(pszFormatDate && wDate)
    {
      wsprintf(pszFormatDate, _T("%04d/%02d/%02d"),
        (wDate / 512) + 1980, (wDate / 32) & 15, wDate & 31);
    }
    if(pszFormatTime && wTime)
    {
      if(bMilliseconds)
      {
        wsprintf(pszFormatTime, _T("%02d:%02d:%02d.%03d"),
          (wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, dwMilliseconds);
      }
      else
      {
        wsprintf(pszFormatTime, _T("%02d:%02d:%02d"),
          (wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2);
      }
    }
  }
  else if(dwOption == 1)
  {
    if(pszFormatDate && wDate)
    {
      wsprintf(pszFormatDate, _T("%04d-%02d-%02d"),
        (wDate / 512) + 1980, (wDate / 32) & 15, wDate & 31);
    }
    if(pszFormatTime && wTime)
    {
      if(bMilliseconds)
      {
        wsprintf(pszFormatTime, _T("%02d:%02d:%02d.%03d"),
          (wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, dwMilliseconds);
      }
      else
      {
        wsprintf(pszFormatTime, _T("%02d:%02d:%02d"),
          (wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2);
      }    
    }
  }
  else if(dwOption == 2)
  {
    if(pszFormatDate && wDate)
    {
      wsprintf(pszFormatDate, _T("%04d%02d%02d"),
        (wDate / 512) + 1980, (wDate / 32) & 15, wDate & 31);
    }
    if(pszFormatTime && wTime)
    {
      if(bMilliseconds)
      {
        wsprintf(pszFormatTime, _T("%02d:%02d:%02d.%03d"),
          (wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, dwMilliseconds);
      }
      else
      {
        wsprintf(pszFormatTime, _T("%02d:%02d:%02d"),
          (wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2);
      }    
    }
  }
  
  return ERR_OK_I;
}

#if 0
WORD dosdate,dostime;
FileTimeToDosDateTime(&bhi.ftLastWriteTime,&dosdate,&dostime);
*timestamp = (WORD)dostime | (((DWORD)dosdate)<<16);
#endif  

U32 CTimeInfo_i::GetFileSizeTime(LPCTSTR pszFile, TCHAR* pszOutTime, DWORD* pFileSize) 
{
  //
  // Open the code module file so that we can get its file date and size
  // 
  HANDLE hFile = CreateFile(pszFile, GENERIC_READ,
    FILE_SHARE_READ, 0, OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, 0);
  
  if(pszOutTime)
    pszOutTime[0] = _T('\0');
  
  if(pFileSize) 
    *pFileSize = 0;
  
  // Get File Size and File Time.
  if (hFile != INVALID_HANDLE_VALUE)
  {
    if(pFileSize) *pFileSize = GetFileSize(hFile, 0);
    if(pszOutTime)
    {
      FILETIME LastWriteTime = {0, };
      if (GetFileTime(hFile, 0, 0, &LastWriteTime))
      {
        FormatTime(pszOutTime, LastWriteTime);
      }
    }
    CloseHandle(hFile);
  }
  else
    return 1;
  
  return 0;
}


// CProfileTime cprof;
// cprof.ProfileStart();
// cprof.ProfileEnd();
// printf("%f ms, %f us, %fns\n", (cprof.Ticks2Seconds(ptime)*1000), (cprof.Ticks2Seconds(ptime)*1000*1000), (cprof.Ticks2Seconds(ptime)*1000*1000*10000));

CProfileTime_i::CProfileTime_i(void)
{
  ZeroMemory(&m_Frequency, sizeof(m_Frequency));
  ZeroMemory(&m_StartCounter, sizeof(m_StartCounter));
  
  QueryPerformanceFrequency(&m_Frequency);  // get ticks/sec resolution 
}

void CProfileTime_i::ProfileStart()
{
  QueryPerformanceCounter(&m_StartCounter);   // start time
}

S64 CProfileTime_i::ProfileEnd()
{
  // get and store finishing time and calc elapsed time(ticks)
  QueryPerformanceCounter(&m_EndCounter);
  return (m_EndCounter.QuadPart - m_StartCounter.QuadPart);   // finish time
}

// The following function convert ticks to seconds
double CProfileTime_i::Ticks2Seconds(S64 ticks)
{
  return static_cast < double > (ticks) / static_cast < double > (m_Frequency.QuadPart);
}

double CProfileTime_i::Ticks2Microseconds(S64 ticks)
{
  double sec = static_cast < double > (ticks) / static_cast < double > (m_Frequency.QuadPart);
  
  return sec*1000.0*1000.0;
}
