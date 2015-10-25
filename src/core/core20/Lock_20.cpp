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

#ifdef ITS_HAVE_THREAD_LOCK 

#if 0 
class CInternalSynchronization
{
public:
  CInternalSynchronization();
  ~CInternalSynchronization();
  LPCRITICAL_SECTION m_pInternalCriticalSection;
};
// friend class CInternalSynchronization;
PRIVATE	CInternalSynchronization s_InternalThreadSynchronization;

CInternalSynchronization::CInternalSynchronization()
{
  m_pInternalCriticalSection = new CRITICAL_SECTION;
  //OutputDbgStr_2(_T("Create CriticalSection Lock(0x%x)!\n"), m_pInternalCriticalSection);
  ::InitializeCriticalSection(m_pInternalCriticalSection); // In low memory situations, InitializeCriticalSection can raise a STATUS_NO_MEMORY exception. 
}

CInternalSynchronization::~CInternalSynchronization()
{
  ::DeleteCriticalSection(m_pInternalCriticalSection);
  delete m_pInternalCriticalSection;
  //OutputDebugString(_T("Delete CriticalSection Lock\n"));
  m_pInternalCriticalSection = NULL;
}

#define	LOCK()		if (s_InternalThreadSynchronization.m_pInternalCriticalSection != NULL) \
::EnterCriticalSection(s_InternalThreadSynchronization.m_pInternalCriticalSection);
#define	UNLOCK()	if (s_InternalThreadSynchronization.m_pInternalCriticalSection != NULL) \
::LeaveCriticalSection(s_InternalThreadSynchronization.m_pInternalCriticalSection);
#else
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
    {
      CloseHandle(m_hMutex);
      OutputDebugString(_T("[its_bcore] Close CriticalSectionLock.\n"));
    }
  }

  VOID Enter() 
  {
    DWORD dwWaitResult; 
    if(m_hMutex == NULL)
      return ;

    // Request ownership of mutex.
    
    dwWaitResult = WaitForSingleObject( 
      m_hMutex,   // handle to mutex
      INFINITE);   // five-second time-out interval
    
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
        if (! ReleaseMutex(m_hMutex)) { 
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
#define	LOCK()		if (g_CsLock.m_hMutex != NULL) \
g_CsLock.Enter();
#define	UNLOCK()	if (g_CsLock.m_hMutex != NULL) \
g_CsLock.Leave();
#endif 

U32 ITS_ThreadLock(VOID)
{
  LOCK();
  
  return 0;
}

U32 ITS_ThreadUnLock(VOID)
{
  UNLOCK();
  
  return 0;
}

#endif // ITS_HAVE_THREAD_LOCK
