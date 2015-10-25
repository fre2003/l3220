#include "StdAfx.h"
#include "common_20.h"

class MemCriticalSection
{
  //CRITICAL_SECTION m_CriticalSection;
  HANDLE m_hMutex;
  
public:
  MemCriticalSection()     
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
  
  ~MemCriticalSection() 
  {
    if(m_hMutex)
    {
      CloseHandle(m_hMutex);
    }
  }
  
  VOID Enter()    // { EnterCriticalSection( &m_CriticalSection );     }
  {
    DWORD dwWaitResult = 0; 
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
  VOID Leave()   //          { LeaveCriticalSection( &m_CriticalSection );     }
  {
    if(m_hMutex)
    {
      ReleaseMutex(m_hMutex);
    }
  }
} g_MemCs;

///////////////////////////////////////////////////////////////////////////////
//
// AllocateMemory
//
// use win32 heap api to allocate memory on the application's heap
// and zero the allocated memory
//
///////////////////////////////////////////////////////////////////////////////
PVOID AllocateMemory(DWORD nMemorySize)
{ 
  PVOID p = NULL;

  g_MemCs.Enter();

  //
  // use HeapAlloc to allocate and clear memory
  //
  p = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nMemorySize);
  
  g_MemCs.Leave();

  return p;
}

///////////////////////////////////////////////////////////////////////////////
//
// FreeMemory
//
// use win32 heap api to free memory previously allocated on the application's
// heap
//
///////////////////////////////////////////////////////////////////////////////
DWORD FreeMemory(PVOID pMemory, BOOL bCheck) 
{
  U32 ret = ERR_OK_I;

  if(pMemory == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }

  g_MemCs.Enter();
  if(bCheck)
  {
    //__try
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
    }
    //__finally 
    //{
      //g_MemCs.Leave();
      //return ERR_INVALID_MEMORY_ADDR_I;
    //}
  } // if(bCheck) 
  
  //
  // use HeapFree to free memory. use return code to log the result, but
  // do not return it to the caller
  //
  BOOL bFreeSuccess = HeapFree(GetProcessHeap(), 0, pMemory);
  if (FALSE == bFreeSuccess)
  {
    //
    // if this assertion fires, it is likely there is a problem with the 
    // memory we are trying to deallocate. Was it allocated using heapalloc
    // and on the same heap? Is this a valid pointer?
    //
    // ASSERT(FALSE);
    ret = ERR_INVALID_MEMORY_ADDR_I;
  }
  
  g_MemCs.Leave();
  
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
void get_mega_kilo_byte( int n, int *mega, int *kilo, int *byte );
BOOL ValidatePtr( char *p, size_t size, char mod_id );
void IncRefCounter( char *p, char mod_id );
void MemoryCheck(char mod_id, int show_mem_type );

#define HPRINTF_BUFFER_SIZE (1024*3)				// must be at least 2048

static CHAR g_szLogBuf[HPRINTF_BUFFER_SIZE];	// wvsprintf never prints more than one K.
static int  g_LogBufIdx = 0;
static HANDLE g_hLogFile = NULL;
static BOOL g_EnableMemChk = TRUE;

//
// IN: the full path file name.
// 
HANDLE LogFileOpen(CONST CHAR* pszLogFile)
{
  HANDLE hLogFile = NULL;
  
  SetFileAttributesA(pszLogFile, FILE_ATTRIBUTE_NORMAL);
  
  hLogFile = CreateFileA(pszLogFile, GENERIC_WRITE, 0, 0,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);
  
  return hLogFile;
}

// Force to flush all data in 'g_szLogBuf' to disk.
VOID LogFileFlush(HANDLE LogFile)
{
	if (LogFile && g_LogBufIdx > 0)
	{
		DWORD NumBytes;
		WriteFile(LogFile, g_szLogBuf, lstrlenA(g_szLogBuf), &NumBytes, 0);
		g_LogBufIdx = 0;
	}
}

VOID LogFilePrint(HANDLE LogFile, PCSTR Format, ...)
{
  if(LogFile == NULL)
  {
    return ;
  }

	if (g_LogBufIdx > (HPRINTF_BUFFER_SIZE-1024))
	{
		DWORD NumBytes;
		WriteFile(LogFile, g_szLogBuf, lstrlenA(g_szLogBuf), &NumBytes, 0);
		g_LogBufIdx = 0;
	}

	va_list arglist;
	va_start( arglist, Format);
	g_LogBufIdx += wvsprintfA(&g_szLogBuf[g_LogBufIdx], Format, arglist);
	va_end( arglist);
}

VOID LogFileClose(HANDLE hLogFile)
{
  if(hLogFile)
  {
    LogFileFlush(hLogFile);
    CloseHandle(hLogFile);
  }
}

U32 ITS_EnableMemCheck(BOOL bEnable)
{
  U32 ret = 0;
  
  g_EnableMemChk = bEnable; 

  return ret;
}

U32 ExceptionExit()
{
  MemoryLeakCheck(-1);
  
  if(g_hLogFile)
  {
    LogFileClose(g_hLogFile);
  }
  
  g_hLogFile = NULL;
  g_LogBufIdx = 0;
  g_EnableMemChk = FALSE;
  
  return 0;
}

typedef struct block_info
{
    struct block_info *next;
    CHAR*  begin;
    DWORD  dwModuleId;
    size_t size;
    int    ref_cnt;
    CONST CHAR* pszFile;
    DWORD  line;
    DWORD dwAllocThreadId;
    DWORD dwFreeThreadId;
#ifdef _DEBUG
    DWORD dwTimeStamp;
#endif 
}MEM_BLOCK_INFO;

typedef struct  
{
  DWORD dwAllocCnt;
  DWORD dwFreeCnt;
  DWORD dwAllocSize;
} MEM_MODULE_STAT_INFO;

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

#define MCHECK_RETURN_ERR( expr, rv, err_info )  \
  do {\
  if( ( expr) )\
    {\
    OutputDbgStr_2("its_bcore: %s(%d) :%s\n", __FILE__, __LINE__, err_info ? err_info : "NULL" );\
    return (rv);\
    }\
  } while ( 0 )


#define MCHECK_ASSERT_RWV( expr )    \
  do {\
  if( !(expr) )\
    {\
    (0);\
    return;\
    }\
  } while ( 0 )


#define MCHECK_ASSERT_RV( expr, rv )       \
  do {\
  if( !(expr) )\
    {\
    (0);\
    return (rv);\
    }\
  } while ( 0 )

#define FORMAT_STRING "%-36s: %-8d[0x%-8x] ( %-4dM %-4dK %-4dB)"

#define PtrLess( left, right )    ( (left) < (right) )
#define PtrMore( left, right )    ( (left) > (right) )
#define PtrEqual( left, right )   ( (left) == (right) )
#define PtrLessEq( left, right )  ( (left) <= (right) )
#define PtrMoreEq( left, right )  ( (left) >= (right) )

#define SENTRY_GARBAGE    0xCC
#define MALLOC_GARBAGE    0xBB
#define FREE_GARBAGE      0xDD
#define SIZEOF_SENTRY     4
  
#define MAX_MOD_ID        1024 

static MEM_BLOCK_INFO* g_blk_info_lists[MAX_MOD_ID]   = {NULL, };
static MEM_MODULE_STAT_INFO g_ModStatInfo[MAX_MOD_ID] = {0, };

CHAR* lstrrchrA(PCSTR string, int ch)
{
	CHAR *start = (CHAR *)string;

	while (*string++)                       /* find end of string */
		;
											/* search towards front */
	while (--string != start && *string != (CHAR) ch)
		;

	if (*string == (CHAR) ch)                /* char found ? */
		return (CHAR *)string;

	return NULL;
}

