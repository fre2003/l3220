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

bool CFileOpenUtil_i::OpenFlashFile(LPCTSTR pszFlashFile, bool bFullScreen) // .swf 
{
  if(!PathFileExists(pszFlashFile))
    return FALSE; 
  
  if(bFullScreen)
  {
    WritePrivateProfileString(_T("Options"), _T("FullScreen"), _T("1"), CSysUtil_i::GetValidFilePath(_T("fmp.ini")) ); 
  }
  
  return (bool)((ShellExecute(NULL, TEXT("open"), CSysUtil_i::GetValidFilePath(_T("fmp.exe")), pszFlashFile, NULL, 
          SW_SHOW|SW_MAXIMIZE) > (HINSTANCE)32)?TRUE:FALSE);
}

bool CFileOpenUtil_i::OpenPdfFile(LPCTSTR pszPdfFile)
{
  return (bool)((ShellExecute(NULL, TEXT("open"), CSysUtil_i::GetValidFilePath(_T("FoxitReader.exe")), 
    pszPdfFile, NULL, SW_SHOW|SW_MAXIMIZE) > (HINSTANCE)32)?TRUE:FALSE);
}

bool CFileOpenUtil_i::OpenPlainFile(LPCTSTR pszFile)
{
  if(!PathFileExists(pszFile))
    return FALSE;
  
  CONST TCHAR* strCmd = CSysUtil_i::GetValidFilePath(_T("common\\ue\\uedit32.exe"));
  
  return (bool)((ShellExecute(NULL, TEXT("open"), strCmd, pszFile, NULL, SW_SHOWMAXIMIZED) > (HINSTANCE)(HINSTANCE)32)?TRUE:FALSE);
}

bool CFileOpenUtil_i::OpenLanguageFile(LPCTSTR pszFile)
{
  if(!PathFileExists(pszFile))
    return FALSE;

  CONST TCHAR* strCmd = CSysUtil_i::GetValidFilePath(_T("common\\npp\\notepad++.exe")); 

  return ((bool)(ShellExecute(NULL, TEXT("open"), strCmd, pszFile, NULL, SW_SHOWMAXIMIZED) > (HINSTANCE)32)?TRUE:FALSE);
}

bool CFileOpenUtil_i::OpenXmlFile(LPCTSTR pszXmlFile)
{
  if(!PathFileExists(pszXmlFile))
    return FALSE;

  return (bool)((ShellExecute(NULL, TEXT("open"), CSysUtil_i::GetValidFilePath(_T("common\\XMLPad1.0.exe")), pszXmlFile, NULL, SW_SHOWMAXIMIZED) >  (HINSTANCE)32)?TRUE:FALSE);
}

bool CFileOpenUtil_i::OpenXmlFile2007(LPCTSTR pszXmlFile)
{
  if(!PathFileExists(pszXmlFile))
    return FALSE;
  
  return (bool)((ShellExecute(NULL, TEXT("open"), CSysUtil_i::GetValidFilePath(_T("common\\XMLNotepad2007\\XmlNotepad.exe")), pszXmlFile, NULL, SW_SHOWMAXIMIZED) >  (HINSTANCE)32)?TRUE:FALSE);
}

bool CFileOpenUtil_i::ExploreFolder(LPCTSTR pszFolderPath)
{
  return (bool)((ShellExecute(NULL, TEXT("Explore"), pszFolderPath, NULL, NULL, SW_SHOWMAXIMIZED) >  (HINSTANCE)32) ? TRUE:FALSE);
}

bool CFileOpenUtil_i::OpenFile(LPCTSTR pszPath)
{
  bool bRet = (bool)((ShellExecute(NULL, TEXT("open"), pszPath, NULL, NULL, SW_SHOW) > (HINSTANCE)32)?TRUE:FALSE);
  if(!bRet)
  {
    TCHAR szBuf[256];
    szBuf[0] = '\0';
    wsprintf(szBuf, _T("Don't find '%s' to execute"), pszPath);
    MessageBox(NULL, szBuf, _T("Error"), MB_ICONINFORMATION|MB_OK|MB_TOPMOST);
  }
  else
    return TRUE;
  
  return FALSE;
}

