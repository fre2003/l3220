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
#include "MMSYSTEM.H"
#pragma warning( disable : 4273 )
#include "TLHELP32.H"
#include <process.h>
#include "../core20/common_20.h"

//#define  HAVE_WIN32_CREATE_THREAD 

#pragma comment(lib, "Winmm.lib")

#ifdef _DEBUG
#define TH_TRACE0(x)              OutputDbgStr_i(_T("Worker Thread ") ## _T(x))
#define TH_TRACE1(x, p1)          OutputDbgStr_i(_T("Worker Thread ") ## _T(x), p1)
#define TH_TRACE2(x, p1, p2)      OutputDbgStr_i(_T("Worker Thread ") ## _T(x), p1, p2)
#define TH_TRACE3(x, p1, p2, p3)  OutputDbgStr_i(_T("Worker Thread ") ## _T(x), p1, p2, p3)
#else
#define TH_TRACE0(x)         
#define TH_TRACE1(x, p1)     
#define TH_TRACE2(x, p1, p2) 
#define TH_TRACE3(x, p1, p2, p3)
#endif


bool CThread_i::ThreadIsAlive(U32 dwThreadID)
{
  HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadID); 
  U32 dwThreadExitCode = 0;
  bool ret = FALSE;
  
  if ((hThread != NULL) && (::GetExitCodeThread(hThread, &dwThreadExitCode)))
  {
    CloseHandle(hThread);
    hThread = NULL;
    ret =  (bool)(dwThreadExitCode == STILL_ACTIVE);
  }
  
  if (hThread != NULL)
  {
    CloseHandle(hThread);
    hThread = NULL;
  }
  
  return ret;
}


IMPLEMENT_DYNAMIC(CThread_i, CObject)

class CThreadProfile
{
public:
  CThreadProfile();
  ~CThreadProfile();

  VOID Add(DWORD dwThreadId, DWORD dwObject);
  VOID Add(DWORD dwThreadId, LPCTSTR pszName);
  VOID Del(DWORD dwThreadId);
  VOID Profile(); // Output to Debugger.
  VOID RemoveAll();
  
  // Worker object
  CDWordArray m_ThreadObjArray;
  LONGLONG     m_lnThd100ns[32];
  
  // 
  CDWordArray m_dwThreadIdArray;
  CStringArray m_strThreadNameArray; 
};

PRIVATE CThreadProfile g_ThreadProfile;

VOID CThreadProfile::Add(DWORD dwThreadId, LPCTSTR pszName)
{
  m_dwThreadIdArray.Add(dwThreadId);
  m_strThreadNameArray.Add(pszName);
}

VOID CThreadProfile::Add(DWORD dwThreadId, DWORD dwObject)
{
  U32 size = m_ThreadObjArray.GetSize();

  for (U32 i = 0; i< size; i++)
  {
    CThread_i* pWkThd = (CThread_i*)g_ThreadProfile.m_ThreadObjArray[i];
    U32 tid = pWkThd->GetID();
    if(tid == dwThreadId)
    {
      m_ThreadObjArray.RemoveAt(i);
      break;
    }
  }
  
  m_ThreadObjArray.Add(dwObject);
}

VOID CThreadProfile::Del( DWORD dwThreadId)
{
  U32 size = m_ThreadObjArray.GetSize();

  for (U32 i = 0; i< size; i++)
  {
    CThread_i* pWkThd = (CThread_i*)g_ThreadProfile.m_ThreadObjArray[i];
    U32 tid = pWkThd->GetID();
    if(tid == dwThreadId)
    {
      m_ThreadObjArray.RemoveAt(i);
      break;
    }
  }
  
  size = m_dwThreadIdArray.GetSize();
  for (i = 0; i< size; i++)
  {
    U32 id = m_dwThreadIdArray.GetAt(i);
    if(id == dwThreadId)
    {
      m_dwThreadIdArray.RemoveAt(i);
      m_strThreadNameArray.RemoveAt(i);
      break;
    }
  }
}


VOID CThreadProfile::Profile()
{
  CThread_i* pWkThd = NULL;
  
  TRACE_I(_T("Thread:  Name                  ID             Status \n"));
  U32 cnt = g_ThreadProfile.m_ThreadObjArray.GetSize();
  for (S32 i = 0; i< g_ThreadProfile.m_ThreadObjArray.GetSize(); i++)
  {
    pWkThd = (CThread_i*)g_ThreadProfile.m_ThreadObjArray[i];
    TRACE_I(_T("        %-32s       0x%-4x          %s     \n"),
    pWkThd->GetName(), pWkThd->GetID(), (pWkThd->IsStart() == 1) ? _T("Started"): _T("UnStarted"));
  }

  cnt = (U32)g_ThreadProfile.m_dwThreadIdArray.GetSize();
  for (U32 k = 0; k< cnt; k++)
  {
    U32 id = g_ThreadProfile.m_dwThreadIdArray.GetAt(k);
    TRACE_I(_T("        %-32s       0x%-4x          %s      \n"),
    g_ThreadProfile.m_strThreadNameArray.GetAt(k), id, 
    CThread_i::ThreadIsAlive(id) ? _T("Started"): _T("UnStarted"));
  }
}


VOID CThreadProfile::RemoveAll()
{
  g_ThreadProfile.m_ThreadObjArray.RemoveAll();
  g_ThreadProfile.m_dwThreadIdArray.RemoveAll();
  g_ThreadProfile.m_strThreadNameArray.RemoveAll();
}

CThreadProfile::CThreadProfile()
{
  RemoveAll();
}

CThreadProfile::~CThreadProfile()
{
  RemoveAll();
}

