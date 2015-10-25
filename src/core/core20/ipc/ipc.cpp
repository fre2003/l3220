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

#undef TRACE

#if 0
UINT32 myTrace(LPCTSTR pFormat, ...)
{
  TCHAR str[512] = {0, };    
  va_list argptr;
  int cnt = 0;

  va_start(argptr, pFormat);
  cnt = wvsprintf(str, pFormat, argptr);
  va_end(argptr);
  if(cnt)
    OutputDebugString(str);

  return 0;
}
#else
#undef TRACE 
#define TRACE {}
#endif
#include "XQueue.h"


typedef struct  
{
  TCHAR name[256];
  CXQueue* m_pQueue;
  BYTE* pQueueData;
} QUEUE_INS;

CIpcMemQueue_i::CIpcMemQueue_i()
{
  m_hQueue = NULL;
}

CIpcMemQueue_i::~CIpcMemQueue_i()
{
  DestroyQueue();
}

//
// CAUTION: The size don't changed if call IpcOpenQueue_2() to open the exiting queue.
//
DWORD CIpcMemQueue_i::CreateQueue(LPCTSTR pszQueueName, DWORD nQueueSize, DWORD nBlockSize)
{
  DWORD ret = ERR_OK_I;
  CXQueue* m_pQueue = NULL;
  
  m_pQueue = new CXQueue();
  if(m_pQueue == NULL)
  {
    m_hQueue = (HANDLE)NULL;
    return ERR_NO_MORE_MEMORY_I;
  }
  
  //
  // the Queue have 'nQueueSize' blocks, every block is 128 bytes.
  // so the queue size is nQueueSize*nBlockSize Bytes.
  //
  LONG lRet = m_pQueue->Create(pszQueueName, nQueueSize/*64*/, nBlockSize/*128*/);
  if(lRet != XQUEUE_ERROR_SUCCESS)
  {
    delete m_pQueue;
    m_pQueue = NULL;
    m_hQueue = (HANDLE)NULL;
    return ERR_QUEUE_CORRUPT_I;
  }
  
  XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();	
  if(!pVB)
  {
    delete m_pQueue;
    m_pQueue = NULL;
    return ERR_QUEUE_CORRUPT_I;
  }
  
  QUEUE_INS* pIns = ( QUEUE_INS*)GlobalAlloc(0, sizeof(QUEUE_INS));
  
  ZeroMemory(pIns, sizeof(QUEUE_INS));
  pIns->m_pQueue = m_pQueue;
  lstrcpy(pIns->name, pszQueueName);
  
  if(pIns)
  {
    ret = ERR_OK_I;
    m_hQueue = (HANDLE)pIns;
    OutputDbgStr_i(_T("Create Queue '%s' OK. \n"), pIns->name);
  }
  
  return ret;
}

DWORD CIpcMemQueue_i::QueueBlockInfo(DWORD & dwTotalDataBlocks,
                            DWORD & dwFreeDataBlocks, 
                            DWORD & dwDataBlockSize,
                            DWORD & dwCount)
{
  QUEUE_INS* pIns = (QUEUE_INS*)m_hQueue;
  CXQueue* m_pQueue = NULL;
  DWORD dwRet = 0;
  
  dwTotalDataBlocks = 0;
  dwFreeDataBlocks  = 0;
  dwDataBlockSize   = 0;

  if(!pIns)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  m_pQueue = pIns->m_pQueue;
    
  if(!m_pQueue)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
  if(pVB)
  {
    dwTotalDataBlocks = pVB->NoDataBlocks;
    dwFreeDataBlocks  = pVB->FreeDataBlocks;
    dwDataBlockSize   = pVB->DataSize;
    dwCount           = pVB->MessageId;

    return ERR_OK_I;
  }
  else
  {
    return ERR_NO_INITIALIZATION_I;
  }
}

