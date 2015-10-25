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

// private helper class
class CLogFileTrace
{
  // friend functions of this class
  friend void SetTraceFilePrefix(LPCTSTR strFilePrefix);
  friend void SetTraceLevel(const int nLevel);
    
public:
  // internal data members
  HANDLE	m_hFile;
  int m_nLevel;
  long m_nThreadId;
  SYSTEMTIME m_timeStart;

  DWORD m_dwCurrentFileSize;
  DWORD m_dwMaxFileSize; // the Limitation of file size specified by caller.
  TCHAR* m_pTraceFilePrefix;
  TCHAR m_strFilePath[1024];

  // close the current trace file
  void CloseTraceFile()
  {
    if(m_hFile) 
    {
#if 0
      ::CloseHandle(m_hFile);
#else
      fclose((FILE*)m_hFile);
#endif 
    }
    m_hFile = NULL;
  }
  
  // open a new trace file
  HANDLE OpenTraceFile()
  {
    // construct the new trace file path
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);
    _stprintf
      (
      m_strFilePath,
      _T("%s_%04d%02d%02d_%02d%02d%02d_%X.log"),
      m_pTraceFilePrefix?m_pTraceFilePrefix:_T("Trace"),
      sysTime.wYear,
      sysTime.wMonth,
      sysTime.wDay,
      sysTime.wHour,
      sysTime.wMinute,
      sysTime.wSecond,
      ::GetCurrentProcessId()
      );

    // create the new trace file
    //SetFileAttribute(m_strFilePath, FILE_ATTRIBUTE_NORMAL);
    
#if 0 
    m_hFile = CreateFile
      (
      m_strFilePath,
      GENERIC_WRITE,
      FILE_SHARE_READ,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL
      );
#else
    m_hFile = (HANDLE)_tfopen(m_strFilePath, _T("w"));
#endif 
    
    // if successful, save the start time variable
    if(m_hFile) 
      m_timeStart = sysTime;

    // return the file handle
    return m_hFile;
  }
  
  // set lock to gain exclusive access to trace
  // functions
  void Lock()
  {
    long nThreadId = ::GetCurrentThreadId();
    while(m_nThreadId!=nThreadId)
    {
      // keep trying until successfully completed the operation
      ::InterlockedCompareExchange((void**)&m_nThreadId, (void*)nThreadId, 0);
      if(m_nThreadId==nThreadId) break;
      ::Sleep(20);
    }
  }
 
  // release lock so that other threads can access 
  // trace functions
  void Unlock()
  {
    // only the thread that set the lock can release it
    ::InterlockedCompareExchange((void**)&m_nThreadId, 0, (void*)::GetCurrentThreadId());
  }

  // set the current trace level
  void SetTraceLevel(const int nLevel) { m_nLevel = nLevel>0?nLevel:0; }

  // set the trace file name prefix
  void SetTraceFilePrefix(LPCTSTR strFilePrefix)
  {
    // close existing trace file first
    CloseTraceFile();

    // copy the file name prefix
    int nSize = strFilePrefix?_tcslen(strFilePrefix):0;
    if(m_pTraceFilePrefix)
    {
      delete [] m_pTraceFilePrefix;
      m_pTraceFilePrefix = NULL;
    }
    m_pTraceFilePrefix = new TCHAR[nSize+1];
    _tcscpy(m_pTraceFilePrefix, nSize>0?strFilePrefix:_T(""));
  }
  
public:
  // constructor and destructor
  CLogFileTrace()
  {
    m_hFile     = NULL;
    m_nLevel    = 0;
    m_nThreadId = 0;
    m_pTraceFilePrefix = NULL;
    m_dwCurrentFileSize = 0;
    m_dwMaxFileSize     = 50*1024*1024; // the max. size is 50M.
  }
  
  ~CLogFileTrace()
  {
    CloseTraceFile();
    if(m_pTraceFilePrefix)
    {
      delete [] m_pTraceFilePrefix;
      m_pTraceFilePrefix = NULL;
    }
  }
};

// the one and only instance of CLogFileTrace
CLogFileTrace g_LogFileTrace;

DWORD ITS_LogFileClose(VOID)
{
  g_LogFileTrace.CloseTraceFile();
  
  g_LogFileTrace.m_dwCurrentFileSize = 0;
  
  return 0;
}

