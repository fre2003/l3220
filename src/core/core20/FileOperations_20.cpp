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
#include "../inc/shlwapi.h"
#include "shellapi.h"


//
// Remove all files and folders under specify folder to Recycle Bin.
// Can remove all readonly and hiden folders and files.
// The files and folders support "*.*".
// 
// Example
// Remove the folder 1 include its folders and files to Recycle bin.
// DeleteFiles(NULL, _T("C:\\1")); 
// 
// pszSource = _T("C:\\1\\c*.*"), the "c:\\1" is not delete, but c*.* will be remove to Recycle Bin.
// pszSource = _T("C:\\1\\*.c"),  remove all *.c files and folders to Recycle Bin.
//

bool  CFileOps_i::DeleteFiles(HWND hWnd, LPCTSTR pszSource, bool bSilent) // support *.*
{
  SHFILEOPSTRUCT FileOp = {0, };
  TCHAR from[MAX_PATH] = {0, };
  lstrcpy(from, pszSource);
  
  FileOp.hwnd = hWnd;
  FileOp.wFunc = FO_DELETE;
  FileOp.pFrom = (LPCTSTR)from; //"C:\\bak\\codefmtbak\\*.*"
  FileOp.fFlags = (FILEOP_FLAGS)((bSilent)?(FOF_SILENT|FOF_NOCONFIRMATION): (FOF_ALLOWUNDO|FOF_SIMPLEPROGRESS)); //(FOF_SIMPLEPROGRESS|FOF_ALLOWUNDO);
  
  if(SHFileOperation(&FileOp) == 0) // if will not be placed in the Recycle Bin, use DeleteFile. 
    return TRUE;
  else
    return FALSE;
}


//
// Copy the specific folder which include all subfolders and files to the other folder.
// Replace all read-only and hidden.
//
// Example:
//  Copy the folder 'exmaples' which include all subfolders and files to folder '1'.
//  CopyFiles(NULL, _T("C:\\examples"), _T("C:\\1")); // C:\\examples --> C:\\1\\examples
//  
//  Copy the files and sub-folders of 'exmaples' to folder '1'. Replace all read-only and hidden.
//  CopyFiles(NULL, _T("C:\\examples\\*"), _T("C:\\1")); // C:\\1 --> C:\\1\\*.*(folder and files from examples).
//

// CopyFiles(NULL, "c:\\tst",  "C:\\bak\\codefmtbak");
// support *.* "c:\\tst"; "C:\\bak\\codefmtbak"; 
bool CFileOps_i::CopyFiles(HWND hWnd, LPCTSTR pszFrom, LPCTSTR pszTo, bool bSilent) 
{
  SHFILEOPSTRUCT FileOp = {0, };
  TCHAR from[MAX_PATH]  = {0, };
  TCHAR to[MAX_PATH]    = {0, };

  lstrcpy(from, pszFrom);
  lstrcpy(to, pszTo);

  ZeroMemory(&FileOp, sizeof(SHFILEOPSTRUCT));

  FileOp.hwnd = hWnd;
  FileOp.wFunc = FO_COPY;
  FileOp.pFrom = (LPCTSTR)from;
  FileOp.pTo = (LPCTSTR)to;
  FileOp.fFlags = (FILEOP_FLAGS)(bSilent ? (FOF_SILENT|FOF_NOCONFIRMATION): (FOF_NOCONFIRMATION|FOF_NOERRORUI));
  
  if(SHFileOperation(&FileOp) == 0)
    return TRUE;
  else
    return FALSE;
}

bool CFileOps_i::Move(HWND hWnd, LPCTSTR pszFrom, LPCTSTR pTo, bool bSilent)
{
  SHFILEOPSTRUCT FileOp = {0, };
  
  FileOp.hwnd = hWnd;
  FileOp.wFunc = FO_MOVE;
  FileOp.pFrom = (LPCTSTR)pszFrom;
  FileOp.pTo = (LPCTSTR)pTo;
  FileOp.fFlags = (FILEOP_FLAGS)(bSilent ? (FOF_SILENT|FOF_NOCONFIRMATION): (FOF_NOCONFIRMATION|FOF_NOERRORUI));
  
  if(SHFileOperation(&FileOp) == 0)
    return TRUE;
  else
    return FALSE;
}


