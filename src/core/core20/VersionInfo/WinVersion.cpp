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
#include "StdAfx.h"

#undef _T 

#ifdef UNICODE 
#define _T(x)      L ## x
#else
#define _T(x)      x
#endif 

#define WUNKNOWNSTR	_T("Unknown Windows Version")

#define W95STR			    _T("Windows 95")
#define W95SP1STR		    _T("Windows 95 SP1")
#define W95OSR2STR      _T("Windows 95 OSR2")
#define W98STR			    _T("Windows 98")
#define W98SP1STR		    _T("Windows 98 SP1")
#define W98SESTR	  	  _T("Windows 98 SE")
#define WMESTR			    _T("Windows ME")

#define WNT351STR		    _T("Windows NT 3.51")
#define WNT4STR		    	_T("Windows NT 4")
#define W2KSTR			    _T("Windows 2000")
#define WXPSTR			    _T("Windows XP")
#define W2003SERVERSTR	_T("Windows 2003 Server")

#define WCESTR		    	_T("Windows CE")

#define WUNKNOWN   	0

#define W9XFIRST  	1
#define W95		    	1
#define W95SP1	   	2
#define W95OSR2	  	3
#define W98         4
#define W98SP1		  5
#define W98SE       6
#define WME			    7
#define W9XLAST		  99

#define WNTFIRST	  101
#define WNT351		  101
#define WNT4		    102
#define W2K		    	103
#define WXP			    104
#define W2003SERVER	105
#define WNTLAST		  199

#define WCEFIRST	  201
#define WCE			    201
#define WCELAST		  299

// from winbase.h
#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s             0
#endif
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS      1
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT           2
#endif
#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE           3
#endif

/*
This table has been assembled from Usenet postings, personal
observations, and reading other people's code.  Please feel
free to add to it or correct it.

  dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
  95             1              4               0             950
  95 SP1         1              4               0            >950 && <=1080
  95 OSR2        1              4               <10           >1080
  98             1              4              10            1998
  98 SP1         1              4              10            >1998 && <2183
  98 SE          1              4              10            >=2183
  ME             1              4              90            3000
  
    NT 3.51        2              3              51
    NT 4           2              4               0            1381
    2000           2              5               0            2195
    XP             2              5               1            2600
    2003 Server    2              5               2            3790
    CE             3
*/

//
// GetWinOSVer
//
  // EXAMPLE:
  //	TCHAR szWinVer[50], szMajorMinorBuild[50];
  //	GetWinOSVer_2(szWinVer,szMajorMinorBuild); 
  //  Print(_T("Operating system:  %s (%s).\r\n"), szWinVer, szMajorMinorBuild);
  //  >> Operating system:  Windows XP (5.1.2600). 
  //

DWORD CVersionInfo_i::GetWinOSVer(TCHAR* pszVersion, TCHAR* pszMajorMinorBuild,INT* pVersion)
{
  if (!pszVersion || !pszMajorMinorBuild)
  {
    return (DWORD)(~0);
  }
  
  pszMajorMinorBuild[0] = _T('\0');
  pszVersion[0] = _T('\0');
  
  lstrcpy(pszVersion, WUNKNOWNSTR);
  if(pVersion)
    *pVersion = WUNKNOWN;
  
  OSVERSIONINFO osinfo = {0,};
  osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  
  if (!GetVersionEx(&osinfo))
    return (DWORD)(~0);
  
  DWORD dwPlatformId   = osinfo.dwPlatformId;
  DWORD dwMinorVersion = osinfo.dwMinorVersion;
  DWORD dwMajorVersion = osinfo.dwMajorVersion;
  DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this
  
  wsprintf(pszMajorMinorBuild, _T("%u.%u.%u"), dwMajorVersion, dwMinorVersion, dwBuildNumber);
  
  if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
  {
    if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
    {
      lstrcpy(pszVersion, W95STR);
      if(pVersion) *pVersion = W95;
    }
    else if ((dwMinorVersion < 10) && 
      ((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
    {
      lstrcpy(pszVersion, W95SP1STR);
      if(pVersion) *pVersion = W95SP1;
    }
    else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
    {
      lstrcpy(pszVersion, W95OSR2STR);
      if(pVersion) *pVersion = W95OSR2;
    }
    else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
    {
      lstrcpy(pszVersion, W98STR);
      if(pVersion) *pVersion = W98;
    }
    else if ((dwMinorVersion == 10) && 
      ((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
    {
      lstrcpy(pszVersion, W98SP1STR);
      if(pVersion) *pVersion = W98SP1;
    }
    else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
    {
      lstrcpy(pszVersion, W98SESTR);
      if(pVersion) *pVersion = W98SE;
    }
    else if (dwMinorVersion == 90)
    {
      lstrcpy(pszVersion, WMESTR);
      if(pVersion) *pVersion = WME;
    }
  }
  else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
    {
      lstrcpy(pszVersion, WNT351STR);
      if(pVersion) *pVersion = WNT351;
    }
    else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
    {
      lstrcpy(pszVersion, WNT4STR);
      if(pVersion) *pVersion = WNT4;
    }
    else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
    {
      lstrcpy(pszVersion, W2KSTR);
      if(pVersion) *pVersion = W2K;
    }
    else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
    {
      lstrcpy(pszVersion, WXPSTR);
      if(pVersion) *pVersion = WXP;
    }
    else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
    {
      lstrcpy(pszVersion, W2003SERVERSTR);
      if(pVersion) *pVersion = W2003SERVER;
    }
  }
  else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
  {
    lstrcpy(pszVersion, WCESTR);
    if(pVersion) *pVersion = WCE;
  }
  
  return 0;
}

bool CVersionInfo_i::IsWin7(VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && 
    m_stOsVer.dwMajorVersion == 7 && 
    m_stOsVer.dwMinorVersion == 2 );
}

bool CVersionInfo_i::IsWin2003 (VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && 
    m_stOsVer.dwMajorVersion == 5 && 
    m_stOsVer.dwMinorVersion == 2 );
}

bool CVersionInfo_i::IsWinXP(VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && 
    m_stOsVer.dwMajorVersion == 5 && 
    m_stOsVer.dwMinorVersion == 1 );
}

bool CVersionInfo_i::IsWin2000 (VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && 
    m_stOsVer.dwMajorVersion == 5 && 
    m_stOsVer.dwMinorVersion == 0 );
}

bool CVersionInfo_i::IsWin98 (VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS && 
    m_stOsVer.dwMajorVersion == 4 && 
    m_stOsVer.dwMinorVersion == 10 );
}
 
bool CVersionInfo_i::IsWinNT (VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT);
}

bool CVersionInfo_i::GetWinServicePack (TCHAR szSp[128])
{
  OSVERSIONINFO m_stOsVer;

  szSp[0] = _T('\0');
  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));

  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  
  lstrcpy(szSp, m_stOsVer.szCSDVersion);

  return TRUE;
}

