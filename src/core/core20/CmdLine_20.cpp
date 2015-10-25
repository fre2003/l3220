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
#include "process.h"
#include "errno.h"

// #define STDOUT_DEBUG 


//
// CCommandLine_i cmd;
// cmd.Execute(_T("C:\1.exe"));
// cmd.StopExecute()
// 

CCommandLine_i::CCommandLine_i(PVOID pUserData)
{
  m_bCancel          = FALSE;
  m_bUseConsoleEvent = FALSE;
  m_dwProcessGroupId = 0;
  m_hWriteSubProcess = 0;
  m_bRuning          = FALSE;
  m_pUserData        = pUserData;

  memset(m_szCommandLine, 0, sizeof(m_szCommandLine));
  memset(m_szDir, 0, sizeof(m_szDir));
  
  m_StatckSize       = 1024*1024;
  m_dwRefreshTimeOut = 100L;
}

VOID CCommandLine_i::SetUserData(PVOID pData)
{
  m_pUserData = pData;
}

U32 CCommandLine_i::SetDir(LPCTSTR pszDir)
{
  if(pszDir == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  if(lstrlen(pszDir) > sizeof(m_szDir)-1)
  {
    return ERR_INVALID_PARAM_I;
  }

  if(PathFileExists(pszDir))
  {
    lstrcpy(m_szDir, pszDir);
  }
  else
  {
    return ERR_INVALID_PATH_I;
  }

  return ERR_OK_I;
}

PVOID CCommandLine_i::GetUserData(VOID)
{
  return m_pUserData;
}

VOID CCommandLine_i::SetThreadStackSize(DWORD dwStackSize)
{
  m_StatckSize = dwStackSize;
}

VOID CCommandLine_i::SetRefreshTimeOut(DWORD dwRefreshTimeOut)
{
  m_dwRefreshTimeOut= dwRefreshTimeOut;
}

VOID CCommandLine_i::UseConsoleEvent(bool bUseConsoleEvent) 
{
  m_bUseConsoleEvent = bUseConsoleEvent; 
}

#if 0 
void CProcessCmdLine_i::Output(LPCTSTR s, DWORD len) 
{
  DWORD dwWriteLen = 0;
  
  CString str((char*)s);

  if(len == 0)
  {
    len = lstrlen(s);
  }
  
  while(dwWriteLen < len)
  {
    // WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s, len, &dwWriteLen, NULL);
    dwWriteLen = fwrite(s+dwWriteLen, sizeof(char), len, stdout);
  }
}
#endif 

void CCommandLine_i::Exit(DWORD dwExitCode)
{
  TRACE_I(_T("Command Line Exit Code:%d \n"), dwExitCode);
}


static void ExecThread(void *ptw) 
{
  CCommandLine_i *tw = reinterpret_cast<CCommandLine_i *>(ptw);
  tw->ProcessThread();
}

//
// ERR_INVALID_PARAM_I : Command Line is too long
// ERR_OK_I: Start to execute
// ERR_BUSY_I : command line is running.
// ERR_INVALID_STATCK_SIZE_I: call SetThreadStackSize() to adjust the stack size to run the command line.
// ERR_TOO_MANY_THREADS_I: Thread is too many threads.
// ERR_UNKNOWN_I: unknown error.
//
//
// ERR_INVALID_PARAM_I : Command Line is too long
// ERR_OK_I: Start to execute
// ERR_BUSY_I : command line is running.
// ERR_INVALID_STATCK_SIZE_I: call SetThreadStackSize() to adjust the stack size to run the command line.
// ERR_TOO_MANY_THREADS_I: Thread is too many threads.
// ERR_UNKNOWN_I: unknown error.
//
DWORD CCommandLine_i::Execute(LPCTSTR pszCommandLine, bool bASync)
{
  U32 ret = ERR_OK_I;
  
  if(m_bRuning)
  {
    return ERR_BUSY_I;
  }
  
  m_bCancel = FALSE;
  m_bRuning = TRUE;
  
  m_szCommandLine[0] = _T('\0');
  
  if(lstrlen(pszCommandLine) > sizeof(m_szCommandLine)-1)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  lstrcpy(m_szCommandLine, pszCommandLine);
  
  if (bASync)
  {
    ret = _beginthread(ExecThread, m_StatckSize, reinterpret_cast < void * >(this));
    
    /* 
    If successful, each of these functions returns a handle to the newly created thread. 
    _beginthread returns ¨C1 on an error, in which case errno is set to EAGAIN if there are too many threads, 
    or to EINVAL if the argument is invalid or the stack size is incorrect. 
    _beginthreadex returns 0 on an error, in which case errno and doserrno are set.
    */
    if (ret == -1 || ret == 0)  // error
    {
      if (errno == EAGAIN)
      {
        ret = ERR_TOO_MANY_THREADS_I;
      }
      else if (ret == EINVAL)
      {
        ret = ERR_INVALID_STATCK_SIZE_I;
      }
      else
      {
        ret = ERR_UNKNOWN_I;
      }
    }
    else
    {
      ret = ERR_OK_I;
    }
  }
  else
  {
    ProcessThread();
    ret = ERR_OK_I;
  }
  
  return ret;
}

// #define USE_CONSOLE_EVENT 

// ERR_OK_I
// ERR_WIN32_ERROR_I
DWORD CCommandLine_i::StopExecute() 
{
  DWORD dwRet = 0;

  if(m_bRuning == FALSE)
  {
    return ERR_OK_I;
  }
  
  if (m_hWriteSubProcess && (m_hWriteSubProcess != INVALID_HANDLE_VALUE)) 
  {
    TCHAR stop[] = _T("\032");
    DWORD bytesWrote = 0;
    BOOL bRet = ::WriteFile(m_hWriteSubProcess, stop, static_cast<DWORD>(lstrlen(stop)), &bytesWrote, NULL);
    if(bRet == FALSE)
    {
      m_bCancel = FALSE;
      dwRet = ERR_WIN32_ERROR_I;
    }
    else
    {
      m_bCancel = TRUE;
      dwRet = ERR_PROCESS_I;
   }
   Sleep(500L);
  }
  else
  {
    dwRet = ERR_OK_I;
  }
  
  if (m_bUseConsoleEvent && m_dwProcessGroupId) 
  {
    // this also doesn't work
    // Output(_T("Attempting to cancel process..."), lstrlen(_T("Attempting to cancel process...")));
    if (!GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, m_dwProcessGroupId)) 
    {
      dwRet = ERR_WIN32_ERROR_I;
      OutputError(_T("BREAK Failed!\n") );
    }
    Sleep(100L);
  }
  
  return dwRet;
}