//
// 
// Return Value: 
//   ERR_OK_I
//   ERR_TIMEOUT_I
//   ERR_INVALID_PARAM_I
//   ERR_QUEUE_SHUTDOWN_I 
//   ERR_NO_MORE_DATA_I
//   ERR_NO_MORE_MEMORY_I
//   ERR_NOT_OPEN_I
//   ERR_QUEUE_CORRUPT_I
//   ERR_UNKNOWN_I
//
DWORD CIpcMemQueue_i::WaitForQueue(PBYTE* pData, 
                          DWORD& dwDataSize, 
                          DWORD dwTimeOut) // wait to write.
{
  QUEUE_INS* pIns = (QUEUE_INS*)m_hQueue;
  CXQueue* m_pQueue = NULL;
  DWORD dwRet = 0;
  LONG lRet   = 0;
   
  dwDataSize  = 0;

  if(!pIns || !pData)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  m_pQueue = pIns->m_pQueue;
  *pData = NULL;
  
  if(!m_pQueue)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
  if(pVB)
  {
#if 0
    if(pVB->UsedListForwardLink == XQUEUE_INVALID_BLOCK)
    {
      dwRet = ERR_NO_MORE_DATA_I;
    }
    else 
#endif
    if(pVB->MessageId == 0xffffffff) 
    {
      dwRet = ERR_QUEUE_SHUTDOWN_I;
    }
    else if(pVB->UsedDataBlocks)
    { 
      // Have used data in block list.
      dwRet = ERR_OK_I;
    }
#if 0
    else if(pVB->FreeDataBlocks >= pVB->NoDataBlocks)
    {
      dwRet = ERR_QUEUE_CORRUPT_I;
    }
#endif
    else
    {
      lRet = m_pQueue->WaitForQueueWrite(dwTimeOut /*INFINITE*/);
      if(lRet == XQUEUE_ERROR_SUCCESS)
      {
        // continue to read data.     
        dwRet = ERR_OK_I;
      }
      else if(lRet == XQUEUE_ERROR_TIMEOUT)
      {
        dwRet = ERR_TIMEOUT_I;
      }
    }
  } // if(pVB)
  else
  {
    dwRet = ERR_NOT_OPEN_I;
  }

  if(dwRet == ERR_OK_I)
  {
    size_t nSize = 0;
    size_t nMessageId = 0;
    
    lRet = 0;
    
#if 0
    lRet = m_pQueue->Read(NULL, &nSize, &nMessageId, NULL, 10); // 10 ms time out for lock when read MMF.
    if (lRet == XQUEUE_ERROR_SUCCESS)
    {
      DWORD dwThreadId = 0;
      BYTE* p = (BYTE*)malloc(nSize); //GlobalAlloc(0,nSize);
      if(p == NULL)
      {
        dwRet = ERR_NO_MORE_MEMORY_I;
        return dwRet;
      }
      
      pIns->pQueueData = p;
      ZeroMemory(p, nSize);
      lRet = m_pQueue->Read((LPBYTE) p, &nSize, &nMessageId, &dwThreadId, 10);
      
      dwSize = nSize;
      return (PVOID)p;
    }
    else
    {
      dwRet = ERR_NO_MORE_DATA_I;
      return dwRet;
    }
#else
    LPBYTE pBuffer = NULL;
    DWORD dwThreadId = 0;
    
    // 5 s time out for update variable blocks when read MMF.
    dwRet  = m_pQueue->Read(&nMessageId, &dwThreadId, &pBuffer, &nSize, 5000); 
    if(pVB->MessageId == 0xffffffff) // Queue need to shutdown now.
    {
      dwRet = ERR_QUEUE_SHUTDOWN_I;
      if(pBuffer)
      {
        free(pBuffer);
        pBuffer = NULL;
        pIns->pQueueData = (BYTE*)NULL;
      }
    }
    else
    {
      if(pBuffer && nSize)
      {
        *pData = pBuffer;
        pIns->pQueueData = (BYTE*)pBuffer;
        dwDataSize = nSize;
        dwRet = ERR_OK_I;
      }
      else
      {
        //dwRet = ERR_UNKNOWN_I;
      }
    }
#endif 
  }
  
  return dwRet;
}


//
// 
// Return Value: 
//   ERR_OK_I
//   ERR_TIMEOUT_I
//   ERR_INVALID_PARAM_I
//   ERR_QUEUE_SHUTDOWN_I 
//   ERR_NO_MORE_DATA_I
//   ERR_NO_MORE_MEMORY_I
//   ERR_NOT_OPEN_I
//   ERR_QUEUE_CORRUPT_I
//   ERR_UNKNOWN_I
//
DWORD CIpcMemQueue_i::WaitForQueue(PBYTE pData, 
                         DWORD& dwDataSize, // in/out
                         DWORD dwTimeOut) // wait to write.
{
  QUEUE_INS* pIns = (QUEUE_INS*)m_hQueue;
  CXQueue* m_pQueue = NULL;
  DWORD dwRet = 0;
  LONG lRet   = 0;
   
  if(!pIns || !pData || !dwDataSize)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  m_pQueue = pIns->m_pQueue;
    
  if(!m_pQueue)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
  if(pVB)
  {
    if(pVB->MessageId == 0xffffffff) 
    {
      dwRet = ERR_QUEUE_SHUTDOWN_I;
    }
    else if(pVB->UsedDataBlocks)
    { 
      // Have used data in block list.
      dwRet = ERR_OK_I;
    }
#if 0
    else if(pVB->FreeDataBlocks >= pVB->NoDataBlocks)
    {
      dwRet = ERR_QUEUE_CORRUPT_I;
    }
#endif
    else
    {
      lRet = m_pQueue->WaitForQueueWrite(dwTimeOut /*INFINITE*/);
      if(lRet == XQUEUE_ERROR_SUCCESS)
      {
        // continue to read data.     
        dwRet = ERR_OK_I;
      }
      else if(lRet == XQUEUE_ERROR_TIMEOUT)
      {
        dwRet = ERR_TIMEOUT_I;
      }
    }
  } // if(pVB)
  else
  {
    dwRet = ERR_NOT_OPEN_I;
  }

  if(dwRet == ERR_OK_I)
  {
    size_t nMessageId = 0;
    
    lRet = 0;
    
    DWORD dwThreadId = 0;
    
   
    pIns->pQueueData = (BYTE*)NULL;
    // 5 s time out for update variable blocks when read MMF.
    dwRet  = m_pQueue->ReadEx(&nMessageId, &dwThreadId, pData, &dwDataSize, 5000); 
    if(dwRet == 0)
    {
      if(pVB->MessageId == 0xffffffff) // Queue need to shutdown now.
      {
        dwRet = ERR_QUEUE_SHUTDOWN_I;
        pIns->pQueueData = (BYTE*)NULL;
      }
      else
      {
        if(dwDataSize)
        {
          pIns->pQueueData = (BYTE*)NULL;
          dwRet = ERR_OK_I;
        }
        else
        {
          dwRet = ERR_UNKNOWN_I;
        }
      }
    }
  }
  
  return dwRet;
}