VOID MemInit(VOID)
{
  memset(&g_ModStatInfo, 0, sizeof(g_ModStatInfo));
  memset(&g_blk_info_lists, 0, sizeof(g_blk_info_lists));

  CHAR szPath[MAX_PATH] = {0, };
  if (GetModuleFileNameA(NULL, szPath, COUNTOF(szPath)-2) <= 0)
  {
    TRACE_I(_T("*** Fail to GetModuleFileName. Error=%d\n"), GetLastError());
  }
  CHAR *lastperiod = lstrrchrA(szPath, ('\\'));
  if (lastperiod)
  {
    lastperiod[0] = 0;
  }
  lastperiod = lstrrchrA(szPath, ('\\'));
  if (lastperiod)
  {
    lastperiod[0] = 0;
  }
  lstrcatA(szPath, "\\tmp\\MemLog.txt");
    
  g_hLogFile = LogFileOpen(szPath);
  SYSTEMTIME tm;
  GetSystemTime(&tm);
  LogFilePrint(g_hLogFile, ("Memory Leak Log %4d-%2d-%2d_%2d%:2d:%2d Start.\r\n"), tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
}

void get_mega_kilo_byte( int n, int *mega, int *kilo, int *byte )
{
  *mega = *kilo = *byte = 0;
  
  if ( n > 1024 * 1024 )
    *mega = n / (1024 * 1024);
  n -= (*mega) * ( 1024 * 1024 );
  if ( n > 1024 )
    *kilo = n / 1024;
  n -= (*kilo) * 1024;
  *byte = n;
}

/*
Create the block_info structure for the block identified by @p and @size
and insert it into the block_info list.
*/
static BOOL CreateBlkInfo( char* p, U32 mod_id, size_t size, CONST CHAR* file_name, DWORD line)
{
  MEM_BLOCK_INFO *pbi;
    
  pbi = (MEM_BLOCK_INFO*)GlobalAlloc(GMEM_FIXED, sizeof(MEM_BLOCK_INFO) );
  
  pbi->dwModuleId = mod_id;
  pbi->begin      = p;
  pbi->size       = size;
  pbi->ref_cnt    = 1;
  pbi->pszFile    = file_name;
  pbi->line       = line;
  pbi->dwAllocThreadId = ::GetCurrentThreadId();
#ifdef _DEBUG
  pbi->dwTimeStamp = GetTickCount();
#endif 
  pbi->next       = g_blk_info_lists[ mod_id ];
  g_blk_info_lists[mod_id] = pbi;

  g_ModStatInfo[mod_id].dwAllocCnt ++;
  g_ModStatInfo[mod_id].dwAllocSize += size;
  
  return TRUE;
}


/*
Extract  the block_info structure for the block identified by @p 
from the block_info list and free it.

  NOTE: the pointer @p MUST point to the beginning of the block.
*/
static BOOL DeleteBlkInfo(PVOID p, U32 mod_id )
{
  MEM_BLOCK_INFO  **ppbi;
  MEM_BLOCK_INFO  *tmp = NULL;
  DWORD tmdif = 0;
  BOOL ret    = FALSE;

  for ( ppbi = &g_blk_info_lists[ mod_id ]; *ppbi; ppbi = &( ( *ppbi )->next ))
  {
    if ( PtrEqual( ( *ppbi )->begin, p ) )
    {
      tmp = *ppbi;
      *ppbi = tmp->next;
      g_ModStatInfo[mod_id].dwFreeCnt ++;
      if (tmp)
      {
         ::GlobalFree((HGLOBAL)tmp);
         tmp = NULL;
      }
      ret  = TRUE;
      break;
    }
  }
  
  return ret;
}


/*
Find the block pointed to by the pointer @p.
It is NOT necessary for the pointer @p to point to the beginning of the block.
*/
static struct block_info * GetBlkInfo( PVOID p, U32 mod_id )
{
  struct block_info  *pbi;
  
  for ( pbi = g_blk_info_lists[mod_id]; pbi; pbi = pbi->next )
  {
    if ( PtrMoreEq( p, pbi->begin ) && PtrLessEq( p, pbi->begin + pbi->size - 1 ))
    {
      return pbi;
    }
  }
  
  /*
  block_info not found, the block pointed to by @p may be:
  1. Garbage
  2. block freed previously
  */
  MCHECK_ASSERT_RV( FALSE, NULL );
}


/*
Validate:
1. the pointer @p points to a valid memory block 
2. the space from the position pointed to by @p to the end of the block is EXACTLY @size bytes long.
*/
BOOL ValidatePtr( char *p, size_t size, U32 mod_id )
{
  struct block_info *pbi;
  long i;
  
  MCHECK_ASSERT_RV( p, FALSE );
  MCHECK_ASSERT_RV( 0 != size, FALSE );
  
  pbi = GetBlkInfo( p, mod_id );
  
  MCHECK_ASSERT_RV( PtrLessEq( p + size, pbi->begin + pbi->size ), FALSE );
  
  for ( i = 0; i < SIZEOF_SENTRY; i++ )
  {
    if ( (char)SENTRY_GARBAGE != ( pbi->begin + pbi->size )[ i ] )
    {
      TRACE_I(_T("Write overflow for pointer allocated at: %s[%d]"), pbi->pszFile, pbi->line );
      MCHECK_ASSERT_RV( FALSE, FALSE );
    }
  }
  
  for ( i = 0; i < SIZEOF_SENTRY; i++ )
  {
    if ( (char)SENTRY_GARBAGE != ( pbi->begin- SIZEOF_SENTRY )[ i ] )
    {
      TRACE_I(_T("Write overflow for pointer allocated at: %s[%d]"), pbi->pszFile, pbi->line);
      MCHECK_ASSERT_RV( FALSE, FALSE );
    }
  }  
  
  return TRUE;
}

/*
Clear the reference counter for all the block_info in the list to zero.
*/
static void ClearRefCounters( char mod_id )
{
  struct block_info *pbi;
  
  for ( pbi = g_blk_info_lists[ mod_id ]; pbi; pbi = pbi->next )
  {
    pbi->ref_cnt = 0;
  }
}


//
// Increase the reference counter for the block_info associated with the block pointed to.
//
void IncRefCounter( char *p, char mod_id )
{
  struct block_info *pbi;
  
  pbi = GetBlkInfo( p, mod_id );
  pbi->ref_cnt++;
}

U32 MemoryLeakCheck_i(U32 nModuleId, U32& TotalMem)
{
  U32 ret = 0;
  struct block_info *pbi;
  int total_mem = 0;
  int total_mem_for_check = 0;
  int mega = 0, kilo = 0, byte = 0;
  
  if(nModuleId >= MAX_MOD_ID)
  {
    TRACE_I(_T("Module ID(%d) is too large, should be be less than %d\n"), nModuleId,MAX_MOD_ID);
    return ERR_INVALID_PARAM_I;
  }
  
  g_MemCs.Enter();
  
  // ClearRefCounters( mod_id );
  if(g_blk_info_lists[nModuleId] == NULL)
  {
    return ret;
  }
  else if(g_blk_info_lists[nModuleId]->ref_cnt == 0)
  {
    TRACE_I("\nModule [%d] Lost the reference.\n", nModuleId);
    return ret;
  }

  TRACE_I("\nModule [%d] Memory Leak Check....\n", nModuleId);

  for ( pbi = g_blk_info_lists[nModuleId]; pbi; pbi = pbi->next )
  {
    if ( 0 == pbi->ref_cnt )
    {
      TRACE_I("memory block allocated at %s [%d] has lost or ...", pbi->pszFile, pbi->line );
      TRACE_I("...the pointers which point to the block has not been visited when memory check\n");
    }
    else
    {
      // Too many references for the block are found 
      ValidatePtr( pbi->begin, pbi->size, nModuleId );
    }
    
    total_mem += pbi->size /* + 2 * SIZEOF_SENTRY */;
    total_mem_for_check += sizeof( *pbi );
    
    LogFilePrint(g_hLogFile, ("%s(%d) : [%X] ModuleId=%d, Add=%d, size=%d\r\n"), 
      pbi->pszFile, pbi->line, pbi->dwAllocThreadId,
      nModuleId, pbi->begin, pbi->size
      );
    
    TRACE_I( ("%s(%d) : [%X] ModuleId=%d, Add=%d, size=%d\r\n"),
      pbi->pszFile, pbi->line,pbi->dwAllocThreadId,
      nModuleId, pbi->begin, pbi->size);
  }

  if(total_mem)
  {
    TotalMem += total_mem;
    TRACE_I("*** Module [%d] Memory Leak!\r\n", nModuleId);
  }
  LogFilePrint(g_hLogFile, ("Memory Leak=%d Bytes, ModuleId=%d.\r\n"), total_mem, nModuleId);
  
  TRACE_I(("Memory Leak=%d Bytes, ModuleId=%d.\r\n"), total_mem, nModuleId);
  
  LogFilePrint(g_hLogFile, ("Memory allocated/release=%d/%d, Size=%d, ModuleId=%d.\r\n\r\n"), 
    g_ModStatInfo[nModuleId].dwAllocCnt, g_ModStatInfo[nModuleId].dwFreeCnt, 
    g_ModStatInfo[nModuleId].dwAllocSize, nModuleId);
  
  TRACE_I(("Memory allocated/release=%d/%d, Size=%d, ModuleId=%d.\r\n"), 
    g_ModStatInfo[nModuleId].dwAllocCnt, g_ModStatInfo[nModuleId].dwFreeCnt, 
    g_ModStatInfo[nModuleId].dwAllocSize, nModuleId);
  
  TRACE_I("Module [%d] Memory Leak End.\r\n\r\n", nModuleId);
  
#if 0 
  get_mega_kilo_byte( total_mem, &mega, &kilo, &byte );
  TRACE_I(FORMAT_STRING"\r\n", "Memory allocated",
    total_mem, total_mem, mega, kilo, byte );
  
  printf("\r\n\r\n" );
  
  get_mega_kilo_byte( total_mem_for_check, &mega, &kilo, &byte );
  printf(FORMAT_STRING"\r\n", "Memory for check allocated",
    total_mem_for_check, total_mem_for_check, mega, kilo, byte );
  
  get_mega_kilo_byte( total_mem + total_mem_for_check, &mega, &kilo, &byte );
  printf(FORMAT_STRING"\r\n", "Total memory allocated",
    total_mem + total_mem_for_check, total_mem + total_mem_for_check, mega, kilo, byte );
  printf("\r\n\r\n" );  
#endif
  
  LogFileFlush(g_hLogFile);

  g_MemCs.Leave();
  
  return ret;
}

U32 MemoryLeakCheck(U32 nModuleId)
{
  U32 ret = 0;
  U32 TotalMem = 0;

  if((nModuleId != -1) && (nModuleId >= MAX_MOD_ID))
  {
    TRACE_I(_T("Module ID(%d) is too large, should be be less than %d\n"), nModuleId,MAX_MOD_ID);
    return ERR_INVALID_PARAM_I;
  }

  if(nModuleId == -1)
  {
    SYSTEMTIME tm;
    GetLocalTime(&tm);

    LogFilePrint(g_hLogFile, "Memory Leak Check at %04d-%02d-%02d %2d:%2d:%2d. \r\n", tm.wYear, tm.wMonth, tm.wDay,
      tm.wHour, tm.wMinute, tm.wSecond);

    for (int i = 0; i< MAX_MOD_ID; i++)
    {
      ret = MemoryLeakCheck_i(i, TotalMem);
    }
    
    LogFilePrint(g_hLogFile, "Total Memory Leak:%d Bytes.\r\n\r\n", TotalMem);
  }
  else
  {
    return MemoryLeakCheck_i(nModuleId, TotalMem);
  }
  
  return ret;
}

// The nModuleId should be less than 512. 
PVOID AllocateMemoryDbg_i(U32 nSize, U32 nModuleId, CONST CHAR* lpszFileName, U32 nLine )
{
  BOOL bRet = FALSE;
  CHAR* p   = NULL;
  CHAR* q   = NULL;
  
  if(nModuleId > MAX_MOD_ID)
  {
    TRACE_I(_T("Module ID(%d) is too large, should be be less than %d\n"), nModuleId, MAX_MOD_ID);
    return NULL;
  }
  
  MCHECK_ASSERT_RV( 0 != nSize, NULL );  
  
  q = (CHAR*)AllocateMemory( nSize + 2 * SIZEOF_SENTRY );  
  
  if(q)
  {
    p = q + SIZEOF_SENTRY;
    memset( q, SENTRY_GARBAGE, SIZEOF_SENTRY );
    memset( p, MALLOC_GARBAGE, nSize );
    memset( p + nSize, SENTRY_GARBAGE, SIZEOF_SENTRY );
    
    g_MemCs.Enter();
    bRet = CreateBlkInfo( p, nModuleId, nSize, lpszFileName, nLine);
    g_MemCs.Leave();
    
    if ( !bRet )
    {
      FreeMemory(q, FALSE);
      p = NULL;
    }
  }
  else
  {
    TRACE_I(("*** Fail to Allocate Memory err=%d\r\n"), GetLastError());
  }  

  return p;
}

U32 FreeMemoryDbg_i(PVOID pMemory, U32 nModuleId, CONST CHAR* lpszFileName, U32 nLine, BOOL bCheck)
{
  U32 ret     = 0;
  struct block_info *pbi = NULL;
  long size = 0;
  CHAR* p = (CHAR*)pMemory;
  
  if(nModuleId >= MAX_MOD_ID)
  {
    TRACE_I(_T("Module ID(%d) is too large, should be be less than %d\n"), nModuleId,MAX_MOD_ID);
    return ERR_INVALID_PARAM_I;
  }
  
  if( pMemory == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  g_MemCs.Enter();
  pbi = GetBlkInfo( pMemory, nModuleId );
  g_MemCs.Leave();
  
  if(pbi == NULL)
  {
    return ERR_INVALID_MODULE_I;
  }

  if( !PtrEqual( p, pbi->begin ) )
  {
    return ERR_INVALID_MEMORY_ADDR_I;
  }
  
  size = pbi->size;
  
  g_MemCs.Enter();
  if(DeleteBlkInfo( p, nModuleId ))
  {
    g_MemCs.Leave();

    memset( p, FREE_GARBAGE, size );
    
    ret = FreeMemory( (CHAR*)p - SIZEOF_SENTRY, bCheck);
  }
  else
  {
    g_MemCs.Leave();
    ret = ERR_INVALID_MEMORY_ADDR_I;
  }

  return ret;
}

#if 0 
// Take over global new and delete
void* operator new(size_t s)
{
  return AllocateMemory(s);
}

void* operator new[](size_t s)
{
  return AllocateMemory(s);
}

void operator delete(void* pMem)
{
  FreeMemory(pMem);
}

void operator delete[] (void* pMem)
{
  FreeMemory(pMem);
}

// And then some crap for taking over MFC allocations.
void* __cdecl operator new(size_t s, DWORD dwModuleId, DWORD, PCSTR pszFileName, int nLine)
{
  return AllocateMemoryDbg_i(s, dwModuleId, pszFileName, nLine);
}

void* __cdecl operator new[](size_t s, DWORD dwModuleId, DWORD, PCSTR pszFileName, int nLine)
{
  return AllocateMemoryDbg_i(s, dwModuleId, pszFileName, nLine);
}

void __cdecl operator delete(void* pMem, DWORD dwModuleId, DWORD, PCSTR pszFileName, int nLine)
{
  FreeMemoryDbg_i(pMem, dwModuleId, pszFileName, nLine);
}

void __cdecl operator delete[](void* pMem,  DWORD dwModuleId, DWORD, PCSTR pszFileName, int nLine)
{
  FreeMemoryDbg_i(pMem, dwModuleId, pszFileName, nLine);
}
#endif 

#if 0 
typedef enum eAllocCheckOutput
{
  ACOutput_Simple,
  ACOutput_Advanced,
  ACOutput_XML
};

// If the following is defined, only the used memories are stored in the hash-table. 
// If the memory is freed, it will be removed from the hash-table (to reduce memory)
// Consequences: At DeInitAllocHook, only Leaks will be reported
#define HASH_ENTRY_REMOVE_AT_FREE


// 0 = Do not write any output during runtime-alloc-call
// 1 = Write only the alloc action (malloc, realloc, free)
// 2 = Write alloc action and callstack only for malloc/realloc
// 3 = Write alloc action and callstack for all actions
static ULONG g_ulShowStackAtAlloc = 0;

// the form of the output file
static eAllocCheckOutput g_CallstackOutputType = ACOutput_Simple;

/*******************************************************************************
 * Hash-Tabelle
 *******************************************************************************/
// Size of Hash-Table (this should be a prime number to avoid collisions)
#define ALLOC_HASH_ENTRIES 1023


// Size of Callstack-trace in bytes (0x500 => appr. 5-9 functions, depending on parameter count for each function)
#define MAX_ESP_LEN_BUF 0x500

// Memory for the EIP-Address (is used by the ShowStack-method)
#define MAX_EIP_LEN_BUF 4

#define ALLOC_ENTRY_NOT_FOUND 0xFFFFFFFF

typedef struct AllocHashEntryType {
  long                       lRequestID;    // RequestID from CRT (if 0, then this entry is empty)
  size_t                     nDataSize;     // Size of the allocated memory
  char                       cRemovedFlag;  // 0 => memory was not yet released
  struct AllocHashEntryType  *Next;
  // Callstack for EIP
  DWORD                      dwEIPOffset;
  DWORD                      dwEIPLen;
  char                       pcEIPAddr[MAX_EIP_LEN_BUF];
  // Callstack for ESP
  DWORD                      dwESPOffset;
  DWORD                      dwESPLen;
  char                       pcESPAddr[MAX_ESP_LEN_BUF];
} AllocHashEntryType;

static AllocHashEntryType AllocHashTable[ALLOC_HASH_ENTRIES];
static ULONG AllocHashEntries = 0;
static ULONG AllocHashCollisions = 0;
static ULONG AllocHashFreed = 0;
static ULONG AllocHashMaxUsed = 0; // maximal number of concurrent entries
static ULONG AllocHashCurrentCount = 0;

static ULONG AllocHashMaxCollisions = 0;
static ULONG AllocHashCurrentCollisions = 0;

// Globale Vars:
static TCHAR *g_pszAllocLogName = NULL;
static FILE *g_fFile = NULL;

// AllocCheckFileOpen
//  Checks if the log-file is already opened
//  if not, try to open file (append or create if not exists)
//  if open failed, redirect output to stdout
static void AllocCheckFileOpen(bool bAppend = true)
{
  // is the File already open? If not open it...
  if (g_fFile == NULL)
    if (g_pszAllocLogName != NULL)
    {
      if (bAppend == false)
        g_fFile = _tfopen(g_pszAllocLogName, _T("w"));
      else
        g_fFile = _tfopen(g_pszAllocLogName, _T("a"));
    }
  if (g_fFile == NULL)
    g_fFile = stdout;
}

// Write Date/Time to specified file (will also work after 2038)
static void WriteDateTime(FILE *fFile, BOOL asXMLAttrs = FALSE)
{
  TCHAR pszTemp[11], pszTemp2[11];

  if (fFile != NULL) 
  {
    _tstrdate( pszTemp );
    _tstrtime( pszTemp2 );
    if (asXMLAttrs == FALSE)
      _ftprintf(fFile,  _T("%s %s"), pszTemp, pszTemp2 );  // also ok after year 2038 (asctime is NOT ok)
    else
      _ftprintf(fFile,  _T("date=\"%s\" time=\"%s\" "), pszTemp, pszTemp2 );  // also ok after year 2038 (asctime is NOT ok)
  }
}  // WriteDateTime

static void AllocHashInit(void)
{
  memset(AllocHashTable, 0, sizeof(AllocHashTable));
  AllocHashEntries = 0;
  AllocHashCollisions = 0;
  AllocHashFreed = 0;
  AllocHashCurrentCount = 0;
  AllocHashMaxUsed = 0;

  AllocHashMaxCollisions = 0;
  AllocHashCurrentCollisions = 0;

  return;
}

// AllocHashDeinit
// Returns the number of bytes, that are not freed (leaks)
static ULONG AllocHashDeinit(void)
{
  ULONG ulRet = 0;
  bool bAppend = g_CallstackOutputType != ACOutput_XML;
  AllocCheckFileOpen(bAppend);  // open global log-file

  if (g_CallstackOutputType == ACOutput_XML)
  {
    _ftprintf(g_fFile, _T("<MEMREPORT "));
    WriteDateTime(g_fFile, TRUE);
    _ftprintf(g_fFile, _T(">\n"));
  }
  else
  {
    _ftprintf(g_fFile, _T("\n##### Memory Report ########################################\n"));
    WriteDateTime(g_fFile);
    _ftprintf(g_fFile, _T("\n"));
  }

#ifndef HASH_ENTRY_REMOVE_AT_FREE
  // output the used memory
  if (g_CallstackOutputType != ACOutput_XML)
    _ftprintf(g_fFile, _T("##### Memory used: #########################################\n"));
  AllocHashOut(g_fFile);
#endif

  // output the Memoty leaks
  if (g_CallstackOutputType != ACOutput_XML)
    _ftprintf(g_fFile, _T("\n##### Leaks: ###############################################\n"));
  ulRet = AllocHashOutLeaks(g_fFile);

  if (g_CallstackOutputType == ACOutput_Advanced)
  {
    // output some statistics from the hash-table
    _ftprintf(g_fFile, _T("***** Hash-Table statistics:\n"));
    _ftprintf(g_fFile, _T("      Table-Size:     %i\n"), ALLOC_HASH_ENTRIES);
    _ftprintf(g_fFile, _T("      Inserts:        %i\n"), AllocHashEntries);
    _ftprintf(g_fFile, _T("      Freed:          %i\n"), AllocHashFreed);
    _ftprintf(g_fFile, _T("      Sum Collisions: %i\n"), AllocHashCollisions);
    _ftprintf(g_fFile, _T("\n"));
    _ftprintf(g_fFile, _T("      Max used:       %i\n"), AllocHashMaxUsed);
    _ftprintf(g_fFile, _T("      Max Collisions: %i\n"), AllocHashMaxCollisions);
  }

  // Free Hash-Table
  ULONG ulTemp;
  AllocHashEntryType *pHashEntry, *pHashEntryOld;

  // Now, free my own memory
  for(ulTemp = 0; ulTemp < ALLOC_HASH_ENTRIES; ulTemp++) 
  {
    pHashEntry = &AllocHashTable[ulTemp];
    while(pHashEntry != NULL) 
    {
      pHashEntryOld = pHashEntry;
      pHashEntry = pHashEntry->Next;
      if (pHashEntryOld != &AllocHashTable[ulTemp]) 
      {
        // now free the dynamically allocated memory
        free(pHashEntryOld);
      }
    }  // while
  }  // for
  // empty the hash-table
  memset(AllocHashTable, 0, sizeof(AllocHashTable));

  if (g_CallstackOutputType == ACOutput_XML)
    _ftprintf(g_fFile, _T("</MEMREPORT>\n"));

  return ulRet;
}  // AllocHashDeinit

// AllocHashFunction
// The has-function (very simple)
static inline ULONG AllocHashFunction(long lRequestID) 
{
  return lRequestID % ALLOC_HASH_ENTRIES;
}  // AllocHashFunction


// AllocHashInsert
//   lRequestID: Key-Word (RequestID from AllocHook)
//   pContext:   Context-Record, for retrieving Callstack (EIP and EBP is only needed)
//   nDataSize:  How many bytes
static void AllocHashInsert(long lRequestID, CONTEXT &Context, size_t nDataSize)
 {
  ULONG HashIdx;
  AllocHashEntryType *pHashEntry;

  // change statistical data
  AllocHashEntries++;
  AllocHashCurrentCount++;
  if (AllocHashCurrentCount > AllocHashMaxUsed)
    AllocHashMaxUsed = AllocHashCurrentCount;

  // generate hash-value
  HashIdx = AllocHashFunction(lRequestID);

  pHashEntry = &AllocHashTable[HashIdx];
  if (pHashEntry->lRequestID == 0) 
  {
    // Entry is empty...
  }
  else 
  {
    // Entry is not empy! make a list of entries for this hash value...
    // change statistical data
    // if this happens often, you should increase the hah size or change the heash-function; 
    // to fasten the allocation time
    AllocHashCollisions++;
    AllocHashCurrentCollisions++;
    if (AllocHashCurrentCollisions > AllocHashMaxCollisions)
      AllocHashMaxCollisions = AllocHashCurrentCollisions;

    while(pHashEntry->Next != NULL) 
    {
      pHashEntry = pHashEntry->Next;
    }

    pHashEntry->Next = (AllocHashEntryType*) calloc(sizeof(AllocHashEntryType), 1);
    pHashEntry = pHashEntry->Next;
  }

  pHashEntry->lRequestID = lRequestID;  // Key-Word
  pHashEntry->nDataSize = nDataSize;
  pHashEntry->Next = NULL;

  // Get EIP and save it in the record
  pHashEntry->dwEIPOffset = Context.Eip;

  if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID) Context.Eip, 
    &(pHashEntry->pcEIPAddr), MAX_EIP_LEN_BUF, &(pHashEntry->dwEIPLen)) == 0) 
  {
    // Could not read memory... remove everything...
    memset(pHashEntry->pcEIPAddr, 0, MAX_EIP_LEN_BUF);
    pHashEntry->dwEIPLen = 0;
    pHashEntry->dwEIPOffset = 0;
  }

  // Get ESP and save it in the record
  pHashEntry->dwESPOffset = Context.Ebp;
  if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID) Context.Ebp, &(pHashEntry->pcESPAddr),
    MAX_ESP_LEN_BUF, &(pHashEntry->dwESPLen)) == 0) 
  {
    // Could not read memory... remove everything...
    memset(pHashEntry->pcESPAddr, 0, MAX_ESP_LEN_BUF);
    pHashEntry->dwESPLen = 0;
    pHashEntry->dwESPOffset = 0;

    // Check if I tried to read too much...
    if (GetLastError() == ERROR_PARTIAL_COPY)
    {
      // ask how many I can read:
      MEMORY_BASIC_INFORMATION MemBuffer;
      DWORD dwRet = VirtualQuery((LPCVOID) Context.Ebp, &MemBuffer, sizeof(MemBuffer));
      if (dwRet > 0)
      {
        // calculate the length
        DWORD len = ((DWORD) MemBuffer.BaseAddress + MemBuffer.RegionSize) - Context.Ebp;
        if ( (len > 0) && (len < MAX_ESP_LEN_BUF) )
        {
          // try to read it again (with the shorter length)
          if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID) Context.Ebp, 
            &(pHashEntry->pcESPAddr), len, &(pHashEntry->dwESPLen)) == 0)
          {
            // ok, now everything goes wrong... remove it...
            memset(pHashEntry->pcESPAddr, 0, MAX_ESP_LEN_BUF);
            pHashEntry->dwESPLen = 0;
            pHashEntry->dwESPOffset = 0;
          }
          else
          {
            pHashEntry->dwESPOffset = Context.Ebp;
          }
        }
      } // VirtualQuery was successfully
    }  // ERROR_PARTIAL_COPY
  }
}

