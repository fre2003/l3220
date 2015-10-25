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
#include "stdlib.h"

#if 0 
class CProfileFunction
{
public:
  CProfileFunction(HANDLE hThread = NULL);
  ~CProfileFunction();
  ULONGLONG m_ullDuration;
  
public:
  virtual DWORD ProfileExit(void) = 0;
  
public:
  static bool ConvertTime(OUT TCHAR m_szDuration[33], ULONGLONG m_ullDuration);
  
private:
  FILETIME m_UserTime;
  HANDLE m_hThread;
};


CProfileFunction::CProfileFunction(HANDLE hThread)
{
  FILETIME t1, t2, t3;
  
  m_hThread = hThread;
  if(m_hThread == NULL)
    m_hThread = GetCurrentThread();
  
  DWORD err = GetThreadTimes(m_hThread,&t1, &t2,&t3, &m_UserTime);
}


CProfileFunction::~CProfileFunction()
{
  FILETIME t1, t2, t3;
  FILETIME UserTime;
  SYSTEMTIME SystemTime;
    
  DWORD err = GetThreadTimes(m_hThread,&t1, &t2,&t3, &UserTime);  
  ULONGLONG start = (*((ULARGE_INTEGER*)&m_UserTime)).QuadPart;
  ULONGLONG end   = (*((ULARGE_INTEGER*)&UserTime)).QuadPart;
  
  m_ullDuration = end - start;
  
  ProfileExit();
  
#if 0  
  TCHAR buf[33] = {0, };
  TCHAR szStart[33] = {0, };
  TCHAR szEnd[33] = {0, };
  wsprintf(buf,     "%d", end-start);
  
  _ui64toa(start, szStart, 10);
  _ui64toa(end, szEnd,  10);
  _ui64toa(end, end-start, 10);
  
  printf("%s --> %s (%s 100-nanosecond)  \n", szStart, szEnd, buf);
  printf("%d.%d ---> %d.%d (%d) \n", 
    m_SystemTime.wSecond, m_SystemTime.wMilliseconds, 
    SystemTime.wSecond, SystemTime.wMilliseconds, 
    SystemTime.wMilliseconds-m_SystemTime.wMilliseconds);
#endif 
}

bool CProfileFunction_i::ConvertTime(OUT TCHAR szDuration[33], ULONGLONG  ullDuration)
{
  szDuration[0] = _T('\0');
  
  if(_ui64toa( ullDuration, szDuration , 10))
    return TRUE;
  else 
    return FALSE;
}

DWORD CProfileFunction_i::ProfileExit()
{
#if 0 
  TCHAR szDuration[33];
  CProfileFunction::ConvertTime( szDuration, m_ullDuration);
  printf("%s 100-nanosecond \n", szDuration);
#endif 

  return 0;
}
#else
ULONGLONG CProfileTime_i::QueryThreadUserTime(HANDLE hThread)
{
  FILETIME t1, t2, t3;
  FILETIME UserTime; 

  if(hThread == NULL)
    hThread = GetCurrentThread();
  
  DWORD err = GetThreadTimes(hThread,&t1, &t2,&t3, &UserTime);
  
  return (*((ULARGE_INTEGER*)&UserTime)).QuadPart;
}
#endif 

bool CProfileTime_i::ConvertTime(OUT TCHAR szDuration[33], ULONGLONG  ullDuration)
{
  szDuration[0] = _T('\0');
  
  if(_ui64toa( ullDuration, szDuration , 10))
    return TRUE;
  else 
    return FALSE;
}
