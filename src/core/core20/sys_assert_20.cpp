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

VOID Assert_i(LPCTSTR exp, LPCTSTR file, int line, BOOL bPopUp) 
{
  TCHAR buffer[1024];
  
  wsprintf(buffer, _T("Assertion [%s] failed at %s %d\0"), exp, file, line);
  if (bPopUp)
  {
    int idButton =::MessageBox(NULL, buffer, _T("Assertion Failure"), MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);
    if (idButton == IDRETRY)
    {
      ::DebugBreak();
    }
    else if (idButton == IDIGNORE)
    {
      // all OK
    }
    else
    {
      abort();
    }
  }
  else
  {
    lstrcat(buffer, _T("\r\n"));
    ::OutputDebugString(buffer);
    ::DebugBreak();
    abort();
  }
}


VOID AssertA_i(CONST CHAR* exp, CONST CHAR* file, int line, BOOL bPopUp) 
{
  CHAR buffer[1024];
  
  sprintf(buffer, ("Assertion [%s] failed at %s %d\0"), exp, file, line);
  if (bPopUp)
  {
    int idButton =::MessageBoxA(NULL, buffer, "Assertion Failure", MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);
    if (idButton == IDRETRY)
    {
      ::DebugBreak();
    }
    else if (idButton == IDIGNORE)
    {
      // all OK
    }
    else
    {
      abort();
    }
  }
  else
  {
    lstrcatA(buffer, ("\r\n"));
    ::OutputDebugStringA(buffer);
    ::DebugBreak();
    abort();
  }
}