// AllocHashFind
//   If ALLOC_ENTRY_NOT_FOUND is returned, the Key was not found!
//   If the Key was found, a pointer to the entry is returned
static AllocHashEntryType *AllocHashFind(long lRequestID) 
{
  ULONG HashIdx;
  AllocHashEntryType *pHashEntry;

  // get the Hash-Value
  HashIdx = AllocHashFunction(lRequestID);

  // Just do some simple checks:
  _ASSERTE(HashIdx < ALLOC_HASH_ENTRIES);

  pHashEntry = &AllocHashTable[HashIdx];
  while(pHashEntry != NULL)
  {
    if (pHashEntry->lRequestID == lRequestID) 
    {
      return pHashEntry;
    }
    pHashEntry = pHashEntry->Next;
  }

  // entry was not found!
  return (AllocHashEntryType*) ALLOC_ENTRY_NOT_FOUND;
}  // AllocHashFind


// AllocHashRemove
//   Return: FALSE (0) : Key was found and removed/marked
//           TRUE (!=0): Key was not found
static BOOL AllocHashRemove(long lRequestID) 
{
  ULONG HashIdx;
  AllocHashEntryType *pHashEntry, *pHashEntryLast;

  // get the Hash-Value
  HashIdx = AllocHashFunction(lRequestID);

  // Just do some simple checks:
  _ASSERTE(HashIdx < ALLOC_HASH_ENTRIES);

  pHashEntryLast = NULL;
  pHashEntry = &AllocHashTable[HashIdx];
  while(pHashEntry != NULL) 
  {
    if (pHashEntry->lRequestID == lRequestID) 
    {
#ifdef HASH_ENTRY_REMOVE_AT_FREE
      AllocHashFreed++;
      AllocHashCurrentCount--;

      // release my memory
      if (pHashEntryLast == NULL)
      {
        // It is an entry in the table, so do not release this memory
        if (pHashEntry->Next == NULL) 
        {
          // It was the last entry, so empty the table entry
          memset(&AllocHashTable[HashIdx], 0, sizeof(AllocHashTable[HashIdx]));
        }
        else
        {
          // There are some more entries, so shorten the list
          AllocHashEntryType *pTmp = pHashEntry->Next;
          *pHashEntry = *(pHashEntry->Next);
          free(pTmp);
        }
        return TRUE;
      }
      else 
      {
        // now, I am in an dynamic allocated entry
        // it was a collision, so decrease the current collision count
        AllocHashCurrentCollisions--;
        pHashEntryLast->Next = pHashEntry->Next;
        free(pHashEntry);
        return TRUE;
      }
#else
      // increase the Remove-Count and let the objet stay in memory
      pHashEntry->cRemovedFlag++;
      return TRUE;
#endif
    }
    pHashEntryLast = pHashEntry;
    pHashEntry = pHashEntry->Next;
  }

  // if we are here, we could not find the RequestID
  return FALSE;
}

