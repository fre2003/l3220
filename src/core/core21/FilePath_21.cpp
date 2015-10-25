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

#pragma comment(lib, "shlwapi.lib")

CString CFilePath_i::GetAppPath(VOID)
{
  TCHAR filepath[MAX_PATH*2];
  HMODULE hInstance = NULL;

  CWinApp*p = AfxGetApp();
  if(p)
  {
    hInstance = AfxGetApp()->m_hInstance;
  }

  filepath[0] = _T('\0');
  //who are we really?  Get the Exe Path
  GetModuleFileName(hInstance,filepath, MAX_PATH-1); 
  //m_strAppName = ShellGetLast_2(CString(filepath));

  return CString(ExtractFilePath(filepath));
}

CFilePath_i::CFilePath_i()
{
}

CFilePath_i::CFilePath_i(LPCTSTR szPath, BOOL bIsFolderPath)
{
	SetPath(szPath, bIsFolderPath);
}

CFilePath_i::CFilePath_i(DWORD dwSpecial)
{
	SetPath(dwSpecial);
}

CFilePath_i::~CFilePath_i()
{

}


void CFilePath_i::SetPath(DWORD dwSpecial)
{
	switch (dwSpecial)
	{
	case 0:
		{
			TCHAR szPath[MAX_PATH];
			GetModuleFileName(0, szPath, MAX_PATH);
			SetPath(szPath);
		}
		break;
  case 1:
		{
			SetPath(GetAppPath());
		}
    break;
	}
}

