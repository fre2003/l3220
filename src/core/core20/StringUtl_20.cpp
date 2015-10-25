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

#pragma warning(disable:4127)	 

TCHAR* CStringUtl::strrchr(LPCTSTR string, INT ch)
{
	TCHAR *start = (TCHAR *)string;

	while (*string++) /* find end of string */
		;
											/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch) 
		return (TCHAR *)string;

	return NULL;
}

TCHAR* CStringUtl::strchr(const TCHAR * string, INT ch)
{
  while (*string && *string != (TCHAR)ch)
    string++;
  
  if (*string == (TCHAR)ch)
    return((TCHAR *)string);

  return(NULL);
}

CHAR* CStringUtl::StrLowerCaseA(CHAR* s)
{
  while (*s)
  {
    if ((*s >= 'A') && (*s <= 'Z'))
    {
      *s = static_cast <CHAR> (*s - 'A' + 'a');
    }
    s++;
  }
  
  return s;
}

TCHAR* CStringUtl::StrLowerCase(TCHAR* s)
{
  while (*s)
  {
    if ((*s >= _T('A')) && (*s <= _T('Z')))
    {
      *s = static_cast <TCHAR> (*s - _T('A') + _T('a'));
    }
    s++;
  }
  
  return s;
}

// change '/' to '\\' 
TCHAR* CStringUtl::PathFormat(TCHAR *szPath) 
{
	TCHAR *str;

  for (str = szPath; *str; ++str)
	{
		if(*str == _T('/'))
			*str = _T('\\');
	}

  return szPath;
}

// change '/' to '\\' and remove all begining and trailing slash and blanks.
TCHAR* CStringUtl::PathFormat(CONST TCHAR *szPath, TCHAR* out)
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

// Check whether exist '/'. 
bool CStringUtl::IsWinPathFormat(CONST TCHAR *szPath) 
{
	CONST TCHAR *str;
  
  for (str = szPath; *str; ++str)
	{
		if(*str == _T('/'))
			return FALSE;
	}

  return TRUE;
}

static INLINE CHAR* strchrA(const CHAR * string, INT ch)
{
  while(*string && *string != (CHAR)ch)
    string++;
  
  if (*string == (CHAR)ch)
    return((CHAR *)string);

  return(NULL);
}

bool CStringUtl::StrTab2SpaceA(CHAR* psz) 
{
  CHAR *c;
  while (c = (CHAR*)strchrA(psz,'\t'))
    *c = ' ';
  
  return TRUE;
}

bool CStringUtl::StrTab2Space(TCHAR* psz) 
{
  TCHAR *c;
  while (c = (CHAR*)strchr(psz,_T('\t')))
    *c = _T(' ');

  return TRUE;
}

bool CStringUtl::TrimString(TCHAR* lpString)
{
  TCHAR* psz;

  if (!lpString || !*lpString)
    return FALSE;

  // Trim left
  psz = lpString;

  while (*psz == _T(' '))
    psz = CharNext(psz);

  MoveMemory(lpString,psz, lstrlen(psz) + 1);

  // Trim right
  psz = StrEnd(lpString);

  while (*(psz = CharPrev(lpString,psz)) == _T(' '))
    *psz = _T('\0');
  
  return TRUE;
}


//   TCHAR p1[10] = {0, };
//   TCHAR p2[10] = {0, };
//   ExtractFirstArgument("\"123456\" \"ABCD EF\"", p1, p2);
//
bool CStringUtl::ExtractFirstArgument(LPCSTR lpArgs,LPSTR lpArg1,LPSTR lpArg2)
{
  LPSTR psz;
  BOOL bQuoted = FALSE;
  
  lstrcpy(lpArg1,lpArgs);
  if (lpArg2)
    *lpArg2 = '\0';
  
  if (!TrimString(lpArg1))
    return FALSE;
  
  if (*lpArg1 == '\"')
  {
    *lpArg1 = ' ';
    TrimString(lpArg1);
    bQuoted = TRUE;
  }
  
  if (bQuoted)
    psz = strchr(lpArg1,'\"');
  else
    psz = strchr(lpArg1,' ');;
  
  if (psz)
  {
    *psz = '\0';
    if (lpArg2)
      lstrcpy(lpArg2,psz + 1);
  }
  
  TrimString(lpArg1);
  
  if (lpArg2)
    TrimString(lpArg2);
  
  return TRUE;  
}


#include <string>
#include <vector>

enum { SURROGATE_LEAD_FIRST = 0xD800 };
enum { SURROGATE_TRAIL_FIRST = 0xDC00 };
enum { SURROGATE_TRAIL_LAST = 0xDFFF };

static unsigned int UTF8Length(const wchar_t *uptr, size_t tlen) 
{
	size_t len = 0;
	for (size_t i = 0; i < tlen && uptr[i];) 
  {
		unsigned int uch = uptr[i];
		if (uch < 0x80)
    {
			len++;
		}
    else if (uch < 0x800) 
    {
			len += 2;
		} 
    else if ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) 
    {
			len += 4;
			i++;
		} 
    else 
    {
			len += 3;
		}
		i++;
	}
	return len;
}

static void UTF8FromUTF16(const wchar_t *uptr, size_t tlen, char *putf, size_t len) 
{
	int k = 0;
	for (size_t i = 0; i < tlen && uptr[i];) 
  {
		unsigned int uch = uptr[i];
		if (uch < 0x80) 
    {
			putf[k++] = static_cast<char>(uch);
		} 
    else if (uch < 0x800) 
    {
			putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
			putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
		}
    else if ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST))
    {
			// Half a surrogate pair
			i++;
			unsigned int xch = 0x10000 + ((uch & 0x3ff) << 10) + (uptr[i] & 0x3ff);
			putf[k++] = static_cast<char>(0xF0 | (xch >> 18));
			putf[k++] = static_cast<char>(0x80 | ((xch >> 12) & 0x3f));
			putf[k++] = static_cast<char>(0x80 | ((xch >> 6) & 0x3f));
			putf[k++] = static_cast<char>(0x80 | (xch & 0x3f));
		} 
    else 
    {
			putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
			putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
			putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
		}
		i++;
	}

	putf[len] = '\0';
}

