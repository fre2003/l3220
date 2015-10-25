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
#include <shlobj.h>
#include <objidl.h>
#include <commdlg.h>
#include <shellapi.h>


#if 0 

  HANDLE hFile;
  bool   bWriteSuccess;

  LPSTR lpData;
  DWORD cbData;
  DWORD dwBytesWritten;
  
  hFile = CreateFile(pszFile,
                     GENERIC_WRITE,
                     FILE_SHARE_READ|FILE_SHARE_WRITE,
                     NULL,
                     CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);

  // failure could be due to missing attributes (2k/XP)
  if (hFile == INVALID_HANDLE_VALUE)
  {
    DWORD dwAttributes = GetFileAttributes(pszFile);
    if (dwAttributes != 0) // INVALID_FILE_ATTRIBUTES FILE_ATTRIBUTE_READONLY
    {
      dwAttributes = dwAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
      hFile = CreateFile(pszFile,
                        GENERIC_WRITE,
                        FILE_SHARE_READ|FILE_SHARE_WRITE,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL | dwAttributes,
                        NULL);
    }
  }

  if (hFile == INVALID_HANDLE_VALUE)
    return FALSE;
  SetEndOfFile(hFile);

  dwFileAttributes = GetFileAttributes(psz);
  bool bReadOnly = (dwFileAttributes != INVALID_FILE_ATTRIBUTES && dwFileAttributes & FILE_ATTRIBUTE_READONLY);


bool FileIO(bool fLoad,LPCSTR psz,bool bNoEncDetect,int *icp, int *ieol,
            bool *pbUnicodeErr,bool *pbFileTooBig,bool bSaveCopy)
{
  SHFILEINFO shfi;
  char tch[MAX_PATH+40];
  bool fSuccess;
  DWORD dwFileAttributes;

  BeginWaitCursor();

  SHGetFileInfo2(psz,0,&shfi,sizeof(SHFILEINFO),SHGFI_DISPLAYNAME);
  FormatString(tch,COUNTOF(tch),(fLoad) ? IDS_LOADFILE : IDS_SAVEFILE,shfi.szDisplayName);

  StatusSetText(hwndStatus,STATUS_HELP,tch);
  StatusSetSimple(hwndStatus,TRUE);

  InvalidateRect(hwndStatus,NULL,TRUE);
  UpdateWindow(hwndStatus);

  if (fLoad)
    fSuccess = EditLoadFile(hwndEdit,psz,bNoEncDetect,icp,ieol,pbUnicodeErr,pbFileTooBig);
  else
    fSuccess = EditSaveFile(hwndEdit,psz,*icp,bSaveCopy);

  dwFileAttributes = GetFileAttributes(psz);
  bReadOnly = (dwFileAttributes != INVALID_FILE_ATTRIBUTES && dwFileAttributes & FILE_ATTRIBUTE_READONLY);

  StatusSetSimple(hwndStatus,FALSE);

  EndWaitCursor();

  return(fSuccess);
}

static int MsgBox(int iType,UINT uIdMsg,...)
{

  char szText [256*2];
  char szBuf  [256*2];
  char szTitle[64];
  int iIcon = 0;
  HWND hwnd;

  if (!GetString(uIdMsg,szBuf,COUNTOF(szBuf)))
    return(0);

  wvsprintf(szText,szBuf,(LPVOID)(&uIdMsg + 1));

  GetString(IDS_APPTITLE,szTitle,COUNTOF(szTitle));

  switch (iType) {
    case MBINFO: iIcon = MB_ICONINFORMATION; break;
    case MBWARN: iIcon = MB_ICONEXCLAMATION; break;
    case MBYESNO: iIcon = MB_ICONQUESTION | MB_YESNO; break;
    case MBYESNOCANCEL: iIcon = MB_ICONQUESTION | MB_YESNOCANCEL; break;
    case MBYESNOWARN: iIcon = MB_ICONWARNING | MB_YESNO; break; }

  if (!(hwnd = GetFocus()))
    hwnd = hwndMain;

  return MessageBoxEx(hwnd,szText,szTitle,MB_SETFOREGROUND | iIcon,
                      MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT));

}

