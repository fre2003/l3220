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

// 
// pszLogFileDir: Specify the log file folder, like 'c:\\mylog\\';
// pszPrefix    : The prefiex string of log file.
// bReplaceF_2ile : TRUE: replace the file if exceed the max file size which specified by dwMaxFileSize. 
// CLogFile LogFile(_T("C:\\MyLogs\\"), _T("MyModule")); --> C:\\MyLogs\\MyModule_YYYYMMDD_HHMMSS_PROCESSID_USERNAME.log

// constructor and destructor
CLogFile_i::CLogFile_i(CONST TCHAR* pszLogFileDir,  
                       CONST TCHAR* pszPrefix, 
                       bool bOutLinePrefix,
                       bool bOutSepByTab,
                       bool bReplaceFile, 
                       DWORD dwMaxFileSize)
{
  m_hFile             = NULL;
  m_bSeprateWithTab   = bOutSepByTab;
  m_bPrintPrefix      = bOutLinePrefix;
  m_nThreadId         = 0;
  m_dwCurrentFileSize = 0;
  m_bReplaceFile      = bReplaceFile;

  m_strFolder[0] = _T('\0');
  m_strPrefix[0] = _T('\0');
  m_strFile[0]   = _T('\0');

  if(pszLogFileDir)
  {
    lstrcpy(m_strFolder, pszLogFileDir);
    if(lstrlen(m_strFolder))
    {
      if(m_strFolder[lstrlen(m_strFolder)-1] != _T('\\'))
      {
        lstrcat(m_strFolder, _T("\\"));
      }
    }    
  }  
  if(pszPrefix)
  {
    lstrcpy(m_strPrefix, pszPrefix);
    if(lstrlen(m_strPrefix))
    {
      lstrcat(m_strPrefix, _T("_"));
    }
  }
  m_dwMaxFileSize     = dwMaxFileSize;  
}

CLogFile_i::~CLogFile_i()
{
    CloseLogFile();
}

// open a new log file
HANDLE CLogFile_i::OpenLogFile(bool bNewFileName)
{
  SYSTEMTIME sysTime;

  if(bNewFileName)
  {
    // construct the new trace file path
    ::GetLocalTime(&sysTime);
  
    TCHAR szUserName[200];
    ZeroMemory(szUserName, sizeof(szUserName));
    DWORD UserNameSize = COUNTOF_I(szUserName)-4;
    if (GetUserName(&szUserName[1], &UserNameSize))
    {
      szUserName[0] = _T('_');
    }
    
    _stprintf(m_strFile,_T("%s%s%04d%02d%02d_%02d%02d%02d_%X%s.log"),
      m_strFolder, m_strPrefix,
      sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, 
      ::GetCurrentProcessId(),szUserName);
  }
    
  m_hFile = CreateFile
    (
    m_strFile,
    GENERIC_WRITE|GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,
    NULL
    );
  
  // if successful, save the start time variable
  if(m_hFile && bNewFileName) 
    m_timeStart = sysTime;
  
  // return the file handle
  return m_hFile;
}

// close the current trace file
void CLogFile_i::CloseLogFile()
{
  if(m_hFile) 
  {
    ::CloseHandle(m_hFile);
  }
  
  m_dwCurrentFileSize = 0;
  m_dwCurrentFileSize = 0;
  m_hFile = NULL;
}

// set lock to gain exclusive access
void CLogFile_i::Lock()
{
  DWORD  nThreadId = ::GetCurrentThreadId();
  
  while(m_nThreadId != nThreadId)
  {
    // keep trying until successfully completed the operation
    ::InterlockedCompareExchange((void**)&m_nThreadId, (void*)nThreadId, 0);

    if(m_nThreadId==nThreadId) 
      break;
    
    ::Sleep(20);
  }
}

// release lock so that other threads
void CLogFile_i::Unlock()
{
  // only the thread that set the lock can release it
  ::InterlockedCompareExchange((void**)&m_nThreadId, 0, (void*)::GetCurrentThreadId());
}


INLINE LPCTSTR GetLevelString(CONST DWORD nLevel)
{
  switch(nLevel)
  {
  case CLogFile_i::LOG_ERROR: return  _T("ERROR ");
  case CLogFile_i::LOG_INFO:  return  _T("INFO  ");
  case CLogFile_i::LOG_DEBUG: return  _T("DEBUG ");
  default:                  return  _T("      ");
  }
}

