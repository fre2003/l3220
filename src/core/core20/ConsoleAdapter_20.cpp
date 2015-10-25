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
/******************************************************************************
Module:  ConsoleAdapter_i.cpp
Author:  Sudheesh.P.S
******************************************************************************/

#include "StdAfx.h" 
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <iostream>
 
/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConsoleAdapter_i::ConsoleAdapter_i( bool bAutoDestroy ): 
                                m_pFileCRTIn(0),
                                m_nCRTIn(0), 
                                m_pFileCRTOut(0),
                                m_nCRTOut(0),
                                m_eConsoleType( BOTH ),
                                m_bDestroyed( false ),
                                m_bAutoDestroy( bAutoDestroy ),
                                m_hSpawnConsole( 0 )
{
    
}

ConsoleAdapter_i::~ConsoleAdapter_i()
{
    if( m_bAutoDestroy || !m_bDestroyed)
    {
       DestroyConsole();
    }
}

/******************************************************************************
Function: CreateConsole
Author  : Sudheesh.P.S
Param   : eConsoleType - Type of console(input/output or both)
Desc    : Allocates a new console and replaces the standard input/output streams
******************************************************************************/

bool ConsoleAdapter_i::CreateConsole( CONSOLETYPE_E eConsoleType )
{
    try
    {
        m_eConsoleType = eConsoleType;
        AllocConsole();
        return ReplaceHandles();
    }
    catch ( ... )
    {
        return false;
    }        
}

/******************************************************************************
Function: SpawnDumpConsole
Author  : Sudheesh.P.S
Param   : lpctszDumConsoleApp - Path of the dump console to be spawned
Param   : eConsoleType - Type of console(input/output or both)
Desc    : Spawns a separate dump console and replaces the standard input/output streams
******************************************************************************/

bool ConsoleAdapter_i::SpawnConsole( LPCTSTR lpctszDumConsoleApp,CONSOLETYPE_E eConsoleType  )
{
    try
    {
        m_eConsoleType = eConsoleType;
        STARTUPINFO stStartUpInfo = {0};
        PROCESS_INFORMATION stProcInfo = {0};
        if(!CreateProcess( lpctszDumConsoleApp,0,0,0,TRUE,CREATE_NEW_CONSOLE,0,0,&stStartUpInfo, &stProcInfo ))               
        {
            DestroyConsole();
            return false;
        }
        m_hSpawnConsole = stProcInfo.hProcess;
        
        // Waiting for the child process to be initialized
        Sleep( 100 );
        
        if(!AttachConsole( stProcInfo.dwProcessId ))
        {
            return false;
        }
        ReplaceHandles();
    }
    catch ( ... )
    {
        return false;
    }
    return true;
}


bool ConsoleAdapter_i::WaitConsoleInput(OUT TCHAR* szInputStr, DWORD dwSize)
{
  if(m_pFileCRTIn)
  {
    if(_fgetts(szInputStr, dwSize, m_pFileCRTIn))
    {
      return true;
    }
  }
  
  return false;  
}


bool ConsoleAdapter_i::WriteConsole(LPCTSTR szInputStr)
{
  if(m_pFileCRTOut)
  {
    std::cout<<static_cast<LPCTSTR>( szInputStr );
    //int ret = _fputts(szInputStr, m_pFileCRTOut);
    return true;
  }
  
  return false;
}


/******************************************************************************
Function: ReplaceHandles
Author  : Sudheesh.P.S
Desc    : Replaces the standard input/output streams
******************************************************************************/

bool ConsoleAdapter_i::ReplaceHandles()
{
    try
    {
        if( ( INPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
        {
            m_nCRTIn= _open_osfhandle( (long) GetStdHandle(STD_INPUT_HANDLE), _O_TEXT );
            if( -1 == m_nCRTIn )
            {
                return false;
            }
            m_pFileCRTIn = _fdopen( m_nCRTIn, "r" );
            if( !m_pFileCRTIn )
            {
                return false;
            }
            m_fOldStdIn = *stdin;
            *stdin = *m_pFileCRTIn;
            // if clear is not done, any cout statement before AllocConsole
            // will cause, the cin after AllocConsole to fail, so very important
            std::cin.clear();
        }
        if( ( OUTPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
        {
            m_nCRTOut= _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT );
            if( -1 == m_nCRTOut )
            {
                return false;
            }
            
            m_pFileCRTOut = _fdopen( m_nCRTOut, "w" );
            if( !m_pFileCRTOut )
            {
                return false;
            }
            
            m_fOldStdOut = *stdout;
            *stdout = *m_pFileCRTOut;
            
            // if clear is not done, any cout statement before AllocConsole
            // will cause, the cout after AllocConsole to fail, so very important
            std::cout.clear();
        }
    }
    catch ( ... )
    {
        return false;
    }        
    return true;
}

/******************************************************************************
Function: AttachConsole
Author  : Sudheesh.P.S
Desc    : Attachs the current process to the console with the given process ID
Note    : The latest Platform SDK has this functions as inbuilt
******************************************************************************/

bool ConsoleAdapter_i::AttachConsole( DWORD dwProcessId )
{
    HMODULE hKernel32 = LoadLibrary( "kernel32.dll" );
    if( !hKernel32 )
    {
        return false;
    }
    typedef  BOOL ( WINAPI *PATTACHCONSOLE)(DWORD);
    PATTACHCONSOLE pAttachConsole = (PATTACHCONSOLE) GetProcAddress( hKernel32,"AttachConsole" );
    if( !pAttachConsole )
    {
        return false;
    }
    BOOL bRet = pAttachConsole( dwProcessId );
    return ( bRet!=FALSE );
}

/******************************************************************************
Function: DestroyConsole
Author  : Sudheesh.P.S
Desc    : Destroys the attached console and restores the input/output handles
******************************************************************************/

bool ConsoleAdapter_i::DestroyConsole()
{    
    try
    {
        FreeConsole(); 
        if( 0 != m_hSpawnConsole )
        {
            // Kill the DumpConsole when not needed
            TerminateProcess( m_hSpawnConsole, 0 );
            CloseHandle( m_hSpawnConsole );
            m_hSpawnConsole = 0;
        }
        if( ( INPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
        {
            if( 0 != m_pFileCRTIn )
            {
                fclose( m_pFileCRTIn );
            }
            _close( m_nCRTIn );
            *stdin = m_fOldStdIn;
        }
    
        if( ( OUTPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
        {
            if( 0 != m_pFileCRTOut )
            {
                fclose( m_pFileCRTOut );
            }
            _close( m_nCRTOut );
            *stdout = m_fOldStdOut;
        }
        m_bDestroyed = true;
        return true;
    }
    catch ( ... )
    {

    }

    return false;
}