DWORD WaitForMessage(HANDLE hQueue, DWORD& dwMsg,     // the message ID
                            PBYTE* pData, DWORD& dwDataSize, // the message parameters.
                            DWORD dwTimeOut) // wait to write.
{
  UNREF_PARAM(dwDataSize);
  QUEUE_INS* pIns   = (QUEUE_INS*)hQueue;
  CXQueue* m_pQueue = NULL;
  DWORD dwRet       = 0;
  LONG lRet         = 0;
  
  if(!pIns || !pData)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  m_pQueue = pIns->m_pQueue;
  pData    = NULL;
  
  if(!m_pQueue)
  {
    dwRet = ERR_INVALID_PARAM_I;
    return dwRet;
  }
  
  XQUEUE_VARIABLE_BLOCK * pVB = m_pQueue->GetVariableBlock();
  if(pVB)
  {
    if(pVB->FreeDataBlocks >= pVB->NoDataBlocks)
    {
        return ERR_QUEUE_CORRUPT_I;
    }
    else if(pVB->UsedListForwardLink == XQUEUE_INVALID_BLOCK)
    {
      return ERR_NO_MORE_DATA_I;
    }
    else if(pVB->UsedDataBlocks)
    { 
      // Have used data in block list, that is to say the data is available, need to read.
    }
    else
    {
      lRet = m_pQueue->WaitForQueueWrite(dwTimeOut/*INFINITE*/);
      if(lRet == XQUEUE_ERROR_SUCCESS)
      {
        // continue to read data.      
      }
      else if(lRet == XQUEUE_ERROR_TIMEOUT)
      {
        dwRet = ERR_TIMEOUT_I;
        return dwRet;
      }
    }
  }
  else
  {
    dwRet = ERR_NOT_OPEN_I;
    return dwRet;
  }
  
  size_t nSize = 0;
  size_t nMessageId = 0;
  
  lRet = 0;
  
#if 0
  lRet = m_pQueue->Read(NULL, &nSize, &nMessageId, NULL, 10); // 10 ms time out for lock when read MMF.
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    DWORD dwThreadId = 0;
    BYTE* p = (BYTE*)malloc(nSize); //GlobalAlloc(0,nSize);
    if(p == NULL)
    {
      dwRet = ERR_NO_MORE_MEMORY_I;
      return dwRet;
    }
    
    pIns->pQueueData = p;
    ZeroMemory(p, nSize);
    lRet = m_pQueue->Read((LPBYTE) p, &nSize, &nMessageId, &dwThreadId, 10);
    
    dwSize = nSize;
    return (PVOID)p;
  }
  else
  {
    dwRet = ERR_NO_MORE_DATA_I;
    return dwRet;
  }
#else
  LPBYTE pBuffer = NULL;
  DWORD dwThreadId = 0;
  dwRet = m_pQueue->Read(&nMessageId, &dwThreadId, &pBuffer, &nSize, 5000); // // 5 s time out for update variable blocks when read MMF.
  *pData = pBuffer;
  pIns->pQueueData = (BYTE*)pBuffer;
#endif 
  return dwRet;
}

