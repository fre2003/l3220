#include "StdAfx.h"
 
//////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////
class CFileVersionInfoDc
{
  // constructors
public:
  CFileVersionInfoDc(LPCTSTR lpszPath = NULL);
  virtual ~CFileVersionInfoDc();
  
  // attributes
public:
  // fixed info
  BOOL GetFileVersion(WORD *pwVersion);
  BOOL GetProductVersion(WORD* pwVersion);

  // string info
  BOOL GetCompanyName(LPTSTR lpszCompanyName, INT nSize);
  BOOL GetFileDescription(LPTSTR lpszFileDescription, INT nSize);
  BOOL GetProductName(LPTSTR lpszProductName, INT nSize);
  
  BOOL GetComments(LPTSTR lpszProductName, INT nSize);
  BOOL GetLegalTrademarks(LPTSTR lpszProductName, INT nSize);
  BOOL GetLegalCopyright(LPTSTR lpszProductName, INT nSize);
    
  // implementation
protected:
  BOOL Init();
  void Release();
  BOOL GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo);
  BOOL GetStringInfo(LPCTSTR lpszKey, LPTSTR lpszReturnValue, INT nReturnValueSize=0);
  
  // implementation
protected:
  BYTE*		m_pData;
  DWORD		m_dwHandle;
  WORD		m_wFileVersion[4];
  WORD		m_wProductVersion[4];
  TCHAR		m_szPath[MAX_PATH*2];
};

// #undef _T
// #ifdef UNICODE 
// #define _T(x)      L ## x
// #else
// #define _T(x)      x
// #endif 

#pragma message("automatic link to VERSION.LIB")
#pragma comment(lib, "version.lib")

CFileVersionInfoDc::CFileVersionInfoDc(LPCTSTR lpszPath)
{
  ZeroMemory(m_szPath, sizeof(m_szPath));
  
  if (lpszPath && lpszPath[0] != 0)
  {
    lstrcpyn(m_szPath, lpszPath, sizeof(m_szPath)-1);
  }
  else
  {
  }
  
  m_pData = NULL;
  m_dwHandle = 0;
  
  for (int i = 0; i < 4; i++)
  {
    m_wFileVersion[i] = 0;
    m_wProductVersion[i] = 0;
  }
  
#ifdef HAVE_VESIONINFO_EXTERN
  m_dwFileFlags = 0;
  m_dwFileOS = 0;
  m_dwFileType = 0;
  m_dwFileSubtype = 0;
#endif 
  
  Init();
}

CFileVersionInfoDc::~CFileVersionInfoDc()
{
  Release();
}

//
// Init
//
BOOL CFileVersionInfoDc::Init()
{
  DWORD dwHandle = 0;
  DWORD dwSize = 0;
  BOOL rc = FALSE;
  
  dwSize = ::GetFileVersionInfoSize(m_szPath, &dwHandle);
  if (dwSize == 0)
  {
    return FALSE;
  }
		
  m_pData = new BYTE [dwSize + 1];	
  ZeroMemory(m_pData, dwSize+1);
  
  rc = ::GetFileVersionInfo(m_szPath, dwHandle, dwSize, m_pData);
  if (!rc)
  {
    return FALSE;
  }
  
  // get fixed info
  VS_FIXEDFILEINFO FixedInfo = {0,};
  
  if (GetFixedInfo(FixedInfo))
  {
    m_wFileVersion[0] = HIWORD(FixedInfo.dwFileVersionMS);
    m_wFileVersion[1] = LOWORD(FixedInfo.dwFileVersionMS);
    m_wFileVersion[2] = HIWORD(FixedInfo.dwFileVersionLS);
    m_wFileVersion[3] = LOWORD(FixedInfo.dwFileVersionLS);
    
    m_wProductVersion[0] = HIWORD(FixedInfo.dwProductVersionMS);
    m_wProductVersion[1] = LOWORD(FixedInfo.dwProductVersionMS);
    m_wProductVersion[2] = HIWORD(FixedInfo.dwProductVersionLS);
    m_wProductVersion[3] = LOWORD(FixedInfo.dwProductVersionLS);
    
#ifdef HAVE_VESIONINFO_EXTERN
    m_dwFileFlags   = FixedInfo.dwFileFlags;
    m_dwFileOS      = FixedInfo.dwFileOS;
    m_dwFileType    = FixedInfo.dwFileType;
    m_dwFileSubtype = FixedInfo.dwFileSubtype;
#endif 
  }
  else
  {
    return FALSE;
  }
    
  return TRUE;		
}