// Rename pszOldName to pszNewName.
// support File or Directory. Rename(_T("C:\\DirOld"), _T("C:\\DirNew"));
bool  CFileOps_i::Rename(LPCTSTR pszOldName, LPCTSTR pszNewName)
{
  SHFILEOPSTRUCT FileOp = {0, };
    
  FileOp.wFunc = FO_RENAME;
  FileOp.pFrom = (LPCTSTR)pszOldName; 
  FileOp.pTo   = (LPCTSTR)pszNewName; 
  FileOp.fFlags = (FILEOP_FLAGS)(FOF_SILENT|FOF_NOCONFIRMATION);
      
  if(SHFileOperation(&FileOp) == 0) // if will not be placed in the Recycle Bin, use DeleteFile. 
    return TRUE;
  else
    return FALSE;
}

//
// pszCmd should be less than 1024 bytes.
//
U32 CProcessUtil_i::RunSilentProcess(LPCTSTR pszCmd, LPCTSTR pAppCmd, DWORD dwTimeOut)
{
  HANDLE hProcess = 0;
  HANDLE hThread = 0;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  TCHAR szCmd[1024] = {0,};
 
  if(pszCmd)
  {
    lstrcpyn(szCmd, pszCmd, sizeof(szCmd)-1);
  }
  
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si); //si.cb = sizeof(STARTUPINFO);
  //si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
  si.hStdOutput = NULL;
  si.hStdInput  = NULL;
  si.hStdError  = NULL;
  // Use this if you want to show the child.
  si.wShowWindow = SW_HIDE;
  
  ZeroMemory( &pi, sizeof(pi) );
  
  //SetCurrentDirectory(pszCurPath);

  //WinExec(pszCmd, SW_SHOW);
  //return FALSE;

  // Start the child process.
  if( !CreateProcess(pAppCmd , // the app line
    szCmd, //           "C:\\pclint\\tst.bat" Command line. , bat  or cmd.exe /k ....
    NULL,             // Process handle not inheritable. 
    NULL,             // Thread handle not inheritable. 
    FALSE,            // Set handle inheritance to FALSE. 
    CREATE_NO_WINDOW, // creation flags.CREATE_NEW_CONSOLE CREATE_NO_WINDOW
    NULL,             // Use parent's environment block. 
    NULL,             // Use parent's starting directory. 
    &si,              // Pointer to STARTUPINFO structure.
    &pi )             // Pointer to PROCESS_INFORMATION structure.
    ) 
  {
    return ERR_WIN32_ERROR_I; 
  }
  
  hProcess = pi.hProcess;
  hThread  = pi.hThread;

  if (hProcess == NULL)
  {
    OutputDbgStr(_T("Error: Internal error. \n"));
    return ERR_WIN32_ERROR_I;
  }
  else
  {
    switch (::WaitForSingleObject(hProcess, dwTimeOut)) // WaitForSingleObject( pi.hProcess,  );
    {
    case WAIT_OBJECT_0:
      break;
    case WAIT_TIMEOUT:
      break;
    default:
      break;
    }
  }
  
  DWORD dwExitCode = 0;
  GetExitCodeProcess(hProcess, &dwExitCode);
  
  // Close process and thread handles. 
  CloseHandle( hProcess );
  CloseHandle( hThread );
  
  return dwExitCode;
}