DWORD CLogFile_i::Printf(LPCTSTR strFormat, ...)
{
  // set lock
  Lock();
  
  try
  {
    // get local time
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);
    
    // get trace file handle	
    HANDLE hFile = m_hFile;
    
    //
    // open the trace file if not already open
    //
    if(hFile == NULL) 
      hFile = OpenLogFile(TRUE);
    
    //
    // if it is already a new day, close the old
    // log file and open a new one
    //
    else if(sysTime.wYear != m_timeStart.wYear ||
      sysTime.wMonth != m_timeStart.wMonth ||
      sysTime.wDay != m_timeStart.wDay)
    {
      CloseLogFile();
      OpenLogFile(TRUE);
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
      int nPos = 0;
      
      if(m_bSeprateWithTab)
      {
        if(m_bPrintPrefix)
        {
          nPos = _stprintf(
            pBuffer,
            _T("%04X\t%02d:%02d:%02d.%03d\t"),
            m_nThreadId,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);
        }
      }
      else 
      {
        if(m_bPrintPrefix)
        {
          nPos = _stprintf(
            pBuffer,
            _T("%04X %02d:%02d:%02d.%03d "),
            m_nThreadId,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);
        }
      }
      
      // print the trace message to buffer
      va_list args;
      va_start(args, strFormat);
      nPos += _vsntprintf(pBuffer+nPos, nMaxSize, strFormat,args);
      va_end(args);
      
      DWORD dwBytes = 0;
      
      // write the buffer to the log file
#ifdef HAVE_UNICODE_I 
      ::WriteFile(hFile, pBuffer, nPos*2, &dwBytes, NULL);
#else
      ::WriteFile(hFile, pBuffer, nPos, &dwBytes, NULL);
#endif
      m_dwCurrentFileSize += dwBytes;
      if((m_dwCurrentFileSize > m_dwMaxFileSize) && hFile)
      {
        CloseLogFile();
        OpenLogFile(m_bReplaceFile);
      }
    }
  }
  catch(...)
  {
    // add code to handle exception (if needed)
    OutputDbgStr_i(_T("*** Fail to Open Log File. \n"));
  }
  
  // release lock
  Unlock();

  return ERR_OK_I;
}


DWORD CLogFile_i::Printf(CONST DWORD dwId, LPCTSTR strFormat, ...)
{
  //
  // if the specified trace level is greater than
  // the current trace level, return immediately
  // 
  DWORD dwLevle    = dwId&0XFF000000;
  DWORD dwModuleId = dwId&0X00FFFFFF;
  
  if(dwLevle & LOG_NONE ) 
  {
    return ERR_INVALID_LEVEL_I;
  }
  
 if(dwLevle&LOG_ERROR || dwLevle&LOG_INFO || dwLevle&LOG_DEBUG)
 {
   // set lock
   Lock();
   
   try
   {
     // get local time
     SYSTEMTIME sysTime;
     ::GetLocalTime(&sysTime);
     
     // get trace file handle	
     HANDLE hFile = m_hFile;
     
     //
     // open the trace file if not already open
     //
     if(hFile == NULL) 
       hFile = OpenLogFile(TRUE);
     
     //
     // if it is already a new day, close the old
     // log file and open a new one
     //
     else if(sysTime.wYear != m_timeStart.wYear ||
       sysTime.wMonth != m_timeStart.wMonth ||
       sysTime.wDay != m_timeStart.wDay)
     {
       CloseLogFile();
       OpenLogFile(TRUE);
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
       int nPos = 0;
       
       if(m_bSeprateWithTab)
       {
         if(m_bPrintPrefix)
         {
           nPos = _stprintf(
             pBuffer,
             _T("%04X\t%02d:%02d:%02d.%03d\t%d\t%s\t"),
             m_nThreadId,
             sysTime.wHour,
             sysTime.wMinute,
             sysTime.wSecond,
             sysTime.wMilliseconds, dwModuleId, GetLevelString(dwLevle));
         }
       }
       else 
       {
         if(m_bPrintPrefix)
         {
           nPos = _stprintf(
             pBuffer,
             _T("%04X %02d:%02d:%02d.%03d %d %s"),
             m_nThreadId,
             sysTime.wHour,
             sysTime.wMinute,
             sysTime.wSecond,
             sysTime.wMilliseconds, dwModuleId, GetLevelString(dwLevle));
         }
       }

       // print the trace message to buffer
       va_list args;
       va_start(args, strFormat);
       nPos += _vsntprintf(pBuffer+nPos, nMaxSize, strFormat,args);
       va_end(args);
       
       DWORD dwBytes = 0;
       
       // write the buffer to the log file
#ifdef HAVE_UNICODE_I
       ::WriteFile(hFile, pBuffer, nPos*2, &dwBytes, NULL);
#else
       ::WriteFile(hFile, pBuffer, nPos, &dwBytes, NULL);
#endif
       m_dwCurrentFileSize += dwBytes;
       if((m_dwCurrentFileSize > m_dwMaxFileSize) && hFile)
       {
         CloseLogFile();
         OpenLogFile(m_bReplaceFile);
       }
     }
   }
   catch(...)
   {
     // add code to handle exception (if needed)
     OutputDbgStr_i(_T("*** Fail to Open Log File. \n"));
   }
   
   // release lock
   Unlock();
 }
 else
 {
   return ERR_INVALID_LEVEL_I;
 }

  return ERR_OK_I;
}