// ReadProcMemoryFromHash
//   Callback-Funtion for StackWalk for my own CallStack from the Hash-Table-Entries
static BOOL __stdcall ReadProcMemoryFromHash(HANDLE hRequestID, 
                                             DWORD64 lpBaseAddress, PVOID lpBuffer, 
                                             DWORD nSize, PDWORD lpNumberOfBytesRead) 
{
  // Try to find the RequestID
  AllocHashEntryType *pHashEntry;
  *lpNumberOfBytesRead = 0;

  pHashEntry = AllocHashFind((LONG) hRequestID);
  if (pHashEntry == (AllocHashEntryType*) ALLOC_ENTRY_NOT_FOUND) 
  {
    // Not found, so I cannot return any memory
    *lpNumberOfBytesRead = 0;
    return FALSE;
  }
  if ( ((DWORD) lpBaseAddress >= pHashEntry->dwESPOffset) && 
    ((DWORD) lpBaseAddress <= (pHashEntry->dwESPOffset+pHashEntry->dwESPLen)) ) 
  {
    // Memory is located in ESP:
    // Calculate the offset
    DWORD dwOffset = (DWORD) lpBaseAddress - pHashEntry->dwESPOffset;
    DWORD dwSize = __min(nSize, MAX_ESP_LEN_BUF-dwOffset);
    memcpy(lpBuffer, &(pHashEntry->pcESPAddr[dwOffset]), dwSize);
    *lpNumberOfBytesRead = dwSize;
    if (dwSize != nSize)
      return FALSE;
  }

  if ( ((DWORD) lpBaseAddress >= pHashEntry->dwEIPOffset) && 
    ((DWORD) lpBaseAddress <= (pHashEntry->dwEIPOffset+pHashEntry->dwEIPLen)) )
  {
    // Memory is located in EIP:
    // Calculate the offset
    DWORD dwOffset = (DWORD) lpBaseAddress - pHashEntry->dwEIPOffset;
    DWORD dwSize = __min(nSize, MAX_ESP_LEN_BUF-dwOffset);
    memcpy(lpBuffer, &(pHashEntry->pcEIPAddr[dwOffset]), dwSize);
    *lpNumberOfBytesRead = dwSize;
    if (dwSize != nSize)
      return FALSE;
  }
  
  if (*lpNumberOfBytesRead == 0)  // Memory could not be found
    return FALSE;

  return TRUE;
}