U32 CProcessUtil_i::RunProcess(LPCTSTR pszCmdLine, DWORD dwTimeOut)
{
  HANDLE hProcess = 0;
  HANDLE hThread = 0;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  TCHAR szCmd[1024] = {0,};
  
  lstrcpy(szCmd, pszCmdLine);
  
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si); //si.cb = sizeof(STARTUPINFO);
  //si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
  si.hStdOutput = NULL;
  si.hStdInput  = NULL;
  si.hStdError  = NULL;

  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOW;

  ZeroMemory( &pi, sizeof(pi) );

  // Start the child process.
  if( !CreateProcess(NULL , // the app line
    szCmd,            // "C:\\pclint\\tst.bat" Command line. , bat  or cmd.exe /k ....
    NULL,             // Process handle not inheritable. 
    NULL,             // Thread handle not inheritable. 
    FALSE,            // Set handle inheritance to FALSE. 
    CREATE_NO_WINDOW, // creation flags.CREATE_NEW_CONSOLE CREATE_NO_WINDOW
    NULL,             // Use parent's environment block. 
    NULL,             // Use parent's starting directory. 
    &si,              // Pointer to STARTUPINFO structure.
    &pi )             // Pointer to PROCESS_INFORMATION structure.
    ) 
  {
    return ERR_WIN32_ERROR_I;
  }
  
  hProcess = pi.hProcess;
  hThread  = pi.hThread;

  if (hProcess == NULL)
  {
    OutputDbgStr(_T("Error: Internal error. \n"));
    return ERR_WIN32_ERROR_I;
  }
  else
  {
    if(dwTimeOut != 0)
    {
      switch (::WaitForSingleObject(hProcess, dwTimeOut)) // WaitForSingleObject( pi.hProcess,  );
      {
      case WAIT_OBJECT_0:
        break;
      case WAIT_TIMEOUT:
        break;
      default:
        break;
      }
    }
  }
  
  DWORD dwExitCode = 0;
  GetExitCodeProcess(hProcess, &dwExitCode);
  
  // Close process and thread handles. 
  CloseHandle( hProcess );
  CloseHandle( hThread );
  
  return dwExitCode;
}


U32 CProcessUtil_i::RunConsoleProcess(LPCTSTR pszCmdLine, DWORD dwTimeOut)
{
  HANDLE hProcess = 0;
  HANDLE hThread = 0;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  TCHAR szCmd[1024] = {0,};
  
  lstrcpy(szCmd, pszCmdLine);
  
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si); //si.cb = sizeof(STARTUPINFO);
  //si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
  si.hStdOutput = NULL;
  si.hStdInput  = NULL;
  si.hStdError  = NULL;

  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOW;

  ZeroMemory( &pi, sizeof(pi) );

  // Start the child process.
  if( !CreateProcess(NULL , // the app line
    szCmd,            // "C:\\pclint\\tst.bat" Command line. , bat  or cmd.exe /k ....
    NULL,             // Process handle not inheritable. 
    NULL,             // Thread handle not inheritable. 
    FALSE,            // Set handle inheritance to FALSE. 
    CREATE_NEW_CONSOLE, // creation flags.CREATE_NEW_CONSOLE CREATE_NO_WINDOW
    NULL,             // Use parent's environment block. 
    NULL,             // Use parent's starting directory. 
    &si,              // Pointer to STARTUPINFO structure.
    &pi )             // Pointer to PROCESS_INFORMATION structure.
    ) 
  {
    return ERR_WIN32_ERROR_I;
  }
  
  hProcess = pi.hProcess;
  hThread  = pi.hThread;

  if (hProcess == NULL)
  {
    OutputDbgStr(_T("Error: Internal error. \n"));
    return ERR_WIN32_ERROR_I;
  }
  else
  {
    if(dwTimeOut != 0)
    {
      switch (::WaitForSingleObject(hProcess, dwTimeOut)) // WaitForSingleObject( pi.hProcess,  );
      {
      case WAIT_OBJECT_0:
        break;
      case WAIT_TIMEOUT:
        break;
      default:
        break;
      }
    }
  }
  
  DWORD dwExitCode = 0;
  GetExitCodeProcess(hProcess, &dwExitCode);
  
  // Close process and thread handles. 
  CloseHandle( hProcess );
  CloseHandle( hThread );
  
  return dwExitCode;
}


#if 0 
bool ImportManifySeting()
{
  TCHAR* pTmpFile = _T("C:\\Manify.reg");
  
  SetFileAttributes(pTmpFile, FILE_ATTRIBUTE_NORMAL);
  
  //GetTemplate(IDR_TXT_MAGNIFY_REG, pTmpFile);
  
  TCHAR szCmd[128];
  szCmd[0] = _T('\0');
  wsprintf(szCmd, _T("regedit /c /s \"%s\""), pTmpFile);
  
  if(PathFileExists(pTmpFile))
  {
    if(RunSilentProcess_2(szCmd, NULL, 10000) == 0)
    {
       DeleteFile(pTmpFile);
    }
    else
      return FALSE;
  }
  else
    return FALSE; 
  
  //GlobalFree(pRegString);
  
  return TRUE;
}


