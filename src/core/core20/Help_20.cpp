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
#include <htmlhelp.h>

#pragma comment(lib, "HtmlHelp.lib")

U32 CFileOpenUtil_i::OpenChmFile(LPCTSTR pszChmFile, HWND hWndParent, LPCTSTR pszCurWordToFind)
{
  if(pszChmFile == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  if(!PathFileExists(pszChmFile))
  {
    return ERR_INVALID_PATH_I;
  }
    
  if (pszCurWordToFind)
  {
    // Only look up in index if we have a string to search for
    HH_AKLINK aklink;
    
    ZeroMemory(&aklink, sizeof(aklink));
    
    aklink.cbStruct     = sizeof(aklink);
    aklink.pszKeywords  = pszCurWordToFind;
    aklink.fIndexOnFail = TRUE;
    HtmlHelp(hWndParent, pszChmFile, HH_ALINK_LOOKUP, (DWORD)&aklink);
    //HtmlHelp(hWndParent, pszChmFile, HH_SYNC, NULL);
  }
  else 
  {
    HtmlHelp(hWndParent, pszChmFile, HH_DISPLAY_TOPIC, NULL); 	// Display help window
  }
  
  return ERR_OK_I;
}