DWORD CCommandLine_i::ProcessThread()
{
  DWORD exitcode = 0;
  bool bRunning = TRUE;
  CHAR buffer[16384];
  
  OSVERSIONINFO osv = {sizeof(OSVERSIONINFO), 0, 0, 0, 0, _T(" ")};
  ::GetVersionEx(&osv);
  
  bool windows95 = osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
  
  SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, 0};
  
  // Output(m_szCommandLine, lstrlen(m_szCommandLine));
  
  sa.bInheritHandle       = TRUE;
  sa.lpSecurityDescriptor = NULL;
  
  SECURITY_DESCRIPTOR sd;
  // If NT make a real security thing to allow inheriting handles
  if (!windows95) 
  {
    ::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    ::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = &sd;
  }
  
  HANDLE hPipeWrite = NULL;
  HANDLE hPipeRead = NULL;
  // Create pipe for output redirection
  // read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
  ::CreatePipe(&hPipeRead, &hPipeWrite, &sa, 0);
  
  // Create pipe for input redirection. In this code, you do not
  // redirect the output of the child process, but you need a handle
  // to set the hStdInput field in the STARTUP_INFO struct. For safety,
  // you should not set the handles to an invalid handle.
  
  m_hWriteSubProcess = NULL;
  m_dwProcessGroupId = 0;
  HANDLE hRead2 = NULL;

  // read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
  ::CreatePipe(&hRead2, &m_hWriteSubProcess, &sa, 0);
  
  ::SetHandleInformation(hPipeRead, HANDLE_FLAG_INHERIT, 0);
  ::SetHandleInformation(m_hWriteSubProcess, HANDLE_FLAG_INHERIT, 0);
  ::SetNamedPipeHandleState(hPipeRead, PIPE_READMODE_BYTE,
                            NULL,     // don't set maximum bytes 
                            NULL);    // don't set maximum time 
  
  // 
  // Make child process use hPipeWrite as standard out, and make
  // sure it does not show on screen.
  //  
  STARTUPINFO si = 
  {
    sizeof(STARTUPINFO), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  
  si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  si.wShowWindow = SW_HIDE;
  si.hStdInput   = hRead2;
  si.hStdOutput  = hPipeWrite;
  si.hStdError   = hPipeWrite;
  
  PROCESS_INFORMATION pi = {0, 0, 0, 0};
  
  BOOL running = ::CreateProcess (
    NULL,
    const_cast<TCHAR *>(m_szCommandLine),
    NULL, NULL,
    TRUE, CREATE_NEW_PROCESS_GROUP,
    NULL,
    PathFileExists(m_szDir)?
    static_cast<const TCHAR *>(m_szDir) : NULL,
    &si, &pi);
  
  if (running)
  {
    m_dwProcessGroupId = pi.dwProcessId;
    
    buffer[0] = ('\0');
    ZeroMemory(buffer, sizeof(buffer));
    
    while (bRunning) 
    {
      
      DWORD bytesRead = 0;
      DWORD bytesAvail = 0;
      Sleep(m_dwRefreshTimeOut);
      
      if (!::PeekNamedPipe(hPipeRead, buffer, sizeof(buffer), &bytesRead, &bytesAvail, NULL))
      {
        #ifdef STDOUT_DEBUG 
        _tprintf(_T("PeekNamedPipe err= %d\n"), GetLastError());
        #endif 
        bytesAvail = 0;
      }
      
      if (bytesAvail > 0)
      {
        buffer[0] = '\0';

        BOOL bResult  = ::ReadFile(hPipeRead, buffer, sizeof(buffer), &bytesRead, NULL);
        if (bResult  && bytesRead)
        {
          // Display the data
          Output(buffer, bytesRead);
        } 
        else 
        {
          bRunning = FALSE;
          if (bResult &&  bytesRead == 0) 
          {
            // End 
          }
          #ifdef STDOUT_DEBUG 
          _tprintf(_T("ReadFile err= %d\n"), GetLastError());
          #endif 
        }
       bytesAvail = 0;
      }
      else 
      {
        if (::GetExitCodeProcess(pi.hProcess, &exitcode))
        {
          if (STILL_ACTIVE != exitcode) 
          {
            #ifdef STDOUT_DEBUG 
            _tprintf(_T("GetExitCodeProcess PeekNamedPipe bytesAvail=0,err=%d\n"), GetLastError());
            #endif 

            bRunning = FALSE; // Exit now 
            
            // Check again in the pipe.
            while(::PeekNamedPipe(hPipeRead, buffer, sizeof(buffer), &bytesRead, &bytesAvail, NULL) && bytesAvail)
            {
              if (bytesAvail > 0)
              {
                buffer[0] = '\0';
                BOOL bResult  = ::ReadFile(hPipeRead, buffer, sizeof(buffer), &bytesRead, NULL);
                if (bResult  && bytesRead)
                {
                  // Display the data
                  Output(buffer, bytesRead);
                }
              }
              Sleep(m_dwRefreshTimeOut);
              bytesAvail = 0;
            } // while(::PeekNamedPipe... 

            #ifdef STDOUT_DEBUG 
            _tprintf(_T("PeekNamedPipe again err= %d\n"), GetLastError());
            #endif 
          } // if (STILL_ACTIVE != exitcode) 
        } // if (::GetExitCodeProcess 
      }
      
      if (m_bCancel) 
      {
        if (WAIT_OBJECT_0 != ::WaitForSingleObject(pi.hProcess, 500)) 
        {
          OutputError(_T("Process failed to respond,forcing abrupt termination...\n"));
          // This function is dangerous: dependant DLLs don't know the process
          // is terminated, and memory isn't released.
          if(::TerminateProcess(pi.hProcess, 1))
          {
            TCHAR buf[512];
            wsprintf(buf, _T("Terminate Process '%s' ok.\n"), m_szCommandLine);
            OutputError(buf);
          }
        }
        bRunning = FALSE;
      }
    }// while(running)
    
    #ifdef STDOUT_DEBUG 
    _tprintf(_T("End. \n"));
    fflush(stdout);
    Sleep(100);
    #endif 
    if (WAIT_OBJECT_0 != ::WaitForSingleObject(pi.hProcess, 500)) 
    {
      OutputError(_T("Process failed to respond,forcing abrupt termination...\n"));
      if (::TerminateProcess(pi.hProcess, 2))
      {
        TCHAR buf[512];
        
        wsprintf(buf, _T("Terminate Process '%s' ok.\n"), m_szCommandLine);
        OutputError(buf);
      }
    }

    ::TerminateProcess(pi.hProcess, 2);
    ::GetExitCodeProcess(pi.hProcess, &exitcode);
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
    pi.hProcess = NULL;
    pi.hThread = NULL;
  } 
  else 
  {
    DWORD nRet = ::GetLastError();
    LPTSTR lpMsgBuf = NULL;
    ::FormatMessage(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        nRet,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   // Default language
        reinterpret_cast<LPTSTR>(&lpMsgBuf),
        0,
        NULL
        );
    OutputError(lpMsgBuf);
    ::LocalFree(lpMsgBuf);
  }
  
  ::CloseHandle(hPipeRead);
  ::CloseHandle(hPipeWrite);
  ::CloseHandle(hRead2);
  ::CloseHandle(m_hWriteSubProcess);

  m_hWriteSubProcess  = NULL;
  m_dwProcessGroupId = 0;
  
  m_bRuning = FALSE;
  
  Exit(exitcode);
  
  return exitcode;
}