void CFilePath_i::SetPath(LPCTSTR szPath, BOOL bIsFolderPath)
{
	TCHAR szParamPath[MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	TCHAR szName[_MAX_FNAME], szExt[_MAX_EXT];

	// Reset
	m_sOriginalPath.Empty();
	m_sDriveLabel.Empty();
	bool m_bIsRelative = FALSE;
	m_aDir.RemoveAll();
	m_sExtName.Empty();

	// Original path
	m_sOriginalPath = szPath;

	// Get args and remove them from path
	szParamPath[0] = 0x0;
	lstrcpy(szParamPath, szPath);

	PathUnquoteSpaces(szParamPath);
	if (szParamPath[0] == 0x0) return;

	_tsplitpath(szParamPath, szDrive, szDir, szName, szExt);

	// Drive
	m_sDrive = szDrive;

	// Directory
	m_sDir = szDir;
	m_sDir.Replace(_T('/'), _T('\\'));
	if (!_sDir.IsEmpty()) _bIsRelative = (_sDir[0] != _T('\\'));
	
	// FileTitle
	if (bIsFolderPath)
	{
		_sDir = AddBackSlash(_sDir);
		_sDir += szName;
		_sDir = AddBackSlash(_sDir);
	}
	else
	{
		_sFileTitle = szName;
	}

	// Get extension name (e.g.: "txt")
  if (!_sFileTitle.IsEmpty())
	{
		_sExtName = szExt;
		_sExtName.Remove(_T('.'));
	}
}

// CString CFilePath_i::GetPath(BOOL bAppendArgs, BOOL bOriginal)
// {
// 	CString sPath;
// 
// 	if (bOriginal)
// 		sPath = _sOriginalPath;
// 	else
// 		sPath = GetLocation() + GetFileName();
// 
// 	return sPath;
// }

// CString	CFilePath_i::GetLocation()
// {
// 	return _sDrive + GetDir();
// }
// 
// CString CFilePath_i::GetFileTitle()
// {
// 	return _sFileTitle;
// }
// 
// CString CFilePath_i::GetFileName()
// {
// 	return _sFileTitle + GetExtension();
// }

// 
// 
// BOOL CLocationPath::IsLocalPath()
// {
// 	return !_sDrive.IsEmpty() && !_bIsRelative;
// }
// 
// BOOL CLocationPath::IsRelativePath()
// {
// 	return _bIsRelative;
// }
// 
// BOOL CFilePath_i::IsFilePath()
// {
// 	return !_sFileTitle.IsEmpty();
// }

// 
// BOOL CLocationPath::ExistFile()
// {
// 	if (!IsFilePath()) return FALSE;
// 
// 	return PathFileExists(GetPath());
// }
// 
// BOOL CLocationPath::ExistLocation()
// {
// 	return PathFileExists(GetLocation());
// }

// CString CFilePath_i::GetAbsolutePath(LPCTSTR szBaseFolder)
// {
// 	if (!IsRelativePath()) return sCEmptyString;
// 
// 	TCHAR	szAbsolutePath[MAX_PATH];
// 	CString sFullPath(szBaseFolder);
// 
// 	if (sFullPath.IsEmpty()) return sCEmptyString;
// 
// 	sFullPath = CFilePath_i::AddBackSlash(sFullPath);
// 	sFullPath += GetPath();
// 
// 	if (!PathCanonicalize(szAbsolutePath, sFullPath)) return sCEmptyString;
// 
// 	return szAbsolutePath;
// }
// 
// CString CFilePath_i::GetRelativePath(LPCTSTR szBaseFolder)
// {
// 	if (IsRelativePath()) return sCEmptyString;
// 
// 	TCHAR	szRelativePath[MAX_PATH];
// 	CString	sRelPath;
// 
// 	PathRelativePathTo(szRelativePath, szBaseFolder, FILE_ATTRIBUTE_DIRECTORY, 
// 					GetPath(), IsFilePath() ? 0 : FILE_ATTRIBUTE_DIRECTORY);
// 
// 	sRelPath = szRelativePath;
// 	if (sRelPath.GetLength() > 1)
// 	{
// 		// Remove ".\" from the beginning
// 		if ((sRelPath[0] == _T('.')) && (sRelPath[1] == _T('\\')))
// 			sRelPath.Right(sRelPath.GetLength() - 2);
// 	}
// 
// 	return sRelPath;
// }

// CString CLocationPath::GetDrive()
// {
// 	return _sDrive;
// }

// CString	CFilePath_i::GetDriveLabel(BOOL bPCNameIfNetwork)
// {
// 	if (_sDriveLabel.IsEmpty() && !IsRelativePath())
// 	{
// 		if ((bPCNameIfNetwork) && (!IsLocalPath()))
// 			_sDriveLabel = GetDir(0);
// 		else
// 		{
// 			CString sRoot;
// 			TCHAR	szVolumeName[256];
// 
// 			szVolumeName[0] = _T('\0');
// 			if (IsLocalPath())
// 			{
// 				sRoot = _sDrive + CString(_T("\\"));
// 			}
// 			else if (GetDirCount() > 1)
// 			{
// 				sRoot.Format(_T("\\\\%s\\%s\\"), GetDir(0), GetDir(1));
// 			}
// 
// 			GetVolumeInformation(sRoot, szVolumeName, 255, NULL, NULL, NULL, NULL, 0);
// 
// 			_sDriveLabel = szVolumeName;
// 		}
// 	}
// 
// 	return _sDriveLabel;
// }
// 

// 
// CFilePath_i::operator LPCTSTR ()
// {
// 	_sLPCTSTRPath = GetPath();
// 	return _sLPCTSTRPath;
// }
// 
// const CFilePath_i& CFilePath_i::operator = (LPCTSTR szPath)
// {
// 	SetPath(szPath);
// 	return *this;
// }


//  /** 
//  * 
//  * Searches for a file in PATH environment variable and other paths as specified 
//  * in SearchPath documentation.
//  * 
//  * @param       lpctszFileName_i - File name to search for.
//  * @param       csFilePath_o     - On return will hold search path.
//  * @return      bool             - Returns execution status.
//  * @exception   Nil
//  * @see         Nil
//  * @since       1.0
// */
// bool FindFile( LPCTSTR lpctszFileName_i, CString& csFilePath_o )
// {
//   const int nAllocLength = MAX_PATH * 2;
// 		TCHAR szPathBuffer[ nAllocLength ] = { 0 };
//     
//     // Search for file
//     if( SearchPath( 0, lpctszFileName_i, 0, nAllocLength, szPathBuffer, 0 ))
//     {
//       csFilePath_o = szPathBuffer;
//       return true;
//     }
//     
//     return false;
// }
// 
// // Retrive the file path from the full file path, 
// // eg. "C:\1\1.txt" -> "c:\1"
// static CString ExtractFilePath(CString FileName) 
// { 
// 	CString Path; 
// 	int i=-1; 
// 	i = FileName.ReverseFind( _T('\\') ); 
// 	if (i==-1) 
// 	  i = FileName.ReverseFind( _T(':') ); 
// 	if (i>=0) 
//   	  Path = FileName.Left(i); 
// 	else 
// 	  Path = FileName; 
//   
// 	return Path; 
// } 
// 

// 
// CString CFilePath_i::GetAppPath(VOID)
// {
//   TCHAR filepath[MAX_PATH];
//   
//   filepath[0] = _T('\0');
//   //who are we really?  Get the Exe Path
//   GetModuleFileName(AfxGetApp()->m_hInstance,filepath, MAX_PATH-1); 
//   m_strAppName = ShellGetLast_2(CString(filepath));
//   return CString(ExtractFilePath(filepath));
// }

// 
// BOOL CFilePath_i::FindDllFiles(CString strSubFolder,
//                              BOOL bFileName)
// {
//   CFileFind finder;
// 
//   strSubFolder.TrimRight(_T("\\"));
//   strSubFolder.TrimRight(_T("/"));
//   
//   CString strWildCard = m_strAppPath+_T("\\")+strSubFolder+_T("\\*.dll"); //look for the plugin files
//   
//   //call this to set up the finder to iterate through all the plugins
//   BOOL bWorking = finder.FindFile(strWildCard); 
//   while (bWorking)
//   {
//     //have to call FindNextFile() before GetFileName()
//     //because FindFile just sets the object up an returns
//     //true if _ANY_ files were found
//     bWorking = finder.FindNextFile(); 
//     if (finder.IsDots() || finder.IsDirectory())
//       continue;
//     
//     m_FileArray.strFilePathArray.Add(finder.GetFilePath()); // C:\1\2.dll 
//     if(bFileName)
//       m_FileArray.strFileNameArray.Add(finder.GetFileName());
//   }
//   
//   return TRUE;
// }


#if 0 

///////////////////////////////////////////////////////////////////////////////
// Include files 
//

#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#include <crtdbg.h>


///////////////////////////////////////////////////////////////////////////////
// Directives 
//

#pragma comment ( lib, "dbghelp.lib" )


///////////////////////////////////////////////////////////////////////////////
// Function declarations 
//

void CreateMiniDump( EXCEPTION_POINTERS* pep ); 

BOOL CALLBACK MyMiniDumpCallback(
	PVOID                            pParam, 
	const PMINIDUMP_CALLBACK_INPUT   pInput, 
	PMINIDUMP_CALLBACK_OUTPUT        pOutput 
); 


///////////////////////////////////////////////////////////////////////////////
// Test data and code 
//

struct A 
{
	int a; 

	A() 
		: a( 0 ) {}

	void Print() 
	{
		_tprintf( _T("a: %d\n"), a ); 
	}
};

struct B 
{
	A* pA; 

	B() 
		: pA( 0 ) {}

	void Print() 
	{
		_tprintf( _T("pA: %x\n"), pA ); 
		pA->Print(); 
	}

};

void DoWork() 
{
	B* pB = new B(); // but forget to initialize B::pA 

	pB->Print(); // here it should crash 
}


///////////////////////////////////////////////////////////////////////////////
// main() function 
//

int main( int argc, char* argv[] ) 
{
	__try 
	{
		DoWork(); 
	}
	__except( CreateMiniDump( GetExceptionInformation() ), EXCEPTION_EXECUTE_HANDLER ) 
	{
	}

	return 0; 
}


///////////////////////////////////////////////////////////////////////////////
// Minidump creation function 
//

void CreateMiniDump( EXCEPTION_POINTERS* pep ) 
{
	// Open the file 

	HANDLE hFile = CreateFile( _T("MiniDump.dmp"), GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

	if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
	{
		// Create the minidump 

		MINIDUMP_EXCEPTION_INFORMATION mdei; 

		mdei.ThreadId           = GetCurrentThreadId(); 
		mdei.ExceptionPointers  = pep; 
		mdei.ClientPointers     = FALSE; 

		MINIDUMP_CALLBACK_INFORMATION mci; 

		mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback; 
		mci.CallbackParam       = 0; 

		MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory); 

		BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
			hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci ); 

		if( !rv ) 
			_tprintf( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() ); 
		else 
			_tprintf( _T("Minidump created.\n") ); 

		// Close the file 

		CloseHandle( hFile ); 

	}
	else 
	{
		_tprintf( _T("CreateFile failed. Error: %u \n"), GetLastError() ); 
	}

}


