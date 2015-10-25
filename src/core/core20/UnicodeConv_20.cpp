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
#include <RPC.H>
#include <RPCDCE.H>

#define MBCSToWChar(c,a,w,i) MultiByteToWideChar(c,0,a,-1,w,i)
#define WCharToMBCS(c,w,a,i) WideCharToMultiByte(c,0,w,-1,a,i,NULL,NULL)

INT CStringUtl::wcs2mbsz(CHAR* mbstr, INT cchMultiByte, CONST WCHAR* wcstr)
{
	INT result = (INT)::WideCharToMultiByte(CP_ACP, 0, wcstr, -1, mbstr, cchMultiByte, NULL, NULL);

  if (result > 0)
		mbstr[result-1] = 0;

	return result;
}

INT CStringUtl::mbs2wcsz(WCHAR* wcstr, INT cchWideChar, CONST CHAR* mbstr)
{
  INT result = (INT)::MultiByteToWideChar(CP_ACP, 0, mbstr, -1, wcstr, cchWideChar);
  if (result > 0)
    wcstr[result-1] = 0;
  
  return result;
}


CString CStringUtl::GUID2String(CONST GUID& ClassGuid)
{
    // 4D36E972-E325-11CE-BFC1-08002BE1031
    CString strGuid; 
    
    strGuid.Format(_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
    ClassGuid.Data1, 
    ClassGuid.Data2,
    ClassGuid.Data3, 
    ClassGuid.Data4[0], 
    ClassGuid.Data4[1], 
    ClassGuid.Data4[2],
    ClassGuid.Data4[3],
    ClassGuid.Data4[4], 
    ClassGuid.Data4[5],
    ClassGuid.Data4[6],
    ClassGuid.Data4[7]
    );
    
    return strGuid;
}

U32 CStringUtl::String2GUID(OUT GUID* pClassGuid, LPCTSTR pszGuid)
{
    // 4D36E972-E325-11CE-BFC1-08002BE1031
    if(pClassGuid == NULL)
    {
      return ERR_INVALID_PARAM_I;
    }
    
    ZeroMemory( pClassGuid, sizeof( GUID ) );
    
    return UuidFromString((unsigned char *)pszGuid, pClassGuid); 
}
