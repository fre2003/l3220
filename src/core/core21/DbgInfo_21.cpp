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


#ifdef HAVE_UNICODE_I
#define ERR_STRING_CASE(ErrVal, ErrStr) case ErrVal: strRet = L##ErrStr; break; 
#else
#define ERR_STRING_CASE(ErrVal, ErrStr) case ErrVal: strRet = ErrStr; break; 
#endif 

// Get last error message
CString GetLastErrorMsg_i(DWORD dwLastErr, BOOL bWinErrorCode)
{
  CString strRet(_T(""));
  
  if( dwLastErr == ERROR_SUCCESS )
  {
    return strRet;
  }
  
  if((dwLastErr == ERR_WIN32_ERROR_I) || bWinErrorCode)
  {
    if(dwLastErr == ERR_WIN32_ERROR_I)
    {
      dwLastErr = ::GetLastError();
    }
    
    LPTSTR lpszMsgBuf;
    if ( !FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dwLastErr,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      RCAST_I( LPTSTR, &lpszMsgBuf ),
      0,
      NULL ))
    {
      // Handle the error.
      return strRet; 
    }
    
    strRet = lpszMsgBuf;
  
    // Free the buffer.
    LocalFree(lpszMsgBuf); 
  }
  else 
  {
    switch(dwLastErr)
    {
    ERR_STRING_CASE(ERR_INVALID_PARAM_I, "Invalid Parameters.");
    ERR_STRING_CASE(ERR_INVALID_PATH_I,  "Invalid Path.");
    ERR_STRING_CASE(ERR_NOT_FOUND_I,     "Don't Found.");
    ERR_STRING_CASE(ERR_BUSY_I,          "Busy.");
    ERR_STRING_CASE(ERR_INVALID_MEMORY_ADDR_I,          "Memory Address is Invalid.");
    ERR_STRING_CASE(ERR_INVALID_STATCK_SIZE_I,          "Stack Size is Invalid.");
    ERR_STRING_CASE(ERR_NOT_EXIST_FILE_I,               "File don't exist.");
    ERR_STRING_CASE(ERR_ALREADY_EXIST_I,                "Already exist.");
    ERR_STRING_CASE(ERR_ALREADY_INITIALIZED_I,          "Already Initialized.");
    ERR_STRING_CASE(ERR_CREATE_FAILURE_I,               "Create Failure.");
    ERR_STRING_CASE(ERR_INVALID_LEVEL_I,                "Invalid Level Value.");
    ERR_STRING_CASE(ERR_INVALID_PATH_ROOT_I,            "Invalid Path Root.");
    ERR_STRING_CASE(ERR_UNC_PATH_I,                     "Invalid UNC Path.");
    ERR_STRING_CASE(ERR_REMOVABLE_PATH_I,               "Invalid Removable Path.");
    ERR_STRING_CASE(ERR_REMOTE_PATH_I,                  "Invalid Remote Path.");
    ERR_STRING_CASE(ERR_GEN_FAILURE_I,                  "A device attached to the system is not functioning.");
    ERR_STRING_CASE(ERR_INVALID_HANDLE_I,               "The specific handle is invalid.");
    ERR_STRING_CASE(ERR_INVALID_FUNCTION_I,             "Incorrect function.");
    ERR_STRING_CASE(ERR_HEAP_LEAK_I,                    "Heap Leak.");
    ERR_STRING_CASE(ERR_TOO_MANY_THREADS_I,             "Tool Many Theads.");
    ERR_STRING_CASE(ERR_NO_INITIALIZATION_I,            "No Initialization.");
    ERR_STRING_CASE(ERR_NO_SERVER_I,                    "No Server.");
    ERR_STRING_CASE(ERR_NO_OWNER_RECIPIENT,             "No Owner Recipient.");
    ERR_STRING_CASE(ERR_PLUGIN_NOT_EXIST_I,             "Plugin don't exist.");
    ERR_STRING_CASE(ERR_PLUGIN_MAX_CNT_I,               "Plugin Tool Many.");
    ERR_STRING_CASE(ERR_PLUGIN_INSTANCE_ALREADY_EXIST_I,    "Plugin Already Exist.");
    ERR_STRING_CASE(ERR_PLUGIN_INSTANCE_NOT_EXIST_I,        "Plugin Don't Exist.");
    ERR_STRING_CASE(ERR_TIMEOUT_I ,                         "Time out occur.");
    ERR_STRING_CASE(ERR_NO_MORE_MEMORY_I ,                  "No More Meomry.");
    ERR_STRING_CASE(ERR_NO_MORE_DATA_I ,                    "No More Data.");
    ERR_STRING_CASE(ERR_OS_QUEUE_FULL_I ,                   "OS Queue is Full.");
    ERR_STRING_CASE(ERR_TIMER_NOT_EXIST_I ,                 "OS Timer don't exist.");
    ERR_STRING_CASE(ERR_TIMER_CREATE_I ,                    "Fail to Create Timer.");
    ERR_STRING_CASE(ERR_INVALID_MODULE_I ,                  "The specific module is invalid.");
    ERR_STRING_CASE(ERR_BUFFER_TOO_SMALL_I ,                "The specific buffer is too small.");
    ERR_STRING_CASE(ERR_QUEUE_INVALID_I ,                   "The specific buffer queue is invalid.");
    ERR_STRING_CASE(ERR_QUEUE_CORRUPT_I ,                   "The specific queue corrupt.");
    ERR_STRING_CASE(ERR_QUEUE_UNAVAILABLE_I ,               "The specific queue unavailable.");
    ERR_STRING_CASE(ERR_QUEUE_DATA_TOO_LARGE_I ,            "The specific queue data tool large.");
    ERR_STRING_CASE(ERR_QUEUE_SHUTDOWN_I ,                  "The specific queue shutdown.");
    ERR_STRING_CASE(ERR_NOT_OPEN_I ,                        "The specific queue don't open.");
    ERR_STRING_CASE(ERR_NOT_FREEE_SPACE_I ,                 "Don't free space.");
    ERR_STRING_CASE(ERR_NO_ADAPTER_I ,                      "The specific Adpater don't exist.");
    ERR_STRING_CASE(ERR_THREAD_RESTART_I ,                  "The thread was suspended, but is now restarte.d");
    ERR_STRING_CASE(ERR_ACCESS_THREAD_I ,                   "The thread was not accessed.");
    ERR_STRING_CASE(ERR_THREAD_SUSPEND_I ,                  "The thread was suspended."); 

    default: strRet = _T("Unknown error"); break;
    }
  }
  
  strRet.TrimRight();

  return strRet;
} 
