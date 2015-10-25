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
#include "common_20.h"
#define WINTRUST_H 
#include "ImageHlp.h"

// When exception, this function should be call.
U32 ExceptionExit_i(VOID)
{
  U32 ret = 0;

  ExceptionExit();  

  return ret;
}


////////////////////////////////////////////////////////////////////////////////
// CreateMiniDump() function
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
    
		MINIDUMP_TYPE mdt       = MiniDumpNormal; 

		BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
			hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0 ); 

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

LONG __stdcall MyCustomFilter( EXCEPTION_POINTERS* pep ) 
{
	CreateMiniDump( pep ); 
  
	return EXCEPTION_EXECUTE_HANDLER; 
}

U32 SetCrashCollection()
{
  SetUnhandledExceptionFilter( MyCustomFilter );

  return 0;
}