#endif 

//////////////////////////////////////////////////////////////////////////
//  PrepareFilterStr()
//  Replace '\n' with '\0'.
//  Use in FileFilter string. 
//////////////////////////////////////////////////////////////////////////
static void PrepareFilterStr(LPTSTR lpFilter)
{
  LPTSTR psz = StrEnd(lpFilter);
  while (psz != lpFilter)
  {
    if (*(psz = CharPrev(lpFilter,psz)) == _T('\n'))
      *psz = _T('\0');
  }
}

#if 0 
bool Style_GetOpenDlgFilterStr(LPSTR lpszFilter,int cchFilter)
{
  int i;
  int iSize;
  char *p1,*p2;
  char tchFilters[NUMLEXERS+1][COUNTOF(lexDefault.szExtensions) * 5 + 64];
  char szExtensions[COUNTOF(lexDefault.szExtensions)];


  GetString(IDS_FILTER_ALL,tchFilters[0],COUNTOF(tchFilters[0]));
  iSize = lstrlen(tchFilters[0]);

  for (i = 0; i < NUMLEXERS; i++)
  {
    lstrcpy(tchFilters[i+1],pLexArray[i]->pszName);
    lstrcat(tchFilters[i+1],"\n");

    lstrcpy(szExtensions,pLexArray[i]->szExtensions);
    p1 = szExtensions;
    while (*p1)
    {
      if (p2 = StrChr(p1,';')) {
        *p2 = '\0'; p2++; }
      else
        p2 = StrEnd(p1);
      if (*p1) {
        lstrcat(tchFilters[i+1],"*.");
        lstrcat(tchFilters[i+1],p1);
        if (*p2)
          lstrcat(tchFilters[i+1],";"); }
      p1 = p2;
    }

    // Check for empty extensions
    p1 = StrEnd(tchFilters[i+1]) - 1;
    if (*p1 == '\n') // no extensions yet, add *.*
      lstrcat(tchFilters[i+1],"*.*\n");
    else
      lstrcat(tchFilters[i+1],"\n");

    iSize += lstrlen(tchFilters[i+1]);
  }

  if (iSize + 2 < cchFilter)
  {
    lstrcpy(lpszFilter,tchFilters[0]);

    for (i = 0; i < NUMLEXERS; i++)
      lstrcat(lpszFilter,tchFilters[i+1]);

    lstrcat(lpszFilter,"\n");
    PrepareFilterStr(lpszFilter);
    return TRUE;
  }
  else
    return FALSE;
}
#endif 

#define NUMLEXERS 22