VOID* GetTemplate(HMODULE hModule, UINT uResId, LPCTSTR pszFile) // Call  GlobalFree(hGlobal) to free.
{
  HGLOBAL hResourceData = 0;
  HRSRC hResource = 0;
  
  HMODULE hMod = 0;
  
  hMod = GetModuleHandle("configres.dll");

  hResource = FindResource(hMod, MAKEINTRESOURCE(uResId), "TXT");
  
  if(hResource)
    hResourceData = LoadResource(hMod, hResource);
  else
    return FALSE;
  
  if (hResourceData)
  {
    DWORD dwSize = SizeofResource(hMod, (HRSRC)hResource);

#if 1
    // hResData is not the real HGLOBAL (we can't lock it)
    // let's make it real
    
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD,dwSize+1); // plus 1 to save string if necessary.
    if (!hGlobal)
    {
      FreeResource(hResource);
      return FALSE;
    }
    
    TCHAR *pDest = reinterpret_cast<TCHAR *> (GlobalLock(hGlobal));
    TCHAR *pSrc  = reinterpret_cast<TCHAR *> (LockResource(hResourceData));
    if (!pSrc || !pDest)
    {
      GlobalFree(hGlobal);
      FreeResource(hResourceData);
      return FALSE;
    };
    
    CopyMemory(pDest, pSrc, dwSize);

    // it is a string , so ... 
    pDest[dwSize] = _T('\0');

    FreeResource(hResourceData);
    GlobalUnlock(hGlobal);
    
    unsigned char* pData = NULL;
    if (pData = reinterpret_cast<unsigned char*> (GlobalLock(hGlobal)) )
    {
      //strTpl = (LPCTSTR)pData;
      //DWORD size = strTpl.GetLength();
    }
    
#else 
    HGLOBAL hRes = LockResource(hResourceData);
    char *pSrc  = (char*)hRes;

    // OutputDebugString(strTpl);
    // The resource template is a string stream end with '\0', not need to copy to temp buffer.
    strTpl = pSrc;
    if(strTpl.GetLength() > dwSize)
      strTpl.SetAt(dwSize, '\0');
    
    FreeResource(hResourceData);
    UnlockResource(hRes);
#endif 


#if 0 
    f = fopen("c:\\1.txt", "w");
    if(f)
    {
      fprintf(f, "%s", (char*)hGlobal);
      fclose(f);
    }
#endif // #if 0
#if 0 
  FILE* f = NULL;
    
    f = fopen(pszFile, "w");
    if(f)
    {
      fprintf(f, "%s", (char*)hGlobal);
      fclose(f);
      GlobalFree(hGlobal);
    }
    else
      return (VOID*)NULL;
#endif 
    //FreeLibrary(hMod);
  }
  else
  {
    //FreeLibrary(hMod);
    return FALSE;
  }

  return NULL;
}
#endif 


//////////////////////////////////////////////////////////////////////////
// Returns associated file icon. For eg: for winword exe this function 
// returns the winword icon
//////////////////////////////////////////////////////////////////////////
HICON CFileAssoc_i::GetAssociatedFileIcon( LPCTSTR lpctszFilePath)
{
 SHFILEINFO shFileInfo = { 0 };
 
 // For retrieving icon
 SHGetFileInfo( lpctszFilePath, 
   FILE_ATTRIBUTE_NORMAL, 
   &shFileInfo, 
   sizeof( shFileInfo ), 
   SHGFI_SMALLICON | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES );
 
 // Icon to return
 return shFileInfo.hIcon;
}



