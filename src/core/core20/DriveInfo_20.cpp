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

#define	ONEK			 1024
#define	SIXTYFOURK (64*ONEK)
#define	ONEM			 (ONEK*ONEK)
#define	ONEG			 (ONEK*ONEK*ONEK)
#define	ONET			 (ONEK*ONEK*ONEK*ONEK)

U32 CSysUtil_i::GetMemInfo(TCHAR szMemInfo[256])
{
  MEMORYSTATUS MemInfo = {0, };

  MemInfo.dwLength = sizeof(MemInfo);
  GlobalMemoryStatus(&MemInfo);
  
  szMemInfo[0] = _T('\0');
  
  // Print out info on memory, rounded up.
  wsprintf( szMemInfo, _T("%d%% memory in use, Physical memory %d:%d MB"), MemInfo.dwMemoryLoad,
    (MemInfo.dwAvailPhys + ONEM - 1) / ONEM, (MemInfo.dwTotalPhys +	ONEM - 1) / ONEM);
 
  return 0;
}

U32 CSysUtil_i::GetMemInfo(DWORD* pPercentInUse, DWORD* pAvailPhysMemMB, DWORD* pTotalPhysMemMB)
{
  MEMORYSTATUS MemInfo = {0, };

  MemInfo.dwLength = sizeof(MemInfo);
  GlobalMemoryStatus(&MemInfo);
   
  *pPercentInUse   = MemInfo.dwMemoryLoad;
  *pAvailPhysMemMB = (MemInfo.dwAvailPhys + ONEM - 1) / ONEM;
  *pTotalPhysMemMB = (MemInfo.dwTotalPhys +	ONEM - 1) / ONEM;

  return 0;
}

bool CDriveInfo_i::GetRootPathName(LPCTSTR szPathName, TCHAR szRootPathName[4])
{
  szRootPathName[0] = _T('\0');
  int iDriveNum = PathGetDriveNumber(szPathName); // 0 through 25 (corresponding to 'A' through 'Z') 
  PathBuildRoot(szRootPathName, iDriveNum);
  
  return TRUE;
}

bool CDriveInfo_i::GetDiskName(LPCTSTR szPathName, TCHAR szDiskName[2])
{
  szDiskName[0] = _T('\0');
  
  if(IsUNCPath(szPathName))
  {
    szDiskName[0] = _T('?');
    szDiskName[1] = _T('\0');
    return FALSE;
  }
  
  int iDriveNum = PathGetDriveNumber(szPathName); // 0 through 25 (corresponding to 'A' through 'Z') 
  wsprintf(szDiskName, _T("%c\0"), (iDriveNum+65));
  
  return TRUE;
}

//
// EXAMPLE
//   temp[0] = '\0';
//   GetSystemDirectory(temp, sizeof(temp)); 
//   GetDriveBytes(temp, &nFree, &nTotal);
//   temp[1] = '\0'; // --> SystemRoot
//////////////////////////////////////////////////////////////////////////
U32 CDriveInfo_i::GetDiskSpace(LPCTSTR szPathName, U64* pFree, U64* pTotal, bool bSupportUNC)
{
  DWORD64 nFreeSpace = 0;
  DWORD64 nTotalSpace = 0;
  DWORD64 nFreeBytesAvailable = 0;
  TCHAR szRoot[MAX_PATH] = {0, };
  
  lstrcpy(szRoot,szPathName);
  //GetRootPathName(szPathName, szRootPathName);
  PathStripToRoot(szRoot);
  
  if (PathIsRoot(szRoot))
  {
    if(!bSupportUNC && IsUNCPath(szRoot))
    {
      return ERR_UNC_PATH_I;
    }
    
    if(GetDiskFreeSpaceEx(szRoot, (ULARGE_INTEGER*)&nFreeBytesAvailable, 
      (ULARGE_INTEGER*)&nTotalSpace, (ULARGE_INTEGER*)&nFreeSpace)) 
    {
      if(pFree)
      {
        *pFree = nFreeSpace;
      }
      if(pTotal)
      {
        *pTotal = nTotalSpace;
      }
      return ERR_OK_I;
    }
  }
  return ERR_WIN32_ERROR_I;
}

U32 CDriveInfo_i::GetDiskSpace(LPCTSTR szPathName, U32* pdwFree, U32* pdwTotal, bool bSupportUNC)
{
  DWORD64 nFreeSpace = 0;
  DWORD64 nTotalSpace = 0;
  DWORD64 nFreeBytesAvailable = 0;
  TCHAR szRoot[MAX_PATH] = {0, };
  
  lstrcpy(szRoot,szPathName);
  PathStripToRoot(szRoot);
  
  if (PathIsRoot(szRoot))
  {
    if(!bSupportUNC && IsUNCPath(szRoot))
    {
      return ERR_UNC_PATH_I;
    }
    
    GetDiskFreeSpaceEx(szRoot, (ULARGE_INTEGER*)&nFreeBytesAvailable, 
      (ULARGE_INTEGER*)&nTotalSpace, (ULARGE_INTEGER*)&nFreeSpace);
    if(pdwFree)
    {
      *pdwFree = (U32)(nFreeSpace/ONEM);
    }
    if(pdwTotal)
    {
      *pdwTotal = (U32)(nTotalSpace/ONEM);
    }
    
    return ERR_OK_I;
  }
  
  return ERR_WIN32_ERROR_I;
}


