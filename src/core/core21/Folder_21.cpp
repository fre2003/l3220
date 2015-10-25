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
#include "stdafx.h"

CString CFilePath_i::GetAppPath(VOID) 
{
  TCHAR filepath[MAX_PATH];
  filepath[0] = _T('\0');
  GetModuleFileName(NULL,filepath, MAX_PATH); 
  
  return CString(ExtractFilePath_i(filepath));
}

bool CFilePath_i::SetFolderAttributes(LPCTSTR pszDirectory, DWORD dwAttrs) // FILE_ATTRIBUTE_READONLY
{
  WIN32_FIND_DATA FileData;
  HANDLE hSearch;
  CString strFindString = pszDirectory;
  
  strFindString.TrimLeft();
  strFindString.TrimRight();
  strFindString.TrimRight(_T("\\ / *.* *"));
  strFindString.TrimRight(_T("/"));
  strFindString += _T("\\*.*");
  
  bool fFinished = FALSE;
  
  // Start searching for .TXT files in the current directory. 
  
  hSearch = FindFirstFile(strFindString, &FileData);
  if (hSearch == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }
  
  // Copy each .TXT file to the new directory 
  // and change it to read only, if not already. 
  
  while (!fFinished)
  {
    CString strFileName = FileData.cFileName ;
    if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      if (strFileName != _T(".") && strFileName != _T(".."))
      {
        CString strDir;
        
        strDir = pszDirectory;
        strDir.TrimRight(_T("\\ / *.* *"));
        strDir += _T("\\");
        strDir += strFileName;
        
        // strFileNameList.AddTail()
        // TRACE("\n'%s' Directory: \n", strDir);  
        SetFolderAttributes(strDir, dwAttrs);
      }
    }
    else
    {
      // filter files.
      CString strFileFullName = pszDirectory;
      strFileFullName.TrimRight(_T("\\ / *.* *"));
      
      strFileFullName += _T("\\");
      strFileFullName += strFileName;
      SetFileAttributes(strFileFullName, dwAttrs);
    }
    
    if (!FindNextFile(hSearch, &FileData))
    {
      if (GetLastError() == ERROR_NO_MORE_FILES)
      {
        // ::MessageBox(NULL, "No more .c files.","Search completed.", MB_OK); 
        fFinished = TRUE;
      }
      else
      {
        break;
      }
    }
  }
  
  // Close the search handle.   
  FindClose(hSearch);

  return TRUE;
}

#if 0 
static bool  path_is_directory(TCHAR *filename)
{
  DWORD   attribs = GetFileAttributes(filename);
  if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    return TRUE;
  else
    return FALSE;
}

static void	path_add_backslash(TCHAR *path)
{
  if (path[lstrlen(path) - 1] != '\\')
    lstrcat(path, _T("\\"));
}

static bool	path_remove_filespec(LPTSTR path)
{
  TCHAR	drive[_MAX_DRIVE];
  TCHAR	dir[_MAX_DIR];
  TCHAR	fname[_MAX_FNAME];
  TCHAR	ext[_MAX_EXT];
  
  _tsplitpath(path, drive, dir, fname, ext);
  _tmakepath(path, drive, dir, NULL, NULL);
  return TRUE;
}

#if 0 
#define MAX_FILES_TO_SELECT  1024 

