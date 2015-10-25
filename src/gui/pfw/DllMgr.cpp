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
#include "DllMgr.h"

CDllMgr::CDllMgr(CString filename)
{
  m_hDllInstance      = NULL;
  m_strFileName       = _T("");
  m_pfnDllDocTemplate = NULL; 
  m_strError          = _T("");
  
  if (filename != _T(""))
  {
    LoadDll(filename);
  }
}

CDllMgr::~CDllMgr()
{
  if (m_hDllInstance != NULL)
  {
    ::FreeLibrary(m_hDllInstance);
  }
}

DWORD CDllMgr::Config(CONST CHAR* pszEntry)
{
  DWORD ret = 0;
  
  m_pszPluginEntry = pszEntry; 
  
  return ret;
}

HINSTANCE CDllMgr::GetPluginInstance(CString strFileName)
{
  if (strFileName == m_strFileName)
  {
    return m_hDllInstance;
  }
  
  return NULL;
}

DWORD CDllMgr::LoadDll(CString filename)
{
  DWORD ret = 0;
  
  m_strError.Empty();
  m_strFileName  = filename; 
  m_hDllInstance =::LoadLibrary(filename);
  
  if (m_hDllInstance == NULL)
  {
    m_hDllInstance = GetModuleHandle(filename);
  }
  
  if (m_hDllInstance != NULL)
  {
    m_pfnDllDocTemplate = (PFN_GET_DOC_TEMPLATE)GetProcAddress(m_hDllInstance, m_pszPluginEntry); 
    if (m_pfnDllDocTemplate == NULL)
    {
      ret = GetLastError();
      ::FreeLibrary(m_hDllInstance);
      m_hDllInstance = NULL;
      m_strError.Format(_T("Fail to Find '%s' in '%s', error[%d]: %s"),
        m_pszPluginEntry, filename, ret, GetLastErrorMsg_i(ret, TRUE));
    }
  }
  else 
  {
    ret            =::GetLastError();
    m_strError.Format(_T("Fail to Load '%s', error[%d]: %s"), filename, ret, GetLastErrorMsg_i(ret, TRUE));
  }
  
  return ret;
}

DWORD CDllMgr::FreeDll()
{
  DWORD dwRet = 0;
   
  if (m_hDllInstance != NULL)
  {
    if (!FreeLibrary(m_hDllInstance))
    {
      dwRet = GetLastError();
      m_strError.Format(_T("Fail to FreeLibrary '%s', error[%d]: '%s'"),
        m_strFileName, dwRet, GetLastErrorMsg_i(dwRet, TRUE));
    }
    else
    {
      m_strError = _T("");
    }
    
    m_hDllInstance      = NULL;
    m_strFileName       = "";
    m_pfnDllDocTemplate = NULL; 
  }
  else
  {
    m_strError = _T("");
  }
  
  return dwRet;
}