bool CIpcMemQueue_i::QueryMessage (DWORD dwTimeOut)
{
  QUEUE_INS* pIns = (QUEUE_INS*)m_hQueue;
  CXQueue* m_pQueue = NULL;
  size_t nSize = 0;
  size_t nMessageId = 0;
  LONG lRet = 0;
  
  if(!pIns)
  {
    return NULL;
  }
  
  m_pQueue = pIns->m_pQueue;
  
  if(!m_pQueue)
  {
    return NULL;
  }
  
  lRet = m_pQueue->WaitForQueueWrite(dwTimeOut);
  lRet = m_pQueue->Read(NULL, &nSize, &nMessageId, NULL, 5);    
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


bool CIpcMemQueue_i::DestroyQueue()
{
  QUEUE_INS* pIns   = (QUEUE_INS*)m_hQueue;
  CXQueue* m_pQueue = NULL;
  
  if(!pIns)
  {
    return FALSE;
  }
  
  m_pQueue = pIns->m_pQueue;
  
  if(!m_pQueue)
  {
    m_pQueue = NULL;
    return FALSE;
  }
  else
  {
    delete pIns->m_pQueue;
    pIns->m_pQueue = NULL;
  }
  
  if(pIns)
  {
    //if(!IsBadReadPtr(pIns, sizeof(pIns)))
    {
      GlobalFree(pIns);
    }
    pIns = NULL;
  }
  
  m_hQueue = NULL;

  return TRUE;
}

bool CIpcMemQueue_i::FreeData(VOID)
{
  QUEUE_INS* pIns = (QUEUE_INS*)m_hQueue;
  
  if(pIns && pIns->pQueueData)
  {
    free(pIns->pQueueData);
        
    pIns->pQueueData = NULL;
    return TRUE;
  }
  
  return FALSE;
}

bool SendToQueue(LPCTSTR pszQueueName, PVOID pData, DWORD nDataSize, DWORD dwTimeOut)
{
  CXQueue* m_pQueue;
  XQUEUE_VARIABLE_BLOCK * pVB = NULL;

  m_pQueue = new CXQueue();
  
  if(m_pQueue == NULL)
  {
    TRACE("Create New Queue fail \n");
    return FALSE;
  }
  
  LONG lRet = m_pQueue->Open(pszQueueName); // queue client
  if (!m_pQueue->IsOpen())
  {
    goto exit;
  }
  
  pVB = m_pQueue->GetVariableBlock();
  if(pVB && (pVB->FreeDataBlocks == 0 || pVB->NoServers == 0))
  {
    goto exit;
  }
  
	if (!m_pQueue->IsEnoughFreeBlocks(nDataSize)) // Are there enough free blocks?
  {
    goto exit;
  }
  
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    if (m_pQueue->Write((PBYTE)pData, nDataSize, dwTimeOut) == XQUEUE_ERROR_SUCCESS) // time out = 100 ms for writing.
    {
      // ... 
    }
    else
    {
      delete m_pQueue;
      TRACE_I(_T("Write Queue Time Out.\n"));
      return FALSE;
    }
  }
  else
  {
    TRACE_I(_T("The Queue is not Open.\n"));
  }
  
exit:
  if (m_pQueue)
  {
    delete m_pQueue;
  } 
  
  return TRUE;
}


bool CIpcMemQueue_i::IsQueueExist (LPCTSTR pszQueueName)
{
  bool ret = FALSE;
  CXQueue* pQueue = NULL;
  
  pQueue = new CXQueue();
  
  if(pQueue)
  {
    ret = (bool)(pQueue->CheckOpen(pszQueueName));
    delete pQueue;
  }  
  
  return ret;
}

DWORD CIpcMemQueue_i::OpenQueue (LPCTSTR pszQueueName, DWORD dwTimeOut)
{
  U32 ret  = ERR_OK_I;
  CXQueue* m_pQueue;
  
  m_pQueue = new CXQueue();
  
  if(m_pQueue == NULL)
  {
    m_hQueue = (HANDLE)0;
    return NULL;
  }
  
  LONG lRet = m_pQueue->Open(pszQueueName);
  
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    m_hQueue = (HANDLE)m_pQueue;
  }
  else
  {
    delete m_pQueue;
    m_hQueue = (HANDLE)0;
  }  
  
  return ret;
}

bool CIpcMemQueue_i::CloseQueue ()
{
  CXQueue* m_pQueue = (CXQueue*)m_hQueue;
  if(m_pQueue == NULL)
  {
    return FALSE;
  }  
  else
  {
    delete m_pQueue;
    return TRUE;
  }
  
  return FALSE;
}

bool CIpcMemQueue_i::ClearQueue (VOID)
{
  CXQueue* m_pQueue = (CXQueue*)m_hQueue;
  if(m_pQueue == NULL)
  {
    return FALSE;
  }  
  else
  {
    m_pQueue->QueueClear();

    return TRUE;
  }
  
  return FALSE;
}

