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


#define IMAGE_SIZEOF_FILE_HEADER 20

#define SIZE_OF_NT_SIGNATURE sizeof(DWORD)

#define NTSIGNATURE(a) ((LPVOID)((BYTE *)a + \
                       ((PIMAGE_DOS_HEADER)a)->e_lfanew))

#define OPTHDROFFSET(a) ((LPVOID)((BYTE *)a + \
                        ((PIMAGE_DOS_HEADER)a)->e_lfanew + \
                        SIZE_OF_NT_SIGNATURE + \
                        sizeof(IMAGE_FILE_HEADER)))

static LPVOID GetModuleEntryPoint(LPVOID lpFile)
{
  PIMAGE_OPTIONAL_HEADER poh;
  poh = (PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(lpFile);
  if (poh != NULL)
    return (LPVOID)poh->AddressOfEntryPoint;
  else
    return NULL;
}

DWORD ImageFileType (LPVOID lpFile)
{
  /* 首先出现的是DOS文件标志 */
  if (*(USHORT *)lpFile == IMAGE_DOS_SIGNATURE)
  {
    /* 由DOS头部决定PE文件头部的位置 */
    if (LOWORD (*(DWORD *)NTSIGNATURE (lpFile)) ==
        IMAGE_OS2_SIGNATURE ||
        LOWORD (*(DWORD *)NTSIGNATURE (lpFile)) ==
        IMAGE_OS2_SIGNATURE_LE)
      return (DWORD)LOWORD(*(DWORD *)NTSIGNATURE (lpFile));
    else if (*(DWORD *)NTSIGNATURE (lpFile) == IMAGE_NT_SIGNATURE)
    return IMAGE_NT_SIGNATURE;
    else
      return IMAGE_DOS_SIGNATURE;
  }
  else
    /* 不明文件种类 */
    return 0;
}

bool IsValidWin32EmulatorImage(LPCTSTR pszProcessName) 
{
  HINSTANCE hIns = NULL;
  bool ret       = FALSE;
  
  hIns = LoadLibrary(pszProcessName);
  if(hIns)
  {
    if(GetProcAddress(hIns, ("ISE_EmuImageEntry")))
    {
      ret = TRUE;
    }
    else
      ret = FALSE;
    FreeLibrary(hIns);
    return ret;
  }
  else
    return FALSE;
}

bool IsWin32ImageFile(LPCTSTR pszFile)
{
  HANDLE hFile = NULL;
  
  //SetFileAttributes(hFile, FILE_ATTRIBUTE_NORMAL);
  
  hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  
  if(hFile)
  {
    char buf[1024];
    DWORD dwReturn = 0;
    
    ZeroMemory(buf, sizeof(buf));
    ReadFile(hFile, buf, sizeof(buf), &dwReturn, NULL);
    CloseHandle(hFile);
        
    if(ImageFileType((LPVOID)buf) == IMAGE_NT_SIGNATURE && IsValidWin32EmulatorImage(pszFile))
    {
      //         IMAGE_IMPORT_BY_NAME* pin = (IMAGE_IMPORT_BY_NAME*)GetModuleEntryPoint(buf);       
      //         CString str  =  pin->Name;
      //         str.MakeLower();
      //         if(str == "EmuMain")
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}