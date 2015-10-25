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

class CModule
{
public:
  CModule(LPCTSTR lpszModuleFileName, LPCSTR lpszProcName);
  ~CModule();
  BOOL IsValidProc();

protected:
  BOOL m_bValidProcName;
private:
  HMODULE m_hModule;
};


CModule::CModule(LPCTSTR lpszModuleFileName, LPCSTR lpszProcName)
{
  m_bValidProcName = FALSE;
  m_hModule = LoadLibrary( lpszModuleFileName );
  if( !m_hModule )
  {
    return ;
  }
  else
  {
    FARPROC ProcAddr = ::GetProcAddress( m_hModule, lpszProcName );
    if( !ProcAddr )
    {
      return ;
    }
  }
}


CModule::~CModule()
{
  if(m_hModule)
    FreeLibrary(m_hModule);
  m_hModule = NULL;
}

BOOL CModule::IsValidProc()
{
  return m_bValidProcName;
}