// Retrun Value
//   ERR_OK_I
//   ERR_TIMEOUT_I 
//   ERR_NOT_OPEN_I
//   ERR_INVALID_PARAM_I
//   ERR_NOT_FREEE_SPACE_I   : the space of queue is small, should use ITS_IpcCreateQueue() to create the enough space.
//   ERR_QUEUE_UNAVAILABLE_I : don't use IpcOpenQueue_2() to open queue.
//   ERR_QUEUE_DATA_TOO_LARGE_2: the input data is too large.
//   ERR_QUEUE_SHUTDOWN_I 
//   
// 
// the nDataSize is limitted to 2^30 bytes.
// Copy data to queue.
//   ERR_OK_I
//   ERR_TIMEOUT_I 
//   ERR_NOT_OPEN_I
//   ERR_INVALID_PARAM_I
//   ERR_NOT_FREEE_SPACE_I     : the space of queue is small, should use IpcCreateQueue_2() to create the enough space.
//   ERR_QUEUE_UNAVAILABLE_I    : don't use IpcOpenQueue_2() to open queue.
//   ERR_QUEUE_DATA_TOO_LARGE_I : the input data is too large.
//   ERR_QUEUE_SHUTDOWN_I 
DWORD CIpcMemQueue_i::SendData (PVOID pData, DWORD nDataSize, DWORD dwOption, DWORD dwTimeOut)
{
  CXQueue* m_pQueue = (CXQueue*)m_hQueue;
  DWORD dwResult = 0;

  if(m_pQueue == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  if (!m_pQueue->IsOpen())
  {
    return ERR_NOT_OPEN_I;
  }
  
  XQUEUE_VARIABLE_BLOCK * pVb = m_pQueue->GetVariableBlock();
  if(pVb)
  {
    if(pVb->FreeDataBlocks == 0)
    {
      dwResult = ERR_NOT_FREEE_SPACE_I;
    }
    else if(pVb->NoServers == 0)
    {
      dwResult = ERR_QUEUE_UNAVAILABLE_I;
    }
    else if(pVb->MessageId == 0xFFFFFFFF)
    {
      dwResult = ERR_QUEUE_SHUTDOWN_I;
    }
    else if(!m_pQueue->IsValidDataSize(nDataSize))
    {
       dwResult = ERR_QUEUE_DATA_TOO_LARGE_I;
    }
    else 
    {
      LONG lRet = XQUEUE_ERROR_SUCCESS;
      if (!m_pQueue->IsEnoughFreeBlocks(nDataSize)) // are there enough free blocks?
      {
        dwResult = ERR_NOT_FREEE_SPACE_I;
      }
      else if ((lRet = m_pQueue->Write((PBYTE)pData, nDataSize, dwTimeOut)) == XQUEUE_ERROR_SUCCESS) // time out = 100 ms for writing.
      {
        dwResult = ERR_OK_I;
      }
      else if(lRet == XQUEUE_ERROR_NO_FREE)
      {
        dwResult = ERR_NOT_FREEE_SPACE_I;
      }
      else 
      {
        TRACE_I(_T("Write Queue Time Out.\n"));
        dwResult = ERR_TIMEOUT_I;
      }
    }
  }
  else
  {
    dwResult = ERR_NOT_OPEN_I;
  }
  
  return dwResult;
}

DWORD CIpcMemQueue_i::Shutdown (DWORD dwTimeOut) // INFINITE
{
  QUEUE_INS* pIns   = (QUEUE_INS*)m_hQueue;
  CXQueue* m_pQueue = NULL;
  DWORD dwResult    = 0;
      
  if(!pIns)
  {
    dwResult = ERR_INVALID_PARAM_I;
    return dwResult;
  }
  
  m_pQueue = pIns->m_pQueue;
  if(!m_pQueue)
  {
    dwResult = ERR_INVALID_PARAM_I;
    return dwResult;
  }
  
  XQUEUE_VARIABLE_BLOCK * pVb = m_pQueue->GetVariableBlock();
  if(pVb)
  {
    if(pVb->FreeDataBlocks == 0)
    {
      dwResult = ERR_NOT_FREEE_SPACE_I;
    }
    else if(pVb->NoServers == 0)
    {
      dwResult = ERR_QUEUE_UNAVAILABLE_I;
    }
    else
    {
      dwResult = m_pQueue->ShutdownSignal(dwTimeOut);
    }
  }
  else
  {
    dwResult = ERR_NOT_OPEN_I;
  }
  
  return dwResult;
}


PVOID WaitQueueMessage(HANDLE hQueue, 
                           DWORD* pBufferSize,
                           DWORD* pThreadId,
                           DWORD dwTimeOut) // wait to write.
{
  QUEUE_INS* pIns = (QUEUE_INS*)hQueue;
  CXQueue* m_pQueue = NULL;

  if(!pIns)
    return NULL;
  
  m_pQueue = pIns->m_pQueue;
  
  if(!m_pQueue)
    return NULL;
  
  LONG lRet = m_pQueue->WaitForQueueWrite(dwTimeOut/*INFINITE*/);
  
  size_t nSize = 0;
  size_t nMessageId = 0;
  
  lRet = 0;
  
  lRet = m_pQueue->Read(NULL, &nSize, &nMessageId, NULL, 10);    
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    DWORD dwThreadId = 0;
    BYTE* p = (BYTE*)GlobalAlloc(0,nSize);
    if(p == NULL)
      return NULL;
    
    if(pBufferSize) *pBufferSize = nSize;
    pIns->pQueueData = p;
    ZeroMemory(p, nSize);
    lRet = m_pQueue->Read((LPBYTE) p, &nSize, &nMessageId, &dwThreadId, 10);
    if(pThreadId) *pThreadId = dwThreadId;
    return (PVOID)p;
  }
  else
    return NULL;
}