// AllocHashOutLeaks
// Write all Memory (with callstack) which was not freed yet
//   Returns the number of bytes, that are not freed (leaks)
ULONG AllocHashOutLeaks(FILE *fFile)
{
  ULONG ulTemp;
  AllocHashEntryType *pHashEntry;
  ULONG ulCount = 0;
  ULONG ulLeaksByte = 0;

  // Move throu every entry
  for(ulTemp = 0; ulTemp < ALLOC_HASH_ENTRIES; ulTemp++) 
  {
    pHashEntry = &AllocHashTable[ulTemp];
    if (pHashEntry->lRequestID != 0) 
    {
      while(pHashEntry != NULL) 
      {
        if ( (pHashEntry->cRemovedFlag <= 0) || (pHashEntry->cRemovedFlag > 1) ) 
        {
          ulCount++;
          if (g_CallstackOutputType == ACOutput_XML)
            _ftprintf(fFile, _T("<LEAK requestID=\"%u\" size=\"%u\">\n"), 
            pHashEntry->lRequestID, pHashEntry->nDataSize);
          else
            _ftprintf(fFile, _T("RequestID: %12i, Removed: %i, Size: %12i\n"), 
            pHashEntry->lRequestID, pHashEntry->cRemovedFlag, pHashEntry->nDataSize);

          CONTEXT c;
          memset( &c, '\0', sizeof c );
          c.Eip = pHashEntry->dwEIPOffset;
          c.Ebp = pHashEntry->dwESPOffset;
          ShowStackRM( NULL, c, fFile, &ReadProcMemoryFromHash, (HANDLE) pHashEntry->lRequestID);
          // Count the number of leaky bytes
          if (pHashEntry->nDataSize > 0)
            ulLeaksByte += pHashEntry->nDataSize;
          else
            ulLeaksByte++;  // If memory was allocated with zero bytes, then just increase the counter 1
          
          if (g_CallstackOutputType == ACOutput_XML)
            _ftprintf(fFile, _T("</LEAK>\n"));  // terminate the xml-node
        }

        pHashEntry = pHashEntry->Next;
      }
    }
  }

  if (g_CallstackOutputType != ACOutput_XML)
    _ftprintf(fFile, _T("\n**** Number of leaks: %i\n"), ulCount);

  return ulLeaksByte;
}  // AllocHashOutLeaks