//////////////////////////////////////////////////////////////////////////
// OpenFileDlg
// EXAMPLE:
// char tch[MAX_PATH]
//  if (!OpenFileDlg(hwndMain,tch,COUNTOF(tch),NULL))
//     return FALSE; 
//////////////////////////////////////////////////////////////////////////
static bool OpenFileDlg(HWND hwnd, LPTSTR lpstrFile, int cchFile, LPCTSTR lpstrInitialDir)
{
  OPENFILENAME ofn;
  TCHAR szFile[MAX_PATH];
  TCHAR szFilter[NUMLEXERS*1024];
  TCHAR tchInitialDir[MAX_PATH];
  static TCHAR tchDefaultExtension[64];
  TCHAR szCurFile[MAX_PATH+40];

  lstrcpy(szFile, _T(" "));
  PrepareFilterStr(szFilter);
  // Style_GetOpenDlgFilterStr(szFilter,COUNTOF(szFilter)); // --> All Files(*.*) 

  if (!lpstrInitialDir && lstrlen(szCurFile)) 
  {
    lstrcpy(tchInitialDir,szCurFile);
    PathRemoveFileSpec(tchInitialDir); 
  }

  ZeroMemory(&ofn,sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = szFilter;
  ofn.lpstrFile = szFile;
  ofn.lpstrInitialDir = (lpstrInitialDir) ? lpstrInitialDir : tchInitialDir;
  ofn.nMaxFile = COUNTOF_I(szFile);
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | /* OFN_NOCHANGEDIR |*/
            /*OFN_DONTADDTORECENT*/0x02000000 | OFN_PATHMUSTEXIST |
              OFN_SHAREAWARE /*| OFN_NODEREFERENCELINKS*/;
  ofn.lpstrDefExt = (lstrlen(tchDefaultExtension)) ? tchDefaultExtension : NULL;

  if (GetOpenFileName(&ofn))
  {
    lstrcpyn(lpstrFile,szFile,cchFile);
    return TRUE; 
  }
  else
    return FALSE;
}


u32 CFileOps_i::GetOpenFileNameDlg(HWND hWnd, 
                            TCHAR* pszFile, // Output file 
                            LPCTSTR lpstrInitialDir, // 
                            LPCTSTR lpstrFilter, //"C File(*.c)\0*.c\0H File(*.h)\0*.h\0All File(*.*)\0*.*\0"
                            LPCTSTR lpstrTitle,  //"Select a C or H file(.c or .h)"
                            LPCTSTR lpstrDefExt  // "c" 
                            ) 
{
  if(pszFile == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }

  //TCHAR* pszBasePath = NULL;
	OPENFILENAME fn;
	bool bResult;
	TCHAR pcOutputName[MAX_PATH] = _T("");
  
	// Get filename to save
	fn.lStructSize = sizeof(OPENFILENAME);
	fn.hwndOwner = hWnd;
	fn.hInstance = NULL;
	fn.lpstrFilter = lpstrFilter; //"C File(*.c)\0*.c\0H File(*.h)\0*.h\0All File(*.*)\0*.*\0";
	fn.lpstrCustomFilter = NULL;
	fn.nMaxCustFilter = 0;
	fn.nFilterIndex = 0;
	fn.lpstrFile = pcOutputName;
	fn.nMaxFile = MAX_PATH;
	fn.lpstrFileTitle = NULL;
	fn.nMaxFileTitle = 0;
	fn.lpstrInitialDir = lpstrInitialDir; 
	fn.lpstrTitle = lpstrTitle; // "Select a C or H file(.c or .h)";
	fn.Flags = /*OFN_HIDEREADONLY |*/ OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_ENABLESIZING;
	fn.nFileOffset = 0;
	fn.nFileExtension = 0;
	fn.lpstrDefExt = lpstrDefExt;
	fn.lCustData = 0;
	fn.lpfnHook = NULL;
	fn.lpTemplateName = NULL;
	//bResult = GetSaveFileName(&fn);
	bResult = GetOpenFileName(&fn);
    
	if(bResult == FALSE)
  {
    return ERR_WIN32_ERROR_I;
  }
  else
  {
      lstrcpy(pszFile, pcOutputName);
      return ERR_OK_I; 
  }

  return ERR_OK_I;
}


U32 CFileOps_i::GetSaveFileNameDlg(HWND hWnd, 
                          TCHAR* pszFile, // Output file 
                          LPCTSTR lpstrInitialDir, // 
                          LPCTSTR lpstrFilter, //"C File(*.c)\0*.c\0H File(*.h)\0*.h\0All File(*.*)\0*.*\0"
                          LPCTSTR lpstrTitle,  //"Select a C or H file(.c or .h)"
                          LPCTSTR lpstrDefExt  // "c" 
                          )
{
  if(pszFile == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }

  pszFile[0] = _T('\0');
  //TCHAR* pszBasePath = NULL;
	OPENFILENAME fn;
	bool bResult;
	TCHAR pcOutputName[MAX_PATH] = _T("");
  
	// Get filename to save
	fn.lStructSize = sizeof(OPENFILENAME);
	fn.hwndOwner = hWnd;
	fn.hInstance = NULL;
	fn.lpstrFilter = lpstrFilter; //"C File(*.c)\0*.c\0H File(*.h)\0*.h\0All File(*.*)\0*.*\0";
	fn.lpstrCustomFilter = NULL;
	fn.nMaxCustFilter = 0;
	fn.nFilterIndex = 0;
	fn.lpstrFile = pcOutputName;
	fn.nMaxFile = MAX_PATH;
	fn.lpstrFileTitle = NULL;
	fn.nMaxFileTitle = 0;
	fn.lpstrInitialDir = lpstrInitialDir; 
	fn.lpstrTitle = lpstrTitle; // "Select a C or H file(.c or .h)";
	fn.Flags = /*OFN_HIDEREADONLY |*/ OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_ENABLESIZING;
	fn.nFileOffset = 0;
	fn.nFileExtension = 0;
	fn.lpstrDefExt = lpstrDefExt;
	fn.lCustData = 0;
	fn.lpfnHook = NULL;
	fn.lpTemplateName = NULL;
	bResult = GetSaveFileName(&fn);
    
	if(bResult == FALSE)
  {
    return ERR_WIN32_ERROR_I;
  }
  else
  {
      lstrcpy(pszFile, pcOutputName);
      return ERR_OK_I;
  }
  
  return ERR_OK_I;
}


static BOOL  path_is_directory(char *filename)
{
  DWORD   attribs = GetFileAttributes(filename);
  if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    return TRUE;
  else
    return FALSE;
}

static void	path_add_backslash(char *path)
{
  if (path[strlen(path) - 1] != '\\')
    strcat(path, "\\");
}

static BOOL	path_remove_filespec(LPTSTR path)
{
  char	drive[_MAX_DRIVE];
  char	dir[_MAX_DIR];
  char	fname[_MAX_FNAME];
  char	ext[_MAX_EXT];
  
  _splitpath(path, drive, dir, fname, ext);
  _makepath(path, drive, dir, NULL, NULL);
  return TRUE;
}

#define MAX_FILES_TO_SELECT  1024 

U32 CFileOps_i::GetOpenMultiFileNameDlg(HWND hWnd,
                                        LPCTSTR pszInitDir,
                                        LPCTSTR pszFileFilter,
                                        LPCTSTR pszTitle,
                                        CStringArray& strFullFileNameArray)
{
  OPENFILENAME fn;
  U32	returnval;
  
  TCHAR* pFileNameBuffer = new TCHAR[MAX_PATH * MAX_FILES_TO_SELECT+1];
  if(pFileNameBuffer == NULL)
    return ERR_NO_MORE_MEMORY_I;
  
  pFileNameBuffer[0] = 0;
  
  fn.lStructSize = sizeof(OPENFILENAME);
  fn.hwndOwner = (HWND) hWnd; // GetWindowLong(NULL, DWL_USER);
  fn.hInstance = NULL;
  fn.lpstrFilter = pszFileFilter;
  fn.lpstrCustomFilter = NULL;
  fn.nMaxCustFilter = 0;
  fn.nFilterIndex = 0;
  fn.lpstrFile = pFileNameBuffer;
  fn.nMaxFile  = MAX_PATH * MAX_FILES_TO_SELECT;
  fn.lpstrFileTitle = NULL;
  fn.nMaxFileTitle = 0;
  fn.lpstrInitialDir = pszInitDir;
  fn.lpstrTitle = pszTitle;
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
    char   *newfilename;
    char	path_buffer[_MAX_PATH];
    char	path_buffer2[_MAX_PATH];
    strcpy(path_buffer, fn.lpstrFile);
    
    if (path_is_directory(fn.lpstrFile) == TRUE) 
    {
      path_add_backslash(path_buffer);
    } 
    else 
    {
      path_remove_filespec(path_buffer);
    }

    //strcpy(last_used_directory, path_buffer);
    
    //CWinApp* pApp = AfxGetApp();
    //pApp->WriteProfileString("MiscConfigure", "InitPath",  path_buffer);
    
    newfilename = fn.lpstrFile + fn.nFileOffset;
    
    CString str;
    while (newfilename[0] != 0)
    {
      strcpy(path_buffer2, path_buffer);
      strcat(path_buffer2, newfilename);      
      str = path_buffer2;
      str.TrimLeft();
      str.TrimRight();
      
      strFullFileNameArray.Add(str);
      newfilename = newfilename + strlen(newfilename) + 1;
    }
    
    return ERR_OK_I;
  }
    
  delete pFileNameBuffer;
  
  return ERR_WIN32_ERROR_I;
}


