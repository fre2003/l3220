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

#if 0 
//////////////////////////////////////////////////////////////////////////
// 
// EXAMPLE
// CreateShortcut_2(_T("C:\\doc\\Manual.pdf"), "Help\\My Manual", "My Manual Doc", SW_SHOWMAXIMIZED,TRUE, 0);
// 
//////////////////////////////////////////////////////////////////////////
U32 CreateShortcut_2(LPCTSTR pszSrcPath,
                        LPCTSTR pszShortcutName, // Relative path. "<Programs>\\Help\\My Manual"
                        LPCTSTR pszDescription, 
                        bool bDelete,
                        bool bDesktop) // Desktop or Programs.
{
  TCHAR szBuf[MAX_PATH] = {0, };
  TCHAR szDesPath[MAX_PATH] = {0,};
  WCHAR* pszDesPath = NULL;
 
  if(SHGetSpecialFolderPath(NULL, szBuf, (bDesktop==0)?CSIDL_PROGRAMS:CSIDL_DESKTOP, 1))
  {
    //CreateDirectory(szBuf, NULL);
  }
  
  wsprintf(szDesPath, _T("\\%s\\.lnk"), pszShortcutName);
  
  HRESULT hres = NULL;
  IShellLink  *psl = NULL; 
  bool   bRet = FALSE;
  IPersistFile *ppf = NULL; 
  
  if(bDelete)
    DeleteFile(szDesPath);
  
  long r = CoInitialize(0);
  if(r == S_OK || r == S_FALSE)
  {
    hres = CoCreateInstance  (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl); //Geth COM Object.
    
    if (!SUCCEEDED(hres)) 
      goto   error;
    
    
    // Query IPersistFile Inteface to save shortcup.
    hres   =   psl->QueryInterface   (IID_IPersistFile, (void   **)&ppf);   
    if   (!SUCCEEDED   (hres))   
      goto   error;
    
    hres =  psl->SetPath(pszSrcPath);   
    
    if (!SUCCEEDED(hres))  
      goto   error;   
    
    hres   =   psl->SetArguments(_T(" ")); // the argument is null.
    if (! SUCCEEDED(hres)) 
      goto   error;   
    
    hres  =  psl->SetDescription (pszDescription);
    if   (!SUCCEEDED(hres)) 
      goto   error;   
    
#ifndef _UNICODE
    WORD wsz[MAX_PATH]; // Unicode buffer.
    //Convert ANSI String to Unicode .
    MultiByteToWideChar(CP_ACP, 0, szDesPath, -1, wsz, MAX_PATH);   
    pszDesPath = (WCHAR*)wsz;
#else
    pszDesPath = (WCHAR*)szDesPath;
#endif 
    
    // Save it 
    hres = ppf->Save(pszDesPath,TRUE);
    if(!SUCCEEDED(hres))
      goto   error;   
    
    bRet = TRUE;
    
error:   
    // Free all resource.
    ppf->Release  ();
    psl->Release  ();
    
    CoUninitialize();
    
    return 0;   
  }
  else
  {
    return 1;
  }
}
#endif 