U32 CDriveInfo_i::GetDiskSpace(LPCTSTR szPathName,
                               OUT TCHAR szFree[32], 
                               OUT TCHAR szUsed[32], 
                               OUT TCHAR szTotal[32],
                               bool bSupportUNC)
{
  U32 ret = ERR_OK_I;
  DWORD64 nFreeSpace  = 0;
  DWORD64 nTotalSpace = 0;
  DWORD64 nUsedSpace  = 0;

  szFree[0]  = _T('\0');
  szUsed[0]  = _T('\0');
  szTotal[0] = _T('\0');
  
  if((ret = GetDiskSpace(szPathName, &nFreeSpace, &nTotalSpace, bSupportUNC)) == ERR_OK_I)
  {
    CFormatString_i strfmt;
    nUsedSpace = nTotalSpace - nFreeSpace;
    strfmt.FormatBytes(szFree,  32, nFreeSpace);
    strfmt.FormatBytes(szTotal, 32, nTotalSpace);
    strfmt.FormatBytes(szUsed,  32, nUsedSpace);
  }
  
  return ret;
}



// EXAMPLE
//   temp[0] = '\0';
//   GetSystemDirectory(temp, sizeof(temp)); 
//   GetDriveBytes(temp, &nFree, &nTotal);
//   temp[1] = '\0'; // --> SystemRoot

//
// szPathName : Pointer to a string that contains the root directory of the volume to be described like C:\\Dir1\\Dir2\\1.txt C:\\Dir1\\Dir2 or \\server\\dir1\dir2 
// ulFsType: 
// 0: NTFS; 1: FAT32; 2:FAT; 0xFFFFFFFF: UNKNOWN
U32 CDriveInfo_i::GetFileSystemType(LPCTSTR szPathName, U32& ulFsType, bool bSupportUNC)
{
  TCHAR szRoot[MAX_PATH] = {0, };
  
  lstrcpy(szRoot, szPathName);
  
  ulFsType = 0xFFFFFFFF;
  
  if(szRoot[0] != 0)
  { 
    TCHAR szFileSystemNameBuffer[128] = {0, };
    PathStripToRoot(szRoot);
    lstrcat(szRoot, _T("\\"));

    if(!bSupportUNC && IsUNCPath(szRoot))
    {
      return ERR_UNC_PATH_I;
    }
    
    if(GetVolumeInformation(szRoot, NULL, 0, NULL, NULL, NULL, szFileSystemNameBuffer, sizeof(szFileSystemNameBuffer)))
    {
      CString str = szFileSystemNameBuffer;
      str.MakeUpper();
      if(str == _T("NTFS"))
      {
        ulFsType = 0;
      }
      else if(str == _T("FAT"))
      {
        ulFsType = 2;
      }
      else if(str == _T("FAT32"))
      {
        ulFsType = 1;
      }
      return ERR_OK_I;
    }
    else 
    {
      // 0x0000007b 
      return ERR_WIN32_ERROR_I;
    }
  }
  else 
  {
    return ERR_INVALID_PARAM_I;
  }
}

U32 CDriveInfo_i::GetFileSystemType(LPCTSTR szPath, TCHAR szFsType[8], bool bSupportUNC)
{
  TCHAR szRoot[MAX_PATH] = {0, };
  
  lstrcpy(szRoot, szPath);
        
  szFsType[0] = _T('\0');
  lstrcpy(szFsType, _T("UNKNOWN"));
  
  if(szRoot[0] != 0)
  { 
    PathStripToRoot(szRoot);
    lstrcat(szRoot, _T("\\"));

    if(!bSupportUNC && IsUNCPath(szRoot))
    {
      return ERR_UNC_PATH_I;
    }
    
    if(GetVolumeInformation(szRoot, NULL, 0, NULL, NULL, NULL, szFsType, 8))
    {
      return ERR_OK_I;
    }
    else
    {
      lstrcpy(szFsType, _T("UNKNOWN"));
      return ERR_WIN32_ERROR_I;
    }
  }
  else 
  {
    return ERR_INVALID_PARAM_I;
  }
}


U32 CDriveInfo_i::GetDriveSerialNumber(LPCTSTR szPathName)
{
  TCHAR szRootPathName[4];
  GetRootPathName(szPathName, szRootPathName);
  
  if (GetDriveType(szRootPathName) != DRIVE_FIXED)
    return 0;
  
  DWORD dwHDSerialNum = 0;
  
  if (!GetVolumeInformation(szRootPathName, NULL, 0, &dwHDSerialNum, NULL, NULL, NULL, 0))
    return (DWORD)-1;
  
  return dwHDSerialNum;
}

bool CDriveInfo_i::IsRemovablePath(LPCTSTR szPathName)
{
  TCHAR szRootPathName[4];
  GetRootPathName(szPathName, szRootPathName);
  
  return (GetDriveType(szRootPathName) == DRIVE_REMOVABLE);
}

bool CDriveInfo_i::IsHardDiskAvailable(LPCTSTR szPathName)
{
  TCHAR szRootPathName[4];
  GetRootPathName(szPathName, szRootPathName);

  return (::GetDriveType(szRootPathName)==DRIVE_FIXED);
}

bool CDriveInfo_i::IsRemotePath(LPCTSTR szPathName)
{
  TCHAR szRootPathName[4];
  GetRootPathName(szPathName, szRootPathName);
  
  return (GetDriveType(szRootPathName) == DRIVE_REMOTE);
}

// Universal Naming Convention (UNC)  
bool CDriveInfo_i::IsUNCPath(LPCTSTR szPathName)
{
  return (StrStr(szPathName, _T("\\\\")) == szPathName);
}

