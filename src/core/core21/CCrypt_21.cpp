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

#pragma optimize( "s", on)

char InnKey[9]="7eOp@m0D";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
CCrypt::CCrypt()
{

}

CCrypt::~CCrypt()
{

}
*/

void Encoding(LPCSTR src, CString& dest)
{
	int l=0;
	char tmp;
	BYTE t, t2;

	dest.Empty();

	for(UINT i = 0; i<strlen(src); i++)
	{
		t = (BYTE)(src[i] ^ InnKey[l]);
		t2 = (BYTE)(t & 0xF);
		t = (BYTE)(t >> 4);
		tmp = (BYTE)('A' + t);
		dest += tmp;
		tmp = (char)('A' + t2);
		dest += tmp;

		l++;
		l=l%8;
	}
}

void Decoding(LPCSTR src, CString& dest)
{
	int l=0, v;
	BYTE t,	c=0;

	dest.Empty();
	for(UINT i=0; i<strlen(src); i++)
	{
		t=src[i];
		if(t>='A' && t<='Q')
			v = t-'A';
		else
			v = 0;
	
		c = (BYTE)(c << 4);
		c |= v;

		if(i%2==1)
		{
			c = (BYTE)(c ^ InnKey[l]);
			dest += (char)c;
			c=0;
			l++;
			l = l%8;
		}
	}
}

#pragma optimize( "s", off)

#pragma warning( disable : 4505 )

// folowing string MUST be at least 64 TCHARs
static TCHAR *szAlphabet =  _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
                            _T("0123456789")
                            _T("abcdefghijklmnopqrstuvwxyz")
                            _T("98");
// encode
static LPTSTR encode21(LPTSTR str)
{
  for (UINT i = 0; i < _tcslen(str); i++)
  {
    UINT n = (UINT) str[i];
    str[i] = szAlphabet[(n + (5*i)) % 64];
  }
  
  return str;
}