/////////////////////////////////////////////////////////////
// 
// EXAMPLE
// CreateShortcut(_T("C:\\doc\\Manual.pdf"), "Help\\My Manual", "My Manual Doc", SW_SHOWMAXIMIZED,TRUE, 0);
// 
//////////////////////////////////////////////////////////////////////////
U32 CFileAssoc_i::CreateShortcut(LPCTSTR pszSrcPath, 
                       LPCTSTR pszShortcutName, // Relative path. "<Programs>\\Help\\My Manual"
                       LPCTSTR pszDescription, 
                       LPCTSTR pszArg, 
                       bool bDelete, 
                       bool bDesktop) // Desktop or Programs.
{
  TCHAR szBuf[MAX_PATH] = {0, };
  TCHAR szDesPath[MAX_PATH] = {0,};
  WCHAR* pszDesPath = NULL;

  // CSIDL_STARTMENU , CSIDL_STARTUP, CSIDL_SENDTO, CSIDL_WINDOWS, CSIDL_PERSONAL(My Documents), CSIDL_DESKTOPDIRECTORY, 
  //  CSIDL_COMMON_ALTSTARTUP, CSIDL_COMMON_PROGRAMS , CSIDL_COMMON_STARTMENU, CSIDL_COMMON_DESKTOPDIRECTORY   CSIDL_DESKTOP
  /*
	  	SHGetSpecialFolderLocation(hwndDlg, CSIDL_STARTMENU, &ppidl);					                           
			SHGetPathFromIDList(ppidl, startmenu);					
			sprintf(linkname, "%s\\CoolPlayer.lnk", startmenu);
  */
  if(SHGetSpecialFolderPath(NULL, szBuf, (bDesktop==0)?CSIDL_COMMON_PROGRAMS:CSIDL_COMMON_DESKTOPDIRECTORY, 1))
  {
    //CreateDirectory(szBuf, NULL);
  }
  
  //wsprintf(szDesPath, _T("%s\\vc6"), szBuf);
  // CreateDirectory(szDesPath, NULL);
  if(bDesktop)
  {
	  CString strFileName = PathGetLast_i(CString(pszShortcutName));
	  wsprintf(szDesPath, _T("%s\\%s.lnk"), szBuf, strFileName);
  }
  else 
  {
	  wsprintf(szDesPath, _T("%s\\%s.lnk"), szBuf, pszShortcutName);
	  //CString strFolder = ShellSplitLast_2(CString(szDesPath));
	  CFileOps_i::MakeDir(szDesPath, TRUE);
  }

  HRESULT hres = NULL;
  IShellLink  *psl = NULL; 
  bool   bRet = FALSE;
  IPersistFile *ppf = NULL;
  
  if(bDelete)
    DeleteFile(szDesPath);
  
  long r = CoInitialize(0);
  if(r == S_OK || r == S_FALSE)
  {	  
    hres = CoCreateInstance  (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl); //Geth COM Object.
    
    if (!SUCCEEDED(hres)) 
      goto   error;
    
    
    // Query IPersistFile Inteface to save shortcup.
    hres   =   psl->QueryInterface   (IID_IPersistFile, (void   **)&ppf);   
    if   (!SUCCEEDED   (hres))   
      goto   error;
    
    hres =  psl->SetPath(pszSrcPath);   
    
    if (!SUCCEEDED(hres))  
      goto   error;   
    
    hres   =   psl->SetArguments(pszArg); // the argument is null.
    if (! SUCCEEDED(hres)) 
      goto   error;   
    
    hres  =  psl->SetDescription (pszDescription);
    if   (!SUCCEEDED(hres)) 
      goto   error;   
    
#ifndef _UNICODE
    WORD wsz[MAX_PATH]; // Unicode buffer.
    //Convert ANSI String to Unicode .
    MultiByteToWideChar(CP_ACP, 0, szDesPath, -1, wsz, MAX_PATH);   
    pszDesPath = (WCHAR*)wsz;
#else
    pszDesPath = (WCHAR*)szDesPath;
#endif 
    
    // Save it 
    hres = ppf->Save(pszDesPath,TRUE);
    if(!SUCCEEDED(hres))
    {
		fprintf(stdout, "fail to save err=%d\n", GetLastError());
		goto   error;   
	}
    
    bRet = TRUE;
    
error:   
    // Free all resource.
    ppf->Release  ();
    psl->Release  ();
    
    CoUninitialize();
    
    // Active ... 
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
    
    return 0;   
  }
  else
  {
    return 1;
  }
}

inline CString ShellGetLastPart(LPCTSTR pszStream) 
{ 
  CString strStream = pszStream;
  int pos = strStream.ReverseFind(':');
  if(pos == -1)
    return CString("");
  
  return strStream.Mid(pos + 1, strStream.GetLength() - pos - 1); 
}


