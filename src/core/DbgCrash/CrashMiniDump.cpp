//////////////////////////////////////////////////////////////////////////
// CrashMiniDump.cpp 
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "LogFile.h"
#include "utl.h"
#include "CrashMiniDump.h"
#include "CrashLog.h"

//#include <dbghelp.h>

//////////////////////////////////////////////////////////////////////////
// DumpMiniDump
//////////////////////////////////////////////////////////////////////////
static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	if (excpInfo == NULL) 
	{
		// Generate exception to get proper context in dump
		__try 
		{
			OutputDbgStr(_T("Raising exception(excpInfo=NULL)\r\n"));
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()), EXCEPTION_CONTINUE_EXECUTION) 
		{
		}
	} 
	else
	{
    OutputDbgStr(_T("Writing minidump...\r\n"));
    PFN_MINIDUMPWRITEDUMP pfnMiniDumpWrite = NULL;
    HINSTANCE hInstace = LoadLibrary(_T("dbghelp.dll"));
    if(hInstace)
    {
      pfnMiniDumpWrite = (PFN_MINIDUMPWRITEDUMP)GetProcAddress(hInstace, ("MiniDumpWriteDump"));
      if(pfnMiniDumpWrite)
      {
        MINIDUMP_EXCEPTION_INFORMATION eInfo;        
        eInfo.ThreadId          = GetCurrentThreadId();
        eInfo.ExceptionPointers = excpInfo;
        eInfo.ClientPointers    = FALSE;
        
        pfnMiniDumpWrite(
          GetCurrentProcess(),
          GetCurrentProcessId(),
          hFile,
          MiniDumpNormal,
          excpInfo ? &eInfo : NULL,
          NULL,
          NULL);
      }
      FreeLibrary(hInstace);
    }
    else
    {
      OutputDbgStr(_T("*** Fail to LoadLibrary 'dbghelp.dll', Error=%d\n"), GetLastError());
    }
	}
}

LONG DumpMiniDumpFile(PEXCEPTION_POINTERS pExceptPtrs)
{
  TCHAR* pszMiniDumpFile = GetFileFullPath(_T("crash\\CrashMini.dmp"));
  
  //OutputDbgStr(_T("MiniDump File=%s\n"), pszMiniDumpFile);
  
  // Force to writable.
  SetFileAttributes(pszMiniDumpFile, FILE_ATTRIBUTE_NORMAL);
  SaveFileName(NULL, pszMiniDumpFile);
  
  // Create the file
  HANDLE hMiniDumpFile = CreateFile (
    pszMiniDumpFile,
    GENERIC_WRITE,
    0,
    NULL,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, // write through 
    NULL);
  
  // Write the minidump to the file
  if (hMiniDumpFile != INVALID_HANDLE_VALUE)
  {
    DumpMiniDump(hMiniDumpFile, pExceptPtrs);
    
    // Close file
    CloseHandle(hMiniDumpFile);
  }
  else
  {
    DWORD dwLastErr = ::GetLastError();
    TCHAR szDumpFile[MAX_PATH];
    TCHAR szTimeBuffer[MAX_PATH]; 
    TCHAR szOldMiniDumpFile[MAX_PATH];
    
    szOldMiniDumpFile[0] = _T('\0');
    szDumpFile[0] = _T('\0');
    lstrcpy(szOldMiniDumpFile, pszMiniDumpFile);
    
    szTimeBuffer[0] = _T('\0');    
    ITS_GetCurrentTime(szTimeBuffer,2);    
    wsprintf(szDumpFile, _T("crash\\CrashMini_%s.dmp"), szTimeBuffer);
    pszMiniDumpFile = GetFileFullPath(szDumpFile);
    SetFileAttributes(pszMiniDumpFile, FILE_ATTRIBUTE_NORMAL);
    SaveFileName(NULL, pszMiniDumpFile);
    
    //OutputDbgStr(_T("*** Fail to Create File '%s', error=%d. Change name to '%s'.\n"), szOldMiniDumpFile, dwLastErr, pszMiniDumpFile), 
    
    hMiniDumpFile = CreateFile(
      pszMiniDumpFile,
      GENERIC_WRITE,
      0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, // write through 
      NULL);
    
    // Write the minidump to the file
    if (hMiniDumpFile != INVALID_HANDLE_VALUE)
    {
      DumpMiniDump(hMiniDumpFile, pExceptPtrs);
      
      // Close file
      CloseHandle(hMiniDumpFile);
    }
    else
    {
      //OutputDbgStr(_T("*** Fail to Create File '%s', error=%d.\n"), pszMiniDumpFile, GetLastError());
    }
  }
  
  return 0;
}

DWORD ShowCrashWindow(LPCTSTR pszFullAppName, LPCTSTR pszLogFile, LPCTSTR pszMiniDumpFile)
{
  if (IsDebuggerPresent())
  {
    // let the debugger catch this -
    // return the magic value which tells Win32 that this handler didn't
    // actually handle the exception - so that things will proceed as per
    // normal.
    OutputDbgStr(_T("*** The Debugger is present.\n"));
    return EXCEPTION_CONTINUE_SEARCH;
  }
  else
  {
    ///////////////////////////////////////////////////////////////////////
    //
    //  pop up our crash report app
    //
    ///////////////////////////////////////////////////////////////////////
    if(pszFullAppName == NULL)
    {
      OutputDbgStr(_T("*** Don't input the Crash Report Application Name.\n"));
      return EXCEPTION_CONTINUE_SEARCH;
    }
    
    TCHAR szCommandLine[MAX_PATH];
    lstrcpy(szCommandLine, pszFullAppName);
    lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
    lstrcat(szCommandLine, 	GetFileFullPath(NULL));
    lstrcat(szCommandLine, _T("\""));

    lstrcat(szCommandLine, _T(" \""));
    lstrcat(szCommandLine, pszLogFile);
    lstrcat(szCommandLine, _T("\""));

    lstrcat(szCommandLine, _T(" \""));
    lstrcat(szCommandLine, pszMiniDumpFile);
    lstrcat(szCommandLine, _T("\""));
    
    OutputDbgStr(_T("CommandLine:%s\n"), szCommandLine);    

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    if (CreateProcess(
      NULL,					 // name of executable module
      szCommandLine, // command line string
      NULL,					 // process attributes
      NULL,					 // thread attributes
      FALSE,				 // handle inheritance option
      0,					 	 // creation flags
      NULL,					 // new environment block
      NULL,					 // current directory name
      &si,					 // startup information
      &pi))					 // process information
    {
      // CrashReport.exe was successfully started, so
      // suppress the standard crash dialog
      return EXCEPTION_EXECUTE_HANDLER;
    }
    else
    {
      // CrashReport.exe was not started - let
      // the standard crash dialog appear
      return EXCEPTION_CONTINUE_SEARCH;
    }
  }  
}