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

PRIVATE TCHAR g_pszInstallPath[MAX_PATH*2] = {0, };

#ifdef ITS_HAVE_LOCK
PRIVATE HANDLE m_hWriteMutex = NULL;

U32 LockInit(VOID)
{
  m_hWriteMutex = CreateMutex(NULL,   // no security attributes
    FALSE,   // initially not owned
    _T("ITS_MUTEX_NAME_2008_1120_WINDOWS")); // mutex name

  if (!m_hWriteMutex)
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  return ERR_OK_I;
}

// 
// Lock() - private function
// 
// Purpose:     Wait for write mutex.
// 
// Parameters:  dwTimeOut - [in] specifies the time-out interval, in
// milliseconds
// 
// 
U32 ITS_Lock(DWORD dwTimeOut)
{
  if (!m_hWriteMutex)
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  DWORD dwWaitResult = WaitForSingleObject(m_hWriteMutex, dwTimeOut);
  if (dwWaitResult == WAIT_OBJECT_0)
  {
    return ERR_OK_I;
  }
  else
  {
    return ERR_TIMEOUT_I;
  }
}


// Purpose:     Unlock write mutex.
// 
// Parameters:  None
// 
U32 Unlock()
{
  if (!m_hWriteMutex)
  {
    return ERR_NO_INITIALIZATION_I;
  }
  
  ReleaseMutex(m_hWriteMutex);
  
  return ERR_OK_I;
}

U32 LockExit()
{
  if (m_hWriteMutex)
  {
    ::CloseHandle(m_hWriteMutex);
  }
  
  m_hWriteMutex = NULL;

  return ERR_OK_I;
}
#endif // ITS_HAVE_LOCK 

PRIVATE TCHAR* lstrrchr(LPCTSTR string, int ch)
{
	TCHAR *start = (TCHAR *)string;

	while (*string++)                       /* find end of string */
		;
											/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;

	return NULL;
}

//
// GetFilePart
// 
#if 0 
PRIVATE TCHAR * GetFilePart(LPCTSTR source)
{
	TCHAR *result = lstrrchr(source, _T('\\'));
	if (result)
		result++;
	else
		result = (TCHAR *)source;
	return result;
}
#endif 

static TCHAR* g_pszFolder = NULL; // C:\install\bin\.dll --> C:\install\bin

#if 0 
PRIVATE TCHAR* GetFileName(TCHAR* szFileName)
{
	lstrcpy(szFileName, g_pszFolder);
	TCHAR *lastperiod = lstrrchr(szFileName, _T('.'));
	if (lastperiod)
  {
    lastperiod[0] = 0;
  }

  return szFileName;
}
#endif 

PRIVATE TCHAR* GetFileFolder(TCHAR* szFileName)
{
	//lstrcpy(szFileName, g_pszFolder);
	TCHAR *lastperiod = lstrrchr(szFileName, _T('\\'));
	if (lastperiod)
  {
    lastperiod[0] = 0;
  }

  return szFileName;
}

PRIVATE DWORD InitModulePath(HMODULE hModule)
{
	ZeroMemory(g_pszInstallPath, sizeof(g_pszInstallPath));
   
	if (GetModuleFileName(hModule, g_pszInstallPath, COUNTOF_I(g_pszInstallPath)-2) <= 0)
  {
    OutputDbgStr(_T("*** Fail to GetModuleFileName. Error=%d\n"), GetLastError());
  }
  
  if(g_pszInstallPath[0] == '\0')
    GetModuleFileName(NULL, g_pszInstallPath, sizeof(g_pszInstallPath)-2);
  
  if(g_pszInstallPath[0] == '\0')
  {
    OutputDbgStr(_T("*** Fail to GetModuleFileName. Error=%d\n"), GetLastError());
  }
  
  // Extract the file name portion and remove it's file extension.
	//g_pszFilePart = GetFilePart(g_pszInstallPath);
	g_pszFolder = GetFileFolder(g_pszInstallPath); // Get Folder like "C:\install\bin".
	//g_pszFolder = GetFileFolder(g_pszInstallPath); // Get Folder like "C:\install".
  
  return 0;
}
 

LPCTSTR CSysUtil_i::GetInstallPath(VOID)
{
  return g_pszInstallPath;
}

LPCTSTR CSysUtil_i::GetValidFilePath(LPCTSTR pszRelFilePath)
{
  static TCHAR buf[MAX_PATH];
  buf[0] = _T('\0');
  
  if(pszRelFilePath[0] == _T('\\'))
    wsprintf(buf, _T("%s%s\0"), g_pszInstallPath, pszRelFilePath);
  else
    wsprintf(buf, _T("%s\\%s\0"), g_pszInstallPath, pszRelFilePath);
  
  return buf;
}

VOID Core20Init(HINSTANCE hInstance)
{
    InitModulePath(hInstance);
    MemInit();
}

VOID Core20Exit()
{
}


#ifdef HAVE_CORE10_DALL_ENTRY
extern "C" int APIENTRY 
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE_L2(_T("its_bcore Initializing!\n"));
    Core10Init(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
    Core10Exit();
		TRACE_L2(_T("its_bcore Terminating!\n"));
	}
	else if (dwReason == DLL_THREAD_ATTACH)
	{
	}
	else if (dwReason == DLL_THREAD_DETACH)
	{
	}
  
	return 1;   // ok
}
#endif  // HAVE_CORE10_DALL_ENTRY
