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
#include "VersionInfo.h"

// #undef _T
// #ifdef UNICODE 
// #define _T(x)      L ## x
// #else
// #define _T(x)      x
// #endif 

#pragma message("automatic link to VERSION.LIB")
#pragma comment(lib, "version.lib")

#if 1 
CFileVersionInfo::CFileVersionInfo(LPCTSTR lpszPath)
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

CFileVersionInfo::~CFileVersionInfo()
{
  Release();
}

//
// Init
//
bool CFileVersionInfo::Init()
{
  DWORD dwHandle = 0;
  DWORD dwSize = 0;
  bool rc = FALSE;
  
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
void CFileVersionInfo::Release()
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
bool CFileVersionInfo::GetFileVersion(WORD * pwVersion)
{
  for (int i = 0; i < 4; i++)
    *pwVersion++ = m_wFileVersion[i];
  
  return TRUE;
}					 	 

//
// GetProductVersion
//
bool CFileVersionInfo::GetProductVersion(WORD * pwVersion)
{
  for (int i = 0; i < 4; i++)
    *pwVersion++ = m_wProductVersion[i];

  return TRUE;
}

#ifdef HAVE_VESIONINFO_EXTERN
//
// GetFileFlags
// 
bool CFileVersionInfo::GetFileFlags(DWORD& rdwFlags)
{
  rdwFlags = m_dwFileFlags;

  return TRUE;
}

//
// GetFileOS
//
bool CFileVersionInfo::GetFileOS(DWORD& rdwOS)
{
  rdwOS = m_dwFileOS;

  return TRUE;
}

//
// GetFileType
//
bool CFileVersionInfo::GetFileType(DWORD& rdwType)
{
  rdwType = m_dwFileType;

  return TRUE;
}

//
// GetFileSubtype
//
bool CFileVersionInfo::GetFileSubtype(DWORD& rdwType)
{
  rdwType = m_dwFileSubtype;

  return TRUE;
}
#endif // HAVE_VESIONINFO_EXTERN

//
// GetCompanyName
//
bool CFileVersionInfo::GetCompanyName(LPTSTR lpszCompanyName, int nSize)
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
bool CFileVersionInfo::GetFileDescription(LPTSTR lpszFileDescription, int nSize)
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
bool CFileVersionInfo::GetProductName(LPTSTR lpszProductName, int nSize)
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
bool CFileVersionInfo::GetComments(LPTSTR lpszProductName, int nSize)
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
bool CFileVersionInfo::GetLegalCopyright(LPTSTR lpszProductName, int nSize)
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
bool CFileVersionInfo::GetLegalTrademarks(LPTSTR lpszProductName, int nSize)
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
bool CFileVersionInfo::GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
  bool rc = FALSE;
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

bool GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId,
                                        DWORD &dwId, bool bPrimaryEnough/*= FALSE*/)
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
bool CFileVersionInfo::GetStringInfo(LPCTSTR lpszKey, LPTSTR lpszReturnValue, INT nReturnValueSize)
{
  bool rc = FALSE;
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

#endif

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

bool CVersionInfo_i::GetCompanyName(CONST TCHAR* pszFile, TCHAR* szCompanyName, INT nSize)
{
  CFileVersionInfo ver(pszFile);

  return (bool)ver.GetCompanyName(szCompanyName, nSize);
}

bool CVersionInfo_i::GetProductName(CONST TCHAR* pszFile, TCHAR* szProductName, INT nSize)
{
  CFileVersionInfo ver(pszFile);

  return (bool)ver.GetProductName(szProductName, nSize);
}

bool CVersionInfo_i::GetFileDescription(CONST TCHAR* pszFile, TCHAR* szFileDesc, INT nSize)
{
  CFileVersionInfo ver(pszFile);

  return (bool)ver.GetFileDescription(szFileDesc, nSize);
}

bool CVersionInfo_i::GetFileVersion(CONST TCHAR* pszFile, WORD wFileVersion[4])
{
  CFileVersionInfo ver(pszFile);

  return (bool)ver.GetFileVersion(wFileVersion);
}

bool CVersionInfo_i::GetProductVersion(CONST TCHAR* pszFile, WORD wProductVersion[4])
{
  CFileVersionInfo ver(pszFile);
    
  return (bool)ver.GetProductVersion(wProductVersion);
}


bool CVersionInfo_i::GetStringInfo(LPCTSTR lpszFile, 
                                   LPCTSTR lpszKey, 
                                   LPTSTR lpszReturnValue, INT nReturnValueSize) 
{
  CFileVersionInfo ver(lpszFile);
  
  return (bool)ver.GetStringInfo(lpszKey, lpszReturnValue, nReturnValueSize);
}
 