static TCHAR last_used_directory [MAX_PATH];
static TCHAR	initialfilename[MAX_PATH * MAX_FILES_TO_SELECT];
// const char* filter =  "C Files (*.c)\0*.c\0h files (*.h)\0*.h\0All files (*.*)\0*.*\0"; 
bool UTL_GetOpenMultiFile(HWND hWnd,
                          LPCTSTR pszInitDir,
                          LPCTSTR pszFileFilter,
                          CStringArray& strFullFileNameArray,
                          CStringArray& strFileNameArry ) // c:/1/2/1.txt, 1.txt 
{
  OPENFILENAME fn;
  bool	returnval;
  
  fn.lStructSize = sizeof(OPENFILENAME);
  fn.hwndOwner = (HWND) GetWindowLong(NULL, DWL_USER);
  fn.hInstance = NULL;
  fn.lpstrFilter = pszFileFilter;
  fn.lpstrCustomFilter = NULL;
  fn.nMaxCustFilter = 0;
  fn.nFilterIndex = 0;
  fn.lpstrFile = initialfilename;
  fn.nMaxFile = MAX_PATH * MAX_FILES_TO_SELECT;
  fn.lpstrFileTitle = NULL;
  fn.nMaxFileTitle = 0;
  fn.lpstrInitialDir = pszInitDir;
  fn.lpstrTitle = NULL;
  fn.Flags =
    OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_EXPLORER |
    OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING;
  fn.nFileOffset = 0;
  fn.nFileExtension = 0;
  fn.lpstrDefExt = NULL;
  fn.lCustData = 0;
  fn.lpfnHook = NULL;
  fn.lpTemplateName = NULL;
  returnval = GetOpenFileName(&fn);
  if (returnval != FALSE) 
  {
    TCHAR   *newfilename;
    TCHAR	path_buffer[_MAX_PATH];
    TCHAR	path_buffer2[_MAX_PATH];
    strcpy(path_buffer, fn.lpstrFile);
    
    if (path_is_directory(fn.lpstrFile) == TRUE) {
      path_add_backslash(path_buffer);
    } else {
      path_remove_filespec(path_buffer);
    }
    strcpy(last_used_directory, path_buffer);
    
//     CWinApp* pApp = AfxGetApp();
//     pApp->WriteProfileString("MiscConfigure", "InitPath",  path_buffer);
    
    newfilename = fn.lpstrFile + fn.nFileOffset;
    
    CString str;
    while (newfilename[0] != 0)
    {
      lstrcpy(path_buffer2, path_buffer);
      lstrcat(path_buffer2, newfilename);
      strFileNameArry.Add(newfilename);
      
      //AddFile(globals.m_hPlaylist, path_buffer2);
      str = path_buffer2;
      str.TrimLeft();
      str.TrimRight();
      
      //TRACE("%s \n", );
      
      strFullFileNameArray.Add(str);
      newfilename = newfilename + strlen(newfilename) + 1;
    }
    
    return TRUE;
  }
  
  return FALSE;
}
#endif 

bool IsValidFileEx(CString strFileName, LPCTSTR pszFilter) 
{
  CString strFilter = pszFilter;
  
  strFileName.Right();
}

// Find files in pszDirectory
bool FindFiles(LPCTSTR pszDirectory, CStringArray& strFullFileNameArray, LPCTSTR pszFilter)
{ 
  if(GetFileAttributes(pszDirectory) & FILE_ATTRIBUTE_DIRECTORY)
  {
    WIN32_FIND_DATA find;
    bool fFinished = FALSE;
    CString strFindString;
    HANDLE hFind = NULL;
    
    strFindString = pszDirectory;
    strFindString.TrimLeft();
    strFindString.TrimRight(); 
    strFindString.TrimRight(_T("\\")); 
    strFindString.TrimRight(_T("/"));
    strFindString += _T("\\*.*");
    
    hFind = ::FindFirstFile(strFindString, &find);
    
    if (hFind == INVALID_HANDLE_VALUE) 
    {
      // printf("No .c files found.");
      return FALSE;
    }
    
    CString strFileName; 
    
    while (!fFinished) 
    {
      strFileName = find.cFileName ;
      if(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        if (strFileName != _T(".") && strFileName != _T("..")) 
        {
          CString strDir;
          strDir = pszDirectory;
          strDir += _T("\\");
          strDir += strFileName;
          //strFileNameList.AddTail()
          RTS_FindFiles(strDir, strFullFileNameArray);
        }
      }
      else
      {
        // TRACE("%s \n", find.cFileName);
        
        // filter files.
#if 1
//         if(strFileName.Right(2) == ".c" 
//           || strFileName.Right(2) == ".C"
//           || strFileName.Right(2) == ".h" 
//           || strFileName.Right(2) == ".H" )
        if(!pszFilter || )
        {
          CString strFileFullName = pszDirectory;
          strFileFullName += _T("\\");
          strFileFullName += strFileName;
          //strFileNameList.AddTail(strFileName);
          strFullFileNameArray.Add(strFileFullName);
        }
#endif
      }
      if ( !::FindNextFile(hFind, &find) )
      {
        if (::GetLastError() == ERROR_NO_MORE_FILES) 
        { 
          //::MessageBox(NULL, "No more .c files.","Search completed.", MB_OK); 
          fFinished = TRUE; 
        }
        else
        { 
          //TRACE("Couldn't find next file."); 
          break;
        }
      }
    }
    
    ::FindClose(hFind);
  }
}

#endif 