///////////////////////////////////////////////////////////////////////////////
// Custom minidump callback 
//

BOOL CALLBACK MyMiniDumpCallback(
	PVOID                            pParam, 
	const PMINIDUMP_CALLBACK_INPUT   pInput, 
	PMINIDUMP_CALLBACK_OUTPUT        pOutput 
) 
{
	BOOL bRet = FALSE; 


	// Check parameters 

	if( pInput == 0 ) 
		return FALSE; 

	if( pOutput == 0 ) 
		return FALSE; 


	// Process the callbacks 

	switch( pInput->CallbackType ) 
	{
		case IncludeModuleCallback: 
		{
			// Include the module into the dump 
			bRet = TRUE; 
		}
		break; 

		case IncludeThreadCallback: 
		{
			// Include the thread into the dump 
			bRet = TRUE; 
		}
		break; 
    
		case ModuleCallback: 
		{
			// Does the module have ModuleReferencedByMemory flag set ? 

			if( !(pOutput->ModuleWriteFlags & ModuleReferencedByMemory) ) 
			{
				// No, it does not - exclude it 

				wprintf( L"Excluding module: %s \n", pInput->Module.FullPath ); 

				pOutput->ModuleWriteFlags &= (~ModuleWriteModule); 
			}

			bRet = TRUE; 
		}
		break; 

		case ThreadCallback: 
		{
			// Include all thread information into the minidump 
			bRet = TRUE;  
		}
		break; 

		case ThreadExCallback: 
		{
			// Include this information 
			bRet = TRUE;  
		}
		break; 

		case MemoryCallback: 
		{
			// We do not include any information here -> return FALSE 
			bRet = FALSE; 
		}
		break; 

		case CancelCallback: 
			break; 
	}

	return bRet; 

}

#endif 


