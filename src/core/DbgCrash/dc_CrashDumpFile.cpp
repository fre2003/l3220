///////////////////////////////////////////////////////////////////////////////
//
// MaxiDump.cpp 
//
// Sample approach to collecting data with MiniDumpWriteDump  
// 
// Author: Oleg Starodumov (www.debuginfo.com)
//
//


///////////////////////////////////////////////////////////////////////////////
// Include files 
//
#include "StdAfx.h"
#include "dc.h"

///////////////////////////////////////////////////////////////////////////////
// main() function 
//

#if 0 
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
#endif 


///////////////////////////////////////////////////////////////////////////////
// Minidump creation function 
//

void dc_CreateMaxDump( EXCEPTION_POINTERS* pep, CONST CHAR* pszCrashMiniDumpFile ) 
{
	// Open the file 

	HANDLE hFile = CreateFileA(pszCrashMiniDumpFile, GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

	if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
	{
		// Create the minidump 

		MINIDUMP_EXCEPTION_INFORMATION mdei; 

		mdei.ThreadId           = GetCurrentThreadId(); 
		mdei.ExceptionPointers  = pep; 
		mdei.ClientPointers     = FALSE; 

		MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | 
		                                          MiniDumpWithFullMemoryInfo | 
		                                          MiniDumpWithHandleData | 
		                                          MiniDumpWithThreadInfo | 
		                                          MiniDumpWithUnloadedModules ); 

		BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
			hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0 ); 

		if( !rv ) 
			dc_OutputDbgStr( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() );  

		// Close the file
		CloseHandle( hFile ); 

	}
	else 
	{
		dc_OutputDbgStr( _T("CreateFile failed. Error: %u \n"), GetLastError() ); 
	}
}

void dc_CreateMiniDump( EXCEPTION_POINTERS* pep, CONST CHAR* pszCrashMiniDumpFile ) 
{
	// Open the file 

	HANDLE hFile = CreateFileA(pszCrashMiniDumpFile, GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 
  
	if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
	{
		// Create the minidump 

		MINIDUMP_EXCEPTION_INFORMATION mdei; 

		mdei.ThreadId           = GetCurrentThreadId(); 
		mdei.ExceptionPointers  = pep; 
		mdei.ClientPointers     = FALSE; 
    
		MINIDUMP_TYPE mdt       = MiniDumpNormal; 
    
		BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
			hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0 ); 

		if( !rv ) 
			dc_OutputDbgStr( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() );

		// Close the file 
		CloseHandle( hFile ); 

	}
	else 
	{
		dc_OutputDbgStr( _T("CreateFile failed. Error: %u \n"), GetLastError() ); 
	}
}
