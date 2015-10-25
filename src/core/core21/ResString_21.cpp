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
#include "stdafx.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if 0 
class CResString  
{
public:
  static CResString& Instance();
  virtual ~CResString();
  
  BOOL SetStringSource(const CString& strSrc); // must the ini file.
  
  const CString LoadString(const CString& strName);
  
private:
  CResString();
  
private:
  CString m_strStringSrc;   // the file saved the string.
  HANDLE m_hXml;
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResString::CResString():m_strStringSrc("") , m_hXml(NULL)
{
#if 0 
  CString strLangXml = UTL_GetValidFilePath("common\\config\\cplt\\language\\cn.xml");
  m_strStringSrc = strLangXml;
  
  CString strSrcDir = strLangXml; // GetExeDir() + m_strStringSrc;
  
  m_hXml = XML_LoadFile(strSrcDir);
#endif // _DEBUG
}

CResString::~CResString()
{
  if (m_hXml)
  {
    XML_FreeFile(m_hXml);
    m_hXml = NULL;
  }
}

CResString& CResString::Instance()
{
  static CResString _Instance;
  return _Instance;
}

const CString CResString::LoadString(const CString& strName)
{
#ifdef STRING_SRC_FILE_USE_INI
  char retVal[1024] ={0};
  CString strSrcDir = GetExeDir() + m_strStringSrc;
  GetPrivateProfileString(SECTION_STRIN_TABLE, strName, NULL, retVal, 1024, strSrcDir);
  return retVal;
#else
  
  ASSERT(m_hXml != NULL);
  
  CString strTmp = "CPLT:COOL_STRING:" + strName;
  CString strRet = _T("");
  
  if(m_hXml)
  {
    strRet = XML_GetElementTextValue(m_hXml, strTmp,  1,1);
  }
  else
  {
    strRet = "Don't find the XML Configuration File.";
  }
  
  return strRet;
  
#endif
}

BOOL CResString::SetStringSource(const CString& strSrc)
{
  m_strStringSrc = strSrc;
  
  if (m_hXml)
  {
    XML_FreeFile(m_hXml);
    m_hXml = NULL;
  }
  
  //CString strLangXml = UTL_GetValidFilePath("common\\config\\cplt\\language\\cn.xml");
  
  CString strSrcDir =  m_strStringSrc;
  
  m_hXml = XML_LoadFile(strSrcDir);
  
  // if the file does not exist, return FALSE;
  // else return TRUE;
  return TRUE;
}
#endif 


// Call this function in DllMain() of DLL or InitInstance() of MFC application.
CONST U32 LoadResStringLib(LPCTSTR pszResStringLibFile/* = NULL*/)
{
  if(pszResStringLibFile == NULL)
    return ERR_INVALID_PARAM_I;
  else
    return ERR_OK_I;
}

CONST CString GetResString(LPCTSTR pszString)
{
  if(pszString == NULL)
    return _T("");
  else
    return pszString;
}