bool WaitForEventQueue(HANDLE hQueue, 
                           DWORD dwTimeOut, 
                           UINT32* pEventId, 
                           BYTE** pOutBuffer, 
                           UINT32* pdwDataSize)
{
  QUEUE_INS* pIns = (QUEUE_INS*)hQueue;
  CXQueue* m_pQueue = NULL;
  if(!pIns)
    return FALSE;
  
  m_pQueue = pIns->m_pQueue;
  
  if(!m_pQueue)
    return FALSE;
  
  LONG lRet = m_pQueue->WaitForQueueWrite(dwTimeOut); //INFINITE
  
  size_t nSize = 0;
  size_t nMessageId = 0;
  
  lRet = 0;
  
  lRet = m_pQueue->Read(NULL, &nSize, &nMessageId, NULL, 10);    
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    DWORD dwThreadId = 0;
#if 1 
    BYTE* p = (BYTE*)GlobalAlloc(0,nSize+2);
    pIns->pQueueData = p;
    ZeroMemory(p, nSize+2);
#else
    if(nSize <= dwSize)
      dwSize = nSize;
#endif  
    if(pIns->pQueueData)
    {
      lRet = m_pQueue->Read((LPBYTE) pIns->pQueueData, (size_t*)&nSize, &nMessageId, &dwThreadId, 10);
      if(nSize)
      {
        UINT32* pTemp = (UINT32*)(pIns->pQueueData);
        if(pEventId)
          *pEventId = *pTemp++;
        if(pdwDataSize)
          *pdwDataSize = nSize-4;
        if(pOutBuffer)
          *pOutBuffer = (BYTE*)pTemp;
        return TRUE;
      }
    }
    else
      return FALSE;
  }
  else
    return FALSE;
  
  return FALSE; 
}


PVOID IPC_WaitForQueueEx(HANDLE hQueue) // wait to write.
{
  QUEUE_INS* pIns = (QUEUE_INS*)hQueue;
  CXQueue* m_pQueue = NULL;
  if(!pIns)
    return NULL;
  
  m_pQueue = pIns->m_pQueue;
  
  if(!m_pQueue)
    return NULL;
  
  LONG lRet = m_pQueue->WaitForQueueWrite(20);//INFINITE
  
  size_t nSize = 0;
  size_t nMessageId = 0;
  
  lRet = 0;
  
  lRet = m_pQueue->Read(NULL, &nSize, &nMessageId, NULL, 10);    
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    DWORD dwThreadId = 0;
    BYTE* p = (BYTE*)GlobalAlloc(0,nSize+2);
    pIns->pQueueData = p;
    ZeroMemory(p, nSize+2);
    lRet = m_pQueue->Read((LPBYTE) p, &nSize, &nMessageId, &dwThreadId, 10);
    if(nSize == 0)
      return NULL;
    else 
      return (PVOID)p;
  }
  else
    return NULL;
}

PVOID IPC_WaitForQueueTimeOut(HANDLE hQueue, UINT32 nTimeOut) // wait to write.
{
  QUEUE_INS* pIns = (QUEUE_INS*)hQueue;
  CXQueue* m_pQueue = NULL;
  if(!pIns)
    return NULL;
  
  m_pQueue = pIns->m_pQueue;
  
  if(!m_pQueue)
    return NULL;
  
  LONG lRet = m_pQueue->WaitForQueueWrite(nTimeOut);
  
  size_t nSize = 0;
  size_t nMessageId = 0;
  
  lRet = 0;
  
  lRet = m_pQueue->Read(NULL, &nSize, &nMessageId, NULL, 10);    
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    DWORD dwThreadId = 0;
    BYTE* p = (BYTE*)GlobalAlloc(0,nSize+2);
    pIns->pQueueData = p;
    ZeroMemory(p, nSize+2);
    lRet = m_pQueue->Read((LPBYTE) p, &nSize, &nMessageId, &dwThreadId, 10);
    return (PVOID)p;
  }
  else
    return NULL;
}

bool IPC_SendToEventQueue(LPCTSTR pszQueueName,
                          UINT32 nEventId ,
                          PBYTE pData, 
                          DWORD nDataSize)
{
  CXQueue* m_pQueue;
  bool ret = FALSE;
  
  m_pQueue = new CXQueue();
  
  if(m_pQueue == NULL)
  {
    TRACE("Create New Queue fail \n");
    return FALSE;
  }
  
  LONG lRet = m_pQueue->Open(pszQueueName); // queue client
  if (!m_pQueue->IsOpen())
  {
    ret = FALSE;
    goto exit;
  }
  
  if (lRet == XQUEUE_ERROR_SUCCESS)
  {
    UINT32* p = (UINT32*)GlobalAlloc(0, nDataSize + 4);
    UINT32* pTemp = p;
    *pTemp++= nEventId;
    memcpy((char*)pTemp, pData, nDataSize);
    if (m_pQueue->Write((LPBYTE)p, nDataSize + 4, 100) == XQUEUE_ERROR_SUCCESS) // time out = 100 ms for writing.
    {
      ret = TRUE;
      if(p) GlobalFree(p);
    }
    else
    {
      if(p) GlobalFree(p);
      goto exit;
    }
  }
  else
  {
  }
  
exit :
  
  if (m_pQueue)
  {
    delete m_pQueue;
  } 
  
  return ret;
}

