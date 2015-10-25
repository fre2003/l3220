
// Win32 DLL Project.

-----------------------------
HOW TO USE the interface.

__declspec(dllexport) DWORD InitExceptionHandler_i(BOOL bCrashLog, BOOL bMiniDump, PVOID pfnExceptionHandlerEx);

DWORD InitExceptionHandler_i(bool bCrashHeader,
                             bool bCallStack,
                             bool bMiniDump,
                             bool bCrashContextStack,
                             bool bModuleList,
                             CHAR* pszSymPath,
                             PVOID pfnExceptionHandlerEx)

/def:".\dbgexp.def" 


// 
// Test the interface by Load Library.
// 
void test2()
{
  typedef VOID (*PFN_INITEXCEPTION)(bool bCrashHeader,
                             bool bCallStack,
                             DWORD dwMiniDump, // 1: MiniDump; 1: MaxDump
                             bool bCrashContextStack,
                             bool bModuleList,
                             CHAR* pszSymPath,
                             PVOID pfnExceptionHandlerEx); // typedef void (*pfnException)(TCHAR* pszCrashLogFile, TCHAR* pszDumpFile);
  
  PFN_INITEXCEPTION pfnInitExceptionHandler = NULL;
  HINSTANCE hInstace = LoadLibrary(_T("its_dbgexpUD.dll"));
  
  pfnInitExceptionHandler = (PFN_INITEXCEPTION )GetProcAddress(hInstace, ("ITS_InitExceptionHandler"));
  if(pfnInitExceptionHandler)
  {
    pfnInitExceptionHandler(0,0, NULL);
  }
  else
  {
    DWORD dwError = GetLastError();
  }
  
  CrashTestFunction(0);

//   if(hInstace)
//   {
//     FreeLibrary(hInstace);
//   }
}

