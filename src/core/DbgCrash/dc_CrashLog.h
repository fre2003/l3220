#ifndef __DC_CRASH_LOG_H__
#define __DC_CRASH_LOG_H__

LONG DumpCrashHeader(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile);
VOID DumpCrashStack(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile);
VOID DumpModuleListLog(HANDLE hLogFile);

LONG DumpExceptionHeader(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile);
CONST TCHAR *GetExceptionDescription(DWORD ExceptionCode);


#endif // __CRASH_LOG_H__
