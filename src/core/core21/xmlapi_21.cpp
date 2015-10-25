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
#define HAVE_XML_STATIC_LIB 
#include <xml_i.h>

CXmlFile_i::CXmlFile_i(TCHAR szSeperate)
{
  m_hXmlFile = NULL; 
  m_szSeperate = szSeperate;
  m_strXmlFile.Empty();
}


CXmlFile_i::~CXmlFile_i()
{
  if(m_hXmlFile)
    XML_FreeFile(m_hXmlFile);
  m_hXmlFile = NULL;
}


U32 CXmlFile_i::LoadFile(LPCTSTR pszFile)
{
  if(m_hXmlFile)
    return ERR_ALREADY_INITIALIZED_I;

  if(pszFile == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  if(PathFileExists(pszFile))
  {
    m_hXmlFile = XML_LoadFile(pszFile);
    m_strXmlFile = pszFile;
  }
  else 
  {
    return ERR_NOT_FOUND_I;
  }
  
  return ERR_OK_I;
}


U32 CXmlFile_i::SaveFile()
{
  if(m_hXmlFile == NULL)
    return ERR_NO_INITIALIZATION_I;
  
  if(XML_SaveFile(m_hXmlFile) == FALSE)
  {
    return ERR_INTERNAL_I;
  }
  
  return ERR_OK_I;
}


U32 CXmlFile_i::ClearFile()
{
  if(m_hXmlFile == NULL)
    return ERR_NO_INITIALIZATION_I;
  
  if(XML_ClearFile(m_hXmlFile) == FALSE)
  {
    return ERR_INTERNAL_I;
  }
  
  return ERR_OK_I;
}


LPCTSTR CXmlFile_i::GetLastErrorDesc()
{
  if(m_hXmlFile == NULL)
    return _T("No Initialization");

  return XML_GetLastErrorDesc(m_hXmlFile);
}

HANDLE CXmlFile_i::FindFirstElementNode(LPCTSTR pszNodeStream, LONG nNodeOffset)
{
  return XML_FindFirstElementNode(m_hXmlFile, pszNodeStream, nNodeOffset, m_szSeperate);
}

HANDLE CXmlFile_i::FindNextElementNode(HANDLE hNode, IN LPCTSTR pszName, OUT LPCTSTR* pszValue)
{
  return XML_FindNextElementNode(hNode, pszName, pszValue);
}

ULONG  CXmlFile_i::GetElementNodeInfo(HANDLE hNode, OUT LPCTSTR* pszName, OUT LPCTSTR* pszValue)
{
  return XML_GetElementNodeInfo(hNode, pszName, pszValue);
}

ULONG  CXmlFile_i::SetElementNodeInfo(HANDLE hNode, LPCTSTR pszName, LPCTSTR pszValue)
{
  return XML_SetElementNodeInfo(hNode, pszName, pszValue);
}

HANDLE CXmlFile_i::FindFirstAttributeNode(LPCTSTR pszNodeStream,LONG nNodeOffset)
{
  return XML_FindFirstAttributeNode(m_hXmlFile, pszNodeStream, nNodeOffset, m_szSeperate);
}

HANDLE CXmlFile_i::FindNextAttributeNode(HANDLE hNode, OUT LPCTSTR* pszName, OUT LPCTSTR* pszValue)
{
  return XML_FindNextAttributeNode(hNode, pszName, pszValue);
}

ULONG  CXmlFile_i::GettAttributeNodeInfo(HANDLE hNode, OUT LPCTSTR* pszName, OUT LPCTSTR* pszValue)
{
  return XML_GettAttributeNodeInfo(hNode, pszName, pszValue);
}

ULONG  CXmlFile_i::SettAttributeNodeInfo(HANDLE hNode, LPCTSTR pszName, LPCTSTR pszValue)
{
  return XML_SettAttributeNodeInfo(hNode, pszName, pszValue);
}

LPCTSTR CXmlFile_i::GetAttributeValue(LPCTSTR pszNodeStream, LONG nNodeOffset, 
                                      LPCTSTR pszAttributeName, HANDLE* phNode)
{
  return XML_GetAttributeValue(m_hXmlFile, pszNodeStream, nNodeOffset, m_szSeperate, pszAttributeName, phNode);
}

LPCTSTR CXmlFile_i::GetElementValue(LPCTSTR pszNodeStream, LONG nNodeOffset, 
                                    LPCTSTR pszElementName, LONG nElementNameOffset, HANDLE* phNode)
{
  return XML_GetElementValue(m_hXmlFile, pszNodeStream, nNodeOffset, m_szSeperate, pszElementName, nElementNameOffset, phNode);
}

