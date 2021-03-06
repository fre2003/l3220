#undef _WIN32_WINDOWS 
#define _WIN32_WINDOWS 0x0500
#include "stdafx.h"
#include "dc_DbgExp.h"
#include "dc_utl.h"
#include "dc_LogFile.h"
#include "dc_CrashLog.h"
#include "shlwapi.h"
#include "..\core20\WalkStack_20.h"
#include "../../inc/ImageHlp.h"
#include "dc_CrashDumpFile.h"

#pragma comment(lib, "shlwapi.lib")

#if 0 
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif 

//////////////////////////////////////////////////////////////////////////

typedef void (*pfnException)(CONST TCHAR* pszLogFile, CONST TCHAR* pszDumpFile);

PRIVATE pfnException g_pfnException    = NULL;
PRIVATE BOOL g_bCrashHeader            = FALSE;
PRIVATE BOOL g_bCallStack              = FALSE;
PRIVATE DWORD g_dwMiniDump               = FALSE;
PRIVATE BOOL g_bCrashContextStack      = FALSE;
PRIVATE BOOL g_bModuleList             = FALSE;
PRIVATE TCHAR g_szLogFile[MAX_PATH]    = { 0, };
PRIVATE TCHAR g_szDumpFile[MAX_PATH]   = { 0, };
PRIVATE TCHAR g_szpszSymPath[MAX_PATH] = { 0, };

#define CHECK_RECURSIVE()\
{\
	static BOOL bFirstTime = TRUE;\
	if (!bFirstTime)\
		return EXCEPTION_CONTINUE_SEARCH;\
	bFirstTime = FALSE;\
}\

LPSTR WINAPI UniCode2Ansi(LPSTR lpa, LPCWSTR lpw, int nChars)
{
	// verify that no illegal character present
	// since lpa was allocated based on the size of lpw
	// don't worry about the number of chars
	lpa[0] = '\0';
	WideCharToMultiByte(CP_ACP, 0, lpw, -1, lpa, nChars, NULL, NULL);
	return lpa;
}

LPWSTR WINAPI Ansi2Unicode(LPWSTR lpw, LPCSTR lpa, int nChars)
{
	// verify that no illegal character present
	// since lpw was allocated based on the size of lpa
	// don't worry about the number of chars
	lpw[0] = L'\0';
	MultiByteToWideChar(CP_ACP, 0, lpa, -1, lpw, nChars);
	return lpw;
}

#define EXP_EXCEPTION_FLAG     0
#define EXP_RAISE_FLAG         1

typedef DWORD (*PFN_EXCEPTION_CALLBACK)(DWORD dwFlag, PVOID pUsrData);
#include "..\core20\WalkStack_20.h"

// Simple implementation of an additional output to the console:
class ExpStackWalker : public StackWalker
{
public:
  ExpStackWalker(DWORD dwProcessId, HANDLE hProcess, LPCTSTR szSymPath, LPCTSTR pszDbgHelpLib, HANDLE hLogFile) 
    : StackWalker(dwProcessId, hProcess, szSymPath, pszDbgHelpLib)
  {
    m_hLogFile = hLogFile;
    dc_LogFilePrint(m_hLogFile, _T("Dump Call Stack:\r\n"));
  }

  virtual void OnOutput(LPCSTR szText) 
  {
    // StackWalker::OnOutput(szText);
  }

  virtual void OnOutputCallStack(LPCSTR szText) 
  { 
    dc_LogFilePrint(m_hLogFile, _T("%s"), szText);
    StackWalker::OnOutput(szText); 
  }
  
private:
  HANDLE m_hLogFile;
};


VOID DumpWalkStack(EXCEPTION_POINTERS* pExp, HANDLE hLogFile)
{
  TCHAR szDbgFilePath[MAX_PATH] = {0, };
  if (GetModuleFileName(NULL, szDbgFilePath, COUNTOF(szDbgFilePath)-2) <= 0)
  {
    dc_OutputDbgStr(_T("*** Fail to GetModuleFileName. Error=%d\n"), GetLastError());
  }

  TCHAR *lastperiod = lstrrchr(szDbgFilePath, _T('\\'));
  if (lastperiod)
  {
    lastperiod[0] = 0;
  }
  lstrcat(szDbgFilePath, _T("\\dbghelp.dll"));
  
#ifdef _DEBUG
  dc_OutputDbgStr(_T("WalkStack: %s, %s \n"), g_szpszSymPath, szDbgFilePath);
#endif 
  
  ExpStackWalker sw(GetCurrentProcessId(), GetCurrentProcess(), g_szpszSymPath, szDbgFilePath, hLogFile);
  sw.ShowCallstack(GetCurrentThread(), pExp->ContextRecord);
}