// Write all used memory to a file
void AllocHashOut(FILE *fFile)
{
  ULONG ulTemp;
  AllocHashEntryType *pHashEntry;
  
  for(ulTemp = 0; ulTemp < ALLOC_HASH_ENTRIES; ulTemp++)
  {
    pHashEntry = &AllocHashTable[ulTemp];
    if (pHashEntry->lRequestID != 0) 
    {
      while(pHashEntry != NULL) 
      {
        if (g_CallstackOutputType == ACOutput_XML)
          _ftprintf(fFile, _T("<MEMUSED requestID=\"%u\" size=\"%u\"\n"), 
          pHashEntry->lRequestID, pHashEntry->nDataSize);
        else
          _ftprintf(fFile, _T("RequestID: %12i, Removed: %i, Size: %12i\n"), 
          pHashEntry->lRequestID, pHashEntry->cRemovedFlag, pHashEntry->nDataSize);
        pHashEntry = pHashEntry->Next;
      }
    }
  }
}  // AllocHashOut 

/*******************************************************************************
 * Ende der Hash-Tabelle
 *******************************************************************************/

#if 0 
void ShowStack( HANDLE hThread, CONTEXT& c, LPCTSTR pszLogFile)
{
  FILE *fFile = stdout;  // default to stdout

  if (pszLogFile != NULL) {  // a filename is available
    // Open the logfile
    fFile = _tfopen(pszLogFile, _T("a"));
    if (fFile != NULL) {  // Is the file too big?
      long size;
      fseek(fFile, 0, SEEK_END);
      size = ftell(fFile);  // Get the size of the file
      if (size >= LOG_FILE_MAX_SIZE) 
      {
        TCHAR *pszTemp = (TCHAR*) malloc(MAX_PATH);
        // It is too big...
        fclose(fFile);
        _tcscpy(pszTemp, pszLogFile);
        _tcscat(pszTemp, _T(".old"));
        _tremove(pszTemp);  // Remove an old file, if exists
        _trename(pszLogFile, pszTemp);  // rename the actual file
        fFile = _tfopen(pszLogFile, _T("w"));  // open new file
        free(pszTemp);
      }
    }
  }  // if (pszLogFile != NULL) 
  if (fFile == NULL) {
    fFile = stdout;
  }

  ShowStack( hThread, c, fFile);

  fclose(fFile);
}


static void ShowStack( HANDLE hThread, CONTEXT& c, FILE *fLogFile)
{
  ShowStackRM(hThread, c, fLogFile, NULL, GetCurrentProcess());
}

