#ifndef __DC_UTL_H__
#define __DC_UTL_H__

#ifndef COUNTOF
#define COUNTOF(array) (sizeof(array)/sizeof(array[0]))
#endif
#define PRIVATE static

DWORD FormatTime(LPTSTR pszFormatTime, FILETIME sFileTime, DWORD dwOption=0);
DWORD GetFileSizeTime(LPCTSTR pszFile, TCHAR* pszOutTime, DWORD* pFileSize=NULL);
DWORD  GetWinOSVer(TCHAR* pszVersion, TCHAR* pszMajorMinorBuild,INT* pVersion=NULL);

DWORD InitModulePath(HMODULE hModule=NULL);
TCHAR* GetFilePart(LPCTSTR source);
TCHAR* GetFileName(TCHAR* szFileName);
DWORD dc_GetCurrentTime(LPTSTR pszCurrentFormatTime, DWORD dwOption);

VOID MakeCrashDir();

//////////////////////////////////////////////////////////////////////////
// Get the full path by the specified file name.
// EXAMPLE:
// TCHAR* pszLogFile = GetFileFullPath(_T("crash.log"));
//
TCHAR* GetCrashLogFileFullPath(CONST TCHAR* pszName); 
TCHAR* GetCrashDmpFileFullPath(CONST TCHAR* pszName);

TCHAR* lstrrchr(LPCTSTR string, int ch);

BOOL dc_GetFileVersion(CONST TCHAR* pszFile, WORD wFileVersion[4]);
BOOL dc_GetProductVersion(CONST TCHAR* pszFile, WORD wProductVersion[4]);
BOOL dc_GetFileDescription(CONST TCHAR* pszFile, TCHAR* szFileDesc, INT nSize);
BOOL dc_GetProductName(CONST TCHAR* pszFile, TCHAR* szProductName, INT nSize);
BOOL dc_GetCompanyName(CONST TCHAR* pszFile, TCHAR* szCompanyName, INT nSize);

#endif // __UTL_H__

