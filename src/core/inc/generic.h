#ifndef __GENERIC_H__
#define __GENERIC_H__

// or use _tcscpy in tchar.h
#ifdef UNICODE
#define STRCPY wcscpy 
#define STRLEN wcslen
#define SPLITPATH  _wsplitpath 
#else
#define STRCPY strcpy 
#define STRLEN strlen
#define SPLITPATH  _splitpath 
#endif 

#endif // __GENERIC_H__