DWORD CLogFile_i::Printf(CONST DWORD dwId, LPCTSTR pszFile, DWORD dwLineNo, LPCTSTR strFormat, ...)
{
  //
  // if the specified trace level is greater than
  // the current trace level, return immediately
  // 
  DWORD dwLevle    = dwId&0XFF000000;
  DWORD dwModuleId = dwId&0X00FFFFFF;
  
  if(dwLevle & LOG_NONE ) 
  {
    return ERR_INVALID_LEVEL_I;
  }
  
  if(dwLevle&LOG_ERROR || dwLevle&LOG_INFO || dwLevle&LOG_DEBUG)
  {
    // set lock
    Lock();
    
    try
    {
      // get local time
      SYSTEMTIME sysTime;
      ::GetLocalTime(&sysTime);
      
      // get trace file handle	
      HANDLE hFile = m_hFile;
      
      //
      // open the trace file if not already open
      //
      if(hFile == NULL) 
        hFile = OpenLogFile(TRUE);
      
      //
      // if it is already a new day, close the old
      // log file and open a new one
      //
      else if(sysTime.wYear != m_timeStart.wYear ||
        sysTime.wMonth != m_timeStart.wMonth ||
        sysTime.wDay != m_timeStart.wDay)
      {
        CloseLogFile();
        OpenLogFile(TRUE);
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
        int nPos = 0;
        
        if(m_bSeprateWithTab)
        {
          if(m_bPrintPrefix)
          {
            nPos = _stprintf(
              pBuffer,
              _T("%s(%d) : %04X\t%02d:%02d:%02d.%03d\t%d\t%s\t"),
              pszFile, dwLineNo, m_nThreadId,
              sysTime.wHour,
              sysTime.wMinute,
              sysTime.wSecond,
              sysTime.wMilliseconds, dwModuleId, GetLevelString(dwLevle));
          }
        }
        else
        {
          if(m_bPrintPrefix)
          {
            nPos = _stprintf(
              pBuffer,
              _T("%s(%d) : %04X %02d:%02d:%02d.%03d %d %s"),
              pszFile, dwLineNo, m_nThreadId,
              sysTime.wHour,
              sysTime.wMinute,
              sysTime.wSecond,
              sysTime.wMilliseconds, dwModuleId, GetLevelString(dwLevle));
          }
        }
        
        // print the trace message to buffer
        va_list args;
        va_start(args, strFormat);
        nPos += _vsntprintf(pBuffer+nPos, nMaxSize, strFormat,args);
        va_end(args);
        
        DWORD dwBytes = 0;
        
        // write the buffer to the log file
#ifdef UNICODE 
        ::WriteFile(hFile, pBuffer, nPos*2, &dwBytes, NULL);
#else
        ::WriteFile(hFile, pBuffer, nPos, &dwBytes, NULL);
#endif
        m_dwCurrentFileSize += dwBytes;
        if((m_dwCurrentFileSize > m_dwMaxFileSize) && hFile)
        {
          CloseLogFile();
          OpenLogFile(m_bReplaceFile);
        }
      }
    }
    catch(...)
    {
      // add code to handle exception (if needed)
      OutputDbgStr_i(_T("*** Fail to Open Log File. \n"));
    }
    
    // release lock
    Unlock();
 }
 else
 {
   return ERR_INVALID_LEVEL_I;
 }
 
 return ERR_OK_I;
}