//
// Release
//
void CFileVersionInfoDc::Release()
{
  // do this manually, because we can't use objects requiring
  // a dtor within an exception handler
  if (m_pData)
    delete [] m_pData;
  m_pData = NULL;
}

//
// GetFileVersion
//
BOOL CFileVersionInfoDc::GetFileVersion(WORD * pwVersion)
{
  for (int i = 0; i < 4; i++)
    *pwVersion++ = m_wFileVersion[i];

  return TRUE;
}					 	 

//
// GetProductVersion
//
BOOL CFileVersionInfoDc::GetProductVersion(WORD * pwVersion)
{
  for (int i = 0; i < 4; i++)
    *pwVersion++ = m_wProductVersion[i];

  return TRUE;
}

#ifdef HAVE_VESIONINFO_EXTERN
//
// GetFileFlags
// 
BOOL CFileVersionInfoDc::GetFileFlags(DWORD& rdwFlags)
{
  rdwFlags = m_dwFileFlags;

  return TRUE;
}

//
// GetFileOS
//
BOOL CFileVersionInfoDc::GetFileOS(DWORD& rdwOS)
{
  rdwOS = m_dwFileOS;

  return TRUE;
}

//
// GetFileType
//
BOOL CFileVersionInfoDc::GetFileType(DWORD& rdwType)
{
  rdwType = m_dwFileType;

  return TRUE;
}

//
// GetFileSubtype
//
BOOL CFileVersionInfoDc::GetFileSubtype(DWORD& rdwType)
{
  rdwType = m_dwFileSubtype;

  return TRUE;
}
#endif // HAVE_VESIONINFO_EXTERN

//
// GetCompanyName
//
BOOL CFileVersionInfoDc::GetCompanyName(LPTSTR lpszCompanyName, int nSize)
{
  if (!lpszCompanyName || !nSize)
    return FALSE;

  ZeroMemory(lpszCompanyName, nSize);

  // Get string lock info.
  GetStringInfo(_T("CompanyName"),     lpszCompanyName, nSize-1);

  return TRUE;
}

//
// GetFileDescription
//
BOOL CFileVersionInfoDc::GetFileDescription(LPTSTR lpszFileDescription, int nSize)
{
  if (!lpszFileDescription)
    return FALSE;
  
  ZeroMemory(lpszFileDescription, nSize);
  GetStringInfo(_T("FileDescription"), lpszFileDescription, nSize-1);
  
  return TRUE;
}

//
// GetProductName
//
BOOL CFileVersionInfoDc::GetProductName(LPTSTR lpszProductName, int nSize)
{
  if (!lpszProductName)
    return FALSE;
  
  ZeroMemory(lpszProductName, nSize);
  GetStringInfo(_T("ProductName"), lpszProductName, nSize-1);
  
  return TRUE;
}

//
// GetComments
//
BOOL CFileVersionInfoDc::GetComments(LPTSTR lpszProductName, int nSize)
{
  if (!lpszProductName)
    return FALSE;
  
  ZeroMemory(lpszProductName, nSize);
  GetStringInfo(_T("Comments"), lpszProductName, nSize-1);
  
  return TRUE;
}

//
// GetLegalCopyright
//
BOOL CFileVersionInfoDc::GetLegalCopyright(LPTSTR lpszProductName, int nSize)
{
  if (!lpszProductName)
    return FALSE;
  
  ZeroMemory(lpszProductName, nSize);
  GetStringInfo(_T("LegalCopyright"), lpszProductName, nSize-1);
  
  return TRUE;
}

//
// GetComments
//
BOOL CFileVersionInfoDc::GetLegalTrademarks(LPTSTR lpszProductName, int nSize)
{
  if (!lpszProductName)
    return FALSE;
  
  ZeroMemory(lpszProductName, nSize);
  GetStringInfo(_T("LegalTrademarks"), lpszProductName, nSize-1);
  
  return TRUE;
}

