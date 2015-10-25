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
#include <stdio.h>
#include <stdlib.h>

#define StrEndA(pStart) (pStart + lstrlenA(pStart))
#define StrEnd(pStart) (pStart + lstrlen(pStart))
#define COUNTOF(ar) (sizeof(ar)/sizeof(ar[0]))

#if 0 
// Returns size in formatted manner
bool GetFormattedSize( LPTSTR pszFormattedSize,  CONST LONGLONG dwSize)
{
  const int BUFF_SIZE = 30;
  return (StrFormatByteSize( dwSize, pszFormattedSize, BUFF_SIZE ) != NULL);
}

bool CFormatString_i::GetFormattedSize(LPTSTR pszFormattedSize, CONST LONGLONG dwSize)
{
  return GetFormattedSize(pszFormattedSize, dwSize);
}
#endif 

//////////////////////////////////////////////////////////////////////////
// Format the input string to local format.
// like "13456" --> "13,456"
//////////////////////////////////////////////////////////////////////////
INT CFormatString_i::FormatNumberStrA(CHAR* lpNumberStr)
{
  char *c;
  char szSep[8];
  int  i = 0;

  if (!lstrlenA(lpNumberStr))
    return(0);

  if (!GetLocaleInfoA(LOCALE_USER_DEFAULT,
                     LOCALE_STHOUSAND,
                     szSep,
                     COUNTOF(szSep)))
    szSep[0] = '\'';

  c = StrEndA(lpNumberStr);

  while ((c = CharPrevA(lpNumberStr,c)) != lpNumberStr)
  {
    if (++i == 3)
    {
      i = 0;
      MoveMemory(c+1,c,lstrlenA(c)+1);
      *c = szSep[0];
    }
  }
  
  return (INT)(lstrlenA(lpNumberStr));
}

INT CFormatString_i::FormatNumberStr(LPTSTR lpNumberStr)
{
  TCHAR *c;
  TCHAR szSep[8];
  int  i = 0;

  if (!lstrlen(lpNumberStr))
    return(0);

  if (!GetLocaleInfo(LOCALE_USER_DEFAULT,
                     LOCALE_STHOUSAND,
                     szSep,
                     COUNTOF(szSep)))
    szSep[0] = _T('\'');

  c = StrEnd(lpNumberStr);

  while ((c = CharPrev(lpNumberStr,c)) != lpNumberStr)
  {
    if (++i == 3)
    {
      i = 0;
      MoveMemory(c+1,c,lstrlen(c)+1);
      *c = szSep[0];
    }
  }
  
  return (INT)(lstrlen(lpNumberStr));
}

U32 CFormatString_i::GetNumberAndSize(LPCTSTR pszString, U32* pNumber, U32* pSize) 
{
  TCHAR* p = NULL; 
	TCHAR *pe;
  TCHAR buf[250];

  ZeroMemory(buf, sizeof(buf));
  
  if(!pszString)
    return 1;
  
  lstrcpy(buf, pszString);
  
	p = buf;

  while(p[0] == _T(' ')) p++;
  
  if(pNumber)
  {
    *pNumber = _tcstoul(p,&pe,0);  // StrToLong
    while(pe[0] == _T(' ')) pe++;
    if(pe[0] == _T('x') || pe[0] == _T('X'))
      pe++;  
    while(pe[0] == _T(' ')) pe++;
  
    p = pe;
    if(p == NULL)
      return 1; 
  }
  
  *pSize = _tcstoul(p,&pe,0);
  while(pe[0] == _T(' ')) pe++;

	if (pe[0]==_T('K') || pe[0]==_T('k') )
	{
		*pSize = *pSize * 1024;
	}
	if (pe[0]==_T('M') || pe[0]==_T('m'))
	{
		*pSize = *pSize * 1024 * 1024;
	}
	if (pe[0]==_T('G') || pe[0]==_T('g'))
	{
		*pSize = *pSize * 1024 * 1024 * 1024;
	}
  
  return 0;
}


TCHAR* CFormatString_i::GetStringSize(LPCSTR pszString, u32* pSize)
{
  TCHAR *p = NULL, *pe = NULL;
  int l;
  p = (TCHAR*)pszString;
  
  if(!pSize  || !p)
    return NULL;
  
  *pSize = _tcstoul(p,&pe,0);
  l = pe-p;
  
  if (pe[0]==_T('K') || pe[0]==_T('k'))
  {
    *pSize = *pSize * 1024;
    l++;
  }
  else if (pe[0]==_T('M') || pe[0]==_T('m'))
  {
  		*pSize = *pSize * 1024 * 1024;
      l++;
  }
  else if (pe[0]==_T('G') || pe[0]==_T('g'))
  {
  		*pSize = *pSize * 1024 * 1024 * 1024;
      l++;
  }  
  else if (pe[0]==_T('T') || pe[0]==_T('t'))
  {
  		*pSize = *pSize * 1024 * 1024 * 1024*1024;
      l++;
  }

  return p;
}