DWORD CLogFile_i::Printf(CONST TCHAR* szModuleName, CONST TCHAR* szLevel, 
                       LPCTSTR pszFile, DWORD dwLineNo, 
                       LPCTSTR strFormat, ...)
{
  if(strFormat)
  {
    // set lock
    Lock();
    
    try
    {
      // get local time
      SYSTEMTIME sysTime;
      ::GetLocalTime(&sysTime);
      
      // get trace file handle	
      HANDLE hFile = m_hFile;
      
      //
      // open the trace file if not already open
      //
      if(hFile == NULL) 
        hFile = OpenLogFile(TRUE);
      
      //
      // if it is already a new day, close the old
      // log file and open a new one
      //
      else if(sysTime.wYear != m_timeStart.wYear ||
        sysTime.wMonth != m_timeStart.wMonth ||
        sysTime.wDay != m_timeStart.wDay)
      {
        CloseLogFile();
        OpenLogFile(TRUE);
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
        int nPos = 0;
        
        if(m_bSeprateWithTab)
        {
          if(m_bPrintPrefix)
          {
            if(pszFile)
            {
              nPos = _stprintf(
                pBuffer,
                _T("%s(%d) : %04X\t%02d:%02d:%02d.%03d\t%s\t%s\t"),
                pszFile, dwLineNo, m_nThreadId,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond,
                sysTime.wMilliseconds, szModuleName?szModuleName:_T(""), szLevel?szLevel:_T(""));
            }
            else
            {
              nPos = _stprintf(
                pBuffer,
                _T("%04X\t%02d:%02d:%02d.%03d\t%s\t%s\t"),
                m_nThreadId,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond,
                sysTime.wMilliseconds, szModuleName?szModuleName:_T(""), szLevel?szLevel:_T(""));
            }
          }
        }
        else
        {
          if(m_bPrintPrefix)
          {
            if(pszFile)
            {
              nPos = _stprintf(
                pBuffer,
                _T("%s(%d) : %04X %02d:%02d:%02d.%03d %s %s"),
                pszFile, dwLineNo, m_nThreadId,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond,
                sysTime.wMilliseconds, szModuleName?szModuleName:_T(""), szLevel?szLevel:_T(""));
            }
            else
            {
              nPos = _stprintf(
                pBuffer,
                _T("%04X %02d:%02d:%02d.%03d %s %s"),
                m_nThreadId,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond,
                sysTime.wMilliseconds, szModuleName?szModuleName:_T(""), szLevel?szLevel:_T(""));
            }
          }
        }
        
        // print the trace message to buffer
        va_list args;
        va_start(args, strFormat);
        nPos += _vsntprintf(pBuffer+nPos, nMaxSize, strFormat,args);
        va_end(args);
        
        DWORD dwBytes = 0;
        
        // write the buffer to the log file
#ifdef UNICODE 
        ::WriteFile(hFile, pBuffer, nPos*2, &dwBytes, NULL);
#else
        ::WriteFile(hFile, pBuffer, nPos, &dwBytes, NULL);
#endif
        m_dwCurrentFileSize += dwBytes;
        if((m_dwCurrentFileSize > m_dwMaxFileSize) && hFile)
        {
          CloseLogFile();
          OpenLogFile(m_bReplaceFile);
        }
      }
    }
    catch(...)
    {
      // add code to handle exception (if needed)
      OutputDbgStr_i(_T("*** Fail to Open Log File. \n"));
    }
    
    // release lock
    Unlock();
 }
 else
 {
   return ERR_INVALID_LEVEL_I;
 }
 
 return ERR_OK_I;
}