//
// GetFixedInfo
//
BOOL CFileVersionInfoDc::GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
  BOOL rc = FALSE;
  UINT nLength = 0;
  VS_FIXEDFILEINFO *pFixedInfo = NULL;
  
  if (!m_pData)
    return FALSE;
  
  if (m_pData)
    rc = ::VerQueryValue(m_pData, _T("\\"), (void **) &pFixedInfo, &nLength);
  else
    rc = FALSE;
		
  if (rc)
    memcpy (&rFixedInfo, pFixedInfo, sizeof (VS_FIXEDFILEINFO));	
  
  return rc;
}

BOOL GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId,
                                        DWORD &dwId, BOOL bPrimaryEnough/*= FALSE*/)
{
	for (LPWORD lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
	{
		if (*lpwData == wLangId)
		{
			dwId = *((DWORD*)lpwData);
			return TRUE;
		}
	}

	if (!bPrimaryEnough)
		return FALSE;

	for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
	{
		if (((*lpwData)&0x00FF) == (wLangId&0x00FF))
		{
			dwId = *((DWORD*)lpwData);
			return TRUE;
		}
	}

	return FALSE;
}

//
// GetStringInfo
// Get the information from specified block.
//
BOOL CFileVersionInfoDc::GetStringInfo(LPCTSTR lpszKey, LPTSTR lpszReturnValue, INT nReturnValueSize)
{
  BOOL rc = FALSE;
#if 0
  DWORD *pdwTranslation = NULL;
#endif 
  UINT nLength = 0;
  LPTSTR lpszValue = NULL;
  
  if (m_pData == NULL)
    return FALSE;
  
  if (!lpszReturnValue)
    return FALSE;
  
  if (!lpszKey)
    return FALSE;
  
  *lpszReturnValue = 0;  

  // catch default information
  LPVOID	lpInfo;
  UINT		unInfoLen;

  // Find best matching language and codepage
#if 1
  rc = ::VerQueryValue(m_pData, _T("\\VarFileInfo\\Translation"), &lpInfo, &unInfoLen);
#else
  rc = ::VerQueryValue(m_pData, _T("\\VarFileInfo\\Translation"), (void**) &pdwTranslation, &nLength);
#endif 
  if (!rc)
    return FALSE;
  
#if 1
  DWORD	dwLangCode = 0;
  if (!GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, FALSE))
  {
    if (!GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, TRUE))
    {
      if (!GetTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), dwLangCode, TRUE))
      {
        if (!GetTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL), dwLangCode, TRUE))
          // use the first one we can get
          dwLangCode = *((DWORD*)lpInfo);
      }
    }
  }
#endif 

  TCHAR szKey[256];
  
#if 0
  wsprintf(szKey, _T("\\StringFileInfo\\%04x%04x\\%s"),LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),lpszKey);
#else
  wsprintf(szKey, _T("\\StringFileInfo\\%04x%04x\\%s"),dwLangCode&0x0000FFFF, (dwLangCode&0xFFFF0000)>>16,lpszKey);
#endif 
  
  rc = ::VerQueryValue(m_pData, szKey, (void**) &lpszValue, &nLength);
  
  if (!rc)
    return FALSE;
  
  lstrcpyn(lpszReturnValue, lpszValue, nReturnValueSize);
  
  return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
// C Routines
//
//////////////////////////////////////////////////////////////////////////


BOOL dc_GetCompanyName(CONST TCHAR* pszFile, TCHAR* szCompanyName, INT nSize)
{
  CFileVersionInfoDc ver(pszFile);

  return (BOOL)ver.GetCompanyName(szCompanyName, nSize);
}

BOOL dc_GetProductName(CONST TCHAR* pszFile, TCHAR* szProductName, INT nSize)
{
  CFileVersionInfoDc ver(pszFile);

  return (BOOL)ver.GetProductName(szProductName, nSize);
}

BOOL dc_GetFileDescription(CONST TCHAR* pszFile, TCHAR* szFileDesc, INT nSize)
{
  CFileVersionInfoDc ver(pszFile);

  return (BOOL)ver.GetFileDescription(szFileDesc, nSize);
}

BOOL dc_GetFileVersion(CONST TCHAR* pszFile, WORD wFileVersion[4])
{
  CFileVersionInfoDc ver(pszFile);

  return (BOOL)ver.GetFileVersion(wFileVersion);
}

BOOL dc_GetProductVersion(CONST TCHAR* pszFile, WORD wProductVersion[4])
{
  CFileVersionInfoDc ver(pszFile);
    
  return (BOOL)ver.GetProductVersion(wProductVersion);
}