#if 0 
//IPC_Print(0x02, "SCL-> ERROR:%d. Fail to Open Serial Device %s!! Please config COM port in MF32.ini and re-start it in MF32.exe Menu->Util->StartSCL \n", GetLastError(), port);
//#include "event.h"
bool IPC_Print(U8 nLevel,LPCTSTR pFormat, ...)
{
  static char str[1024] = {0, };
  int cnt = 0;
  va_list argptr;
  UINT32* p = (UINT32*)str;
  
  ZeroMemory(str, sizeof(str)); 	
  
  va_start(argptr, pFormat);
  cnt = vsprintf(&str[4], pFormat, argptr);
  va_end(argptr);
  
  int size = lstrlen(&str[4]);
 	*p++ = nLevel;
  
  if(cnt)
  {
    //IPC_SendToQueueEx(EMU_TRACE_QUEUE_NAME, EV_EMU_TRACE, (BYTE*)str, size+4);
    return TRUE;
  }
  else
    return FALSE;
}

bool IPC_PrintV1Ex(LPCTSTR pFormat, ...)
{
  static char str[1024] = {0, };
  int cnt = 0;
  va_list argptr;
  UINT32* p = (UINT32*)str;
  
  ZeroMemory(str, sizeof(str)); 	
  
  va_start(argptr, pFormat);
  cnt = vsprintf(str, pFormat, argptr);
  va_end(argptr);
  
  int size = lstrlen(str);
  
  if(cnt)
  {
    IPC_SendToQueue(EMU_MFE_TRACE_QUEUE_NAME, (BYTE*)str, cnt+1);
    return TRUE;
  }
  else
    return FALSE;
}

bool IPC_PrintV1(LPCTSTR pszString)
{
  if(pszString)
  {
    IPC_SendToQueue(EMU_MFE_TRACE_QUEUE_NAME, (BYTE*)pszString, strlen(pszString)+1);
    return TRUE;
  }
  else
    return FALSE;
}

bool IPC_PrintI(LPCTSTR pFormat, ...) // INFO
{
  static char str[1024] = {0, };
  int cnt = 0;
  va_list argptr;
  UINT32* p = (UINT32*)str;
  
  ZeroMemory(str, sizeof(str)); 	
  
  va_start(argptr, pFormat);
  cnt = vsprintf(&str[4], pFormat, argptr);
  va_end(argptr);
  
  int size = lstrlen(&str[4]);
 	*p++ = 0;
  
  if(cnt)
  {
    //IPC_SendToQueueEx(EMU_TRACE_QUEUE_NAME, EV_EMU_TRACE, (BYTE*)str, size+4);
    return TRUE;
  }
  else
    return FALSE;
}

bool IPC_PrintW(LPCTSTR pFormat, ...) // WARNING
{
  static char str[1024] = {0, };
  int cnt = 0;
  va_list argptr;
  UINT32* p = (UINT32*)str;
  
  ZeroMemory(str, sizeof(str)); 	
  
  va_start(argptr, pFormat);
  cnt = vsprintf(&str[4], pFormat, argptr);
  va_end(argptr);
  
  int size = lstrlen(&str[4]);
 	*p++ = 1;
  
  if(cnt)
  {
    //IPC_SendToQueueEx(EMU_TRACE_QUEUE_NAME, EV_EMU_TRACE, (BYTE*)str, size+4);
    return TRUE;
  }
  else
    return FALSE;
}

bool IPC_PrintE(LPCTSTR pFormat, ...) // ERROR
{
  static char str[1024] = {0, };
  int cnt = 0;
  va_list argptr;
  UINT32* p = (UINT32*)str;
  
  ZeroMemory(str, sizeof(str)); 	
  
  va_start(argptr, pFormat);
  cnt = vsprintf(&str[4], pFormat, argptr);
  va_end(argptr);
  
  int size = lstrlen(&str[4]);
 	*p++ = 2;
  
  if(cnt)
  {
    //IPC_SendToQueueEx(EMU_TRACE_QUEUE_NAME, EV_EMU_TRACE, (BYTE*)str, size+4);
    return TRUE;
  }
  else
    return FALSE;
}

//static HANDLE MFE_R_hQueue     = NULL;
//static HANDLE MFE_S_hQueue     = NULL;
//static HANDLE MFE_hSerialRevQueue = NULL;
//
HANDLE IPC_GetMFEQueue(UINT8 nIndex)
{
  //	if(nIndex == 0)    
  //		return MFE_R_hQueue;
  //	else if(nIndex == 1)  
  //		return MFE_S_hQueue;
  //	else if(nIndex == 2)  
  //		return MFE_hSerialRevQueue;
  //	else 
  //		return NULL;
  return NULL;
}

