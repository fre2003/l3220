// hprintf behaves similarly to printf, with a few vital differences.
// It uses wvsprintf to do the formatting, which is a system routine,
// thus avoiding C run time interactions. For similar reasons it
// uses WriteFile rather than fwrite.
// The one limitation that this imposes is that wvsprintf, and
// therefore hprintf, cannot handle floating point numbers.

// Too many calls to WriteFile can take a long time, causing
// confusing delays when programs crash. Therefore I implemented
// a simple buffering scheme 

#include "stdafx.h"
#include "dc.h"

#define HPRINTF_BUFFER_SIZE (1024*3)				// must be at least 2048

static TCHAR g_szLogBuf[HPRINTF_BUFFER_SIZE];	// wvsprintf never prints more than one K.
static int  g_LogBufIdx = 0;
static TCHAR g_OutputDgbStr[1024];

#define DBG_EXP_HEAD  _T("[dbgexp] ") 

// Print to Debugger.
VOID dc_OutputDbgStr(LPCTSTR Format, ...)
{
	va_list arglist;
  
  g_OutputDgbStr[0] = _T('\0');
	va_start( arglist, Format);

  lstrcpy(g_OutputDgbStr, DBG_EXP_HEAD);

	wvsprintf(&g_OutputDgbStr[sizeof(DBG_EXP_HEAD)-1], Format, arglist);
  OutputDebugString(g_OutputDgbStr);

	va_end(arglist);
}

//
// IN: the full path file name.
// 
HANDLE dc_LogFileOpen(CONST TCHAR* pszLogFile)
{
  HANDLE hLogFile = NULL;
  
  SetFileAttributes(pszLogFile,FILE_ATTRIBUTE_NORMAL);
  
  hLogFile = CreateFile(pszLogFile, GENERIC_WRITE, 0, 0,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);
  
  return hLogFile;
}

// Force to flush all data in 'g_szLogBuf' to disk.
VOID dc_LogFileFlush(HANDLE LogFile)
{
	if (g_LogBufIdx > 0)
	{
		DWORD NumBytes;
		WriteFile(LogFile, g_szLogBuf, lstrlen(g_szLogBuf), &NumBytes, 0);
		g_LogBufIdx = 0;
	}
}

// Write specified textg to 'cache'.
VOID dc_LogFileWrite(HANDLE LogFile, LPCTSTR pszMsg)
{
	if (g_LogBufIdx > (HPRINTF_BUFFER_SIZE-1024))
	{
		DWORD NumBytes;
		WriteFile(LogFile, g_szLogBuf, lstrlen(g_szLogBuf), &NumBytes, 0);
		g_LogBufIdx = 0;
	}
  
  TCHAR* pRet = lstrcpyn(&g_szLogBuf[g_LogBufIdx], pszMsg, sizeof(g_szLogBuf)-g_LogBufIdx);
  DWORD len = lstrlen(g_szLogBuf) - g_LogBufIdx;
  
	g_LogBufIdx += len;
}

VOID dc_LogFilePrint(HANDLE LogFile, LPCTSTR Format, ...)
{
	if (g_LogBufIdx > (HPRINTF_BUFFER_SIZE-1024))
	{
		DWORD NumBytes;
		WriteFile(LogFile, g_szLogBuf, lstrlen(g_szLogBuf), &NumBytes, 0);
		g_LogBufIdx = 0;
	}

	va_list arglist;
	va_start( arglist, Format);
	g_LogBufIdx += wvsprintf(&g_szLogBuf[g_LogBufIdx], Format, arglist);
	va_end( arglist);
}


VOID dc_LogFileClose(HANDLE hLogFile)
{
  CloseHandle(hLogFile);
}