//////////////////////////////////////////////////////////////////////////
// Returns type of exe file. i.e. whether it's a console application or a
// windows application or an MS-DOS application
// RETRUN:
// WIN_TYPE_APP     :  Windows Application.
// WIN_TYPE_CONSOLE :  Win32 Console Application
// WIN_TYPE_MS_DOS  :  MS-DOS .exe, .com or .bat file
// WIN_TYPE_UNKNOWN : Unknown file type.
// pszTypeName : should be 80 chars.
//////////////////////////////////////////////////////////////////////////
CFileAssoc_i::ENUM_WIN_TYPE CFileAssoc_i::GetExeType( LPCTSTR pszFilePath, OUT TCHAR* pszTypeName)
{
  TCHAR szFile[MAX_PATH] = {0,};
    
    if(pszFilePath == NULL)
    {
      GetModuleFileName(NULL, szFile, MAX_PATH-1);
      pszFilePath = &szFile[0];
    }
    
    SHFILEINFO shFileInfo = { 0 };
    
    // Second call is for retrieving file type
    const DWORD dwRetVal = SHGetFileInfo( pszFilePath, 
      FILE_ATTRIBUTE_NORMAL, 
      &shFileInfo, 
      sizeof( shFileInfo ), 
      SHGFI_EXETYPE );

    if( dwRetVal )
    {
    /************************************************************************
    dwRetVal is interpreted as follows...
    LOWORD = NE or PE and HIWORD = 3.0, 3.5, or 4.0  Windows application 
    LOWORD = MZ and HIWORD = 0  MS-DOS .exe, .com, or .bat file 
    LOWORD = PE and HIWORD = 0  Win32 console application 
      ************************************************************************/
      const WORD wLowWord =  LOWORD( dwRetVal );
      const WORD wHiWord = HIWORD( dwRetVal );
      const WORD wPEWord = MAKEWORD( 'P', 'E' );
      const WORD wMZWord = MAKEWORD( 'M', 'Z' );
      const WORD wNEWord = MAKEWORD( 'N', 'E' );
      
      // Read above comments to understand what's happening
      if( wLowWord == wPEWord || wLowWord == wNEWord )
      {
        if( wHiWord == 0 )
        {
          return WIN_TYPE_CONSOLE_E; // "Win32 Console Application"
        }
        else
        {
          return WIN_TYPE_APP_E; // "Windows application" 
        }// End if
      }
      else if( wLowWord == wMZWord && wHiWord == 0 )
      {
        return WIN_TYPE_MS_DOS_E; // "MS-DOS .exe, .com or .bat file"
      }
      else
      {
        return WIN_TYPE_UNKNOWN_E; //"Unknown file type"
      }// End if

      if(pszTypeName)
      {
        lstrcpy(pszTypeName, shFileInfo.szTypeName);
      }
    }// End if
    else
    {
      return WIN_TYPE_UNKNOWN_E;
    }
}


#if 0 
// Find files in pszDirectory
void FindFiles(PCSTR pszDirectory, CStringArray& strFullFileNameArray)
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
        strFindString.TrimRight("\\"); 
        strFindString.TrimRight("/");
        strFindString += "\\*.*";
        
        TRACE("Is Directory\n");
        
        hFind = FindFirstFile(strFindString, &find);
        
        if (hFind == INVALID_HANDLE_VALUE) 
        {
            // printf("No .c files found.");
        }
        
        CString strFileName;
        
        while (!fFinished) 
        {
            //lstrcpy(szNewPath, szDirPath); 
            //lstrcat(szNewPath, find.cFileName); 
            strFileName = find.cFileName ;
            if(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (strFileName != "." && strFileName != "..") 
                {
                    CString strDir;
                    strDir = pszDirectory;
                    strDir += "\\";
                    strDir += strFileName;
                    //strFileNameList.AddTail()
                    //TRACE("\n'%s' Directory: \n", strDir);  
                    FindFiles(strDir, /*strFileNameList ,*/strFullFileNameArray);
                }
            }
            else
            {
                // TRACE("%s \n", find.cFileName);

              // filter files.
#if 1
                if(strFileName.Right(2) == ".c" 
                  || strFileName.Right(2) == ".C"
                  || strFileName.Right(2) == ".h" 
                  || strFileName.Right(2) == ".H" )
                {
                    CString strFileFullName = pszDirectory;
                    strFileFullName += "\\";
                    strFileFullName += strFileName;
                    //strFileNameList.AddTail(strFileName);
                    strFullFileNameArray.Add(strFileFullName);
                }
#endif
            }
            if ( !FindNextFile(hFind, &find) )
            {
                if (GetLastError() == ERROR_NO_MORE_FILES) 
                { 
                    //::MessageBox(NULL, "No more .c files.","Search completed.", MB_OK); 
                    fFinished = TRUE; 
                }
                else
                { 
                    TRACE("Couldn't find next file."); 
                    break;
                }
            }
        }
        
        FindClose(hFind);
    }
}
#endif 

