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
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#pragma warning(push)
#pragma warning(disable : 4127)			// conditional expression is constant

enum    { BUFFER_SIZE = 4096 };

CTrace_i:: CTrace_i(LPCTSTR lpszFile, int nLine)
          : m_szFile(lpszFile), m_nLine(nLine)
{
  ShowFullFilePath();
  ShowThreadId();
}

VOID CTrace_i::ShowFullFilePath(bool bShow)
{
  m_bShowFullFilePath = bShow;
}

VOID CTrace_i::ShowThreadId(bool bShow)
{
  m_bShowThreadId = bShow;
}

void CTrace_i:: operator() (LPCTSTR lpszFormat, ...)
{
		va_list va;
    va_start(va, lpszFormat);
    
    TCHAR buf1[BUFFER_SIZE];
    TCHAR buf2[BUFFER_SIZE];
    
    // add the __FILE__ and __LINE__ to the front
    LPTSTR cp = (LPTSTR) m_szFile;
    
    if (!m_bShowFullFilePath)
    {
      cp = _tcsrchr(m_szFile, _T('\\'));
      if (cp)
        cp++;
    }
    
    if (m_bShowThreadId)
    {
      if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : [%X] %s"), cp, m_nLine, GetCurrentThreadId(), lpszFormat) < 0)
        buf1[BUFFER_SIZE-1] = _T('\0');
    }
    else
    {
      if (_sntprintf(buf1, BUFFER_SIZE-1, _T("%s(%d) : %s"), cp, m_nLine, lpszFormat) < 0)
        buf1[BUFFER_SIZE-1] = _T('\0');
    }
    
    // format the message as requested
    if (_vsntprintf(buf2, BUFFER_SIZE-1, buf1, va) < 0)
      buf2[BUFFER_SIZE-1] = _T('\0');
    
    va_end(va);
    
    int len = lstrlen(buf2);
    
    TCHAR* p = buf2;

    // write it out
    while (len>0)
    {
      OutputDebugString(p);
      p += 1024;
      len -= 1024;
    }
}
