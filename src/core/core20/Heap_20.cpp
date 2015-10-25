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
#include "common_20.h"
#include "DynArray_20.h"

//#define CRI_SEC 

class MemCriticalSection
{
#ifdef CRI_SEC 
  CRITICAL_SECTION m_CriticalSection;
#else 
  HANDLE  m_hSemaphore;
#endif 

public:
  MemCriticalSection()
#ifdef CRI_SEC 
  {
    InitializeCriticalSection(&m_CriticalSection);
  }  
#else
  {
    // Create a semaphore with initial 1
    m_hSemaphore = CreateSemaphore( 
        NULL, // no security attributes
        1,   // initial count
        1,   // maximum count
        NULL);  // unnamed semaphore
    if (m_hSemaphore == NULL) 
    {
        // Check for error.
    }
  }
#endif 
  
  ~MemCriticalSection() 
#ifdef CRI_SEC 
  {
    DeleteCriticalSection(&m_CriticalSection);
  }
#else
  {
    // The system closes the handle automatically when the process terminates 
#if 0 
    if(m_hSemaphore)
    {
      CloseHandle(m_hSemaphore);
    }
#endif 
  }
#endif 
  
  VOID Enter() 
#ifdef CRI_SEC 
  {
    EnterCriticalSection( &m_CriticalSection );  
  }
#else 
  {
    DWORD dwWaitResult = 0; 
    if(m_hSemaphore == NULL)
      return ;
    
    dwWaitResult = WaitForSingleObject(m_hSemaphore,    // handle to semaphore
      INFINITE); // INFINITE time-out interval    
    switch (dwWaitResult)
    {  
      // The semaphore object was signaled.
    case WAIT_OBJECT_0:
      // OK to open another window.
      break;
      
      // Semaphore was nonsignaled, so a time-out occurred.
    case WAIT_TIMEOUT:
      // Cannot open another window.
      break;
    }
  }
#endif 

  VOID Leave() 
#ifdef CRI_SEC 
  {
    LeaveCriticalSection( &m_CriticalSection );     
  }
#else
  {
    if(m_hSemaphore)
    {
      if (!ReleaseSemaphore(m_hSemaphore, // handle to semaphore
        1,     // increase count by one
        NULL)) // not interested in previous count
      {
        // Deal with the error.
      }
    }
  }
#endif 
};

PRIVATE MemCriticalSection g_MemCs;

///////////////////////////////////////////////////////////////////////////////
//
// AllocateMemory_i
//
// use win32 heap api to allocate memory on the application's heap
// and zero the allocated memory
//
///////////////////////////////////////////////////////////////////////////////
PVOID AllocateMemory_i(DWORD nMemorySize)
{
  PVOID p = NULL;
  
  if(nMemorySize >0 && nMemorySize < 0xFFFFFFFF)
  {
    //g_MemCs.Enter();

    //
    // use HeapAlloc to allocate and clear memory
    //
    //p = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nMemorySize);
    p = HeapAlloc(GetProcessHeap(), 0, nMemorySize);
    
    //g_MemCs.Leave();
  }
  else
  {
    p = NULL;
  }
  
  return p;
}

///////////////////////////////////////////////////////////////////////////////
//
// FreeMemory_2
//
// use win32 heap api to free memory previously allocated on the application's
// heap
//
///////////////////////////////////////////////////////////////////////////////
DWORD FreeMemory_2(PVOID pMemory, bool bCheck) 
{
  U32 ret = 0;

  if(pMemory == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }

  //g_MemCs.Enter();
  if(bCheck)
  {
    //
    // get size of the allocated memory
    //
    SIZE_T nMemorySize = HeapSize(GetProcessHeap(), 0, pMemory);
    
    if (-1 == nMemorySize)
    {
      //
      // don't exit -- try freeing anyway
      //
    }
    else
    {
      //
      // fill memory with 0xdd's before freeing, so it is easier to debug 
      // failures caused by using pointer to deallocated memory
      //
      if (0 != pMemory)
      {
        FillMemory(pMemory, nMemorySize, 0xDD);
      }
    }
  } // if(bCheck) 
  
  //
  // use HeapFree to free memory. use return code to log the result, but
  // do not return it to the caller
  //
  bool bFreeSuccess = HeapFree(GetProcessHeap(), 0, pMemory);
  if (FALSE == bFreeSuccess)
  {
    //
    // if this assertion fires, it is likely there is a problem with the 
    // memory we are trying to deallocate. Was it allocated using heapalloc
    // and on the same heap? Is this a valid pointer?
    //
    ret = ERR_INVALID_MEMORY_ADDR_I;
  }
  
  //g_MemCs.Leave();
  
  return ret;
}