static void ShowStackRM( HANDLE hThread, CONTEXT& c, FILE *fLogFile, 
                        PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryFunction, 
                        HANDLE hSWProcess) 
{
  // normally, call ImageNtHeader() and use machine info from PE header
  // but we assume that it is an I386 image...
  DWORD imageType = IMAGE_FILE_MACHINE_I386;
  HANDLE hProcess = GetCurrentProcess(); // hProcess normally comes from outside but we only do the stackdump in our own process
  int frameNum; // counts walked frames
  DWORD64 offsetFromSymbol; // tells us how far from the symbol we were
  DWORD offsetFromLine; // tells us how far from the line we were
  DWORD symOptions; // symbol handler settings
  
  static IMAGEHLP_SYMBOL64 *pSym = NULL;
  char undName[MAXNAMELEN]; // undecorated name
  char undFullName[MAXNAMELEN]; // undecorated name with all shenanigans
  IMAGEHLP_MODULE64 Module;
  IMAGEHLP_LINE64 Line;
  BOOL bXMLTagWrote;
  
  std::string symSearchPath;
  
  static bFirstTime = TRUE;
  
  // If no logfile is present, outpur to "stdout"
  if (fLogFile == NULL) 
  {
    fLogFile = stdout;
  }
  
  STACKFRAME64 s; // in/out stackframe
  memset( &s, '\0', sizeof s );
  
  if ( (g_bInitialized == FALSE) && (bFirstTime == TRUE) ) 
  {
    InitStackWalk();
  }
  
  if (g_bInitialized == FALSE)
  {
    // Could not init!!!!
    bFirstTime = FALSE;
    _ftprintf(fLogFile, _T("%lu: Stackwalker not initialized (or was not able to initialize)!\n"), g_dwShowCount);
    return;
  }
  
  // Critical section begin...
  EnterCriticalSection(&g_csFileOpenClose);
  
  InterlockedIncrement((long*) &g_dwShowCount);  // erhöhe counter
  
  
  // NOTE: normally, the exe directory and the current directory should be taken
  // from the target process. The current dir would be gotten through injection
  // of a remote thread; the exe fir through either ToolHelp32 or PSAPI.
  
  if (pSym == NULL) {
    pSym = (IMAGEHLP_SYMBOL64 *) malloc( IMGSYMLEN + MAXNAMELEN );
    if (!pSym) goto cleanup;  // not enough memory...
  }
  
  if (g_CallstackOutputType != ACOutput_XML)
  {
    _ftprintf(fLogFile, _T("%lu: "), g_dwShowCount);
    WriteDateTime(fLogFile);
    _ftprintf(fLogFile, _T("\n"));
  }
  
  
  if (bFirstTime) 
  {
    
    CHAR *tt, *p;
    
    tt = (CHAR*) malloc(sizeof(CHAR) * TTBUFLEN); // Get the temporary buffer
    if (!tt) goto cleanup;  // not enough memory...
    
    // build symbol search path from:
    symSearchPath = "";
    // current directory
    if ( GetCurrentDirectoryA( TTBUFLEN, tt ) )
      symSearchPath += tt + std::string( ";" );
    // dir with executable
    if ( GetModuleFileNameA( 0, tt, TTBUFLEN ) )
    {
      for ( p = tt + strlen( tt ) - 1; p >= tt; -- p )
      {
        // locate the rightmost path separator
        if ( *p == '\\' || *p == '/' || *p == ':' )
          break;
      }
      // if we found one, p is pointing at it; if not, tt only contains
      // an exe name (no path), and p points before its first byte
      if ( p != tt ) // path sep found?
      {
        if ( *p == ':' ) // we leave colons in place
          ++ p;
        *p = '\0'; // eliminate the exe name and last path sep
        symSearchPath += tt + std::string( ";" );
      }
    }
    // environment variable _NT_SYMBOL_PATH
    if ( GetEnvironmentVariableA( "_NT_SYMBOL_PATH", tt, TTBUFLEN ) )
      symSearchPath += tt + std::string( ";" );
    // environment variable _NT_ALTERNATE_SYMBOL_PATH
    if ( GetEnvironmentVariableA( "_NT_ALTERNATE_SYMBOL_PATH", tt, TTBUFLEN ) )
      symSearchPath += tt + std::string( ";" );
    // environment variable SYSTEMROOT
    if ( GetEnvironmentVariableA( "SYSTEMROOT", tt, TTBUFLEN ) )
      symSearchPath += tt + std::string( ";" );    
    
    if ( symSearchPath.size() > 0 ) // if we added anything, we have a trailing semicolon
      symSearchPath = symSearchPath.substr( 0, symSearchPath.size() - 1 );
    
    // why oh why does SymInitialize() want a writeable string?
    strncpy( tt, symSearchPath.c_str(), TTBUFLEN );
    tt[TTBUFLEN - 1] = '\0'; // if strncpy() overruns, it doesn't add the null terminator
    
    // init symbol handler stuff (SymInitialize())
    if ( ! pSI( hProcess, tt, false ) )
    {
      if (g_CallstackOutputType != ACOutput_XML)
        _ftprintf(fLogFile, _T("%lu: SymInitialize(): GetLastError = %lu\n"), g_dwShowCount, gle );
      if (tt) free( tt );
      goto cleanup;
    }
    
    // SymGetOptions()
    symOptions = pSGO();
    symOptions |= SYMOPT_LOAD_LINES;
    symOptions &= ~SYMOPT_UNDNAME;
    symOptions &= ~SYMOPT_DEFERRED_LOADS;
    pSSO( symOptions ); // SymSetOptions()
    
    // Enumerate modules and tell dbghlp.dll about them.
    // On NT, this is not necessary, but it won't hurt.
    EnumAndLoadModuleSymbols( hProcess, GetCurrentProcessId(), fLogFile );
    
    if (tt) 
      free( tt );
  }  // bFirstTime = TRUE
  bFirstTime = FALSE;
  
  // init STACKFRAME for first call
  // Notes: AddrModeFlat is just an assumption. I hate VDM debugging.
  // Notes: will have to be #ifdef-ed for Alphas; MIPSes are dead anyway,
  // and good riddance.
  s.AddrPC.Offset = c.Eip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Ebp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.Ebp;
  s.AddrStack.Mode = AddrModeFlat;
  
  memset( pSym, '\0', IMGSYMLEN + MAXNAMELEN );
  pSym->SizeOfStruct = IMGSYMLEN;
  pSym->MaxNameLength = MAXNAMELEN;
  
  memset( &Line, '\0', sizeof Line );
  Line.SizeOfStruct = sizeof Line;
  
  memset( &Module, '\0', sizeof Module );
  Module.SizeOfStruct = sizeof Module;
  
  for ( frameNum = 0; ; ++ frameNum )
  {
    // get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
    // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
    // assume that either you are done, or that the stack is so hosed that the next
    // deeper frame could not be found.
    // CONTEXT need not to be suplied if imageTyp is IMAGE_FILE_MACHINE_I386!
    if ( ! pSW( imageType, hSWProcess, hThread, &s, NULL, ReadMemoryFunction, pSFTA, pSGMB, NULL ) )
      break;
    
    bXMLTagWrote = FALSE;
    
    if (g_CallstackOutputType == ACOutput_Advanced)
      _ftprintf(fLogFile, _T("\n%lu: %3d"), g_dwShowCount, frameNum);
    if ( s.AddrPC.Offset == 0 )
    {
      // Special case: If we are here, we have no valid callstack entry!
      switch(g_CallstackOutputType)
      {
      case ACOutput_Simple:
        _ftprintf(fLogFile, _T("%lu: (-nosymbols- PC == 0)\n"), g_dwShowCount);
        break;
      case ACOutput_Advanced:
        _ftprintf(fLogFile, _T("   (-nosymbols- PC == 0)\n"));
        break;
      case ACOutput_XML:
        // TODO: ....
        _ftprintf(fLogFile, _T("<STACKENTRY decl=\"(-nosymbols- PC == 0)\"/>\n"));
        break;
      }
    }
    else
    {
      // we seem to have a valid PC
      undName[0] = 0;
      undFullName[0] = 0;
      offsetFromSymbol = 0;
      // show procedure info (SymGetSymFromAddr())
      if ( ! pSGSFA( hProcess, s.AddrPC.Offset, &offsetFromSymbol, pSym ) )
      {
        if (g_CallstackOutputType == ACOutput_Advanced)
        {
          if ( gle != 487 )
            _ftprintf(fLogFile, _T("   SymGetSymFromAddr(): GetLastError = %lu\n"), gle );
          else
            _ftprintf(fLogFile, _T("\n"));
        }
      }
      else
      {
        // UnDecorateSymbolName()
        pUDSN( pSym->Name, undName, MAXNAMELEN, UNDNAME_NAME_ONLY );
        pUDSN( pSym->Name, undFullName, MAXNAMELEN, UNDNAME_COMPLETE );
        if (g_CallstackOutputType == ACOutput_Advanced)
        {
          if (strlen(undName) > 0)
            fprintf(fLogFile, "     %s %+ld bytes\n", undName, (long) offsetFromSymbol );
          else
          {
            fprintf(fLogFile, "     Sig:  %s %+ld bytes\n", pSym->Name, (long) offsetFromSymbol );
            strcpy(undName, pSym->Name);
          }
          fprintf(fLogFile, "%lu:     Decl: %s\n", g_dwShowCount, undFullName );
        }
      }
      //if (g_CallstackOutputType == ACOutput_XML)
      //  fprintf(fLogFile, "decl=\"%s\" decl_offset=\"%+ld\" ", SimpleXMLEncode(undName).c_str(), (long) offsetFromSymbol);
      
      // show line number info, NT5.0-method (SymGetLineFromAddr())
      offsetFromLine = 0;
      if ( pSGLFA != NULL )
      { // yes, we have SymGetLineFromAddr()
        if ( ! pSGLFA( hProcess, s.AddrPC.Offset, &offsetFromLine, &Line ) )
        {
          if ( (gle != 487) && (frameNum > 0) )  // ignore error for first frame
          {
            if (g_CallstackOutputType == ACOutput_XML)
            {
              _ftprintf(fLogFile, _T("<STACKENTRY "));
              bXMLTagWrote = TRUE;
              fprintf(fLogFile, "decl=\"%s\" decl_offset=\"%+ld\" ", SimpleXMLEncode(undName).c_str(), (long) offsetFromSymbol);
              _ftprintf(fLogFile, _T("srcfile=\"SymGetLineFromAddr(): GetLastError = %lu\" "), gle);
            }
            else
              _ftprintf(fLogFile, _T("%lu: SymGetLineFromAddr(): GetLastError = %lu\n"), g_dwShowCount, gle );
          }
        }
        else
        {
          switch(g_CallstackOutputType)
          {
          case ACOutput_Advanced:
            fprintf(fLogFile, "%lu:     Line: %s(%lu) %+ld bytes\n", g_dwShowCount,
              Line.FileName, Line.LineNumber, offsetFromLine );
            break;
          case ACOutput_Simple:
            fprintf(fLogFile, "%lu: %s(%lu) %+ld bytes (%s)\n", g_dwShowCount,
              Line.FileName, Line.LineNumber, offsetFromLine, undName);
            break;
          case ACOutput_XML:
            _ftprintf(fLogFile, _T("<STACKENTRY "));
            bXMLTagWrote = TRUE;
            fprintf(fLogFile, "decl=\"%s\" decl_offset=\"%+ld\" ", SimpleXMLEncode(undName).c_str(), (long) offsetFromSymbol);
            fprintf(fLogFile, "srcfile=\"%s\" line=\"%lu\" line_offset=\"%+ld\" ", 
              SimpleXMLEncode(Line.FileName).c_str(), Line.LineNumber, offsetFromLine, undName);
            break;
          }
        }
      } // yes, we have SymGetLineFromAddr()
      
      // show module info (SymGetModuleInfo())
      if ( (g_CallstackOutputType == ACOutput_Advanced) || (g_CallstackOutputType == ACOutput_XML) )
      {
        if ( ! pSGMI( hProcess, s.AddrPC.Offset, &Module ) )
        {
          if (g_CallstackOutputType == ACOutput_Advanced)
            _ftprintf(fLogFile, _T("%lu: SymGetModuleInfo): GetLastError = %lu\n"), g_dwShowCount, gle );
        }
        else
        { // got module info OK
          char ty[80];
          switch ( Module.SymType )
          {
          case SymNone:
            strcpy( ty, "-nosymbols-" );
            break;
          case SymCoff:
            strcpy( ty, "COFF" );
            break;
          case SymCv:
            strcpy( ty, "CV" );
            break;
          case SymPdb:
            strcpy( ty, "PDB" );
            break;
          case SymExport:
            strcpy( ty, "-exported-" );
            break;
          case SymDeferred:
            strcpy( ty, "-deferred-" );
            break;
          case SymSym:
            strcpy( ty, "SYM" );
            break;
#if API_VERSION_NUMBER >= 9
          case SymDia:
            strcpy( ty, "DIA" );
            break;
#endif
          default:
            _snprintf( ty, sizeof ty, "symtype=%ld", (long) Module.SymType );
            break;
          }
          
          if (g_CallstackOutputType == ACOutput_XML)
          {
            // now, check if the XML-Entry is written...
            if (bXMLTagWrote == FALSE) 
            {
              _ftprintf(fLogFile, _T("<STACKENTRY "));
              bXMLTagWrote = TRUE;
              fprintf(fLogFile, "decl=\"%s\" decl_offset=\"%+ld\" ", SimpleXMLEncode(undName).c_str(), (long) offsetFromSymbol);
              _ftprintf(fLogFile, _T("srcfile=\"\" "));
              bXMLTagWrote = TRUE;
            }
          }
          
          if (g_CallstackOutputType == ACOutput_Advanced)
          {
            fprintf(fLogFile, "%lu:     Mod:  %s, base: %08lxh\n", g_dwShowCount,
              Module.ModuleName, Module.BaseOfImage );
            if (Module.SymType == SymNone) { // Gebe nur aus, wenn keine Symbole vorhanden sind!
              _ftprintf(fLogFile, _T("%lu:     Offset: 0x%8.8x\n"), g_dwShowCount, s.AddrPC.Offset);
              fprintf(fLogFile, "%lu:     Sym:  type: %s, file: %s\n", g_dwShowCount,
                ty, Module.LoadedImageName );
            }
          }
          else
          {
            // XML:
            if (bXMLTagWrote == TRUE)
              fprintf(fLogFile, "module=\"%s\" base=\"%08lx\" ", Module.ModuleName, Module.BaseOfImage);
          }
        } // got module info OK
      }
      if ( (g_CallstackOutputType == ACOutput_XML) && (bXMLTagWrote == TRUE) )
        _ftprintf(fLogFile, _T("/>\n"));  // terminate the XML node
      
    } // we seem to have a valid PC
    
    // no return address means no deeper stackframe
    if ( s.AddrReturn.Offset == 0 )
    {
      // avoid misunderstandings in the printf() following the loop
      SetLastError( 0 );
      break;
    }
    
  } // for ( frameNum )
  
  if ( (g_CallstackOutputType != ACOutput_XML) && (gle != 0) )
    _ftprintf(fLogFile, _T("\n%lu: StackWalk(): GetLastError = %lu\n"), g_dwShowCount, gle );
  
cleanup:
  //if (pSym) free( pSym );
  if (fLogFile) {
    _ftprintf(fLogFile, _T("\n\n"));
    if (g_dwShowCount % 1000)
      fflush(fLogFile);
  }
  
  LeaveCriticalSection(&g_csFileOpenClose);
  // Critical section end...
}  // ShowStackRM

#endif // 

#endif 
