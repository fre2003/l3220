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

CFileFinder_i::CFileFinder_i()
{
  m_FilesFound.RemoveAll();
}

CFileFinder_i::~CFileFinder_i()
{
  m_FilesFound.RemoveAll();
}

s32 CFileFinder_i::FindFiles(LPCTSTR szBaseFolder, LPCTSTR szFileMask, bool bSubFolders)
{
  CFileFind	finder;
  CString		sFullMask;
  CString sBaseFolder;
  BOOL		bFound, bValidFile;
  
  sBaseFolder = AddBackSlash_i(szBaseFolder);
  
  // Find directories
  if (bSubFolders)
  {
    sFullMask = sBaseFolder + CString(_T("*.*"));
    bFound = finder.FindFile(sFullMask);
    while (bFound)
    {
      bFound = finder.FindNextFile();
      if ((finder.IsDirectory()) && (!finder.IsDots()))
      {
        sBaseFolder = sBaseFolder + finder.GetFileName();
        
        // Recursive call
        FindFiles(sBaseFolder, szFileMask, bSubFolders);
      }
    }
  }
  
  finder.Close();
  
  DWORD	dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED | 
    FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | 
    FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY;
  
  sFullMask = sBaseFolder + szFileMask;
  bFound = finder.FindFile(sFullMask);
  while (bFound)
  {
    bFound = finder.FindNextFile();
    if (!finder.IsDots())
    {
      // check constrains
      bValidFile = TRUE;
      bValidFile = finder.MatchesMask(dwFileAttributes);
      
      // Add file to list
      if (bValidFile)
      {
        CString sName = finder.GetFilePath();
        if (finder.IsDirectory()) 
        {
          //sName += _T("\\");
          continue;
        }
        if(PathFileExists(sName))
          m_FilesFound.Add(sName);
      }
    }
  }
  
  return GetFileCount();
}


CString CFileFinder_i::GetFilePath(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= GetFileCount())) 
    return _T("");

	return m_FilesFound[nIndex];
}

VOID CFileFinder_i::RemoveAll()
{
	if (GetFileCount() > 0) 
    m_FilesFound.RemoveAll();
}

INT CFileFinder_i::GetFileCount()
{
	return m_FilesFound.GetSize();
}

//////////////////////////////////////////////////////////////////////////
// Example 
//////////////////////////////////////////////////////////////////////////
#if 0 
  CFileFinder m_FileFind;
  int cnt = m_FileFind.FindFiles(GetAppPath_2() + _T("\\plugin"), _T("*.dll"));
  for (int i = 0; i<cnt; i++)
  {
    CString strFile = m_FileFind.GetFilePath(cnt);
    TRACE1("File:%s\n", strFile);
  }
#endif