U32 CFileOps_i::ShowFilePropertiesDlg(LPCTSTR lpszFileName)
{
  if(!lpszFileName )
  {
    return ERROR_INVALID_PARAMETER;
  }
  
  SHELLEXECUTEINFO shExecInfo = { 0 };
  shExecInfo.cbSize = sizeof(shExecInfo);
  shExecInfo.lpFile = lpszFileName;
  shExecInfo.lpVerb = _T("properties" ); // Context menu item
  shExecInfo.fMask  = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI; // This is the key, see MSDN
  
  // Show properties
  if(!ShellExecuteEx( &shExecInfo ))
  {
    return GetLastError();
  }
  else
    return 0;
}

bool CFileOps_i::IsShortcut(LPCTSTR lpctszPath)
{
  SHFILEINFO shFileInfo = { 0 };
  shFileInfo.dwAttributes = SFGAO_LINK;
  return ((SHGetFileInfo( lpctszPath, 
    0, 
    &shFileInfo, 
    sizeof( shFileInfo ),
    SHGFI_ATTR_SPECIFIED | SHGFI_ATTRIBUTES )) && ( shFileInfo.dwAttributes & SFGAO_LINK ));
}

// Select a file in explorer
U32 CFileOps_i::OpenFileInExplorer(LPCTSTR lpctszFileToSelect)
{  
  U32 ret = 0;
  TCHAR szCmd[MAX_PATH];
  
  // This is the command line for explorer which tells it to select the given file  
  wsprintf(szCmd, _T( "/Select, %s" ), lpctszFileToSelect);
  
  // Prepare shell execution params  
  SHELLEXECUTEINFO shExecInfo   = { 0 };  
  shExecInfo.cbSize             = sizeof(shExecInfo);  
  shExecInfo.lpFile             = _T( "Explorer.exe" );  
  shExecInfo.lpParameters       = szCmd;  
  shExecInfo.nShow              = SW_SHOWNORMAL;  
  shExecInfo.lpVerb             = _T( "Open" ); // Context menu item   
  shExecInfo.fMask              = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI;      
  if(!ShellExecuteEx( &shExecInfo ))
    ret = GetLastError();
  
  return ret;
}