#if 0 
// New codepage detection functions from Textview 6.0
#define NCP_DEFAULT            0
#define NCP_UTF8               1
#define NCP_UTF8_SIGN          2
#define NCP_UNICODE            4
#define NCP_UNICODE_REVERSE    8
#define NCP_UNICODE_BOM       16

bool SaveFile(LPCSTR pszFile, int iCodePage, bool bSaveCopy)
{
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

  // get text
  cbData = SendMessage(hwnd,SCI_GETLENGTH,0,0);
  lpData = GlobalAlloc(GPTR,cbData + 1);
  SendMessage(hwnd,SCI_GETTEXT,GlobalSize(lpData),(LPARAM)lpData);
  
  if (cbData == 0)
    bWriteSuccess = SetEndOfFile(hFile);

  else
  {
    if (iCodePage & NCP_UNICODE)
    {
      LPWSTR lpDataWide;
      int    cbDataWide;
      CPINFO cpi;
      UINT   uCP_UTF8;

      // UTF-8 text is interpreted as ANSI when saving as Unicode on Windows 95
      uCP_UTF8 = (GetCPInfo(CP_UTF8, &cpi) || IsValidCodePage(CP_UTF8)) ? CP_UTF8 : CP_ACP;

      lpDataWide = GlobalAlloc(GPTR,cbData * 2 + 16);
      cbDataWide = MultiByteToWideChar(uCP_UTF8,0,lpData,cbData,lpDataWide,GlobalSize(lpDataWide));

      if (iCodePage & NCP_UNICODE_BOM) 
      {
        if (iCodePage & NCP_UNICODE_REVERSE)
          WriteFile(hFile,(LPCVOID)"\xFE\xFF",2,&dwBytesWritten,NULL);
        else
          WriteFile(hFile,(LPCVOID)"\xFF\xFE",2,&dwBytesWritten,NULL); 
      }
      
      if (iCodePage & NCP_UNICODE_REVERSE)
      {
        _swab((char*)lpDataWide,(char*)lpDataWide,cbDataWide * sizeof(WCHAR));
      }

      bWriteSuccess = WriteFile(hFile,lpDataWide,cbDataWide * sizeof(WCHAR),&dwBytesWritten,NULL);

      GlobalFree(lpData);
    }

    else if (iCodePage & NCP_UTF8)
    {
      if (iCodePage & NCP_UTF8_SIGN)
        WriteFile(hFile,(LPCVOID)"\xEF\xBB\xBF",3,&dwBytesWritten,NULL);

      bWriteSuccess = WriteFile(hFile,lpData,cbData,&dwBytesWritten,NULL);
    }

    else // convert text to 8bit
    {
      //LPWSTR lpDataWide = GlobalAlloc(GPTR,cbData * 2 + 16);
      //int    cbDataWide = MultiByteToWideChar(CP_UTF8,0,lpData,cbData,lpDataWide,GlobalSize(lpDataWide));

      //ZeroMemory(lpData,GlobalSize(lpData));
      //cbData = WideCharToMultiByte(CP_ACP,0,lpDataWide,cbDataWide,lpData,GlobalSize(lpData),NULL,NULL);
      //GlobalFree(lpDataWide);

      bWriteSuccess = WriteFile(hFile,lpData,cbData,&dwBytesWritten,NULL);

      GlobalFree(lpData);
    }
  }

  CloseHandle(hFile);

  if (bWriteSuccess)
  {
    if (!bSaveCopy)
      SendMessage(hwnd,SCI_SETSAVEPOINT,0,0);

    return TRUE;
  }

  else
    return FALSE;

}
#endif 
