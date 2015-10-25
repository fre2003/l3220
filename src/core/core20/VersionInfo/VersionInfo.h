//////////////////////////////////////////////////////////////////////////
// VersionInfo.h: interface 
//
/////////////////////////////////////////////////////////////////////////
#if !defined(__VERSIONINFO_H__)
#define __VERSIONINFO_H__

#ifndef HAVE_NO_CPP_APIS 
class CFileVersionInfo
{
  // constructors
public:
  CFileVersionInfo(LPCTSTR lpszPath = NULL);
  virtual ~CFileVersionInfo();
  
  // attributes
public:
  // fixed info
  bool GetFileVersion(WORD *pwVersion);
  bool GetProductVersion(WORD* pwVersion);

  // string info
  bool GetCompanyName(LPTSTR lpszCompanyName, INT nSize);
  bool GetFileDescription(LPTSTR lpszFileDescription, INT nSize);
  bool GetProductName(LPTSTR lpszProductName, INT nSize);
  
  bool GetComments(LPTSTR lpszProductName, INT nSize);
  bool GetLegalTrademarks(LPTSTR lpszProductName, INT nSize);
  bool GetLegalCopyright(LPTSTR lpszProductName, INT nSize);
  bool GetStringInfo(LPCTSTR lpszKey, LPTSTR lpszReturnValue, INT nReturnValueSize=0);
    
  // implementation
protected:
  bool Init();
  void Release();
  bool GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo);
  
  // implementation
protected:
  BYTE*		m_pData;
  DWORD		m_dwHandle;
  WORD		m_wFileVersion[4];
  WORD		m_wProductVersion[4];
  TCHAR		m_szPath[MAX_PATH*2];
};
#endif //HAVE_NO_CPP_APIS

/*
//
// Get the File Version Information
// Company,Product,description,File Version,Product Version.
//

// 
// EXAMPLE:
// TCHAR szBuf[200];
// GetCompanyName(szModName,szBuf, sizeof(szBuf)-1);
//
// GetFileVersion(dwBuf);
//  Print(_T("   FileVer:    %d.%d.%d.%d\r\n"), dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]); // -->1.0.1.500

BOOLEAN GetCompanyName(CONST TCHAR* pszFile, TCHAR* szCompanyName, INT nSize);
BOOLEAN GetProductName(CONST TCHAR* pszFile, TCHAR* szProductName, INT nSize);
BOOLEAN GetFileDescription(CONST TCHAR* pszFile, TCHAR* szFileDesc, INT nSize);
BOOLEAN GetFileVersion(CONST TCHAR* pszFile, WORD wFileVersion[4]);
BOOLEAN GetProductVersion(CONST TCHAR* pszFile, WORD wProductVersion[4]);

BOOLEAN IsWin2003(VOID);
BOOLEAN IsWinXP(VOID);
BOOLEAN IsWin2000(VOID);
BOOLEAN IsWin98(VOID);
BOOLEAN IsWinNTSerial(VOID);
BOOLEAN GetWinServicePack(TCHAR szSp[128]);

//
// Get WinOsVerion
//
// EXAMPLE:
//	TCHAR szWinVer[50], szMajorMinorBuild[50];
//	GetWinOSVer(szWinVer,szMajorMinorBuild); 
//  Print(_T("Operating system:  %s (%s).\r\n"), szWinVer, szMajorMinorBuild);
//  >> Operating system:  Windows XP (5.1.2600). 
//
DWORD GetWinOSVer(TCHAR* pszVersion, TCHAR* pszMajorMinorBuild, INT* pVersion=NULL);


//////////////////////////////////////////////////////////////////////////

#ifndef HAVE_NO_VERSIONINFO_LIB
#define __VERSIONINFO_LIBNAME1__	"VersionInfo" 

#ifdef UNICODE
#define __VERSIONINFO_LIBNAME2__	__VERSIONINFO_LIBNAME1__##"U"
#pragma message("VersionInfo UNICODE Version!")
#else
#define __VERSIONINFO_LIBNAME2__	__VERSIONINFO_LIBNAME1__
#endif 

#ifdef _DEBUG
#define __VERSIONINFO_LIBNAME3__	__VERSIONINFO_LIBNAME2__##"D"
#pragma message("VersionInfo DEBUG Version!")
#else
#define __VERSIONINFO_LIBNAME3__	__VERSIONINFO_LIBNAME2__
#endif

#if (defined(_DLL) || defined(_AFXDLL)) && !defined HAVE_RSS_STATIC_LIB
	#define __VERSIONINFO_LIBNAME4__	__VERSIONINFO_LIBNAME3__
#elif defined HAVE_VERSIONINFO_STATIC_LIB 
	#define __VERSIONINFO_LIBNAME4__	__VERSIONINFO_LIBNAME3__##"s"
#else
	#define __VERSIONINFO_LIBNAME4__	__VERSIONINFO_LIBNAME3__
#endif

#if _MSC_VER < 1300
	#define __VERSIONINFO_LIBNAME__	__VERSIONINFO_LIBNAME4__##".lib"
#elif _MSC_VER == 1300
	#define __VERSIONINFO_LIBNAME__	__VERSIONINFO_LIBNAME4__##"70.lib"
#elif _MSC_VER < 1400
	#define __VERSIONINFO_LIBNAME__	__VERSIONINFO_LIBNAME4__##"71.lib"
#elif _MSC_VER < 1500
	#define __VERSIONINFO_LIBNAME__	__VERSIONINFO_LIBNAME4__##"80.lib"
#elif _MSC_VER == 1500
	#define __VERSIONINFO_LIBNAME__	__VERSIONINFO_LIBNAME4__##"90.lib"
#else
	#error "Unsupported VC++ version"
#endif

#pragma comment(lib, __VERSIONINFO_LIBNAME__)

#pragma message("Automatically linking with " __VERSIONINFO_LIBNAME__)
#endif // HAVE_NO_VERSIONINFO_LIB 
*/

#endif // !defined(__VERSIONINFO_H__)
