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
#ifndef __DLLMGR_H__
#define __DLLMGR_H__

#include "stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 
// Declare the function pointer declarations.
// 
typedef CPluginEntry_i* (*PFN_GET_DOC_TEMPLATE)();
typedef DWORD(*PFN_GET_PLUGIN_TYPE) ();

class CDllMgr
{
public:
  CDllMgr(CString strFileName = _T(""));  // constructor
  ~CDllMgr(); // destructor
  
  DWORD LoadDll(CString filename);
  DWORD FreeDll();
  HINSTANCE GetPluginInstance(CString strFileName);
  
public:
  DWORD Config(CONST CHAR* pszEntry);
  
public:
  // MFC Document Template
  PFN_GET_DOC_TEMPLATE m_pfnDllDocTemplate; 
  CPluginEntry_i* m_pTemplate;
  CString m_strFileName;  // The filename of DLL.
  CString m_strError;
  HINSTANCE m_hDllInstance;   // pointer to Instance of DLL.   
  
protected:
  CONST CHAR* m_pszPluginEntry; 
};

#endif // __DLLMGR_H__
