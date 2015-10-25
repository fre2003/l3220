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
#pragma warning(disable:4273)
#include "TLHELP32.H"

#undef TRACE_I 
#define TRACE_I  {}
 


//
// ulTimeOut: time out in millsecond.
// Return Value:
//   ERR_OK_I
//   ERR_OS_QUEUE_FULL_I 
//   ERR_WIN32_ERROR_I 
//

U32 PostThreadMessage_i(
                           DWORD idThread, // thread identifier
                           UINT Msg,       // message
                           WPARAM wParam,  // first message parameter
                           LPARAM lParam,   // second message parameter
                           U32 ulTimeOut
                           )
{
  U32 ulRet = ERR_OK_I;
  BOOL bRet = FALSE;
  
  //
  // the thread is running, post the message.
  //
  // The function fails if the specified thread does not have a message queue
  //
  bRet = ::PostThreadMessage(idThread, Msg, wParam, lParam);
  if(bRet == FALSE)
  {
    if(GetLastError()== ERROR_INVALID_THREAD_ID)
    {
      U32 ulMaxTimeOut = 0;
      
      return ERR_INVALID_PARAM_I;

      // if idThread is not a valid thread identifier, 
      // or if the thread specified by idThread does not have a message queue.
      bRet = ::PostThreadMessage(idThread, Msg, wParam, lParam);
      while(bRet == FALSE)
      {
        bRet = ::PostThreadMessage(idThread, Msg, wParam, lParam);
        if(!bRet)
        {
          Sleep(5);
          DWORD dwError = GetLastError();        
          if(dwError == ERROR_NOT_ENOUGH_QUOTA)
          {
            TRACE_I(_T("Warning: Fail to PostThreadMessage() 1, Not enough quota is available to post message. [%d], try again.\n"), dwError);
            Sleep(5);
            ulRet = ERR_OS_QUEUE_FULL_I;
          }
          ulMaxTimeOut += 5;
          if(ulMaxTimeOut  > ulTimeOut)
          {
            break; // Fail 
          }
        }
      } // While 
    }
    else
    {
      // Maybe Thread Queue is full.
      U32 ulMaxTimeOut = 0;
      bRet = ::PostThreadMessage(idThread, Msg, wParam, lParam);
      while(bRet == FALSE)
      {
        bRet = ::PostThreadMessage(idThread, Msg, wParam, lParam);
        if(!bRet)
        {
          Sleep(5);
          ulMaxTimeOut += 5;
          DWORD dwError = GetLastError();        
          if(dwError == ERROR_NOT_ENOUGH_QUOTA)
          {
            TRACE_I(_T("Warning: Fail to PostThreadMessage() 2, Not enough quota is available to post message. [%d], try again.\n"), dwError);
            Sleep(5);
            ulMaxTimeOut += 5;
            ulRet = ERR_OS_QUEUE_FULL_I;
          }
          if(ulMaxTimeOut > ulTimeOut)
          {
            break; // Fail 
          }
        }
      } // while
    }
  }

  if(bRet == FALSE)
  {
    if(ERR_OS_QUEUE_FULL_I != ulRet)
    {
       ulRet = ERR_WIN32_ERROR_I;
    }
    TRACE_I(_T("Error: Fail to PostThreadMessage to 0x%x, msg=%d, err=%d\n"), idThread, Msg, GetLastError());
    Sleep(0);
  }
  
  return ulRet;
}

INLINE BOOL IsExcludeThread(THREADENTRY32 &te, DWORD * dwExcludedThreadId)
{
  for (int i = 0; i < 64; i++)
  {
    if(dwExcludedThreadId[i] == 0)
    {
      break;
    }
    
    if (te.th32ThreadID == dwExcludedThreadId[i])
    {
      return TRUE;
    }
  }
  
  return FALSE;
}


U32 CThread_i::TerminateThreads(DWORD dwProcessId, DWORD dwExcludedThreadId[64])
{
  if(dwProcessId == 0)
  {
    dwProcessId = ::GetCurrentProcessId();
  }
  
  HANDLE hProcess = OpenProcess(
    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE, dwProcessId);
  
  if (hProcess == NULL)
    return ERR_WIN32_ERROR_I;
  
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, dwProcessId);
  if (hSnap == INVALID_HANDLE_VALUE)
    return ERR_WIN32_ERROR_I;
  
  THREADENTRY32 te;
  
  memset(&te, 0, sizeof(te));
  te.dwSize = sizeof(te);
  
  if (Thread32First(hSnap, &te) == FALSE)
  {
    CloseHandle(hSnap);
    return ERR_WIN32_ERROR_I;
  }
  
  int cnt = 0;
  
  // now enum all thread for this processId
  do
  {
    if (te.th32OwnerProcessID != dwProcessId)
    {
      continue;
    }
    
    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
    if (hThread == NULL)
    {
      continue;
    }
    
    if(IsExcludeThread(te, dwExcludedThreadId))
    {
      continue;
    }
    
    int i =  10; 
    while(!TerminateThread(hThread, 1))
    {
      if(i-- < 0)
        break;
      else
        Sleep(10);
    }
    
    CloseHandle(hThread);
  } while(Thread32Next(hSnap, &te) != FALSE);
  
  CloseHandle(hSnap);
  
  return ERR_OK_I;
}


