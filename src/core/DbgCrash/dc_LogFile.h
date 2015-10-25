#ifndef __DC_LOG_FILE_H__
#define __DC_LOG_FILE_H__

HANDLE dc_LogFileOpen(CONST TCHAR* pszLogFile);
VOID dc_LogFileWrite(HANDLE LogFile, LPCTSTR pszMsg);
VOID dc_LogFilePrint(HANDLE LogFile, LPCTSTR Format, ...);
VOID dc_LogFileFlush(HANDLE LogFile);
VOID dc_LogFileClose(HANDLE hLogFile);

VOID dc_OutputDbgStr(LPCTSTR Format, ...);

#endif // __LOG_FILE_H__
