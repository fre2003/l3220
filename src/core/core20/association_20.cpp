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
#include "stdafx.h"
#include <htmlhelp.h>
#include <SHLOBJ.H>



//
// Associate the specific file.
// EXAMPLE:
// AssociateFileIcon_2("prj", "C:\\VCProject.dll,-1", "VC Project File", "C:\\PrjDlg.exe");
// AssociateFileIcon_2("prj", "C:\\VCProject.dll,-1", "VC Project File", "C:\\PrjDlg.exe /o");
// 
//
// AssociateFileIcon_l2("prj", "C:\\VCProject.dll,-1", "VC Project File", "C:\\PrjDlg.exe");
// AssociateFileIcon_l2("prj", "C:\\VCProject.dll,-1", "VC Project File", "C:\\PrjDlg.exe /o");
// 
bool CFileAssoc_i::AssociateFileIcon(LPCTSTR pszFileEx,   // "dsp"
                          LPCTSTR pszIconFile, // ".\\bin\\VCProject.dll,-10"
                          LPCTSTR pszDescription,  // "VC Project File"
                          LPCTSTR pszAppToOpen, 
                          bool bActive)    // Can NULL if necessary.  .exe
{
  HKEY phKey = NULL;
  CString sApp, sExtName;
  
  sApp = pszFileEx;
  sApp += _T("_file");
  
  sExtName = _T(".");
  sExtName += pszFileEx;
  
  // .dsp
  RegCreateKey(HKEY_CLASSES_ROOT, sApp, &phKey);
  if(phKey)
  {
    RegSetValue(phKey, _T(""), REG_SZ, pszDescription, 0);
    RegCloseKey(phKey);
  }
  
  RegCreateKey(HKEY_CLASSES_ROOT, sExtName, &phKey);
  if(phKey)
  {
    RegSetValue(phKey, _T(""), REG_SZ, sApp, 0);
    RegCloseKey(phKey);
  }
  
  RegCreateKey(HKEY_CLASSES_ROOT,sApp, &phKey);
  if(phKey)
  {
    if(pszAppToOpen)
    {
      CString app;
      app = pszAppToOpen; 
      app += _T(" \"%1\"");
      RegSetValue(phKey, _T("shell\\open\\command"), REG_SZ,app, MAX_PATH);
    }
    else
    {
      RegSetValue(phKey, _T("shell\\open\\command"), REG_SZ, NULL, 0);
    }
    RegSetValue(phKey, _T("DefaultIcon"), REG_SZ, pszIconFile, MAX_PATH);
    RegCloseKey(phKey);
  }
  
  if(bActive)
  {
    // Active ... 
     SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0 );
  }

  return TRUE;
}