void QuickDumpHead( EXCEPTION_POINTERS* pExPtrs )
{
  PEXCEPTION_RECORD Exception = pExPtrs->ExceptionRecord;
  PCONTEXT Context            = pExPtrs->ContextRecord;
  
  TCHAR szCrashModulePathName[MAX_PATH* 2];
  ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));  
  TCHAR* pszCrashModuleFileName = _T("Unknown");  
  MEMORY_BASIC_INFORMATION MemInfo;  
  if (VirtualQuery((void*)Context->Eip, &MemInfo, sizeof(MemInfo)) &&
    (GetModuleFileName((HINSTANCE) MemInfo.AllocationBase,
    szCrashModulePathName, sizeof(szCrashModulePathName) - 2) > 0))
  {
    pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
  }
  
  dc_OutputDbgStr(_T("*** Caused %s (0x%08x) in module %s at %04x:%08x. *** \r\n"), 
    GetExceptionDescription(Exception->ExceptionCode),
    Exception->ExceptionCode, pszCrashModuleFileName, Context->SegCs, Context->Eip);
}

//////////////////////////////////////////////////////////////////////////
// CrashHandlerExceptionFilter
//////////////////////////////////////////////////////////////////////////
LONG CrashHandlerExceptionFilter_i(EXCEPTION_POINTERS* pExPtrs)
{
  LONG ret = 0;
  
  CHECK_RECURSIVE();

  dc_OutputDbgStr(_T("**** Crash Occur(Code:0x%08x, Address:0x%08x) ****\n"), 
    pExPtrs->ExceptionRecord->ExceptionCode, pExPtrs->ExceptionRecord->ExceptionAddress);
  
  if(g_dwMiniDump == 1)
  {
    dc_CreateMiniDump(pExPtrs, g_szDumpFile);
  }
  else if(g_dwMiniDump == 2)
  {
    dc_CreateMaxDump(pExPtrs, g_szDumpFile);
  }

  QuickDumpHead(pExPtrs);   
  
  InitModulePath(NULL);
  
  if(pExPtrs == NULL)
  {
    dc_OutputDbgStr(_T("**** EXCEPTION_POINTERS is NULL.\n"));
    return 0;
  }
  
  dc_OutputDbgStr(_T("Crash Log File:%s\n"), g_szLogFile);
  
  HANDLE hLogFile = dc_LogFileOpen(g_szLogFile);
  if (hLogFile == INVALID_HANDLE_VALUE)
	{
    DumpExceptionHeader(pExPtrs, NULL);
		dc_OutputDbgStr(_T("*** Error : Fail to create exception log file.\r\n"));
		return EXCEPTION_CONTINUE_SEARCH;
	}
  
  if(g_bCrashHeader)
  {
    DumpCrashHeader(pExPtrs, hLogFile);
  }
  if(g_bCallStack)
  {
    DumpWalkStack(pExPtrs, hLogFile);
  }
  
  if(g_bCrashContextStack)
  {
    DumpCrashStack(pExPtrs, hLogFile);
  }
  
  if(g_bModuleList)
  {
    DumpModuleListLog(hLogFile);
  }
  
  dc_LogFilePrint(hLogFile, _T("\r\n===== [end of %s] =====\r\n"), g_szLogFile);
  dc_LogFileFlush(hLogFile); 
  dc_LogFileClose(hLogFile);
  
  ret = EXCEPTION_EXECUTE_HANDLER;
  
  if(g_pfnException)
  {
    g_pfnException(g_szLogFile, g_szDumpFile);
  }
  
  return ret;
}

//////////////////////////////////////////////////////////////////////////
// InitException
//////////////////////////////////////////////////////////////////////////
DWORD InitExceptionHandler_i(bool bCrashHeader,
                             bool bCallStack,
                             DWORD dwMiniDump,
                             bool bCrashContextStack,
                             bool bModuleList,
                             CONST CHAR* pszCrashLogFile,
                             CONST CHAR* pszCrashDumpFile,
                             CHAR* pszSymPath,
                             PVOID pfnExceptionHandlerEx)
{
  g_bCrashHeader       = bCrashHeader;
  g_bCallStack         = bCallStack;
  g_dwMiniDump         = dwMiniDump;
  g_bCrashContextStack = bCrashContextStack;
  g_bModuleList        = bModuleList; 
  
  if(pszCrashLogFile)
    lstrcpyA(g_szLogFile, pszCrashLogFile);
  else 
    lstrcpyA(g_szLogFile, "C:\\crash.log");
  
  if(pszCrashDumpFile)
    lstrcpyA(g_szDumpFile, pszCrashDumpFile);
  else 
    lstrcpyA(g_szDumpFile, "C:\\crash.dmp");
  
  if (IsDebuggerPresent()) 
  {
    return DWORD(-1);
  }
  
  g_szpszSymPath[0] = '\0';
  
  if(bCallStack && pszSymPath)
  {
    lstrcpy(g_szpszSymPath, pszSymPath);
  }
  
  g_pfnException = (pfnException)pfnExceptionHandlerEx;
  
  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashHandlerExceptionFilter_i);
  
  return 0;
}
