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
#include <process.h>  
#include "core_common.h"

typedef VOID (*PFN_CONSOLEINPUTHANLDERA)(CONST CHAR szInputBuf[512], U16 nNumberOfCharsRead);
typedef VOID (*PFN_CONSOLEINPUTHANLDER)(CONST TCHAR szInputBuf[512], U16 nNumberOfCharsRead);

#define CHK_RET(bSuccess) { if(!bSuccess) { return GetLastError(); } } 
#define MAX_TITLE_SIZE 256 

PRIVATE U32 ConsoleDisableClose(VOID);

#if 0
bool NewThread(VOID* lpFunctionAddress, LPVOID pParam)
{
  HANDLE m_hThread  = NULL;
  DWORD m_dwThreadID = 0;
  
  m_hThread = CreateThread( NULL, 
                            0, // use default stack size  
                            (LPTHREAD_START_ROUTINE)lpFunctionAddress, // thread function 
                            pParam, // argument to thread function 
                            0,
                            &(m_dwThreadID)
                            );
  if(m_hThread) 
  {
    CloseHandle(m_hThread);
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}

bool WINAPI HandlerRoutine(
  DWORD dwCtrlType   //  control signal type
)
{
  bool bRet = FALSE;

  switch(dwCtrlType)
  {
  case CTRL_C_EVENT: // A CTRL+C signal was received
    break;
  case CTRL_BREAK_EVENT:
    break;
  case CTRL_CLOSE_EVENT:
    break;
  case CTRL_SHUTDOWN_EVENT:
    break;
  }

  return bRet;
}
#endif

static VOID ThreadControlling(VOID* pParam)
{
  DWORD cNumRead; 
  DWORD dwTryCnt = 0;
  
  if(pParam == NULL)
  {
    return ;
  }
  
  SetThreadPriority(GetCurrentProcess(), THREAD_PRIORITY_HIGHEST);
  
  while (1) 
  {
    // Loop to read and handle the input events. 
    HANDLE hStdConsole = GetStdHandle(STD_INPUT_HANDLE); 
    if(hStdConsole == NULL)
    {
      return ;
    }
    
    TCHAR szInputBuf[512+1];
    
    // Wait for the events. 
    //  reads keyboard input from a console's input buffer 
    szInputBuf[0] = _T('\0');
    cNumRead = 0;
    // wait until call ::FreeConsole().
    if(ReadConsole(hStdConsole, szInputBuf, 512, &cNumRead, NULL))
    {
      szInputBuf[512] = _T('\0');
      if(cNumRead != 0)
      {
        dwTryCnt = 0;
        szInputBuf[cNumRead] = _T('\0');
        ((CConsoleLog_i*)pParam)->Receiver(szInputBuf,cNumRead);
      }
      else 
      {
        U32 err = ::GetLastError();
        if(err == ERROR_OPERATION_ABORTED) // when close console,this error code wil return.
        {
          break; // exit thread now.
        }
      }
    }
    else 
    {
      U32 err = ::GetLastError();
      if(err == ERROR_INVALID_HANDLE)
      {
        break;
      }
      else 
      {
        if(dwTryCnt ++ > 100) // 100*50 ms = 5 Seconds
        {
          dwTryCnt = 0;
          break;
        }
        Sleep(50);
      }
    }

    
#if 0 
      // Wait for the events. 
    if (! ReadConsoleInput( 
          hConsole,      // input buffer handle 
          irInBuf,     // buffer to read into 
          128,         // size of read buffer 
          &cNumRead) ) // number of records read 
    {
      TRACE_I(_T("ReadConsoleInput Error: %d\n"), GetLastError());
    }
    
    // Dispatch the events to the appropriate handler. 
    for (i = 0; i < cNumRead; i++) 
    {
      switch(irInBuf[i].EventType) 
      { 
      case KEY_EVENT: // keyboard input 
        //KeyEventProc(irInBuf[i].Event.KeyEvent); 
        break; 
      
      case MOUSE_EVENT: // mouse input 
        //MouseEventProc(irInBuf[i].Event.MouseEvent); 
        break; 
        
      case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
        //ResizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent); 
        break; 
        
      case FOCUS_EVENT:  // disregard focus events 
        
      case MENU_EVENT:   // disregard menu events 
        break; 
        
      default: 
        // MyErrorExit("unknown event type"); 
        break; 
      } 
    }
#endif 
  } // while(1)
  
  return  ;
}

void CConsoleLog_i::Receiver(TCHAR szInputBuf[512], u32 nNumberOfCharsRead)
{
  UNREF_PARAM(szInputBuf);
  UNREF_PARAM(nNumberOfCharsRead);
  return ;
}


HANDLE CConsoleLog_i::GetConsoleHandle(VOID)
{
  return ::GetStdHandle(STD_INPUT_HANDLE); 
}


// 
// ERR_OK_I
// ERR_ALREADY_INITIALIZED_I
// ERR_WIN32_ERROR_I
// 
U32 CConsoleLog_i::Create(CONST TCHAR *szTitle, bool bNoClose, bool bLineInput, bool bAutoReceive)
{
	// Has console been already created?
	if(m_hConsole) 
  {
    return ERR_ALREADY_INITIALIZED_I;
  }  
  
  m_hConsole        = NULL;
  m_bShowConsole    = FALSE;
  
	// Allocate a new console
	if(!AllocConsole())
  {
    m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!m_hConsole)
    {
      return ERR_WIN32_ERROR_I;
    }
    else
    {
      if(szTitle != NULL)
      {
        SetConsoleTitle(szTitle);
      }
      m_bShowConsole = TRUE;
      return ERR_OK_I;
    }
  }
	else
  {
	  // Create the actual console
    m_hConsole = CreateFile(_T("CONOUT$"), GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	  if(m_hConsole == INVALID_HANDLE_VALUE)
		  return ERR_WIN32_ERROR_I;
  }
  
  if(bLineInput)
  {
	  SetConsoleMode(m_hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_LINE_INPUT );  // ENABLE_WINDOW_INPUT 
  }
  else
  { 
	  SetConsoleMode(m_hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT ); // ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT 
  }
	
	// if true, disable the [x] button of the console
	if(bNoClose)
		ConsoleDisableClose();
	
	// set the console title
	if(szTitle != NULL)
  {
    SetConsoleTitle(szTitle);
  }
  
  //SetConsoleCtrlHandler(HandlerRoutine, TRUE);
  
  if(bLineInput && bAutoReceive)
  {
    if(_beginthread(ThreadControlling, NULL, this) != -1)
    {
      ;
    }
  }
  
  m_bShowConsole = TRUE;
  
  return ERR_OK_I;
}

// Specifies a COORD structure containing the new size, in rows and columns, of the screen buffer
U32 CConsoleLog_i::SetScreenBufferSize(COORD dwSize)
{
  u32 ret = ERR_OK_I;
  
  if(m_hConsole)
  {
    if(::SetConsoleScreenBufferSize(m_hConsole, dwSize))
    {
      return ERR_OK_I;
    }
    else 
    {
      ret =  ERR_WIN32_ERROR_I;
    }
  }
  else 
  {
    ret = ERR_NO_INITIALIZATION_I; 
  }
  
  return ret;
}

// Specifies a COORD structure containing the new size, in rows and columns, of the screen buffer
U32 CConsoleLog_i::SetScreenSize(COORD coBufferSize, COORD coWinSize)
{
  u32 ret = ERR_OK_I;
  
  if(m_hConsole)
  {
    COORD  WinSize = {0};
    WinSize = GetLargestConsoleWindowSize(m_hConsole);
    
    SMALL_RECT srctWindow;
    if(coWinSize.X && coWinSize.Y)
    {
      // Set srctWindow to the current window size and location.  
      srctWindow.Bottom = min(coWinSize.Y, WinSize.Y);
      srctWindow.Right  = min(coWinSize.X, WinSize.X);
      srctWindow.Left   = 0;
      srctWindow.Top    = 0;
      
      if (! SetConsoleWindowInfo( 
        m_hConsole,      // screen buffer handle 
        TRUE,         // absolute coordinates 
        &srctWindow)) // specifies new location 
      {
        ret =  ERR_WIN32_ERROR_I;
      } 
    }
    
    if(::SetConsoleScreenBufferSize(m_hConsole, coBufferSize))
    {
      ret = ERR_OK_I;
    }
    else 
    {
      ret =  ERR_WIN32_ERROR_I;
    }
  }
  else 
  {
    ret = ERR_NO_INITIALIZATION_I;
  }
  
  return ret;
}


inline TCHAR* GetTitle()
{
  //
	// Retrieves the title of console.
  //
	static TCHAR szWindowTitle[MAX_TITLE_SIZE] = _T("");
	GetConsoleTitle(szWindowTitle, sizeof(szWindowTitle));
  
	return szWindowTitle;
}

inline HWND GetHWND()
{
	// try to find our console window and return its HWND
	return FindWindow(_T("ConsoleWindowClass"), GetTitle());
}

//
// Disable the close button.
//
PRIVATE U32 ConsoleDisableClose()
{
	HWND hWnd = GetHWND();
	
  //
	// Disable the [x] button if we found this console.
  //
	if(hWnd != NULL)
	{
		HMENU hMenu = GetSystemMenu(hWnd,0);
		if(hMenu != NULL)
		{
			DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			DrawMenuBar(hWnd);
		}
    else
    {
      return ERR_WIN32_ERROR_I;
    }
	}
  else
  {
    return ERR_NO_INITIALIZATION_I;
  }

  return ERR_OK_I;
}


U32 CConsoleLog_i::Clear(VOID)
{
	if(!m_hConsole) 
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  //
  // This code is copied from one of Microsoft's knowledge base(Q99261) articles.
  //  
  COORD coordScreen = { 0, 0 };
  
  BOOL bSuccess;
  DWORD cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD dwConSize;
  
  // get the number of character cells in the current buffer
  bSuccess = GetConsoleScreenBufferInfo( m_hConsole, &csbi );
  CHK_RET(bSuccess);
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  
  // fill the entire screen with blanks
  bSuccess = FillConsoleOutputCharacter( m_hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten );
  CHK_RET(bSuccess);
  
  // get the current text attribute
  bSuccess = GetConsoleScreenBufferInfo( m_hConsole, &csbi );
  CHK_RET(bSuccess);
  
  // now set the buffer's attributes accordingly
  bSuccess = FillConsoleOutputAttribute( m_hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten );
  CHK_RET(bSuccess);
  
  // put the cursor at (0, 0)
  bSuccess = SetConsoleCursorPosition( m_hConsole, coordScreen );
  CHK_RET(bSuccess);
  
  return 0;
}


// wColor Combine the following value.
// FOREGROUND_GREEN | FOREGROUND_INTENSITY  FOREGROUND_RED FOREGROUND_BLUE
// BACKGROUND_GREEN | BACKGROUND_INTENSITY  BACKGROUND_RED BACKGROUND_BLUE
U32 CConsoleLog_i::Color(WORD wColor/*FOREGROUND_GREEN | FOREGROUND_INTENSITY*/)
{
	if(!m_hConsole) 
  {
    return ERR_NO_INITIALIZATION_I;
  }

  BOOL bRet = FALSE;

	// no color specified, reset to defaults (white font on black background)
	if(wColor != NULL)
		bRet = SetConsoleTextAttribute(m_hConsole, wColor );
	else
		bRet = SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // white text on black bg
  
  return (bRet==TRUE)?ERR_OK_I:ERR_WIN32_ERROR_I;
}

// show/hide the console
U32 CConsoleLog_i::Show(bool bShow)
{
	if(!m_hConsole) 
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  //
	// Get out console's HWND and show/hide the console.
  //
	HWND hWnd = GetHWND();
	if(hWnd != NULL)
  {
    BOOL ret = ShowWindow(hWnd, SW_HIDE ? SW_SHOW : bShow);
    if(ret == 0) // previously  hide
    {
      m_bShowConsole = TRUE;
      return ERR_WIN32_ERROR_I;
    }
    else
    {
      // previously visible
      m_bShowConsole = FALSE;
    }
  }
  
  return ERR_OK_I;
}


// ERR_OK_I
// ERR_WIN32_ERROR_I
// Close the console
U32 CConsoleLog_i::Close(VOID)
{  
  //
  // Free the console, now all ConsoleOutput APIs can't be used anymore until we Create() it again.
  //
  if(!FreeConsole())
  {
    return ERR_WIN32_ERROR_I;
  }
  
  m_bAutoConfig  = FALSE;
  m_hConsole     = NULL;    
  
  return ERR_OK_I;
}

void CConsoleLog_i::operator()(LPCTSTR pszFormat, ...)
{
  if(m_hConsole == NULL)
  {
    return ;
  }
  
  if(pszFormat == NULL)
  {
    return ;
  }
  
  if(m_bShowConsole == FALSE)
    return ;
  
  DWORD dwRet = ERR_OK_I;
  TCHAR szOutput[1024] = {0, };
  int cnt = 0;
  va_list argptr;
  
  //ZeroMemory(szOutput, sizeof(szOutput));
  
  va_start(argptr, pszFormat);
  cnt = wvsprintf(szOutput, pszFormat, argptr);
  va_end(argptr);
  szOutput[1024-1] = _T('\0');
  
  if(cnt == 0)
  {
    return;
  }
  
  DWORD cCharsWritten = 0;
  
  WriteConsole(m_hConsole, szOutput, cnt, &cCharsWritten, NULL);
  if(!cCharsWritten)
  {
    dwRet = ERR_WIN32_ERROR_I; //GetLastError();
  }  
  else
  {
    dwRet = ERR_OK_I;
  } 
}


//
// ERR_OK_I 
// ERR_INVALID_PARAM_I
// ERR_WIN32_ERROR_I
// ERR_BUSY_I
// ERR_NO_INITIALIZATION_I 
// 
U32 CConsoleLog_i::Print(CONST TCHAR * pFormat, ...)
{ 
  if(m_hConsole == NULL)
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  if(pFormat == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  if(m_bShowConsole == FALSE)
  {
    return ERR_NO_OWNER_RECIPIENT;
  }
  
  DWORD dwRet = ERR_OK_I;
  TCHAR szOutput[1024];
  int cnt = 0;
  va_list argptr;
  
  ZeroMemory(szOutput, sizeof(szOutput));
  
  va_start(argptr, pFormat);
  cnt = wvsprintf(szOutput, pFormat, argptr);
  va_end(argptr);
  szOutput[1024-1] = _T('\0');
  
  if(cnt == 0)
  {
    return ERR_NO_MORE_DATA_I;
  }
  
  DWORD cCharsWritten = 0;
  
  WriteConsole(m_hConsole, szOutput, cnt, &cCharsWritten, NULL);
  if(!cCharsWritten)
  {
    dwRet = ERR_WIN32_ERROR_I; 
  }  
  else
  {
    dwRet = ERR_OK_I;
  }
  
  return dwRet;
}


U32 CConsoleLog_i::PrintA(CONST CHAR * pFormat, ...)
{
  if(m_hConsole == NULL)
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  if(pFormat == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
    
  if(m_bShowConsole == FALSE)
    return ERR_NO_OWNER_RECIPIENT;

  DWORD dwRet = ERR_OK_I;
  CHAR szOutput[1024+1] = {0, };
  int cnt = 0;
  va_list argptr;
  
  ZeroMemory(szOutput, sizeof(szOutput));
  
  va_start(argptr, pFormat);
  cnt = wvsprintfA(szOutput, pFormat, argptr);
  va_end(argptr);
  szOutput[1024-1] = '\0';

  if(cnt == 0)
  {
    return ERR_NO_MORE_DATA_I;
  }
  
  DWORD cCharsWritten = 0;
  
  WriteConsoleA(m_hConsole, szOutput, cnt, &cCharsWritten, NULL);
  if(!cCharsWritten)
  {
    dwRet = ERR_WIN32_ERROR_I; //GetLastError();
  }  
  else
  {
    dwRet = ERR_OK_I;
  }
  
  return dwRet;
}


U32 CConsoleLog_i::Output(CONST VOID *lpBuffer, DWORD nNumberOfCharsToWrite, DWORD* pNumberOfCharsWritten)
{
  DWORD dwRet = 0;
  
  if(m_hConsole == NULL)
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  if(m_bShowConsole == FALSE)
  {
    return ERR_NO_OWNER_RECIPIENT;
  }

  if(lpBuffer == NULL || nNumberOfCharsToWrite == 0)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  DWORD cCharsWritten = 0;
  WriteConsole(m_hConsole, lpBuffer, nNumberOfCharsToWrite, &cCharsWritten, NULL);
  if(pNumberOfCharsWritten)
    *pNumberOfCharsWritten = cCharsWritten;
  
  if(cCharsWritten != nNumberOfCharsToWrite)
  {
    dwRet = ERR_WIN32_ERROR_I; //GetLastError();
  }
  else
  {
    dwRet = ERR_OK_I;
  }
  
  return dwRet;
}


PRIVATE WORD GetColor(HANDLE hConsole)
{
  BOOL bSuccess;
  //DWORD cCharsWritten;

  CONSOLE_SCREEN_BUFFER_INFO csbi = {0, };
  //DWORD dwConSize;
  
  // get the number of character cells in the current buffer
  bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
  if(bSuccess)
  {
    return csbi.wAttributes;
  }
  else 
  {
    return 0;
  }
}

U32 CConsoleLog_i::OutputStyledText(CONST VOID *lpBuffer, 
                                    WORD wColor,
                                    DWORD nNumberOfCharsToWrite, 
                                    DWORD* pNumberOfCharsWritten
                                    )
{
  DWORD dwRet = 0;
   
  if(m_hConsole == NULL)
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  if(m_bShowConsole == FALSE)
  {
    return ERR_NO_OWNER_RECIPIENT;
  }

  if(lpBuffer == NULL || nNumberOfCharsToWrite == 0)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  DWORD cCharsWritten = 0;
  WORD wPreColor = GetColor(m_hConsole);
  
  Color(wColor);
  WriteConsole(m_hConsole, lpBuffer, nNumberOfCharsToWrite, &cCharsWritten, NULL);
  if(pNumberOfCharsWritten)
    *pNumberOfCharsWritten = cCharsWritten;
  
  if(cCharsWritten != nNumberOfCharsToWrite)
  {
    dwRet = ERR_WIN32_ERROR_I; //GetLastError();
  }
  else
  {
    dwRet = ERR_OK_I;
  }
  Color(wPreColor);
  
  return dwRet;
}


U32 CConsoleLog_i::OutputA(CONST VOID *lpBuffer, DWORD nNumberOfCharsToWrite, DWORD* lpNumberOfCharsWritten)
{
  DWORD dwRet = 0;
  
  if(m_hConsole == NULL)
    return ERR_NO_INITIALIZATION_I;
  
  if(lpBuffer == NULL || nNumberOfCharsToWrite == 0)
    return ERR_INVALID_PARAM_I;
    
  if(m_bShowConsole == FALSE)
    return ERR_NO_OWNER_RECIPIENT;
  
  DWORD cCharsWritten = 0;
  WriteConsoleA(m_hConsole, lpBuffer, nNumberOfCharsToWrite, &cCharsWritten, NULL);
  
  if(lpNumberOfCharsWritten)
    *lpNumberOfCharsWritten = cCharsWritten;
  if(cCharsWritten != nNumberOfCharsToWrite)
  {
    dwRet = ERR_WIN32_ERROR_I;
  }
  else
  {
    dwRet = ERR_OK_I;
  }

  return dwRet;
}


bool CConsoleLog_i::WaitInput(PVOID pBuffer,    // data buffer
                              U32 nNumberOfCharsToRead, // number of characters to read
                              U32* pNumberOfCharsRead   // number of characters read
                              )
{
  HANDLE hStdConsole =::GetStdHandle(STD_INPUT_HANDLE);
  
  if (hStdConsole == NULL || pBuffer == NULL || nNumberOfCharsToRead == 0)
  {
    TRACE_I(_T("*** Fail to Find Console Handle\n"));
    return FALSE;
  }
  
  // Wait for the events. 
  //  reads keyboard input from a console's input buffer 
  if (::ReadConsole(hStdConsole, pBuffer, nNumberOfCharsToRead, pNumberOfCharsRead, NULL))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

bool CConsoleLog_i::IsInput(VOID)
{
  DWORD dwEvents     = 0;
  HANDLE hStdConsole =::GetStdHandle(STD_INPUT_HANDLE);
  DWORD recRead      = 0;
  
  //FlushConsoleInputBuffer(::GetStdHandle(STD_INPUT_HANDLE));
  GetNumberOfConsoleInputEvents(hStdConsole, &dwEvents);
  
  INPUT_RECORD pRec[50];
  
  BOOL bRet = PeekConsoleInput(hStdConsole,   // handle to console input buffer
    pRec,  // peek data
    50,    // number of records to read
    &recRead   // number of records read
    );
  
  UNREF_PARAM(bRet);

  for (DWORD i = 0; i< recRead; i++)
  {
      if(pRec[i].EventType == KEY_EVENT && pRec[i].Event.KeyEvent.bKeyDown) 
      {
        return TRUE;
      }
  }

  return FALSE;
}

bool CConsoleLog_i::WaitInputA(PVOID pBuffer,   // data buffer
                           U32 nNumberOfCharsToRead,    // number of characters to read
                           U32* pNumberOfCharsRead  // number of characters read
                           )
{
  HANDLE hStdConsole = ::GetStdHandle(STD_INPUT_HANDLE);
  
  if (hStdConsole == NULL || pBuffer == NULL || nNumberOfCharsToRead == 0)
  {
    TRACE_I(_T("*** Fail to Find Console Handle\n"));
    return FALSE;
  }
  
  // Wait for the events. 
  //  reads keyboard input from a console's input buffer 
  if (::ReadConsoleA(hStdConsole, pBuffer, nNumberOfCharsToRead, pNumberOfCharsRead, NULL))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


CConsoleLog_i::CConsoleLog_i(bool bAutoConfig, LPCTSTR pszTitleName)
{
  m_hConsole = NULL;
  m_bAutoConfig = bAutoConfig;
  
  if(m_bAutoConfig)
  {
    //CFileAssoc_i fileAssoc;
    //m_FileType = fileAssoc.GetExeType(NULL);
    
    CString strTitle;
    SYSTEMTIME tm = {0, };

    GetLocalTime(&tm);
    
    strTitle.Format(_T("%s %04d-%02d-%02d %02d:%02d:%2d"), (pszTitleName==NULL)?_T("Logging Console"):pszTitleName, 
      tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute,tm.wMilliseconds);
    
    Create(strTitle, FALSE, FALSE, FALSE);
  }
}


CConsoleLog_i::~CConsoleLog_i()
{
#if 0 
  if(m_bAutoConfig)
  {
    Close();
  }
  m_hConsole     = NULL;
#endif 
  
  m_bShowConsole = FALSE;
}
 
