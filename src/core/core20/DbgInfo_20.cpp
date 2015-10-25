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


#define MSG_BUFFER_SIZE 1024 

// Custom message box
static int CustomMsgBox( LPCTSTR lpctszMessage, 
                 LPCTSTR lpctszTitle, 
                 UINT uFlags,
                 HWND hWndParent = 0,
                 LPCTSTR lpctszIconResource = 0 )
{
  // Fill out message box parameters
  MSGBOXPARAMS msgParams = { 0 };
  msgParams.hwndOwner = hWndParent;
  msgParams.cbSize = sizeof( msgParams );
  msgParams.dwStyle = uFlags | ( lpctszIconResource ? MB_USERICON : 0 );
  msgParams.hInstance = 0;//( lpctszIconResource_i ? AfxGetInstanceHandle() : 0 );
  msgParams.lpszCaption = lpctszTitle;
  msgParams.lpszText = lpctszMessage;
  msgParams.lpszIcon = lpctszIconResource;
  
  // Show message box
  return MessageBoxIndirect( &msgParams );
}

// Shows error dialog
INT ITS_ShowError( LPCTSTR lpctszErrorMessage, ... )
{
  va_list vaArgList;
  va_start( vaArgList, lpctszErrorMessage );
  
  TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };
  
  _vsntprintf( szMsgBuffer, COUNTOF_I( szMsgBuffer ), lpctszErrorMessage, vaArgList );

  return CustomMsgBox( szMsgBuffer, 
    _T( "Error!" ), 
    MB_OK | MB_ICONERROR, 
    GetForegroundWindow() );
}

// Shows warning dialog
INT ITS_ShowWarning( LPCTSTR lpctszWarningMessage, ... )
{
  va_list vaArgList;
  va_start( vaArgList, lpctszWarningMessage );
  
  TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };
  
  _vsntprintf ( szMsgBuffer, COUNTOF_I( szMsgBuffer ), lpctszWarningMessage, vaArgList );
  
  return CustomMsgBox( szMsgBuffer, 
    _T( "Warning!" ),
    MB_OK | MB_ICONWARNING,
    GetForegroundWindow() );
}


// Shows information dialog
INT ITS_ShowInfo( LPCTSTR lpctszInfoMessage, ... )
{
  va_list vaArgList;
  va_start( vaArgList, lpctszInfoMessage);
  
  TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };
  
  _vsntprintf ( szMsgBuffer, COUNTOF_I( szMsgBuffer ), lpctszInfoMessage, vaArgList );
  
  return CustomMsgBox( szMsgBuffer, 
    _T( "Info" ),
    MB_OK | MB_ICONINFORMATION,
    GetForegroundWindow() );
}

// Shows information dialog
BOOL ITS_ShowQuestion( LPCTSTR lpctszQuestionMsg, ... )
{
  va_list vaArgList;
  va_start( vaArgList, lpctszQuestionMsg );
  
  TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };
  
  _vsntprintf ( szMsgBuffer, COUNTOF_I( szMsgBuffer ), lpctszQuestionMsg, vaArgList );
  return CustomMsgBox( szMsgBuffer, 
    _T( "Confirm" ), 
    MB_YESNO | MB_ICONQUESTION,
    GetForegroundWindow() ) == IDYES ;
}

#if 1 
DWORD OutputDbgStr_i(LPCTSTR szFormat, ...) 
{
	va_list arglist;
  TCHAR szOutputDgbStr[1024]; 
  
  szOutputDgbStr[0] = _T('\0');

	va_start(arglist, szFormat);
	wvsprintf(szOutputDgbStr, szFormat, arglist); // Max is 1024.
	va_end(arglist);
  
  szOutputDgbStr[1024-1] = _T('\0');
  
  OutputDebugString(szOutputDgbStr);

  return 0;
}
#else
DWORD  OutputDbgStr_2(LPCTSTR szFormat, ...) 
{
  va_list arglist;
  TCHAR szOutputDgbStr[1024]; 

  // get local time
  SYSTEMTIME sysTime;
  ::GetLocalTime(&sysTime);

  szOutputDgbStr[0] = _T('\0');
  
  int nPos = _stprintf
    (
    szOutputDgbStr,
    _T("%02d:%02d:%02d.%03d: "), 
    sysTime.wHour,
    sysTime.wMinute,
    sysTime.wSecond,
    sysTime.wMilliseconds
    );
  
  if(nPos > sizeof(szOutputDgbStr))
    return ;
  
  va_start(arglist, szFormat);
  nPos += _vsntprintf(szOutputDgbStr+nPos, sizeof(szOutputDgbStr)-1-nPos, szFormat, arglist);
  // wvsprintf(szOutputDgbStr, szFormat, arglist); // Max is 1024.
  va_end(arglist);
  
  OutputDebugString(szOutputDgbStr);

  return 0;
}
#endif 

DWORD OutputDbgStrA_i(LPCSTR szFormat, ...) 
{
	va_list arglist;
  CHAR szOutputDgbStr[1024]; 
  
  szOutputDgbStr[0] = ('\0');

	va_start(arglist, szFormat);
	wvsprintfA(szOutputDgbStr, szFormat, arglist); // Max is 1024.
	va_end(arglist);
  
  OutputDebugStringA(szOutputDgbStr);

  return 0;
}