U32 ExceptionExit()
{    
  return 0;
}

typedef struct  
{
    CHAR*  pHeapAllocAddr;
    DWORD  dwHeapAllocSize;
    DWORD  dwModuleId;
    DWORD  dwRefCnt;
    CONST CHAR* pszFileName;
    DWORD  dwLine;
    DWORD  dwAllocThreadId;
    DWORD  dwFreeThreadId;
#ifdef _DEBUG
    DWORD dwTimeStamp;
#endif 
  DWORD dwAllocCnt;
  DWORD dwFreeCnt;
  DWORD dwAllocTotalSize;
} HEAP_BLK_INFO;

CDynArray<HEAP_BLK_INFO> g_HeapBlockArray;

#define SENTRY_GARBAGE    0xCC
#define MALLOC_GARBAGE    0xBB
#define FREE_GARBAGE      0xDD
#define SIZEOF_SENTRY     4
  
#define MAX_MOD_ID        1024 

VOID MemInit(VOID)
{

}

U32 MemoryLeakCheck_i(U32 nModuleId, U32& TotalMem)
{
  g_MemCs.Enter();
  
  bool bFoundMod = FALSE;
  int size = g_HeapBlockArray.GetSize();
  
  for (int i = 0; i< size; i++)
  {
    HEAP_BLK_INFO* pHeapBlk = g_HeapBlockArray[i];
    
    if(nModuleId == -1)
    {
      TotalMem += pHeapBlk->dwHeapAllocSize;
      if(TotalMem == 0)
        return 0;
      bFoundMod = TRUE;
      OutputDbgStrA_i( ("%s(%d) : [%04X] Memory Leak, ModuleId=%d, Add=%08x, Size=%d\r\n"),
        pHeapBlk->pszFileName,
        pHeapBlk->dwLine,
        pHeapBlk->dwAllocThreadId,
        pHeapBlk->dwModuleId,
        pHeapBlk->pHeapAllocAddr-SIZEOF_SENTRY,
        pHeapBlk->dwHeapAllocSize - 2 * SIZEOF_SENTRY);         
    }
    else if(pHeapBlk->dwModuleId == nModuleId)
    {
      TotalMem += pHeapBlk->dwHeapAllocSize;
      bFoundMod = TRUE;
       if(TotalMem == 0)
        return 0;
      OutputDbgStrA_i( ("%s(%d) : [%04X] Memory Leak, ModuleId=%d, Add=%08x, Size=%dBytes\r\n"),
        pHeapBlk->pszFileName,
        pHeapBlk->dwLine,
        pHeapBlk->dwAllocThreadId,
        pHeapBlk->dwModuleId,
        pHeapBlk->pHeapAllocAddr-SIZEOF_SENTRY,
        pHeapBlk->dwHeapAllocSize-2 * SIZEOF_SENTRY);  
      break;
    }
  }
  
  g_MemCs.Leave();
  
  return 0;
}


U32 ProfileHeapLeak_i(U32 nModuleId, HEAP_PROFILE_INFO_I* pHeapProfileInfo)
{
  g_MemCs.Enter();
  
  bool bFoundMod = FALSE;
  int size = g_HeapBlockArray.GetSize();
  
  for (int i = 0; i< size; i++)
  {
    HEAP_BLK_INFO* pHeapBlk = g_HeapBlockArray[i];
    if(pHeapBlk->dwModuleId == nModuleId)
    {
      pHeapProfileInfo->dwAllocThreadId = pHeapBlk->dwAllocThreadId;
      pHeapProfileInfo->dwHeapAllocSize = pHeapBlk->dwHeapAllocSize - 2 * SIZEOF_SENTRY;
      pHeapProfileInfo->dwLine          = pHeapBlk->dwLine;
      pHeapProfileInfo->dwModuleId      = pHeapBlk->dwModuleId;
#ifdef _DEBUG
      pHeapProfileInfo->dwTimeStamp     = pHeapBlk->dwTimeStamp;
#endif
      pHeapProfileInfo->pHeapAllocAddr  = pHeapBlk->pHeapAllocAddr-SIZEOF_SENTRY;
      pHeapProfileInfo->pszFileName     = pHeapBlk->pszFileName;
      bFoundMod = TRUE;
      break;
    }
  }
  
  g_MemCs.Leave();

  return 0;
}