U32 CThread_i::GetThreadTime (DWORD dwThreadID, TCHAR szUserTime[128], TCHAR szCreateTime[128])
{
    szUserTime[0]   = _T('\0');
    szCreateTime[0] = _T('\0');

    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadID);
    if (hThread == NULL)
    { 
      return ERR_ACCESS_THREAD_I;
    }
    
    SYSTEMTIME st;
    FILETIME CreationTime, ExitTime,KernelTime,UserTime; 
    GetThreadTimes(hThread, &CreationTime, &ExitTime, &KernelTime, &UserTime);
    FileTimeToSystemTime(&UserTime, &st);
    wsprintf(szUserTime, _T("%02d:%02d:%02d.%03d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    FileTimeToSystemTime(&CreationTime, &st);
    wsprintf(szCreateTime, _T("%02d:%02d:%02d.%03d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    
    CloseHandle(hThread);
    
    return 0;
}

U32 CThread_i::GetThreadTime(DWORD dwThreadID, FILETIME& UserTime)
{
    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadID);
    if (hThread == NULL)
    { 
      return ERR_ACCESS_THREAD_I;
    }
    
    FILETIME CreationTime, ExitTime,KernelTime;
    GetThreadTimes(hThread, &CreationTime, &ExitTime, &KernelTime, &UserTime);
    //FileTimeToSystemTime(&UserTime, &sSystemTime); // 64-bit count of 100-nanosecond time units 
    
    CloseHandle(hThread);

    return 0;
}


U32 CThread_i::GetProfileInfo(THREAD_PROFILE& ThdProfile)
{
  U32 ret = ERR_OK_I;
  
#if 0 
  SYSTEMTIME st;
  FILETIME CreationTime, ExitTime,KernelTime,UserTime; 
  GetProcessTimes(GetCurrentProcess(), &CreationTime, &ExitTime, &KernelTime, &UserTime);
  FileTimeToSystemTime(&UserTime, &st);
  strProcessUserTime.Format(_T("%02d:%02d:%02d.%03d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif 

  ThdProfile.aszUserTime.RemoveAll();
  ThdProfile.aszNameArray.RemoveAll();
  ThdProfile.adwIdArray.RemoveAll();
  ThdProfile.adwStatus.RemoveAll();
  ThdProfile.adwQueueMessages.RemoveAll();
  
  for (int j = 0; j<64; j++)
  {
    ThdProfile.m_lnThd100nsTime[j] = 0;
  }
  
#if 0 
  CProfileTime cprof;  
  cprof.ProfileStart();
  LONGLONG ln100ns = (LONGLONG)(cprof.Ticks2Seconds(cprof.ProfileEnd())*1000.0*1000.0*10.0);
#endif 
  
#if 0 
  for (int i = 0; i < g_ThreadProfile.m_ThreadIdArray.GetSize(); i++)
  {
    FILETIME ft;
    
    GetThreadTime_2(g_ThreadProfile.m_ThreadIdArray[i], ft);
    
    if (i < 32)
    {
      ThdProfile.m_lnThd100nsTime[i] = (LONGLONG) ((ft.dwHighDateTime << 32) | (ft.dwLowDateTime));
    }
  }
#endif 
  
  U32 size = g_ThreadProfile.m_ThreadObjArray.GetSize();
  CThread_i* pWkThd = NULL;
  
  for (U32 i = 0; i < size; i++)
  {
    TCHAR szUserTime[128] = { 0, };
    TCHAR szCreateTime[128] = { 0, };
    
    pWkThd = (CThread_i*)g_ThreadProfile.m_ThreadObjArray[i];
    if (pWkThd)
    {
      ThdProfile.adwQueueMessages.Add(0 /*pWkThd->GetMessages()*/);
      ThdProfile.aszNameArray.Add(pWkThd->GetName());
      ThdProfile.adwIdArray.Add(pWkThd->GetID());

      long dwStatus  = 0;
      dwStatus = pWkThd->IsStart() ? 1: 0;
      if(pWkThd->IsShutdown()) 
        dwStatus = 2;

      ThdProfile.adwStatus.Add(dwStatus);
      GetThreadTime(pWkThd->GetID(), szUserTime, szCreateTime);
      ThdProfile.aszUserTime.Add(szUserTime);
      ThdProfile.aszCreateTime.Add(szCreateTime);

      FILETIME ft = {0, };
      
      GetThreadTime(pWkThd->GetID(), ft);
      if (i < 64)
      {
        ThdProfile.m_lnThd100nsTime[i] = (LONGLONG) ((ft.dwHighDateTime << 32) | (ft.dwLowDateTime));
      }
    }
  }
  
  size = g_ThreadProfile.m_dwThreadIdArray.GetSize();
  
  for (i = 0; i < size; i++)
  {
    TCHAR szUserTime[128] = { 0, };
    TCHAR szCreateTime[128] = { 0, };
    
    U32 id = g_ThreadProfile.m_dwThreadIdArray[i];
    if(id)
    {
      ThdProfile.adwQueueMessages.Add(0);
      ThdProfile.aszNameArray.Add(g_ThreadProfile.m_strThreadNameArray.GetAt(i));
      ThdProfile.adwIdArray.Add(id);
      
      long dwStatus  = 0;
      dwStatus = CThread_i::ThreadIsAlive(id) ? 1: 0;
      
      ThdProfile.adwStatus.Add(dwStatus);
      GetThreadTime(id, szUserTime, szCreateTime);
      ThdProfile.aszUserTime.Add(szUserTime);
      ThdProfile.aszCreateTime.Add(szCreateTime);
      
      FILETIME ft = {0, };
      
      GetThreadTime(id, ft);
      if (i < 64)
      {
        ThdProfile.m_lnThd100nsTime[i] = (LONGLONG) ((ft.dwHighDateTime << 32) | (ft.dwLowDateTime));
      }
    }
  }
  
  return ret;
}


#include <ERRNO.H>

#define TMP_OUTPUT_FILE _T("C:\\__thread_perf_tmp.bin")

// Run ProcMon.exe to monitor process performance.
U32 CThread_i::LaunchProcMon(LPCTSTR pszOutputFile, 
                             LPCTSTR pszProcessTitle,
                             LPCTSTR pszAdapter1ConnectionName, 
                             LPCTSTR pszAdapter2ConnectionName)
{ 
  DWORD dwProcessId = ::GetCurrentProcessId();
  
  SetFileAttributes(TMP_OUTPUT_FILE, FILE_ATTRIBUTE_NORMAL);
  
  FILE* f = _tfopen(TMP_OUTPUT_FILE, _T("wb"));
  if(f)
  {
    for (int i = 0; i< g_ThreadProfile.m_ThreadObjArray.GetSize(); i++)
    {
      CThread_i* pWkThd = (CThread_i*)g_ThreadProfile.m_ThreadObjArray[i];
      
      DWORD id = pWkThd->GetID();
      TCHAR buffer[256] = {0, };
      
      fwrite(&id,1, 4, f);
      
      lstrcpy(buffer, pWkThd->GetName());
      fwrite(buffer, 1, sizeof(buffer), f);
    }
    
    for (i = 0; i< g_ThreadProfile.m_dwThreadIdArray.GetSize(); i++)
    {
      U32 id= g_ThreadProfile.m_dwThreadIdArray[i];
      if(id)
      { 
        TCHAR buffer[256] = {0, };
        fwrite(&id,1, 4, f);        
        lstrcpy(buffer,  g_ThreadProfile.m_strThreadNameArray.GetAt(i));
        fwrite(buffer, 1, sizeof(buffer), f);
      }
    }

    fclose(f);
  }
  
  CString strProcName;
  strProcName.Format(_T("%s\\procmon.exe"), CFilePath_i::GetAppPath());
  
  CString strNetadpArg;
  CString strNetadpArg1,strNetadpArg2;
  if(pszAdapter1ConnectionName && (pszAdapter1ConnectionName[0] != '\0'))
  {
    strNetadpArg1.Format(_T("-netadp \"%s\""), pszAdapter1ConnectionName);
  }
  if(pszAdapter2ConnectionName && (pszAdapter2ConnectionName[0] != '\0'))
  {
    strNetadpArg2.Format(_T("-netadp \"%s\""), pszAdapter2ConnectionName);
  }  
  
  strNetadpArg.Format(_T("%s %s "), strNetadpArg1, strNetadpArg2);
  
  CString strTitle;
  if(pszProcessTitle)
  {
    strTitle.Format(_T("/title \"%s\" "), pszProcessTitle);
  }

  // /name "PerM" /sleep 2000 /netadp "Adp1" /netadp "Wireless Network Connection" /thf "C:\thf.bin"
  int ret = 0;
  CString strCmd;
  CString strOutCmdFmt;
  if(pszOutputFile)
  {
    CFileOps_i::MakeDir(pszOutputFile, TRUE);
    strOutCmdFmt.Format(_T(" -o \"%s\"  "), pszOutputFile);
  }
  
  strCmd.Format(_T("%s -pid \"%d\" -sleep 1000 %s -thf \"%s\"  %s %s "), 
    strProcName, dwProcessId, strNetadpArg, TMP_OUTPUT_FILE, strOutCmdFmt, strTitle);
  
  ret = CProcessUtil_i::RunConsoleProcess(strCmd, 500);  
  
  return ret;

#if 0 

#ifdef HAVE_UNICODE_I
  ret = _spawnl( _P_NOWAIT, strProcName, strProcName, L"", strPidArg, L"-sleep 1000", strNetadpArg, TMP_OUTPUT_FILE, NULL );
#else
  ret = _spawnl( _P_NOWAIT, strProcName, strProcName, "", strPidArg, "-sleep 1000", strNetadpArg, TMP_OUTPUT_FILE, NULL );
#endif

  if(ret == -1)
  {
    if(errno == E2BIG)
    {
      ret = ERR_INVALID_PARAM_I;
    }
    else if(errno == ENOENT)
    {
      ret = ERR_NOT_FOUND_I;
    }
    else
    {
      ret = ERR_WIN32_ERROR_I;
    }
  }
  else
  {
    ret = ERR_OK_I;
  }
#endif 
  
  return ret;
}


U32 CThread_i::ProfileThread(LPCTSTR pszFileName)
{
  g_ThreadProfile.Profile();
  
  if(pszFileName)
  {
    SetFileAttributes(pszFileName, FILE_ATTRIBUTE_NORMAL);
    FILE* f = _tfopen(pszFileName, _T("w"));
    if(f)
    {
      CThread_i* pWkThd = NULL;
      
      SYSTEMTIME st = {0, };
      FILETIME CreationTime, ExitTime,KernelTime,UserTime = {0, };
      GetProcessTimes(GetCurrentProcess(), &CreationTime, &ExitTime, &KernelTime, &UserTime);
      FileTimeToSystemTime(&UserTime, &st);
      
      _ftprintf(f,_T("Dump Worker Threads in Process(0x%04x), User Time:%02d:%02d:%02d.%03d : \r\n"), 
        GetCurrentProcessId(), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
      
      for (int i = 0; i< g_ThreadProfile.m_ThreadObjArray.GetSize(); i++)
      {
        pWkThd = (CThread_i*)g_ThreadProfile.m_ThreadObjArray[i];
        TCHAR szUserTime[128] = { 0, };        
        TCHAR aCreateTime[128] = { 0, };        
        GetThreadTime(pWkThd->GetID(), szUserTime, aCreateTime);
        _ftprintf(f, _T("  %-32s       0x%04x  %s  %s     %s     \r\n"),
          pWkThd->GetName(), pWkThd->GetID(), aCreateTime, szUserTime, 
          (pWkThd->IsStart() == 1) ? _T("Started") : _T("UnStarted"));
      }

      for (i = 0; i< g_ThreadProfile.m_dwThreadIdArray.GetSize(); i++)
      {
        U32 id = g_ThreadProfile.m_dwThreadIdArray[i];
        TCHAR szUserTime[128] = { 0, };
        TCHAR aCreateTime[128] = { 0, };
        GetThreadTime(id, szUserTime, aCreateTime);
        _ftprintf(f, _T("  %-32s       0x%04x  %s  %s     %s     \r\n"),
          g_ThreadProfile.m_strThreadNameArray.GetAt(i), 
          g_ThreadProfile.m_dwThreadIdArray.GetAt(i),
          aCreateTime, szUserTime, CThread_i::ThreadIsAlive(id)?_T("Started"):_T("UnStarted"));
      }
      
      fclose(f);
    }
  }
  else 
  {
    ;
  }
  
  return 0;
}


#if 0 
class CInternalSynchronization
{
public:
  CInternalSynchronization();
  ~CInternalSynchronization();
  LPCRITICAL_SECTION m_pInternalCriticalSection;
};
#endif 

#if 0
//friend class CInternalSynchronization;
PRIVATE	CInternalSynchronization s_InternalThreadSynchronization;
PRIVATE CRITICAL_SECTION g_CriticalSection = {0,};
#endif 

#if 0
class CriticalSectionLock
{
public:
  HANDLE m_hMutex;
  
public:
  CriticalSectionLock() 
  {
    // Create a mutex with no initial owner.  
    m_hMutex = CreateMutex( 
      NULL,                       // no security attributes
      FALSE,                      // initially not owned
      NULL);  // name of mutex
    
    if (m_hMutex == NULL) 
    {
      // Check for error.
    } 
  }

  ~CriticalSectionLock()
  {
    if(m_hMutex)
      CloseHandle(m_hMutex);
  }

  VOID Enter(DWORD dwMilliseconds=INFINITE) 
  {
    DWORD dwWaitResult; 
    if(m_hMutex == NULL)
      return ;

    // Request ownership of mutex.
    
    dwWaitResult = WaitForSingleObject( 
      m_hMutex,   // handle to mutex
      dwMilliseconds);   // five-second time-out interval
    
    switch (dwWaitResult) 
    {
      // The thread got mutex ownership.
    case WAIT_OBJECT_0: 
      __try 
      { 
        // Write to the database.
      }      
      __finally 
      { 
        // Release ownership of the mutex object.
        if (! ReleaseMutex(m_hMutex)) 
        { 
          // Deal with error.
        }        
        break; 
      } 
      
      // Cannot get mutex ownership due to time-out.
    case WAIT_TIMEOUT: 
      return ; 
      
      // Got ownership of the abandoned mutex object.
    case WAIT_ABANDONED: 
      return ; 
    }
  }

  VOID Leave() 
  {
    if(m_hMutex)
    {
      ReleaseMutex(m_hMutex);
    }
  }
} g_CsLock;

#define	LOCK()		// if (g_CsLock.m_hMutex != NULL) g_CsLock.Enter();
#define	UNLOCK()	// if (g_CsLock.m_hMutex != NULL) g_CsLock.Leave();
#else // if 0 
#define	LOCK()	
#define	UNLOCK()	
#endif 

#define ERROR_EXP() 
#define ERROR_EXP1(dwError)
#define ERROR_EXP2(StrErr)

#if 0 
CString GetLastSystemErrorMsg(DWORD dwSysErrorCode)
{
  CString strErrMsg("");
  LPVOID lpMsgBuf = NULL;
  
  ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    dwSysErrorCode,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL
    );
  
  if (lpMsgBuf != NULL)
  {
    strErrMsg = (LPCSTR)lpMsgBuf;
    ::LocalFree(lpMsgBuf);
  };
  
  return strErrMsg;
}
#endif 

//
CThread_i::CThread_i(PVOID pOwnerObject, // = NULL
                     LPARAM lParam,      // = 0L
                     LPCTSTR pszName,
                     DWORD dwControlCode)  // = MESSAGE_DRIVEN_MODE
{
  m_pOwnerObject          = pOwnerObject;
  m_lParam								= lParam;
  m_dwThreadExitCode		  = (DWORD)-1;
  m_dwStackSize           = 0;
  m_dwControlCode         = dwControlCode;
  m_bStart                = FALSE;  
  m_hDuplicateThread      = NULL;
  m_dwPostMsgTimeOut      = 1000; // 1000ms
  m_dwTimerId             = 0;
  m_dwTimeOut             = 0;
  m_bSwitch               = TRUE;
  m_bShutdown             = FALSE;
  m_QuitMsgCnt            = 0;
  m_szThreadName[0]       = _T('\0');
  m_szExpcetionInfo[0]    = _T('\0');
  m_hThread               = NULL;
  
  if(pszName)
  {
    lstrcpyn(m_szThreadName, pszName, sizeof(m_szThreadName));
  }
  else
  {
    lstrcpyn(m_szThreadName, _T("Unknown Name"), sizeof(m_szThreadName) );
  }
  
  //::InitializeCriticalSection(&g_CriticalSection);
  
  if(dwControlCode == MESSAGE_DRIVEN_MODE)
  {
    NewThread();
  }
}


U32 CThread_i::BeginThread(DWORD (__stdcall *ThreadBody )(void *), 
                          void * arglist,
                          CONST TCHAR szThreadName[80])
{
  DWORD dwThreadID = 0;
  
  HANDLE hThread = BEGINTHREADEX(0, 0, ThreadBody, arglist, 0, &dwThreadID);  
  UNREF_PARAM(hThread);
  if(dwThreadID)
  {
    TH_TRACE2("Thread '%s' [id=0x%x] Create!\n", szThreadName, dwThreadID);
    g_ThreadProfile.Add(dwThreadID, (LPCTSTR)szThreadName);
  }
  
  return dwThreadID;
}


u32 CThread_i::EndThread(U32 dwExitCode)
{
  U32 id = ::GetCurrentThreadId();
  g_ThreadProfile.Del(id);
  
  //_endthreadex(dwExitCode);
  
  return dwExitCode;
}


U32 CThread_i::NewThread(VOID) 
{
  DWORD ret = 0;
  
#ifdef HAVE_SHUTDOW_EVENT
  m_hShutdownEvent = ::CreateEvent(NULL, // security
    TRUE,   // manual-reset
    FALSE,  // not signaled
    NULL);  // anonymous
#endif
  
#ifdef HAVE_WIN32_CREATE_THREAD
  m_hThread = ::CreateThread(NULL,
    m_dwStackSize,
    &ThreadControllingFunction,
    this, // lpParameter 
    CREATE_SUSPENDED, // resume it by Start()
    &m_dwThreadID);
#else
  m_hThread = BEGINTHREADEX(0, m_dwStackSize, ThreadControllingFunction, this, CREATE_SUSPENDED, &m_dwThreadID );
#endif 
  
  if (m_hThread != NULL)
  {
    // DuplicateHandle使用保证句柄是有效的
    ::DuplicateHandle (GetCurrentProcess (),
      m_hThread, GetCurrentProcess (), &m_hDuplicateThread,
      0, FALSE, DUPLICATE_SAME_ACCESS);
      
      TH_TRACE2("Thread '%s' [id=0x%x] Create!\n", m_szThreadName, m_dwThreadID);
      g_ThreadProfile.Add(m_dwThreadID, (DWORD)this);
  }
  else
  {
    ret = GetLastError();
    TH_TRACE2("create Thread '%s' [id=0x%x] Fail\n", m_szThreadName, m_dwThreadID);
  }
  
  return ret;
}

CThread_i::~CThread_i()
{
  if (IsThreadAlive()) 
  {
    try 
    {
      Kill(0, TRUE);
    }
    catch (...) 
    {
      ;
    };
  }
  
  if(m_dwTimerId)
  {
    timeKillEvent(m_dwTimerId);
    m_dwTimerId = 0;
  }
  
  m_pOwnerObject          = NULL;
  m_lParam								= NULL;
  m_dwThreadExitCode		  = (DWORD)ERR_UNKNOWN_I;
  m_dwStackSize           = 0;
  m_dwControlCode         = ERR_UNKNOWN_I;
  m_bStart                = FALSE;  
  m_hDuplicateThread      = NULL;
  m_dwPostMsgTimeOut      = 1000; // 1000ms
  m_dwTimerId             = 0;
  m_dwTimeOut             = 0;
  m_bSwitch               = TRUE;
  m_bShutdown             = FALSE;
}

bool CThread_i::IsThreadAlive()
{
  LOCK();
  
  if ((m_hDuplicateThread != NULL) && (::GetExitCodeThread(m_hDuplicateThread, &m_dwThreadExitCode)))
  {
    UNLOCK();
    return (bool)(m_dwThreadExitCode == STILL_ACTIVE);
  }
  else
  {
    UNLOCK();
    return FALSE;
  }
}

DWORD CThread_i::GetID() const
{
  return m_dwThreadID;
}

bool CThread_i::Shutdown(DWORD dwTimeOut/*=INFINITE*/)
{
  if(!m_bStart && !m_hDuplicateThread)
  {
    return TRUE;
  }
  
  LOCK();
  
  m_bShutdown = TRUE;
  
  #ifdef HAVE_SHUTDOW_EVENT
  SetEvent(m_hShutdownEvent);
  #endif 
  
  if(m_dwTimerId)
  {
    timeKillEvent(m_dwTimerId);
    m_dwTimerId = 0;
  }
  UNLOCK();
  
  if(m_bStart && m_hDuplicateThread && IsThreadAlive())
  {
    if(m_QuitMsgCnt<2)
    {
      if(!PostMessage(WM_QUIT))
      {
        return FALSE;
      }
      else 
      {
        m_QuitMsgCnt ++;
      }
    }
    
    // 
    // Wait for the thread signal.
    // 要确保传递给WaitForSingleObject的句柄是有效的,故使用了m_hDuplicateThread. 
    // 
    if(::WaitForSingleObject(m_hDuplicateThread, dwTimeOut) == WAIT_OBJECT_0)
    {
      m_bStart     = FALSE;
      m_QuitMsgCnt = 0;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    m_QuitMsgCnt = 0;
    return TRUE;
  }
}


VOID CALLBACK CThread_i::ThreadTimerRoutine(UINT uID,    //    
                                            UINT uMsg,    // Reserved.
                                            DWORD dwUser, //  
                                            DWORD dw1,    //  Reserved.
                                            DWORD dw2)    //  Reserved.
{
  UNREF_PARAM(dw1);
  UNREF_PARAM(dw2);
  UNREF_PARAM(uMsg);
  UNREF_PARAM(uID);

#if 0 
  timeKillEvent(uID);
#endif 
    
  if (dwUser == NULL)
  {
    OutputDebugString(_T("*** CThread_i dwUser is NULL.\n"));
    return ;
  }
  else
  {
    ((CThread_i *)dwUser)->ThreadTimerHandler();
    
#if 0
    // Start again...
    DWORD dwTimeOut    = ((CThread_i *)dwUser)->m_dwTimeOut;
    DWORD dwResolution = ((CThread_i *)dwUser)->m_dwResolution;
    
    ((CThread_i *)dwUser)->m_dwTimerId = timeSetEvent(dwTimeOut, dwResolution, 
      ((CThread_i *)dwUser)->ThreadTimerRoutine,
      (DWORD)dwUser, TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
#endif 
  }
}

///////////////////////////////////////////////////////////////////////////////
// SetThreadTimer
// ERR_OK_I
// ERR_TIMER_NOT_EXIST_I
// ERR_TIMER_CREATE_I 
// nTimeOut=0: Kill Timer.
// Return Value: 
// ERR_OK_I
// ERR_TIMER_NOT_EXIST_I
// ERR_TIMER_CREATE_I 
///////////////////////////////////////////////////////////////////////////////
DWORD CThread_i::SetThreadTimer(DWORD nTimeOut, DWORD dwResolution) // INFINITE
{
  DWORD dwRet = 0;
  MMRESULT ret = 0;
  
  LOCK();
  m_dwResolution = dwResolution;

  if(nTimeOut == 0)
  {
    if(m_dwTimerId)
    {
      ret = timeKillEvent(m_dwTimerId);
      if(ret == TIMERR_NOERROR)
      {
         dwRet = ERR_OK_I; // OK
      }
      else // if(ret == MMSYSERR_INVALPARAM)
      {
         dwRet = ERR_TIMER_NOT_EXIST_I;
      }
    }
    m_dwTimerId = 0;
    m_dwTimeOut = 0;
  }
  else
  {
    if(m_dwTimeOut != nTimeOut)
    {
      if(m_dwTimerId)
      {
        ret = timeKillEvent(m_dwTimerId);
        if(ret == TIMERR_NOERROR)
        {
           dwRet = ERR_OK_I; // OK
        }
        else // if(ret == MMSYSERR_INVALPARAM)
        {
           dwRet = ERR_TIMER_NOT_EXIST_I;
        }
      }

      m_dwTimerId = 0;
      m_dwTimeOut = nTimeOut;

      // This function returns NULL if it fails and the timer event was not created. 
      // (This identifier is also passed to the callback function.)
      
#if 0
      m_dwTimerId = timeSetEvent(m_dwTimeOut, m_dwResolution,
        ThreadTimerRoutine, (DWORD)this, TIME_ONESHOT|TIME_CALLBACK_FUNCTION);
#else
      m_dwTimerId = timeSetEvent(m_dwTimeOut, m_dwResolution,
        ThreadTimerRoutine, (DWORD)this, TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
#endif 
      
      if(m_dwTimerId)
      {
        dwRet = ERR_OK_I;
      }
      else 
      {
        dwRet = ERR_TIMER_CREATE_I;
      }
    }
    else
    {
      // 
    }    
  }  
  
  UNLOCK();

  return dwRet;
}


///////////////////////////////////////////////////////////////////////////////
// Kill
// Force to Kill WorkerThread.
///////////////////////////////////////////////////////////////////////////////
VOID CThread_i::Kill(DWORD dwExitCode, // Exit Code = 0
                         bool bForceClose) // = FALSE
{  
  LOCK();
  m_bShutdown = TRUE;

  if(m_dwTimerId)
  {
    timeKillEvent(m_dwTimerId);
    m_dwTimerId = 0;
  }
  
  if(!m_bStart)
  {
			if (m_hThread != NULL) 
      {
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
      }
      if(m_hDuplicateThread != NULL)
      {
        ::CloseHandle(m_hDuplicateThread);
        m_hDuplicateThread = NULL;
      }
  }
	else if (IsThreadAlive() || m_hThread != NULL)
	{
		bool bIsTerminated = TRUE;
		DWORD dwError      = ERROR_SUCCESS;

		if (m_hDuplicateThread != NULL) 
      bIsTerminated = (bool)(::TerminateThread(m_hDuplicateThread, dwExitCode) ? TRUE:FALSE);
		if (!bIsTerminated) 
      dwError = ::GetLastError();
    
		if (bIsTerminated || bForceClose)
		{
			// the thread has been successfully destroyed...
			// or ignore the terminate-thread operation at all
			if (bForceClose)
        m_dwThreadExitCode = dwExitCode;
			if (m_hThread != NULL) 
        ::CloseHandle(m_hThread);
			if (m_hDuplicateThread != NULL) 
        ::CloseHandle(m_hDuplicateThread);
		}
		else
		{
			// if an error occurs while terminating the thread just leave the method
			// by throwing an appropriate exception
      ERROR_EXP1(dwError);
		}
	}
	else
	{
		m_dwThreadExitCode = dwExitCode;
	}
  m_hThread = NULL;
  m_hDuplicateThread = NULL;
  
  UNLOCK();

  TH_TRACE2("'%s' [id=0x%x]  Kill\n", m_szThreadName, m_dwThreadID);
}

///////////////////////////////////////////////////////////////////////////////
// CleanupThread
///////////////////////////////////////////////////////////////////////////////
DWORD CThread_i::CleanupThread(DWORD dwExitCode)
{
  LOCK();
  
  g_ThreadProfile.Del(m_dwThreadID);

  // if receive the WM_QUIT message.
  if (m_hThread != NULL) 
  {
    ::CloseHandle(m_hThread);
    m_hThread    = NULL;
  }
  
  m_dwThreadID = 0;
	m_dwThreadExitCode = dwExitCode;
  
  if(m_hDuplicateThread)
  {
    ::CloseHandle(m_hDuplicateThread);
    m_hDuplicateThread = NULL;
  }
  
  m_bStart = FALSE;
  
  UNLOCK();
  
  return 0;
}

DWORD CThread_i::ThreadInit(VOID)
{
  DWORD dwRet = 0;
  
  TH_TRACE2("'%s' [id=0x%x] Init.\n", m_szThreadName, m_dwThreadID);
  
  return dwRet;
}

///////////////////////////////////////////////////////////////////////////////
// ThreadExit
///////////////////////////////////////////////////////////////////////////////
DWORD CThread_i::ThreadExit(DWORD dwExitCode)
{
  DWORD dwRet = 0;
  
  UNREF_PARAM(dwExitCode);

  TH_TRACE2("'%s' [id=0x%x] Exit.\n", m_szThreadName, m_dwThreadID);
  
  return dwRet;
}


///////////////////////////////////////////////////////////////////////////////
// ThreadTimerHandler
/////////////////////////////////////////////////////////////////////////////// 
DWORD CThread_i::ThreadTimerHandler()
{
  DWORD dwRet = 0;
  
  if(m_bSwitch && m_bStart && m_hDuplicateThread)
  {
    PostMessage(WM_TIMER, 0, 0);
  }
  
  return dwRet;
}

bool CThread_i::TimerMessageSwitch(bool bSwitch)
{
  //LOCK();

  m_bSwitch = bSwitch;

  //UNLOCK();

  return m_bSwitch;
}

// return messages found in the calling thread's message queue. 
U32 CThread_i::GetMessages(VOID) 
{
  U32 ret = 0;

#if 0
  MSG msg = {0, };
  while (::PeekMessage(&msg,   // message information
    NULL,   // handle to window
    0,  // first message
    0,  // last message
    PM_NOREMOVE   // removal options
    ))
  {
    ret ++;
  }
  return ret;
#else
  ret = ::GetQueueStatus(QS_POSTMESSAGE); 
#endif

  return ret;
}

// return messages found in the calling thread's message queue. 
bool CThread_i::PeekMessage(MSG* pMsg)
{  
  return ::PeekMessage(pMsg,   // message information
    NULL,   // handle to window
    0,  // first message
    0,  // last message
    PM_REMOVE   // removal options
    ) ? TRUE:FALSE;
}

bool CThread_i::IsStart(VOID) 
{
  return m_bStart;
}

bool CThread_i::IsShutdown(BOOL bCheckQuitMessage) 
{  
  if(m_hDuplicateThread == NULL || m_dwThreadID == 0)
  {
    return TRUE;
  }
  
  if(bCheckQuitMessage)
  {
    MSG msg          = {0, 0, 0, };
    if(::PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_NOREMOVE))
    {
      if(msg.message == WM_QUIT)
      {
        return TRUE;
      }
    }
  }
  
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// The Main Thread Handler function 
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CThread_i::ThreadControllingFunction(LPVOID lpParameter)
{
  DWORD dwExitCode = 0;
  BOOL bRet        = 0;
  MSG msg          = {0, 0, 0, };

  if(lpParameter == NULL)
  {
    TH_TRACE0("System Error, fail to start thread.\n");
    return dwExitCode;
  }
  
  //
  // Initialize Thread.
  //
  ((CThread_i *)lpParameter)->ThreadInit();
  
  try
  {
    //
    // if m_dwControlCode is SELF_DRIVEN_MODE, call ThreadHandler until it return.
    // otherwise, this funciton will enter the message loop with "while(GetMessage...) {}".
    //
    if(((CThread_i *)lpParameter)->m_dwControlCode == SELF_DRIVEN_MODE)
    {
      // Full Controlled by clients.
      dwExitCode = ((CThread_i *)lpParameter)->ThreadHandler(msg.message, msg.wParam, msg.lParam);
      TH_TRACE3("[Self-driven](Name=%s, ID=0x%x) Exit. ret=%d\n",
        ((CThread_i*)lpParameter)->GetName(), ((CThread_i*)lpParameter)->GetID(), dwExitCode);
    }
    else
    {
      //
      // Message Loop until receive WM_QUIT mesasge.
      //    
      while( (bRet = ::GetMessage( &msg, NULL, 0, 0 )) != 0)
      {
        if(((CThread_i *)lpParameter)->m_bShutdown == TRUE)
        {
          TH_TRACE2("[Message-driven](Name=%s, ID=0x%x) is shutdown. Prepare to exit.\n", ((CThread_i *)lpParameter)->GetName(), ((CThread_i *)lpParameter)->GetID());
          break;
        }
        
        if (bRet == -1)
        {
          // if hWnd is an invalid window handle or lpMsg is an invalid pointer. 
          // handle the error and possibly exit
          TH_TRACE3("[Message-driven](Name=%s, ID=0x%x), GetMessage return -1, err=%d. Prepare to exit.\n", ((CThread_i *)lpParameter)->GetName(), ((CThread_i *)lpParameter)->GetID(), ::GetLastError());
        }
        else
        {
          if(msg.message == 0)
          {
            continue;
          }

          // 
          dwExitCode = ((CThread_i *)lpParameter)->ThreadHandler(msg.message, msg.wParam, msg.lParam); 
          if(dwExitCode == (DWORD)(-1))
          {
            TH_TRACE2("[Message-driven](Name=%s, ID=0x%x), return -1. Prepare to exit.\n", ((CThread_i *)lpParameter)->GetName(), ((CThread_i *)lpParameter)->GetID());
            break;
          }
        }
        
        msg.message = 0;
        msg.wParam  = 0;
        msg.lParam  = 0;
      } // while(..)
      
      if(bRet == 0)
      {
        TH_TRACE2("[Message-driven](Name=%s, ID=0x%x) Exit, with WM_QUIT \n", ((CThread_i *)lpParameter)->GetName(), ((CThread_i *)lpParameter)->GetID());
      }
      else if(bRet == -1)
      {
        TH_TRACE3("[Message-driven](Name=%s, ID=0x%x) Exit, err=%d \n", ((CThread_i *)lpParameter)->GetName(), ((CThread_i *)lpParameter)->GetID(), ::GetLastError());
      }
    } // else      
  }
  catch (...)
  {
    ;
  }
    
  //
  // Call the virtual function ThreadExit to notify the user-defined exit function.
  //
  ((CThread_i *)lpParameter)->ThreadExit(dwExitCode);
  
  //
  // Cleanup the context of this class, set the handle and thread id is zero.
  //
  ((CThread_i *)lpParameter)->CleanupThread(dwExitCode);  
  
  ///////////////////////////////////////////////////////////////////////////////
  // test code....
  ///////////////////////////////////////////////////////////////////////////////
#if 0 
  //WaitForMultipleObjects();
  do
  {
    // Process any pending messages.
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE|PM_QS_SENDMESSAGE)) // PM_QS_SENDMESSAGE
    {
      if(msg.message == CThread_i::WT_MSG_STOP)
      {
        break;
      }
      else if(msg.message == CThread_i::WT_MSG_KILL)
      {
        break;
      }
      else
      {
        dwExitCode = ((CThread_i *)lpParameter)->ThreadHandler(msg, wParam, lParam);
        continue;
      }
    }// while PeekMessage
    
    // Check Message is Available.
    WaitMessage();
    
  } while (1);
#endif 
  
  return dwExitCode;
}

///////////////////////////////////////////////////////////////////////////////
// Start
///////////////////////////////////////////////////////////////////////////////
DWORD CThread_i::Start(VOID)
{
  DWORD ret = 0; 
  
  LOCK();
  
  if(m_bStart)
  {
    TH_TRACE1("[id=%d], is already started.\n", m_dwThreadID);
    UNLOCK(); 
    return ERR_OK_I;
  }
  
  if((m_dwControlCode == SELF_DRIVEN_MODE) && (!m_bStart))
  {
    ret = NewThread();
  }
  
  if (m_hThread != NULL)
  {
    // the return value is the thread's previous suspend count.  
    ret = ResumeThread(m_hThread);
    
    // If the return value is one, the thread was suspended, but is now restarted. 
    // Any return value greater than one means the thread remains suspended.
    
    if(ret == -1) // the function fails 
    {
      m_bStart = FALSE;
      TH_TRACE1("fail to resume thread [id=%d].\n", m_dwThreadID);
    }
    else
    {
      if(ret == 1) // the specified thread was suspended but was restarted 
      {
        ret = ERR_THREAD_RESTART_I;
      }
      else if(ret == 0) // the specified thread was not suspended 
      {
        ret = ERR_OK_I;
      }
      else  // If the return value is greater than 1, the specified thread is still suspended.  
      {
        ret = ERR_THREAD_SUSPEND_I;
        TH_TRACE1("The thread %d was suspended, but is now restarted.\n", m_dwThreadID);
      }
      m_bStart = TRUE;
      g_ThreadProfile.Add(m_dwThreadID, (DWORD)this);
    }
  }
  else 
  {
    ;
  }

  if(ret == ERR_OK_I || ret == ERR_THREAD_RESTART_I)
  {
      TH_TRACE1("[id=%d] start.\n", m_dwThreadID);
  }
  UNLOCK();
  
  return ret;
}


///////////////////////////////////////////////////////////////////////////////
// Stop
///////////////////////////////////////////////////////////////////////////////
bool CThread_i::Stop(DWORD dwTimeout, bool bForceClose)
{
  DWORD dwExitCode = 0;

  if(!m_bStart || !m_hDuplicateThread)
  {
    return TRUE;
  }  
  
  bool bRet = Shutdown(dwTimeout);
  
  if (bRet)
  {
    if (m_hThread != NULL)
    {
      ::CloseHandle(m_hThread);
      m_hThread = NULL;
    }
    if (m_hDuplicateThread != NULL)
    {
      ::CloseHandle(m_hDuplicateThread);
      m_hDuplicateThread = NULL;
    }
    m_bStart = FALSE;
    return TRUE;
  }
  else
  {
    if (bForceClose)
    {
      if ((m_hDuplicateThread) && IsThreadAlive())
      {
        bool bIsTerminated = TRUE;
        
        if (m_hDuplicateThread != NULL)
          bIsTerminated = ::TerminateThread(m_hDuplicateThread, dwExitCode)?TRUE:FALSE;

        if (!bIsTerminated)
          TH_TRACE1("Fail to TerminateThread, err=%d", ::GetLastError());
        
        if (bIsTerminated || bForceClose)
        {
          // the thread has been successfully destroyed...
          // or ignore the terminate-thread operation at all
          if (m_hThread != NULL)
            ::CloseHandle(m_hThread);
          if (m_hDuplicateThread != NULL)
            ::CloseHandle(m_hDuplicateThread);
          m_bStart = FALSE;
          return TRUE;
        }
        else
        {
          return FALSE;
        }
      } // if (m_hDuplicateThread != NULL && IsThreadAlive())
    } // if (bForceClose)
  } // 

  return TRUE;

#if 0 
	enum {DELAY_INTERVAL_BASE = 100};
  if (IsThreadAlive())
  {
    CString strError;
    int nAttemption = 0, nMaxAttemptions;
    nMaxAttemptions = (dwTimeout != INFINITE) ? ((int)dwTimeout * 100)/100 : -1;
    
    if (!PostMessage(WM_QUIT))
    {
      LOCK();
      dwExitCode = m_dwThreadExitCode;
      ERROR_EXP();
      UNLOCK();
    }
    else
    {
      
    }
    
    while (IsThreadAlive() && (nMaxAttemptions == -1 || nAttemption++ < nMaxAttemptions))
    {
      ::Sleep(DELAY_INTERVAL_BASE);
    }
    
    LOCK();    
    dwExitCode = m_dwThreadExitCode;
    if (m_dwThreadExitCode != STILL_ACTIVE)
    {
      UNLOCK();
      return TRUE;
    }
    else
    {
      UNLOCK();
      return FALSE;
    }
  }
  else
  {
    LOCK(); 
    if (m_hThread != NULL)
    {
      ::CloseHandle(m_hThread);
    }
    dwExitCode = m_dwThreadExitCode;
    UNLOCK();
    return TRUE;
  };
#endif
}

///////////////////////////////////////////////////////////////////////////////
// The thread to which the message is posted must have created a message queue, 
// or else the call to PostThreadMessage fails. 
///////////////////////////////////////////////////////////////////////////////
bool CThread_i::PostMessage(UINT Msg,       // message to post
                            WPARAM wParam,  // first message parameter
                            LPARAM lParam   // second message parameter
                            )
{
  bool bReturn = FALSE;
  
  if (NULL != m_hThread)
  {
    //
    // the thread is running, post the message.
    //
    // The function fails if the specified thread does not have a message queue
    //
    bReturn = ::PostThreadMessage(m_dwThreadID, Msg, wParam, lParam)?TRUE:FALSE;
    if(bReturn == FALSE)
    {
      Sleep(10);
      bReturn = ::PostThreadMessage(m_dwThreadID, Msg, wParam, lParam)?TRUE:FALSE;
    }
  }
  else
  {
      TH_TRACE0("the handle is NULL when post message.\n");
  }
  
  return bReturn;
}


HANDLE CThread_i::GetThreadHandle(VOID) CONST
{
  return m_hDuplicateThread;
}

VOID CThread_i::SetPostMsgTimeOut(DWORD dwPostMsgTimeOut)
{
  m_dwPostMsgTimeOut = dwPostMsgTimeOut;
}


VOID CThread_i::SetParam (LPARAM lParam)
{
  m_lParam = lParam;
}


VOID CThread_i::CleanObject(VOID)
{
  m_pOwnerObject = NULL;
}


PVOID CThread_i::GetObject(VOID)  CONST
{
  return m_pOwnerObject;
}


LPARAM CThread_i::GetParam(VOID)  CONST
{
  return m_lParam;
}


LPCTSTR CThread_i::GetName(VOID)  CONST
{
  return (LPCTSTR)m_szThreadName;
}
