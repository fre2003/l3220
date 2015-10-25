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

bool RegExportCygwin();

// void ExpandEnvironmentStringsEx(LPSTR lpSrc,DWORD dwSrc)
// {
//   char szBuf[312];
// 
//   if (ExpandEnvironmentStrings(lpSrc,szBuf, COUNTOF(szBuf)))
//     lstrcpyn(lpSrc,szBuf,dwSrc);
// }

CygwinUtl_i::CygwinUtl_i(VOID)
{   
  GetTempPath(MAX_PATH-1,m_szTempPath);
  if(!PathFileExists(m_szTempPath))
    lstrcpy(m_szTempPath,_T("C:\\"));
}

bool CygwinUtl_i::IsCygwinRunning(VOID)
{
  bool bRet = FALSE; 
  
  if(CProcessUtil_i::IsProcessRunning(_T("bash.exe")))
    bRet = TRUE;
  
  return bRet;
}

bool CygwinUtl_i::KillCygwinProcess(VOID)
{
  bool bRet = FALSE; 
  
  if(CProcessUtil_i::KillProcess(_T("bash.exe"), 200))
    bRet = TRUE;
  
  return bRet;
}

bool CygwinUtl_i::IsCygwinRegistered(VOID)
{
  bool bRet = FALSE;

#if 0 
  DWORD ret = ERROR_SUCCESS;
  DWORD dwType = REG_SZ;
  DWORD cbData = 512;
  char szBuf[512] = {'\0', };
  DWORD dwIndex = 0;
#endif // #if 0
  HKEY hKey = NULL;

  if(RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Cygnus Solutions\\Cygwin"), &hKey) == ERROR_SUCCESS && hKey)
  {
    bRet = TRUE;
    RegCloseKey(hKey);
  }
  
#if 0 
  //HKEY_LOCAL_MACHINE\SOFTWARE\Cygnus Solutions\Cygwin\mounts v2
  //[HKEY_LOCAL_MACHINE\SOFTWARE\Cygnus Solutions]
  //ret = SHGetValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Cygnus Solutions\\Cygwin", "Install Dirs", &dwType, (LPVOID)szBuf, &cbData);
  ret = SHEnumKeyEx(HKEY_LOCAL_MACHINE"SOFTWARE\\Cygnus Solutions\\Cygwin", dwIndex, szBuf, &cbData);
  if(ret == ERROR_SUCCESS)
  {
#if 0
    PathRemoveFileSpec(szBuf); // -->.. \MsDev98 Folder
    if(PathFileExists(szBuf))
    {
      strcpy(pszInsPath, szBuf);
      return TRUE;
    }
#endif // #if 0
    bRet = TRUE;
  }
#endif // #if 0
  
#if 0 
  if(bRet)
  {
    ITS_RegDisableCygwin(); // Disable Cygwin
  }
#endif // #if 0
  
  return bRet;
}

bool CygwinUtl_i::RegEnableCygwin(VOID)
{
  bool ret = FALSE;
  TCHAR szCmd[MAX_PATH];

  szCmd[0] = _T('\0');
  wsprintf(szCmd, _T("%s\\+cygwin.reg"), m_szTempPath);
  if(!PathFileExists(szCmd))
    return FALSE;
  
  szCmd[0] = _T('\0');
  wsprintf(szCmd, _T("regedit /c /s \"%s\\+cygwin.reg\""), m_szTempPath);
  
  if(CProcessUtil_i::RunSilentProcess(szCmd, NULL, 50000) == 0)
  {
    //SetFileAttributes(tmpFile, FILE_ATTRIBUTE_NORMAL);
    ret = TRUE;
  }
  else
  {
    ret = FALSE;
  }
  
  szCmd[0] = _T('\0');
  wsprintf(szCmd, _T("%s\\+cygwin.reg"), m_szTempPath);
  DeleteFile(szCmd);
  
  return ret;
}

bool CygwinUtl_i::RegDisableCygwin(VOID)
{
  TCHAR szCmd[256];

  RegExportCygwin();
  
  szCmd[0] = _T('\0');
  wsprintf(szCmd, _T("regedit /c /s \"%s\\-cygwin.reg\""), m_szTempPath);
  
  if(CProcessUtil_i::RunSilentProcess(szCmd, NULL, 50000) == 0)
  {
    //SetFileAttributes(tmpFile, FILE_ATTRIBUTE_NORMAL);
  }
  else
  {
    return FALSE;
  }

  //RegEnableCygwin();

  return TRUE;
}

bool CygwinUtl_i::RegExportCygwin(TCHAR* pExportFile)
{
  TCHAR szCmd[256];
  TCHAR szFile[MAX_PATH];

  wsprintf(szFile, _T("%s\\+cygwin.reg"), m_szTempPath);
  SetFileAttributes(szFile, FILE_ATTRIBUTE_NORMAL);
  
  szCmd[0] = _T('\0');
  szFile[0] = _T('\0');
  
  wsprintf(szCmd, _T("regedit /e /s \"%s\\+cygwin.reg\" \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\" "),
    m_szTempPath);
  
  if(pExportFile)
  {
    SetFileAttributes(pExportFile, FILE_ATTRIBUTE_NORMAL);
    TCHAR szFile[MAX_PATH];
    wsprintf(szFile, _T("%s\\+cygwin.reg"), m_szTempPath);
    CopyFile(szFile, pExportFile, FALSE);
  }
  
  if(CProcessUtil_i::RunSilentProcess(szCmd, NULL, 50000) == 0)
  {
    //SetFileAttributes(tmpFile, FILE_ATTRIBUTE_NORMAL);
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}