U32 CThread_i::SuspendResumeThreads(DWORD dwProcessId, DWORD dwExcludedThreadId[64], BOOL bSuspend)
{
  if(dwProcessId == 0)
  {
    dwProcessId = ::GetCurrentProcessId();
  }
  
  HANDLE hProcess = OpenProcess(
    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE, dwProcessId);
  
  if (hProcess == NULL)
    return ERR_WIN32_ERROR_I;
  
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, dwProcessId);
  if (hSnap == INVALID_HANDLE_VALUE)
    return ERR_WIN32_ERROR_I;
  
  THREADENTRY32 te;
  
  memset(&te, 0, sizeof(te));
  te.dwSize = sizeof(te);
  
  if (Thread32First(hSnap, &te) == FALSE)
  {
    CloseHandle(hSnap);
    return ERR_WIN32_ERROR_I;
  }
  
  int cnt = 0;
  
  // now enum all thread for this processId
  do
  {
    if (te.th32OwnerProcessID != dwProcessId)
    {
      continue;
    }
    
    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
    if (hThread == NULL)
    {
      continue;
    }
    
    if(IsExcludeThread(te, dwExcludedThreadId))
    {
      continue;
    }
    
    int i =  10; 
    if(bSuspend)
    {
      while( ::SuspendThread(hThread) == -1)
      {
        if(i-- < 0)
          break;
        else
          Sleep(10);
      }
    }
    else 
    {
      while( ::ResumeThread(hThread) == -1)
      {
        if(i-- < 0)
          break;
        else
          Sleep(10);
      }
    }
    
    CloseHandle(hThread);
  } while(Thread32Next(hSnap, &te) != FALSE);
  
  CloseHandle(hSnap);
  
  return ERR_OK_I;
}



U32 CProfileTime_i::GetThreadsTime(DWORD dwProcessId, THREAD_TIME ThreadTimeArray[], U32 ulThreadTimeArrayCnt)
{
  DWORD dwCnt = 0;
  HANDLE hProcess = ::OpenProcess(
    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE, dwProcessId);

  if (hProcess == NULL)
    return ERR_WIN32_ERROR_I;
  
  HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, dwProcessId);
  if (hSnap == INVALID_HANDLE_VALUE)
    return ERR_WIN32_ERROR_I;
  
  THREADENTRY32 te;
  memset(&te, 0, sizeof(te));
  te.dwSize = sizeof(te);

  if (::Thread32First(hSnap, &te) == FALSE)
  {
    ::CloseHandle(hSnap);
    return ERR_WIN32_ERROR_I;
  }
  
  int cnt = 0;

  // now enum all thread for this processId
  do
  {
    if (te.th32OwnerProcessID != dwProcessId)
    {
      continue;
    }
    
    HANDLE hThread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
    if (hThread == NULL)
    {
      continue;
    }

    if(dwCnt < ulThreadTimeArrayCnt)
    {
      FILETIME CreationTime, ExitTime,KernelTime,UserTime; 
      ::GetThreadTimes(hThread, &CreationTime, &ExitTime, &KernelTime, &UserTime);
      
      //FileTimeToSystemTime(&UserTime, &st);
      //wsprintf(ThreadTime[dwCnt].szTime, _T("%02d:%02d:%02d.%03d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
      //ThreadTime[dwCnt].wHour         = st.wHour;
      //ThreadTime[dwCnt].wMinute       = st.wMinute;
      //ThreadTime[dwCnt].wSecond       = st.wSecond;

      ThreadTimeArray[dwCnt].CreationTime  = CreationTime;
      ThreadTimeArray[dwCnt].KernelTime    = KernelTime;
      ThreadTimeArray[dwCnt].UserTime      = UserTime;
      ThreadTimeArray[dwCnt].dwThreadId    = te.th32ThreadID;
      dwCnt ++;
    }
    
    CloseHandle(hThread);
  } while(Thread32Next(hSnap, &te) != FALSE);
  
  CloseHandle(hSnap);

  return ERR_OK_I;
}