bool IPC_CreateMFEQueue()
{
  //	MFE_R_hQueue        = IPC_CreateQueue(MFE_RECEIVER_QUEUE_NAME);
  //	MFE_S_hQueue        = IPC_CreateQueue(MFE_SENDER_QUEUE_NAME);
  //	MFE_hSerialRevQueue = IPC_CreateQueue(MFE_SERIAL_R_QUEUE_NAME);
  
  return TRUE;
}

bool IPC_DestroyMFEQueue()
{
  //	if(MFE_R_hQueue)
  //	{
  //		IPC_DestroyQueue(MFE_R_hQueue);
  //		MFE_R_hQueue = NULL;
  //	}
  //
  //	if(MFE_hSerialRevQueue)
  //	{
  //		IPC_DestroyQueue(MFE_R_hQueue);
  //		MFE_hSerialRevQueue = NULL;
  //	}
  //
  return TRUE;
}

bool PrintMFETrace(LPCTSTR pFormat, ...)
{
  static char str[1024*2] = {0, };
  int cnt = 0;
  va_list argptr;
  UINT32* p = (UINT32*)str;
  
  ZeroMemory(str, sizeof(str)); 	
  
  va_start(argptr, pFormat);
  cnt = vsprintf(str, pFormat, argptr);
  va_end(argptr);
  
  int size = lstrlen(str);
  
  if(cnt)
  {
    IPC_SendToQueue(EMU_MFE_TRACE_QUEUE_NAME, (BYTE*)str, size);
    return TRUE;
  }
  else
    return FALSE;
}
#endif


// Memory Map API.
CIpcMem_i::CIpcMem_i()
{
  m_hMemMap = NULL;
}

CIpcMem_i::~CIpcMem_i()
{
  UnMemMap();
}

DWORD CIpcMem_i::MemMap (LPCTSTR pszName, DWORD dwSize)
{
  U32 ret = ERR_OK_I;
  CXMemMapFile* m_pMemMap;    
  m_pMemMap = new CXMemMapFile();
  
  m_pMemMap->MapMemory(pszName, dwSize, FALSE, NULL);
  m_hMemMap = (HANDLE)(m_pMemMap);
  
  return ret;    
}


DWORD CIpcMem_i::MemMapFile (LPCTSTR lpszFilename, 
					DWORD dwNumberOfBytesToMap,
          bool bReadOnly, 
					DWORD dwShareMode/* = 0*/,  // FILE_SHARE_DELETE, FILE_SHARE_READ, FILE_SHARE_WRITE 
					bool bGrowable/* = FALSE*/,
					DWORD dwStartOffset
					)

{
  U32 ret = ERR_OK_I;

  CXMemMapFile* m_pMemMap;    
  m_pMemMap = new CXMemMapFile();
  
  bool bMapFile = m_pMemMap->MapFile(lpszFilename, 
    bReadOnly,
    dwShareMode, 
    FALSE, 
    FALSE, 
    bGrowable,
    dwStartOffset, 
    dwNumberOfBytesToMap);

  m_hMemMap = bMapFile?(HANDLE)m_pMemMap:NULL;

  return ret;
}




DWORD CIpcMem_i::MemMapExisting (LPCTSTR pszName, DWORD dwSize)
{
  U32 ret = ERR_OK_I;
  CXMemMapFile* m_pMemMap;    

  m_pMemMap = new CXMemMapFile();
  
  m_pMemMap->MapExistingMemory(pszName, dwSize, FALSE );
  m_hMemMap = (HANDLE)m_pMemMap;

  return ret;    
}

PVOID CIpcMem_i::GetMemMapAddr ()
{
  CXMemMapFile* m_pMemMap = (CXMemMapFile*)m_hMemMap;
  if( m_pMemMap)
  {
    return m_pMemMap->GetMappedAddress();
  }
  else
    return NULL;
}

U32 CIpcMem_i::GetMemMapSize (VOID)
{
  CXMemMapFile* m_pMemMap = (CXMemMapFile*)m_hMemMap;
  if( m_pMemMap)
  {
    return m_pMemMap->GetLength();
  }
  else
    return 0;
}

bool CIpcMem_i::MemMapFlush (DWORD dwSize)
{
  CXMemMapFile* m_pMemMap = (CXMemMapFile*)m_hMemMap;
  if( m_pMemMap)
  {
    bool bRet = m_pMemMap->Flush(dwSize);
    UNREF_PARAM(bRet);
    return TRUE;
  } 
  else
    return FALSE;
}

bool CIpcMem_i::UnMemMap (VOID)
{
  CXMemMapFile* m_pMemMap = (CXMemMapFile*)m_hMemMap;
  if( m_pMemMap)
  {
    m_pMemMap->UnMap();
    
    delete m_pMemMap; 
    m_pMemMap = NULL;
    m_hMemMap = NULL;
    return TRUE;
  }
  
  return FALSE;
}

