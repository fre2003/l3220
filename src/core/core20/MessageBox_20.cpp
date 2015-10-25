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

#if 0 
U32 MessageBox_i(HWND hWnd, LPCTSTR pszTitle, LPCTSTR pFormat, ...)
{
  int cnt = 0;
  va_list argptr;
  TCHAR szMsgStr[256] = {0, };

  szMsgStr[0] = _T('\0');
  
  va_start(argptr, pFormat);
  cnt = wvsprintf(szMsgStr, pFormat, argptr);
  va_end(argptr);
  
  if(cnt <= 0)
    return 1;
  
  MessageBox(hWnd, szMsgStr, pszTitle, MB_OK|MB_ICONINFORMATION|MB_TOPMOST);
  
  return 0;
}

U32 MessageBoxEx(HWND hWnd, LPCTSTR pszTitle, LPCTSTR pFormat, ...)
{
  int cnt = 0;
  va_list argptr;

  TCHAR szMsgStr[256] = {0, };
  
  va_start(argptr, pFormat);
  cnt = wvsprintf(szMsgStr, pFormat, argptr);
  va_end(argptr);
  
  if(cnt <= 0)
    return 1;
  
  if(MessageBox(hWnd, szMsgStr, pszTitle, MB_YESNO|MB_ICONINFORMATION|MB_TOPMOST) == IDYES)
    return 0;
  else
    return 1; 
}

void DoEvents( HWND hWnd_i )
{
  // Message structure
  MSG stMsg = { 0 };
  
  // Peek message flags
  const DWORD dwPMFlags = PM_REMOVE; /*| ( OSVer::Instance().IsXP() ? PM_QS_PAINT | PM_QS_INPUT : 0 );*/
  
  // Flush out any keyboard, mouse, or painting message, without yielding control
  while( PeekMessage( &stMsg, 
    hWnd_i, 
    0, 
    0, 
    dwPMFlags ))
  {
    TranslateMessage( &stMsg );
    DispatchMessage( &stMsg );
  }// End while
}

#endif 