//////////////////////////////////////////////////////////////////////////
// Format Byts. -->Bytes, KB, MB, GB 
//////////////////////////////////////////////////////////////////////////
U32 CFormatString_i::FormatBytesA(CHAR* pszOutput, INT nOutputSize, S64 ulBytes, bool bLocal)
{
  char tch[128];
  int i = 0;
  long double dBytes = (long double)ulBytes;
  static const char *pBytes[] = { ("Bytes"), ("KB"), ("MB"), ("GB"), ("TB") };

  if (ulBytes > 1023)
  {
    for (i = 0; i < 5; i++)
    {
      if (dBytes >= 1024.0)
        dBytes /= 1024.0;
      else
        break;
    }
    wsprintfA(tch, ("%.2f"), dBytes);
    if(bLocal)
      GetNumberFormatA(LOCALE_USER_DEFAULT,0,tch,NULL, pszOutput, nOutputSize);
    else
      lstrcpyA(pszOutput, tch);
    lstrcatA(pszOutput, (" "));
    lstrcatA(pszOutput,pBytes[i]);
  }
  else
  {
    wsprintfA(pszOutput, ("%i"), ulBytes); // wnsprintf and wvnsprintf do not support floating point or pointer types.
    if(bLocal)
      FormatNumberStrA(pszOutput);
    lstrcatA(pszOutput, (" "));
    lstrcatA(pszOutput, pBytes[0]);
  }
  
  return 0;
}


// Converts a numeric value into a string that represents the number 
// expressed as a size value in bytes, kilobytes, megabytes, or gigabytes, depending on the size.
//
//        532 -> 532 bytes
//       1340 -> 1.3KB
//      23506 -> 23.5KB
//    2400016 -> 2.4MB
// 2400000000 -> 2.4GB
U32 CFormatString_i::FormatBytes(TCHAR* pszOutput, INT nOutputSize, S64 ulBytes, bool bLocal)
{
  static const TCHAR *pBytes[] = { _T("Bytes"), _T("KB"), _T("MB"), _T("GB"), _T("TB") };
  TCHAR tch[128];
  int i = 0;
  long double dBytes = (long double)ulBytes;
  
  tch[0] = _T('\0');
  if (ulBytes > 1023)
  {
    for (i = 0; i < 5; i++)
    {
      if (dBytes >= 1024.0)
        dBytes /= 1024.0;
      else
        break;
    }

    if(i == 2) // MB 
    {
      _stprintf(tch, _T("%.0f"), dBytes);
    }
    else if(i==3) // GB
    {
      _stprintf(tch, _T("%.1f"), dBytes);
    }
    else // TB 
    {
       _stprintf(tch, _T("%.2f"), dBytes);
    }
    
    // function formats a number string as a number string customized for a specified locale.
    if(bLocal)
      GetNumberFormat(LOCALE_USER_DEFAULT,0, tch, NULL, pszOutput, nOutputSize);
    else
      lstrcpy(pszOutput, tch);
    lstrcat(pszOutput, _T(" "));
    lstrcat(pszOutput, pBytes[i]);
  }
  else
  {
    _stprintf(pszOutput, _T("%i"), ulBytes);
    if(bLocal)
      FormatNumberStr(pszOutput);
    lstrcat(pszOutput, _T(" "));
    lstrcat(pszOutput, pBytes[0]);
  }
  
  return 0;
}


//        532 -> 532
//       1340 -> 1.3K
//      23506 -> 23.5K
//    2400016 -> 2.4M
// 2400000000 -> 2.4G
U32 CFormatString_i::FormatString(TCHAR* pszOutput, INT nOutputSize, S64 ulBytes, bool bLocal)
{
  static const TCHAR *pBytes[] = { _T(""), _T("K"), _T("M"), _T("G"), _T("T") };
  TCHAR tch[128];
  int i = 0;
  long double dBytes = (long double)ulBytes;
  
  tch[0] = _T('\0');
  if (ulBytes > 1023)
  {
    for (i = 0; i < 5; i++)
    {
      if (dBytes >= 1024.0)
        dBytes /= 1024.0;
      else
        break;
    }

    if(i == 2) // MB 
    {
      _stprintf(tch, _T("%.0f"), dBytes);
    }
    else if(i==3) // GB
    {
      _stprintf(tch, _T("%.1f"), dBytes);
    }
    else // TB 
    {
       _stprintf(tch, _T("%.2f"), dBytes);
    }
    
    // function formats a number string as a number string customized for a specified locale.
    if(bLocal)
      GetNumberFormat(LOCALE_USER_DEFAULT,0, tch, NULL, pszOutput, nOutputSize);
    else
      lstrcpy(pszOutput, tch);
    lstrcat(pszOutput, _T(" "));
    lstrcat(pszOutput, pBytes[i]);
  }
  else
  {
    _stprintf(pszOutput, _T("%i"), ulBytes);
    if(bLocal)
      FormatNumberStr(pszOutput);
    lstrcat(pszOutput, _T(" "));
    lstrcat(pszOutput, pBytes[0]);
  }
  
  return 0;
}
