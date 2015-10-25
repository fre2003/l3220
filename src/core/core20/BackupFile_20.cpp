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

U32 ITS_BackupFile(LPCTSTR lpszFile)
{
  U32 ret = 0;

	if(!lpszFile) 
    return 1;
  
	// If file doesn't exist, nothing to do.
	if (!PathFileExists(lpszFile))
  {
    return 0;
  }
  
  TCHAR szPath[MAX_PATH];
  TCHAR szSuffix[20];

	for (int i = 1; i <= 9999; i++)
	{
    wsprintf(szSuffix, _T(".%04d"), i);

    szPath[0] = '\0';
    lstrcpy(szPath, lpszFile);
    lstrcat(szPath, szSuffix);
		if (!PathFileExists(lpszFile))
		{
			// File doesn't exist, so we can use it.
			if(CopyFile(lpszFile, szPath, TRUE) != TRUE) 
        ret = GetLastError();
			break;
		}
	}

	return ret;
}