static void ExpandEnvironmentStringsEx(LPTSTR lpSrc,DWORD dwSrc)
{
  TCHAR szBuf[312];

  if (ExpandEnvironmentStrings(lpSrc,szBuf,COUNTOF_I(szBuf)))
    lstrcpyn(lpSrc,szBuf,dwSrc);
}

static void PathCanonicalizeEx(LPTSTR lpSrc)
{
  TCHAR szDst[MAX_PATH];
  
  if (PathCanonicalize(szDst,lpSrc))
    lstrcpy(lpSrc, szDst);
}


//  Purpose: Try to get the path to which a lnk-file is linked
//
//  Manipulates: pszResPath
bool CFileOps_i::PathGetLnkPath(LPCTSTR pszLnkFile, LPTSTR pszResPath, INT cchResPath)
{
  IShellLink       *psl;
  WIN32_FIND_DATA  fd;
  bool             bSucceeded = FALSE;
  
  if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IShellLink, (void**)&psl)))
  {
    IPersistFile *ppf;

    if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile,(void **)&ppf)))
    {
#ifndef HAVE_UNICODE_I
      WORD wsz[MAX_PATH];
      MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,
                          pszLnkFile,-1,wsz,MAX_PATH);
      if (SUCCEEDED(ppf->Load(wsz,STGM_READ)))
      {
        if (NOERROR == psl->GetPath(pszResPath,cchResPath,&fd,0))
          bSucceeded = TRUE;
      }
