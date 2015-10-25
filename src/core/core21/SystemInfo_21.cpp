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


// Determines if the current Windows OS version is NT 
// TRUE if Windows NT running
bool CSysInfo_i::IsNT() const
{
  BOOL bNt = FALSE;

  OSVERSIONINFO	osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&osvi))
	{
		TRACE(_T("COXSysInf::IsNT: GetVersionEx() has failed\n"));
		return (FALSE);
	}
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		// We're running on NT.
		bNt=TRUE;
	else
		bNt=FALSE;

	return (bool)(bNt);
}

bool CSysInfo_i::IsNTServer() const
{
  bool bNt = FALSE; 
	
  if (!IsNT())
  {
    return (FALSE);
  }
	else 
	{
		HKEY hKey;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			_T("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"), 
			0, KEY_EXECUTE, &hKey) != ERROR_SUCCESS)
			return (FALSE);

		DWORD dwType, dwSize;
		CString sProductType;
		LPTSTR lpProductType=sProductType.GetBuffer(MAX_PATH+1);
		if(RegQueryValueEx(hKey,_T("ProductType"),NULL,&dwType,
			(LPBYTE)lpProductType,&dwSize)!=ERROR_SUCCESS)
		{
			sProductType.ReleaseBuffer();
			return (FALSE);
		}
		sProductType.ReleaseBuffer();
		ASSERT(dwType==REG_SZ);
		RegCloseKey(hKey);

		if (sProductType==_T("WinNT"))
			bNt=FALSE;
		else if (sProductType==_T("LanmanNT"))
			bNt=TRUE;
		else if (sProductType==_T("ServerNT"))
			bNt=TRUE;
		else 
			return (FALSE);
	}
	
	return (bNt);
}

bool CSysInfo_i::GetComputerName(CString *psComputerName) const
{
	//	--- In:			
	//	--- Out:		CString *psComputerName: Name of local computer
	//	--- Returns:	bool - TRUE if success
	//	---	Effect:		Retrieves the name of the local computer

	ASSERT(psComputerName != NULL);

	bool	bReturn;
	DWORD	dwBufSize;
	TCHAR	buffer[MAX_COMPUTERNAME_LENGTH + 1];

	dwBufSize =  sizeof(buffer)/sizeof(buffer[0]);

	bReturn = ::GetComputerName(buffer, &dwBufSize);

	if (bReturn)
		*psComputerName = buffer;
  
	return (bReturn);
}

bool CSysInfo_i::GetUserName(CString *psUserName) const
{
	//	--- In:			
	//	--- Out:		CString *psUserName: Name of current user
	//	--- Returns:	bool - TRUE if success
	//	---	Effect:		Retrieves the name of the current user

	ASSERT(psUserName != NULL);

	bool	bReturn;
	DWORD	dwBufSize;
	TCHAR	buffer[1024+1];

	dwBufSize = sizeof(buffer)/sizeof(buffer[0]);

	bReturn = ::GetUserName(buffer, &dwBufSize);

	if (bReturn)
		*psUserName = buffer;
 
	return (bReturn);
}


bool CSysInfo_i::GetUserAndDomainName(CString *psUserName, CString *psDomainName) const
{
	//	--- In:			
	//	--- Out:		CString *psUserName: Name of current user
	//					CString *psDomainName: Name of current domain logged on to
	//	--- Returns:	bool - TRUE if success
	//	---	Effect:		Retrieves the name of the current user and the domain. Works only
	//					on Win NT.

	ASSERT(psUserName != NULL);
	ASSERT(psDomainName != NULL);

	bool	bSuccess = FALSE;
	#define BUF_SIZE	512
	DWORD	dwBufSize = BUF_SIZE;
	TCHAR	buffer[BUF_SIZE];

	if (!IsNT())
  {
    return (FALSE);
  }
	else 
	{
		HANDLE hToken(0);
		if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken))
		{
			if (GetLastError() == ERROR_NO_TOKEN)
				// We'll try to open the process token since no thread token exists
			{
				if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
				{
 					return FALSE;
				}
			}
			else
				// Failed to get the thread token
			{
 				return FALSE;
			}
		}

		ASSERT(hToken != NULL);
		bSuccess = GetTokenInformation(hToken, TokenUser, buffer, dwBufSize, &dwBufSize);

		if (!bSuccess)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				// Allocate a bigger bufer and try again
			{
 				CloseHandle(hToken);
				return FALSE;
			}
			else
				// We have an error while getting the token info
			{
 				CloseHandle(hToken);
				return FALSE;
			}
		}

		CloseHandle(hToken);

		SID_NAME_USE	Snu;
		DWORD			dwUserName = BUF_SIZE;
		DWORD			dwDomainName = BUF_SIZE;
		TCHAR			userName[BUF_SIZE];
		TCHAR			domainName[BUF_SIZE];

		bSuccess = LookupAccountSid(NULL, ((PTOKEN_USER)buffer)->User.Sid, userName, &dwUserName, domainName, &dwDomainName, &Snu);
		if (bSuccess)
		{
			*psUserName = userName;
			*psDomainName = domainName;
		}
	}
 
	return (bSuccess);
}