static U32 DoRemoveShortcut(LPCTSTR pszSrcPath,// Relative path. "<Programs>\\Help\\My Manual" 
                       BOOL bDesktop,
                       BOOL  bDeleteFile, 
                       DWORD ProgramID, // Desktop or Programs. CSIDL_COMMON_PROGRAMS or CSIDL_PROGRAMS
                       DWORD DeskDirId) // CSIDL_COMMON_DESKTOPDIRECTORY, CSIDL_DESKTOPDIRECTORY
{
  TCHAR szBuf[MAX_PATH] = {0, };
  TCHAR szDesPath[MAX_PATH] = {0,};
  WCHAR* pszDesPath = NULL;
  CString strFolder;
    
  if(SHGetSpecialFolderPath(NULL, szBuf, (bDesktop==0)?ProgramID:DeskDirId, 1))
  {
    //CreateDirectory(szBuf, NULL);
  }
  
//   if(pszShortcutName == NULL)
//   {
//     if(PathFileExists(pszSrcPath))
//     {
//       RemoveDirectory(pszSrcPath);
//     }
//   }
  
#if 1
//   if(SHGetSpecialFolderPath(NULL, szBuf, (bDesktop==0)?CSIDL_PROGRAMS:CSIDL_DESKTOP, 1))
//   {
//   }
  
  if(bDesktop)
  {
    if(bDeleteFile)
    {
      CString strFileName = ShellGetLastPart(CString(pszSrcPath));
	    wsprintf(szDesPath, _T("%s\\%s.lnk"), szBuf, strFileName);
    }
    else 
    {
	    wsprintf(szDesPath, _T("%s\\%s"), szBuf, pszSrcPath); 
    }
  }
  else 
  {
    if(bDeleteFile)
    {
	    wsprintf(szDesPath, _T("%s\\%s.lnk"), szBuf, pszSrcPath);
    }
	  else 
    {
      wsprintf(szDesPath, _T("%s\\%s"), szBuf, pszSrcPath); 
    }
  }

  if(bDeleteFile)
  {
    if(PathFileExists(szDesPath))
      DeleteFile(szDesPath);
  }
  else
  {
    CString strCmd;
    CFileOps_i::DeleteFiles(NULL, szDesPath);
    
    //strCmd.Format("rd /s /q \"%s\"", szDesPath);
    //system(strCmd);
    RemoveDirectory(szDesPath);
  }
   
  TRACE("%s \n", szDesPath);

#endif 
  
  return 0;
}

U32 CFileAssoc_i::RemoveShortcut(LPCTSTR pszSrcShortcut,  // File or folder name , like "WinRar" or "WinRar\\WinRar Help" 
                                bool bFile, // file or Folder
                                bool bDesktop) // Desktop or Programs.
{
#if 0 
  TCHAR szBuf[MAX_PATH] = {0, };
  TCHAR szDesPath[MAX_PATH] = {0,};
  CString strFolder;
  UNREF_PARAM(pszShortcutName);

  if(SHGetSpecialFolderPath(NULL, szBuf, (bDesktop==0)?CSIDL_PROGRAMS:CSIDL_DESKTOP, 1))
  {
  }
  
  if(bDesktop)
  {
	  CString strFileName = PathGetLast_i(CString(pszShortcutName));
	  wsprintf(szDesPath, _T("%s\\%s.lnk"), szBuf, strFileName);
  }
  else 
  {
	  wsprintf(szDesPath, _T("%s\\%s.lnk"), szBuf, pszShortcutName);
	  CString str(pszShortcutName);
	  if(str.Find('\\') != -1)
		strFolder = PathSplitLast_i(CString(szDesPath));
  }
  
  DeleteFile(szDesPath);
  RemoveDirectory(strFolder);

#else
  DoRemoveShortcut(pszSrcShortcut, bDesktop, bFile, CSIDL_COMMON_PROGRAMS, CSIDL_COMMON_DESKTOPDIRECTORY);
  DoRemoveShortcut(pszSrcShortcut, bDesktop, bFile, CSIDL_PROGRAMS, CSIDL_DESKTOPDIRECTORY);
#endif 
	// Active ... 
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0 );

  return 0;
}