#else
      if (SUCCEEDED(ppf->Load(pszLnkFile,STGM_READ)))
      {
        if (NOERROR == psl->GetPath(pszResPath,cchResPath,&fd,0))
          bSucceeded = TRUE;
      }  
#endif 

      ppf->Release( );
    }
    psl->Release();
  }

  // This additional check seems reasonable
  if (!lstrlen(pszResPath))
    bSucceeded = FALSE;

  if (bSucceeded) 
  {
    ExpandEnvironmentStringsEx(pszResPath,cchResPath);
    PathCanonicalizeEx(pszResPath);
  }

  return(bSucceeded);
}




/*
  if (PathIsLnkFile(szFileName))
    PathGetLnkPath(szFileName,szFileName,COUNTOF(szFileName));
*/

//
//
//  Purpose: Determine wheter pszPath is a Windows Shell Link File by
//           comparing the filename extension with ".lnk"
//
bool CFileOps_i::PathIsLnkFile(LPCTSTR pszPath)
{
  TCHAR tchResPath[MAX_PATH];
  
  if (!pszPath || !*pszPath)
    return FALSE;

  if (lstrcmpi(PathFindExtension(pszPath), _T(".lnk")))
    return FALSE;

  else
    return PathGetLnkPath(pszPath,tchResPath,COUNTOF_I(tchResPath));
}


#if 0 
///////////////////////////////////////////////////////////////////////////////
//
//
//  Name: PathIsLnkToDirectory()
//
//  Purpose: Determine wheter pszPath is a Windows Shell Link File which
//           refers to a directory
//
//  Manipulates: pszResPath
//
bool ITS_PathIsLnkToDirectory(LPCTSTR pszPath,LPTSTR pszResPath, INT cchResPath)
{  
  TCHAR tchResPath[MAX_PATH];
  
  if (PathIsLnkFile(pszPath)) 
  {
    if (PathGetLnkPath(pszPath,tchResPath,sizeof(TCHAR)*COUNTOF(tchResPath))) 
    {
      if (PathIsDirectory(tchResPath)) 
      {
        lstrcpyn(pszResPath,tchResPath,cchResPath);
        return (TRUE); 
      }
      else
        return FALSE; 
    }
    else
      return FALSE; 
  }
  else
    return FALSE;
}


void GetDefaultDir(LPTSTR lpOpenWithDir,int cchOpenWithDir)
{
  LPITEMIDLIST pidl;
  LPMALLOC     lpMalloc;
  
  if (NOERROR == SHGetSpecialFolderLocation(
    NULL,CSIDL_DESKTOP,&pidl))
  {
    SHGetPathFromIDList(pidl,lpOpenWithDir);
    if (NOERROR == SHGetMalloc(&lpMalloc))
    {
      lpMalloc->Free(pidl);
      lpMalloc->Release();
    }
  }
  else
    GetWindowsDirectory(lpOpenWithDir,cchOpenWithDir);
}
#endif 


//Determines whether a path to a file system object such as a file or directory is valid. 
bool CFileOps_i::IsFileExist(LPCTSTR pszFile)
{
  if(pszFile)
    return PathFileExists(pszFile);
  else
    return FALSE;
}
