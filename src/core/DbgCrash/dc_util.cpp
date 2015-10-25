#include "stdafx.h"
#include "dc.h"

//////////////////////////////////////////////////////////////////////////
// Format the specified FILETIME to output in a human readable format without using the C run time.
// sFileTime is the UTC-based file time to be converted into a local file time. 
//////////////////////////////////////////////////////////////////////////
DWORD FormatTime(LPTSTR pszFormatTime, FILETIME sFileTime, DWORD dwOption)
{
	WORD Date = 0, Time = 0;
	pszFormatTime[0] = _T('\0');

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
	}

  return 0;
}

TCHAR* lstrrchr(LPCTSTR string, int ch)
{
	TCHAR *start = (TCHAR *)string;

	while (*string++)                       /* find end of string */
		;
											/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;

	return NULL;
}

//
// GetFilePart
// 
TCHAR * GetFilePart(LPCTSTR source)
{
	TCHAR *result = lstrrchr(source, _T('\\'));
	if (result)
		result++;
	else
		result = (TCHAR *)source;
	return result;
}

static TCHAR g_szModuleName[MAX_PATH*2];
static TCHAR* g_pszFilePart = NULL;

// Combine the .exe file path and pzName.
TCHAR* GetCrashLogFileFullPath(CONST TCHAR* pszName)
{
  if(pszName == NULL)
  {
    g_szModuleName[0] = _T('\0');
	  if (GetModuleFileName(NULL, g_szModuleName, COUNTOF(g_szModuleName)-2) <= 0)
    {
      lstrcpy(g_szModuleName, _T("Unknown"));
      dc_OutputDbgStr(_T("*** Fail to GetModuleFileName. Error=%d\n"), GetLastError());
    }
  }
  else
  {
    lstrcpy(g_pszFilePart, pszName);
  }

  return g_szModuleName;
}

// Combine the .exe file path and pzName.
TCHAR* GetCrashDmpFileFullPath(CONST TCHAR* pszName)
{
  if(pszName == NULL)
  {
    g_szModuleName[0] = _T('\0');
	  if (GetModuleFileName(NULL, g_szModuleName, COUNTOF(g_szModuleName)-2) <= 0)
    {
      lstrcpy(g_szModuleName, _T("Unknown"));
      dc_OutputDbgStr(_T("*** Fail to GetModuleFileName. Error=%d\n"), GetLastError());
    }
  }
  else
  {
    lstrcpy(g_pszFilePart, pszName);
  }

  return g_szModuleName;
}

DWORD InitModulePath(HMODULE hModule)
{
	ZeroMemory(g_szModuleName, sizeof(g_szModuleName));
   
	if (GetModuleFileName(hModule, g_szModuleName, COUNTOF(g_szModuleName)-2) <= 0)
  {
    lstrcpy(g_szModuleName, _T("Unknown"));
    dc_OutputDbgStr(_T("*** Fail to GetModuleFileName. Error=%d\n"), GetLastError());
  }
  
  // Extract the file name portion and remove it's file extension.
	g_pszFilePart = GetFilePart(g_szModuleName);
  
  return 0;
}

TCHAR* GetFileName(TCHAR* szFileName)
{
	lstrcpy(szFileName, g_pszFilePart);
	TCHAR *lastperiod = lstrrchr(szFileName, _T('.'));
	if (lastperiod)
  {
    lastperiod[0] = 0;
  }

  return szFileName;
}

VOID MakeCrashDir()
{
  TCHAR path[MAX_PATH] = { 0, };
  
  if (GetModuleFileName(NULL, path, COUNTOF(path) - 2) <= 0)
  {
    return;
  }
  
  TCHAR *p = lstrrchr(path, _T('\\'));
	if (p)
  {
    *p = '\0';
  }
  
  TCHAR dir[MAX_PATH] = { 0, };
  
  if (p)
  {
    wsprintf(dir, _T("%s\\crash"), path);
    CreateDirectory(dir, NULL);
  }
  else
    CreateDirectory(_T("crash"), NULL);
}

DWORD GetFileSizeTime(LPCTSTR pszFile, TCHAR* pszOutTime, DWORD* pFileSize) 
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


/////////////////////////////////////////////////////////////////////////
// GetCurrentTime_L1
//////////////////////////////////////////////////////////////////////////
DWORD ITS_GetCurrentTime(LPTSTR pszCurrentFormatTime, DWORD dwOption)
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
  
  FormatTime(pszCurrentFormatTime, CurrentTime, dwOption);
  
  return 0;
}

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
DWORD  GetWinOSVer(TCHAR* pszVersion, TCHAR* pszMajorMinorBuild,INT* pVersion)
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

BOOL ITS_IsWin2003(VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && 
    m_stOsVer.dwMajorVersion == 5 && 
    m_stOsVer.dwMinorVersion == 2 );
}

BOOL ITS_IsWinXP(VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && 
    m_stOsVer.dwMajorVersion == 5 && 
    m_stOsVer.dwMinorVersion == 1 );
}

BOOL ITS_IsWin2000(VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && 
    m_stOsVer.dwMajorVersion == 5 && 
    m_stOsVer.dwMinorVersion == 0 );
}

BOOL ITS_IsWin98(VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS && 
    m_stOsVer.dwMajorVersion == 4 && 
    m_stOsVer.dwMinorVersion == 10 );
}

BOOL ITS_IsWinNT(VOID)
{
  OSVERSIONINFO m_stOsVer;

  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  return ( m_stOsVer.dwPlatformId   == VER_PLATFORM_WIN32_NT);
}

BOOL ITS_GetWinServicePack(TCHAR szSp[128])
{
  OSVERSIONINFO m_stOsVer;

  szSp[0] = _T('\0');
  ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));

  m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
  GetVersionEx( &m_stOsVer );
  
  lstrcpy(szSp, m_stOsVer.szCSDVersion);

  return TRUE;
}
