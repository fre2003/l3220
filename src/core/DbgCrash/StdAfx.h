//
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#if !defined(__STDAFX_H___)
#define __STDAFX_H___

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT   0x500    //Win2000 
#define _WIN32_WINDOWS 0x500  

#if _WIN32_WINNT < 0x0500 
#error "Win32 Version Unsupportted."  
#endif 

#include <windows.h> 
#include <tchar.h>

#endif // __STDAFX_H___
