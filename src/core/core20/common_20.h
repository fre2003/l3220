#if !defined(__COMMON_H__)
#define __COMMON_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT   0x500    //Win2000 
#define _WIN32_WINDOWS 0x0500

//#include <windows.h> 
#include <afx.h>
#include <tchar.h>
#include <stdio.h>
#include <core_i.h>
#include "../inc/shlwapi.h"

#undef ASSERT 

#define StrEnd(pStart) (pStart + lstrlen(pStart))

#define ASSERT(expr) \
        do { if (!(expr))  \
              { \
              OutputDebugString(_T("*** Assertion Failed *** \n"));\
              TCHAR buf[512];\
              wsprintf(buf, _T("%s(%d) : Assertion Failed '%s'. \0\n"), __FILE__, __LINE__, #expr);\
              OutputDebugString(buf);\
              DebugBreak(); \
              } \
        } while (0)


VOID OutputDbgStr(LPCTSTR Format, ...);
DWORD SaveToNewFile(LPCTSTR pszNewFile, PVOID pData, DWORD dwSize);
BOOL FilePrint(LPCTSTR Format, ...);

#undef CORE_TRACE 
#ifdef _DEBUG
#define CORE_TRACE OutputDbgStr
#else
#define CORE_TRACE  
#endif 

#define MALLOC(x)  AllocateMemory_i(x)

#ifdef _DEBUG
#define FREE(p)     FreeMemory_i(p, TRUE)
#else
#define FREE(p)     FreeMemory_i(p, FALSE) 
#endif 

VOID MemInit(VOID);
U32  ExceptionExit();
U32 MemoryLeakCheck(U32 nModuleId);

void Core20Init(HINSTANCE hInstance);
void Core20Exit();


typedef unsigned (__stdcall *PTHREAD_START) (void*);

#define BEGINTHREADEX(lpsa, cbStack, lpStartAddr, lpvThreadParm, fdwCreate, lpIDThread) \
		((HANDLE)_beginthreadex(                     \
				(void*)(lpsa),                           \
				(unsigned)(cbStack),                     \
				(PTHREAD_START)(lpStartAddr),            \
				(void*)(lpvThreadParm),                  \
				(unsigned)(fdwCreate),                   \
				(unsigned*)(lpIDThread)))

#endif // __COMMON_H__