static size_t UTF16Length(const char *s, unsigned int len) 
{
	size_t ulen = 0;
	size_t charLen;

	for (size_t i=0; i<len;) 
  {
		unsigned char ch = static_cast<unsigned char>(s[i]);
		if (ch < 0x80)
    {
			charLen = 1;
		}
    else if (ch < 0x80 + 0x40 + 0x20) 
    {
			charLen = 2;
		}
    else if (ch < 0x80 + 0x40 + 0x20 + 0x10) 
    {
			charLen = 3;
		} 
    else 
    {
			charLen = 4;
			ulen++;
		}

		i += charLen;
		ulen++;
	}
	return ulen;
}

static size_t UTF16FromUTF8(const char *s, size_t len, wchar_t *tbuf, size_t tlen) 
{
	size_t ui=0;
	const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
	size_t i=0;
	while ((i<len) && (ui<tlen)) 
  {
		unsigned char ch = us[i++];
		if (ch < 0x80) 
    {
			tbuf[ui] = ch;
		} 
    else if (ch < 0x80 + 0x40 + 0x20) 
    {
			tbuf[ui] = static_cast<wchar_t>((ch & 0x1F) << 6);
			ch = us[i++];
			tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
		}
    else if (ch < 0x80 + 0x40 + 0x20 + 0x10)
    {
			tbuf[ui] = static_cast<wchar_t>((ch & 0xF) << 12);
			ch = us[i++];
			tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + ((ch & 0x7F) << 6));
			ch = us[i++];
			tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
		}
    else {
			// Outside the BMP so need two surrogates
			int val = (ch & 0x7) << 18;
			ch = us[i++];
			val += (ch & 0x3F) << 12;
			ch = us[i++];
			val += (ch & 0x3F) << 6;
			ch = us[i++];
			val += (ch & 0x3F);
			tbuf[ui] = static_cast<wchar_t>(((val - 0x10000) >> 10) + SURROGATE_LEAD_FIRST);
			ui++;
			tbuf[ui] = static_cast<wchar_t>((val & 0x3ff) + SURROGATE_TRAIL_FIRST);
		}
		ui++;
	}

	return ui;
} 


static std::wstring StringFromUTF8(const char *s)
{
	size_t sLen = s ? strlen(s) : 0;
	size_t wideLen = UTF16Length(s, sLen);
	std::vector<wchar_t> vgc(wideLen + 1);
	size_t outLen = UTF16FromUTF8(s, sLen, &vgc[0], wideLen);
	vgc[outLen] = 0;
	return std::wstring (&vgc[0], outLen);
}

static std::string UTF8FromString(const std::wstring &s)
{
	size_t sLen = s.size();
	size_t narrowLen = UTF8Length(s.c_str(), sLen);
	std::vector<char> vc(narrowLen + 1);
	UTF8FromUTF16(s.c_str(), sLen, &vc[0], narrowLen);
  
	return std::string(&vc[0], narrowLen);
}

bool CStringUtl::StringToUTF8(IN CONST wchar_t* pString, OUT char* pOutString, U32 ulOutSize)
{
  const std::wstring s(pString);
  std::string str = UTF8FromString(s);
  
  memcpy(pOutString, str.c_str(), min(str.size(), ulOutSize));
  
  return TRUE;
}

bool CStringUtl::UTF8ToString(IN CONST char* pString, OUT wchar_t* pOutString, U32 ulOutSize)
{ 
  std::wstring str = StringFromUTF8(pString);
  int s = str.size();

  lstrcpynW(pOutString, str.c_str(), ulOutSize);
  
  return TRUE;
}




std::wstring  StringFromInteger(int i) 
{
	wchar_t number[32];
#if defined(__BORLANDC__)
	wsprintf(number, L"%0d", i);
#elif defined(_MSC_VER) && (_MSC_VER > 1310)
	swprintf(number, 30, L"%0d", i);
#else
	swprintf(number, L"%0d", i);
#endif
	return std::wstring (number);
}


#if 0 
//
//  Code Page Default Values.
//
#define CP_ACP                    0           // default to ANSI code page
#define CP_OEMCP                  1           // default to OEM  code page
#define CP_MACCP                  2           // default to MAC  code page
#define CP_THREAD_ACP             3           // current thread's ANSI code page
#define CP_SYMBOL                 42          // SYMBOL translations

#define CP_UTF7                   65000       // UTF-7 translation
#define CP_UTF8                   65001       // UTF-8 translation
#endif 

#if 0 
#define MBCSToWChar(c,a,w,i) MultiByteToWideChar(c,0,a,-1,w,i)
#define WCharToMBCS(c,w,a,i) WideCharToMultiByte(c,0,w,-1,a,i,NULL,NULL)

UINT TextA2W(UINT uCP,LPSTR lpString)
{
  WCHAR wsz[256] = {_T("1")};
  MBCSToWChar(uCP,lpString,wsz,256 / sizeof(WCHAR));
}

//
//  A2W: Convert Dialog Item Text form Unicode to UTF-8 and vice versa
//
UINT TextU2W(UINT uCP,LPSTR lpString,int nMaxCount)
{
  WCHAR wsz[256] = {_T("1")};
  
  WCharToMBCS(uCP,wsz,lpString,nMaxCount);
  return 0;
}
#endif 