DWORD ITS_LogFileConfig(LPCTSTR strFilePrefix, CONST DWORD dwMaxFileSize)
{
  DWORD ret = 0; 
  
  // set lock
  g_LogFileTrace.Lock();
  
  // set trace file name prefix and the max. file size
  g_LogFileTrace.SetTraceFilePrefix(strFilePrefix);
  g_LogFileTrace.m_dwMaxFileSize = dwMaxFileSize;
  
  // release lock
  g_LogFileTrace.Unlock();
  
  return ret;
}


DWORD ITS_LogFileSetLevel(CONST DWORD dwLevel)
{
  DWORD ret = 0;

  // set lock
  g_LogFileTrace.Lock();

  // set trace level
  g_LogFileTrace.SetTraceLevel(dwLevel);
  
  // release lock
  g_LogFileTrace.Unlock();

  return ret;
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
u32 ITS_LogFileWrite(CONST DWORD nLevel, LPCTSTR strFormat, ...)
{
  //
  // if the specified trace level is greater than
  // the current trace level, return immediately
  // 
  if(g_LogFileTrace.m_nLevel == 0 || nLevel > (CONST DWORD)g_LogFileTrace.m_nLevel) 
  {
    return ERR_INVALID_PARAM_I;
  }
  
  // set lock
  g_LogFileTrace.Lock();
  
  try
  {
    // get local time
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);
    
    // get trace file handle	
    HANDLE hFile = g_LogFileTrace.m_hFile;
    
    //
    // open the trace file if not already open
    //
    if(hFile == NULL) 
      hFile = g_LogFileTrace.OpenTraceFile();

    //
    // if it is already a new day, close the old
    // trace file and open a new one
    //
    else if(sysTime.wYear != g_LogFileTrace.m_timeStart.wYear ||
      sysTime.wMonth != g_LogFileTrace.m_timeStart.wMonth ||
      sysTime.wDay != g_LogFileTrace.m_timeStart.wDay)
    {
      g_LogFileTrace.CloseTraceFile();
      g_LogFileTrace.OpenTraceFile();
    }
    
    //
    // write the trace message
    //
    if(hFile)
    {
      const int nMaxSize = 5*1024;
      TCHAR pBuffer[nMaxSize+80];
      
      //
      // print time stamp and thread id to buffer
      //
      int nPos = _stprintf
        (
        pBuffer,
        _T("%04X %02d:%02d:%02d.%03d: "),
        g_LogFileTrace.m_nThreadId,
        sysTime.wHour,
        sysTime.wMinute,
        sysTime.wSecond,
        sysTime.wMilliseconds        
        );
      
      // print the trace message to buffer
      va_list args;
      va_start(args, strFormat);
      nPos += _vsntprintf(pBuffer+nPos, nMaxSize,strFormat,args);
      va_end(args);
      
      // print the end of the line to buffer
      _stprintf(pBuffer+nPos,_T("\r\n"));
      
      // write the buffer to the trace file
      DWORD dwBytes = 0;
      int cnt = 0;
      cnt = lstrlen(pBuffer);
#if 0 
      ::WriteFile(hFile, pBuffer, cnt, &dwBytes, NULL);
#else
      //_ftprintf((FILE*)hFile, pBuffer);
      dwBytes = fwrite(pBuffer, sizeof(TCHAR), cnt, (FILE*)hFile);
#endif 
      g_LogFileTrace.m_dwCurrentFileSize += dwBytes;
      if((g_LogFileTrace.m_dwCurrentFileSize > g_LogFileTrace.m_dwMaxFileSize) && hFile)
      {
#if 0 
        CloseHandle(hFile);
#else
        fclose((FILE*)hFile);
#endif 
        g_LogFileTrace.m_hFile = NULL;
#if 0 
        g_LogFileTrace.m_hFile = CreateFile
          (g_LogFileTrace.m_strFilePath,
          GENERIC_WRITE,
          FILE_SHARE_READ,
          NULL,
          CREATE_ALWAYS,
          FILE_ATTRIBUTE_NORMAL,
          NULL
          );
#else
      g_LogFileTrace.m_hFile = (HANDLE)_tfopen(g_LogFileTrace.m_strFilePath, _T("w"));
#endif 
      }
    }
  }
  catch(...)
  {
    // add code to handle exception (if needed)
  }
  
  // release lock
  g_LogFileTrace.Unlock();

  return 0;
}

