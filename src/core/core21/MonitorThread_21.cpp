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
//
// WorkerThread.cpp 
//
#include "stdafx.h" 
#include <process.h>
#include "../core20/common_20.h"

PRIVATE HANDLE  g_hDevMonitorThread = NULL;
PRIVATE DWORD   g_dwThreadID = 0;
PRIVATE BOOL    g_bMonitorNetworkAdapter = FALSE;
PRIVATE HANDLE  m_hStopEvent = NULL;

PRIVATE DWORD WINAPI DevMonitorThread(LPVOID lpParameter);

DWORD CDeviceMgr_i::StartDeviceMonitor(bool bMonitorNetworkAdapter)
{
  g_hDevMonitorThread = BEGINTHREADEX(0, 0, 
    DevMonitorThread, this,
    0, &g_dwThreadID );
  
  m_hStopEvent = CreateEvent(NULL,
    FALSE, // auto-reset event
    FALSE,  // initial state is not signaled
    NULL);  // object not named
  
  if(g_hDevMonitorThread)
  {
    g_bMonitorNetworkAdapter = bMonitorNetworkAdapter;
        
    return ERR_OK_I;
  }
  
  return ERR_ACCESS_THREAD_I;
}


DWORD CDeviceMgr_i::StopDeviceMonitor(U32 dwMilliseconds)
{
  SetEvent(m_hStopEvent);
  
  U32 ret = WaitForSingleObject(g_hDevMonitorThread, dwMilliseconds);
  if(ret == WAIT_OBJECT_0)
  {
    g_bMonitorNetworkAdapter = FALSE;
    ret = ERR_OK_I;
  }
  else if(ret == WAIT_TIMEOUT)
  {
    ret = ERR_TIMEOUT_I;
  }
  else 
  {
    ret = ERR_WIN32_ERROR_I;
  }
  
  return ret;
}


PRIVATE DWORD WINAPI DevMonitorThread(LPVOID lpParameter)
{
  CDeviceMgr_i* pThis = (CDeviceMgr_i*)lpParameter;
  
  if(pThis == NULL)
    return ERR_THREAD_SHUTDOWN_I;
  
  CDeviceMgr_i::COM_PORT_INFO ArrayCom[32] = {0, };
  CDeviceMgr_i::NET_ADAPTER_INFO AdapterInfoList[10] = {0, };
  U32 ulArrayComCnt = 0;
  U8 AdapterListCnt = 0;
  
  while (1)
  {
    pThis->EnumComPort(ArrayCom, ulArrayComCnt);
    if(g_bMonitorNetworkAdapter)
    {
      pThis->GetAdapterInfo(AdapterInfoList, AdapterListCnt, 0);
    }
    
    DWORD dwWaitResult = WaitForSingleObject(m_hStopEvent, 3000);
    if(dwWaitResult == WAIT_OBJECT_0)
      break;
    
    //pThis->DeviceMonitor();
  }
  
  CloseHandle(m_hStopEvent);
  CloseHandle(g_hDevMonitorThread);
  m_hStopEvent = NULL;
  g_dwThreadID = 0;
  g_bMonitorNetworkAdapter = FALSE;
  g_hDevMonitorThread = NULL;
  
  return 0;
}


