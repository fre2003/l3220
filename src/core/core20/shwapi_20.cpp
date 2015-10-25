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

#if 0 
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <windows.h>

void    path_add_backslash(char *path)
{
  if (path[strlen(path) - 1] != '\\')
    strcat(path, "\\");
}

BOOL    path_is_relative(char *path)
{
  char    drive[_MAX_DRIVE];
  char    dir[_MAX_DIR];
  char    fname[_MAX_FNAME];
  char    ext[_MAX_EXT];
  
  _splitpath(path, drive, dir, fname, ext);
  if (!*drive) {
    char    buffer[MAX_PATH];
    char   *position;
    
    strcpy(buffer, dir);
    if (buffer[0] == '\\' && buffer[1] == '\\') {
      position =
        strchr(strchr
        (strchr(strchr(buffer, '\\') + 1, '\\') +
        1, '\\') + 1, '\\') + 1;
      *position = '\0';
      if (GetDriveType(buffer) == DRIVE_REMOTE)
        return FALSE;
      return TRUE;
    } else
      return TRUE;
  }
  return FALSE;
}

BOOL    path_remove_filespec(LPTSTR path)
{
  char    drive[_MAX_DRIVE];
  char    dir[_MAX_DIR];
  char    fname[_MAX_FNAME];
  char    ext[_MAX_EXT];
  
  _splitpath(path, drive, dir, fname, ext);
  _makepath(path, drive, dir, NULL, NULL);
  return TRUE;
}

void    path_unquote(LPTSTR path)
{
  char    pathbuffer[MAX_PATH];
  int     lengte;
  
  if (path[0] == '\"') {
    strcpy(pathbuffer, path + 1);
    lengte = strlen(pathbuffer);
    pathbuffer[lengte - 1] = '\0';
    strcpy(path, pathbuffer);
  }
}


// GetFileExtA -- ANSI version
// 
// This function is thought to be used only for filenames which have been
// validated by 'GetFullPathName' or similar functions.
LPCSTR GetFileExtA(LPCSTR pszPathA, int iLen /*= -1*/)
{
	// Just search the last '.'-character which comes after an optionally 
	// available last '\'-char.
	int iPos = iLen >= 0 ? iLen : strlen(pszPathA);
	while (iPos-- > 0)
	{
		if (pszPathA[iPos] == '.')
			return &pszPathA[iPos];
		if (pszPathA[iPos] == '\\')
			break;
	}

	return NULL;
}

// GetFileExtW -- Unicode version
//
// This function is thought to be used only for filenames which have been
// validated by 'GetFullPathName' or similar functions.
LPCWSTR GetFileExtW(LPCWSTR pszPathW, int iLen /*= -1*/)
{
	// Just search the last '.'-character which comes after an optionally 
	// available last '\'-char.
	int iPos = iLen >= 0 ? iLen : wcslen(pszPathW);
	while (iPos-- > 0)
	{
		if (pszPathW[iPos] == L'.')
			return &pszPathW[iPos];
		if (pszPathW[iPos] == L'\\')
			break;
	}

	return NULL;
}

#endif 
