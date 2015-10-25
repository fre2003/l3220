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
//////////////////////////////////////////////////////////////////////////
// This file implement all functions to this module only.
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"


//////////////////////////////////////////////////////////////////////////
// 
// Function:
// This debugging function prints out a string to the Debugger
// (the debug output in VC).
// An optional set of substitutional parameters can be specified,
// and the final output will be the processed result of these combined
// with the format string, just like printf.
// A newline is always output after every call to this function.
// 
// Arguments:
//   LPCTSTR fmt - Format string (printf style).
//   ...        - Variable number of arguments.
// 
// Returns:
//    VOID
//////////////////////////////////////////////////////////////////////////
#if 0 
VOID OutputDbgStr(LPCTSTR szFormat, ...) 
{
  va_list arglist;
  TCHAR szOutputDgbStr[512]; 
  int nPos = 0;

  // get local time
  SYSTEMTIME sysTime;
  ::GetLocalTime(&sysTime);

  szOutputDgbStr[0] = _T('\0');
  
#if 0 
  nPos = _stprintf
    (
    szOutputDgbStr,
    _T("%02d:%02d:%02d_%03d: "), 
    sysTime.wHour,
    sysTime.wMinute,
    sysTime.wSecond,
    sysTime.wMilliseconds
    );
#else
  nPos = _stprintf
    (
    szOutputDgbStr,
    _T("[its_bcore] "), 
    sysTime.wHour,
    sysTime.wMinute,
    sysTime.wSecond,
    sysTime.wMilliseconds
    );
#endif 
  
  if(nPos > sizeof(szOutputDgbStr))
    return ;

  va_start(arglist, szFormat);
  nPos += _vsntprintf(szOutputDgbStr+nPos, sizeof(szOutputDgbStr)-1-nPos, szFormat, arglist);
  // wvsprintf(szOutputDgbStr, szFormat, arglist); // Max is 1024.
  va_end(arglist);
  
  OutputDebugString(szOutputDgbStr);
}
#else
#define DBG_HEAD  _T("[its_bcore] ")
VOID OutputDbgStr(LPCTSTR szFormat, ...) 
{
	va_list arglist;
  TCHAR szOutputDgbStr[1024]; 
  
  szOutputDgbStr[0] = _T('\0');
  lstrcpy(szOutputDgbStr, DBG_HEAD);
  
  va_start(arglist, szFormat);
	wvsprintf(&szOutputDgbStr[lstrlen(DBG_HEAD)], szFormat, arglist); // Max is 1024.
	va_end(arglist);
  
  OutputDebugString(szOutputDgbStr);
}

#endif 

DWORD SaveToNewFile(LPCTSTR pszFile, PVOID pData, DWORD dwSize)
{
  HANDLE hFile;
  DWORD dwRet = 0;
  DWORD dwBytesWritten;
  
  hFile = CreateFile(pszFile,
                     GENERIC_WRITE,
                     FILE_SHARE_READ|FILE_SHARE_WRITE,
                     NULL,
                     CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);

  // failure could be due to missing attributes (2k/XP)
  if (hFile == INVALID_HANDLE_VALUE)
  {
    DWORD dwAttributes = GetFileAttributes(pszFile);
    if (dwAttributes != 0) // INVALID_FILE_ATTRIBUTES FILE_ATTRIBUTE_READONLY
    {
      dwAttributes = dwAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
      hFile = CreateFile(pszFile,
                        GENERIC_WRITE,
                        FILE_SHARE_READ|FILE_SHARE_WRITE,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL | dwAttributes,
                        NULL);
    }
  }

  if (hFile == INVALID_HANDLE_VALUE)
    return GetLastError();

  if(WriteFile(hFile,(LPCVOID)pData, dwSize, &dwBytesWritten, NULL) == FALSE)
    dwRet = GetLastError();
  
  CloseHandle(hFile);

  return dwRet;
}