U32 ProfileHeapLeak_i(PCSTR pszFile)
{
  bool bFoundMod = FALSE;
  
  g_MemCs.Enter();
  
  FILE* f = fopen(pszFile, ("w"));
  if(f)
  {
    int size = g_HeapBlockArray.GetSize();
    U32 ulTotalMem = 0;
    for (int i = 0; i< size; i++)
    {
      HEAP_BLK_INFO* pHeapBlk = g_HeapBlockArray[i];
      ulTotalMem += pHeapBlk->dwHeapAllocSize;
      
      fprintf(f, "%s(%d) : [%04X] Memory Leak, ModuleId=%d, Add=%08x, Size=%d\r\n",
        pHeapBlk->pszFileName,
        pHeapBlk->dwLine,
        pHeapBlk->dwAllocThreadId,
        pHeapBlk->dwModuleId,
        pHeapBlk->pHeapAllocAddr-SIZEOF_SENTRY,
        pHeapBlk->dwHeapAllocSize - 2 * SIZEOF_SENTRY);
    }
    fclose(f);
  }
  
  g_MemCs.Leave();
  
  return 0;
}

// The nModuleId should be less than 512. 
PVOID AllocateMemoryDbg_i(U32 nSize, U32 nModuleId, CONST CHAR* pszFileName, U32 nLine )
{
  bool bRet = FALSE;
  CHAR* p   = NULL;
  CHAR* q   = NULL;
  
  if(nSize == 0 || nSize == 0xFFFFFFFF)
  {
    TRACE_I(_T("Heap Allocate Module(%d), input parameter(%x) is invalid"), nModuleId, nSize);
    return NULL;
  }
  
  if(nSize&0x80000000)
  {
    return NULL;
  }
  
  U32 ulAllocatedSize = nSize + 2 * SIZEOF_SENTRY;
  if(nSize > ulAllocatedSize)
  {
    return NULL;
  }

  q = (CHAR*)AllocateMemory_i( ulAllocatedSize );  
  
  if(q)
  {
    p = q + SIZEOF_SENTRY;
    memset( q, SENTRY_GARBAGE, SIZEOF_SENTRY );
    memset( p, MALLOC_GARBAGE, nSize );
    memset( p + nSize, SENTRY_GARBAGE, SIZEOF_SENTRY );
    
    HEAP_BLK_INFO* pHeapBlk = new HEAP_BLK_INFO;
    if(pHeapBlk == NULL)
    {
      FreeMemory_2(q, FALSE);
      return NULL;
    }
    
    g_MemCs.Enter();
    
    pHeapBlk->dwAllocCnt ++;
    pHeapBlk->dwAllocThreadId = ::GetCurrentProcessId();
    pHeapBlk->dwAllocTotalSize += nSize;
    pHeapBlk->dwFreeThreadId  = 0;
    pHeapBlk->dwHeapAllocSize = nSize+2 * SIZEOF_SENTRY;
    pHeapBlk->dwFreeCnt  = 0;
    pHeapBlk->dwLine     = nLine;
    pHeapBlk->dwModuleId = nModuleId;
#ifdef _DEBUG
    pHeapBlk->dwTimeStamp = GetTickCount();
#endif
    pHeapBlk->pHeapAllocAddr = q;
    pHeapBlk->pszFileName    = pszFileName;
    g_HeapBlockArray.Add(pHeapBlk);
    
    g_MemCs.Leave();
  }
  else
  {
    TRACE_I(_T("*** Fail to Allocate Memory err=%d\r\n"), GetLastError());
  }  

  return p;
}

