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

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "StdAfx.h"
#include <types_i.h>
#include <windows.h>
#include "tchar.h"
#include "Shlwapi.h"
#include <gui_i.h>


//
// EXAMPLS
// GetResourceAsString(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_NODE), "NODE"));
// 
// utility functions
// 
CString CSysUtls_i::GetResourceAsString(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType)
{
  HRSRC hRsrc = ::FindResource(hModule, lpName, lpType);
  if (hRsrc != NULL)
  {
    HGLOBAL hGlob = ::LoadResource(hModule, hRsrc);
    if (hGlob != NULL)
    {
      LPVOID pRes = ::LockResource(hGlob);
      if (pRes != NULL)
      {
        DWORD dwSize = ::SizeofResource(hModule, hRsrc);
        if(dwSize < 0x0FFFFFFF)
        {
          return CString((char *)pRes, dwSize);
        }
      }
    }
  }
   
  // The system automatically deletes these resources 
  // Don't need to Free it.
  
  TRACE("Resource not found \n");
  
  return "";
}

