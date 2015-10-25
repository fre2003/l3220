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


// change '/' to '\\' and remove all trailing  slash and blanks.
TCHAR* PathFormatEx(CONST TCHAR *szPath, TCHAR* out)
{
	CONST TCHAR *str;
  int i = 0;

  str = szPath;
  
  while(*str == _T(' ')) str++; // Skip blank.

  for (; *str; str++)
  {
		if(*str == _T('/'))
    {
      *out = _T('\\');
    }
    else 
      *out = *str;
    out ++;
    i ++;
  }
  *out = *str;

  if(i)
  {
    i = 0;
    while(out[--i] == _T(' ') || out[i] == _T('\\')) 
      out[i] = _T('\0'); // SKip '\\';
  }
  
  return out;
}


// Create the Dir, support nesting path.
// if bRemoveFileSpec = TRUE, Removes the trailing file name and backslash from a path.
U32 CFileOps_i::MakeDir(LPCTSTR pszDir, bool bRemoveFileSpec)
{
  TCHAR  *endptr = NULL, *ptr = NULL, *slash = NULL, *pStrDir = NULL;
  TCHAR szDir[MAX_PATH];
  
  szDir[0] = _T('\0');
  
  int len = strlen(pszDir);
  if(len >= MAX_PATH)
  {
    StrNCpy(szDir, pszDir, MAX_PATH);
    PathFormatEx(szDir, szDir);
  }
  else 
  {
    PathFormatEx(pszDir, szDir); // PathRemoveBackslash 
  }
  
  if(bRemoveFileSpec)
    PathRemoveFileSpec(szDir);
  
  pStrDir = szDir;
  
  // Try to make the directory.
  if (CreateDirectory(pStrDir, NULL) == TRUE)
  {
    return (DWORD)0;
  }
  else if(GetLastError() == ERROR_FILE_EXISTS)
  {
    return ERROR_FILE_EXISTS;
  }
  else
  {
    DWORD dwErr = GetLastError() ;
    UNREF_PARAM(dwErr);
  }
  
  endptr = CStringUtl::strrchr(pStrDir, _T('\0'));
  slash = CStringUtl::strrchr(pStrDir, _T('\\'));
  
  // Search upward for the non-existing parent.
  while (slash != NULL) 
  {
    ptr = slash;
    *ptr = _T('\0');
    
    // If reached an existing parent, break.		
    if (PathFileExists(pStrDir) == TRUE)
      break;
    
    // If non-existing parent
    else 
    {
      slash = CStringUtl::strrchr(pStrDir, _T('\\'));
      
      // If under / or current directory, make it.			
      if (slash == NULL || slash == pStrDir) 
      {
        if ((CreateDirectory(pStrDir, NULL) != TRUE) && (GetLastError() != ERROR_FILE_EXISTS))
        {
          return (DWORD)(ERR_WIN32_ERROR_I);
        }
      }
    }
  }
  
  // Create directories starting from upmost non-existing parent.
  while (((ptr = CStringUtl::strchr(pStrDir, _T('\0'))) != endptr) && ptr) 
  {
    *ptr = _T('\\');
		if ((CreateDirectory(pStrDir, NULL) != TRUE) && (GetLastError() != ERROR_FILE_EXISTS))
    {
			return (DWORD)(ERR_WIN32_ERROR_I);
    }
  }
  
  return (0);
}


#if 0 
//////////////////////////////////////////////////////////////////////////
// Given a pathname, simplify that path by removing, duplicate contiguous slashes.
//////////////////////////////////////////////////////////////////////////
static TCHAR* simplify(const TCHAR *str)
{
	int i;
	size_t mbPathlen;	/* length of multi-byte path */
	size_t wcPathlen;	/* length of wide-character path */
	wchar_t *wptr;		/* scratch pointer */
	wchar_t *wcPath;	/* wide-character version of the path */
	TCHAR *mbPath;		/* The copy fo the path to be returned */
	
						/*
						*  bail out if there is nothing there.
	*/
	
	if (!str)
		return (NULL);
	
  /*
	*  Get a copy of the argument.
	*/	
	if ((mbPath = strdup(str)) == NULL) {
		return (NULL);
	}
	
	/*
	*  convert the multi-byte version of the path to a
	*  wide-character rendering, for doing our figuring.
	*/
	
	mbPathlen = lstrlen(mbPath);
	
	if ((wcPath = (wchar_t* )calloc(sizeof (wchar_t), mbPathlen+1)) == NULL) {
		free(mbPath);
		return (NULL);
	}
	
	if ((wcPathlen = mbstowcs(wcPath, mbPath, mbPathlen)) == (size_t)-1) {
		free(mbPath);
		free(wcPath);
		return (NULL);
	}
	
	/*
	*  remove duplicate slashes first ("//../" -> "/")
	*/	
  for (wptr = wcPath, i = 0; i < (int)wcPathlen; i++) 
  {
    *wptr++ = wcPath[i];
    
    if (wcPath[i] == '/') 
    {
      i++;
      
      while (wcPath[i] == '/') 
      {
        i++;
      }
      
      i--;
    }
  }
	
	*wptr = '\0';
	
	/*
	*  now convert back to the multi-byte format.
	*/
	
	if (wcstombs(mbPath, wcPath, mbPathlen) == (size_t)-1) {
		free(mbPath);
		free(wcPath);
		return (NULL);
	}
	
	free(wcPath);
	return (mbPath);
}
#endif 