bool IsValidHeapAddrDbg(CONST PVOID pHeapAddr)
{
  CHAR* pAddr = (CHAR*)pHeapAddr - SIZEOF_SENTRY;

  if(pAddr[0] == (CHAR)SENTRY_GARBAGE && 
    pAddr[1] == (CHAR)SENTRY_GARBAGE &&
    pAddr[2] == (CHAR)SENTRY_GARBAGE &&
    pAddr[3] == (CHAR)SENTRY_GARBAGE)
  {
    DWORD nMemorySize = HeapSize(GetProcessHeap(), 0, pAddr);
    
    if (-1 == nMemorySize)
    {
      return FALSE;
    }
    else
    {
      CHAR* p = &pAddr[nMemorySize-4];
      if(p[0] == (CHAR)SENTRY_GARBAGE && 
        p[1] == (CHAR)SENTRY_GARBAGE &&
        p[2] == (CHAR)SENTRY_GARBAGE &&
        p[3] == (CHAR)SENTRY_GARBAGE)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
  }
  else 
  {
    return FALSE;
  }
   
  return FALSE;
}

bool IsBadHeap_i(CONST PVOID  pMem)
{
  if((pMem == NULL) || ((U32)pMem == -1))
    return FALSE;
  
  if(IsValidHeapAddrDbg(pMem))
  {
    CHAR* pHeap = (CHAR*)pMem - SIZEOF_SENTRY;
    return (HeapValidate(GetProcessHeap(), 0, (CHAR*)pHeap) != 0)?FALSE:TRUE; 
  }
  else
  {
    return FALSE;
  }
}


// ERR_INVALID_PARAM_I
// ERR_INVALID_MEMORY_ADDR_I
// ERR_HEAP_LEAK_I
U32 FreeMemoryDbg_i(PVOID pMemory, U32 nModuleId, CONST CHAR* lpszFileName, U32 nLine, bool bCheck)
{
  UNREF_PARAM(bCheck);

  U32 ret             = 0;
  U32 dwHeapAllocSize = 0;
  CHAR* p = (CHAR*)pMemory - SIZEOF_SENTRY;
  
  if( pMemory == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  g_MemCs.Enter();

  bool bFound = FALSE;
  bool bFoundMod = FALSE;
 
  DWORD cnt = g_HeapBlockArray.GetSize();

  if(cnt == 0)
  {
    g_MemCs.Leave();
    return 0;
  }
  
  DWORD idx = 0;  
  HEAP_BLK_INFO* pHeapBlk = NULL;
  HEAP_BLK_INFO* pRemoveHeapBlk = NULL;

  for (DWORD i = 0; i< cnt; i++)
  {
      pHeapBlk = g_HeapBlockArray[i];
      
      if(nModuleId != -1)
      {
        if((pHeapBlk->dwModuleId == nModuleId))
        {
          bFoundMod = TRUE;
          if(pHeapBlk->pHeapAllocAddr == p)
          {
            bFound = TRUE;
            dwHeapAllocSize  = pHeapBlk->dwHeapAllocSize;
            idx = i;
            pRemoveHeapBlk = pHeapBlk;
            break;
          }
        }
      }
      else
      {
        bFoundMod = TRUE;
        if(pHeapBlk->pHeapAllocAddr == p)
        {
          bFound = TRUE;
          dwHeapAllocSize  = pHeapBlk->dwHeapAllocSize;
          idx = i;
          pRemoveHeapBlk = pHeapBlk;
          break;
        }
      }
  }
  
  if(!bFoundMod)
  {
    g_MemCs.Leave();
    return ERR_INVALID_MODULE_I;
  }
  
  if(!bFound)
  {
    g_MemCs.Leave();
    return ERR_INVALID_MEMORY_ADDR_I;
  }
  else
  {
    int size = dwHeapAllocSize - 2*SIZEOF_SENTRY;
    if(size>0)
    {
      memset( pMemory, FREE_GARBAGE, size );
    }
    else
    {
      g_MemCs.Leave();
      return ERR_HEAP_LEAK_I;
    }
    
    ret = FreeMemory_2(p, FALSE);
    if(ret == ERR_OK_I)
    {
      //fprintf(stdout, " Remove: %08x\n", g_HeapBlockArray[idx]->pHeapAllocAddr);
      if(g_HeapBlockArray.RemoveAt(idx) == FALSE)
      {
        TRACE_I(_T("*** Heap Free Fail.\n"));
      }
    }
  }
  
  g_MemCs.Leave();
  
  return ret;
}

