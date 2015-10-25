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
//////////////////////////////////////////////////////////////////////////
// CPluginManager.cpp implementation of the CPluginManager_i class.
/////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DllMgr.h" 
#include "../../inc/shlwapi.h" 

#pragma comment(lib, "shlwapi.lib")


////////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
CPluginManager_i::CPluginManager_i(CONST CHAR pszPluginEntry[128])
{
  m_PluginNum    = 0;
  m_strLastError = _T("");
  
  lstrcpyA(m_pszPluginEntry, pszPluginEntry); 
}


CPluginManager_i::~CPluginManager_i()
{
  m_pszPluginEntry[0] = 0;
  
  UnLoadAllPlugins();
}


DWORD CPluginManager_i::LoadAllPlugins(CStringArray& strFileArray, DWORD dwOption)
{
  DWORD ret    = 0; 

  UnLoadAllPlugins();
  
  m_strLastError = _T("");
  
  m_PluginNum = strFileArray.GetSize();
  if(m_PluginNum == 0)
  {
    return ERR_PLUGIN_NOT_EXIST_I;
  }
  
  int cnt = m_PluginNum;
  ret = 0;
  
  CDllMgr* pDllFile = NULL;

  for (int i = 0; i< cnt; i++)
  {
    CString strFile = strFileArray.GetAt(i);
    
    if(!PathFileExists(strFile))
    {
      m_strLastError.Format(_T("The plug-in File '%s' don't exist.\n"), strFile);
      return ERR_PLUGIN_NOT_EXIST_I;
    }
    
    pDllFile = new CDllMgr;
    if(pDllFile)
    {
      pDllFile->Config(m_pszPluginEntry);
      DWORD dwRet = pDllFile->LoadDll(strFile);
      if(dwRet)
      {
        m_strLastError = pDllFile->m_strError;
        OutputDbgStr_i(_T("%s\n"), m_strLastError);
        ret = dwRet;
        delete pDllFile;
        pDllFile = NULL;
      }
      else 
      {
        m_PluginObjList.AddTail((PVOID)pDllFile); 
      }
    }
    else 
    {
      m_strLastError.Format(_T("Fail to find the free space to allocate plugin context."));
    }    
    pDllFile = NULL;
  }
    
  // AddDocTemplate 
  RegisterPlugin(NULL);
  
  return ret; 
}


DWORD CPluginManager_i::RegisterPlugin(PVOID pFile)
{
  CDllMgr* pDllFile = (CDllMgr*)pFile;
  int iNum                = 0;
  CPluginEntry_i*	pTemplate = NULL; 
  POSITION pos;
  
  if(pDllFile == NULL)
  {
    // register doc templates from loaded DLL's
    // Iterate through the list in head-to-tail order.
    for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
    {
      pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos ); 
      if(pDllFile)
      {
        if(pDllFile->m_pfnDllDocTemplate) //  Only register doctemplate the plugin
        {
          pTemplate = pDllFile->m_pfnDllDocTemplate();	// return template pointer.
          if (pTemplate && !IsBadReadPtr(pTemplate, sizeof(pTemplate)))
          {
            if(pTemplate->m_pCustomClass == NULL)
            {
              AddDocTemplate(pTemplate);
            }
            
            pDllFile->m_pTemplate = pTemplate;
            pTemplate->m_strPluginFileName = pDllFile->m_strFileName;
            
            pTemplate = NULL;
            iNum++;
          }
          else
          {
            pDllFile->FreeDll();
            U32 ret = ::GetLastError();
            m_strLastError.Format(_T("Don't find Doc Template in '%s', error:%s[%d]."), pDllFile->m_strFileName, GetLastErrorMsg_i(ret, TRUE), ret);
          }
        }
      }
    }
  }
  else 
  {
    if(pDllFile->m_pfnDllDocTemplate) //  Only register doctemplate the plugin
    {
      pTemplate = pDllFile->m_pfnDllDocTemplate();	// return template pointer.
      if (pTemplate && !IsBadReadPtr(pTemplate, sizeof(pTemplate)))
      {
        AddDocTemplate(pTemplate);
        
        pDllFile->m_pTemplate = pTemplate;
        pTemplate->m_strPluginFileName = pDllFile->m_strFileName;
        
        pTemplate = NULL;
        iNum++;
      }
      else
      {
        pDllFile->FreeDll();
        U32 ret = ::GetLastError();
        m_strLastError.Format(_T("Don't find Plugin in '%s', error:%s[%d]."), pDllFile->m_strFileName, GetLastErrorMsg_i(ret, TRUE), ret);
      }
    }
  }
  
  return iNum ;
}


DWORD CPluginManager_i::UnLoadAllPlugins(DWORD dwOption)
{
  POSITION pos; 

  // Iterate through the list in head-to-tail order.
  for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
  {
    CDllMgr* pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos );
    if(pDllFile)
    { 
      pDllFile->FreeDll();
      delete pDllFile;
    }
  }
  
  m_PluginObjList.RemoveAll();
  
  m_PluginNum = 0;
  
  return 0;
}


DWORD CPluginManager_i::UnloadPlugin(LPCTSTR pszFile)
{ 
  POSITION pos; 
  
  // Iterate through the list in head-to-tail order.
  for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
  {
    POSITION curpos = pos; 
    CDllMgr* pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos );
    if(pDllFile && (pDllFile->m_strFileName == pszFile))
    {
      pDllFile->FreeDll();
      pDllFile = NULL;
      if(curpos)
      {
        m_PluginObjList.RemoveAt(curpos);
      }
      break;
    }
  }
  
  m_PluginNum = m_PluginObjList.GetCount();
  
  return ERR_OK_I;
}


DWORD CPluginManager_i::LoadPlugin(LPCTSTR pszFile)
{
  DWORD ret = 0; 
  CString	strFile = pszFile;
  
  if(strFile.IsEmpty())
  {
    m_strLastError.Format(_T("The input plug-in file is emptry"));
    return ERR_INVALID_PARAM_I;
  }
  
  if(!PathFileExists(strFile))
  {
    m_strLastError.Format(_T("The plug-in File '%s' don't exist"), strFile);
    return ERR_NOT_FOUND_I;
  }

  POSITION pos; 
  CDllMgr* pDllFile = NULL;

  ret = ERR_PLUGIN_NOT_EXIST_I;

  // Iterate through the list in head-to-tail order.
  for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
  {
    pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos );
    if(pDllFile && (pDllFile->m_strFileName == pszFile))
    {
      ret = ERR_ALREADY_EXIST_I;
      break;
    }
  }
  
  // Don't find the Plugin 
  if(ret != ERR_ALREADY_EXIST_I)
  {
    pDllFile = new CDllMgr;
    if(pDllFile)
    {
      pDllFile->Config(m_pszPluginEntry);
      DWORD dwRet = pDllFile->LoadDll(strFile);
      if(dwRet)
      {
        m_strLastError = pDllFile->m_strError;
        OutputDbgStr_i(_T(" %s\n"),  m_strLastError);
        ret = dwRet;
        delete pDllFile;
        pDllFile = NULL;
      }
      else 
      {
        if(RegisterPlugin((PVOID)pDllFile))
        {
          m_PluginObjList.AddTail((PVOID)pDllFile); 
          ret = ERR_OK_I;
        }
        else 
        {
          delete pDllFile;
          pDllFile = NULL;
        }
      }
    }
  }
  
  return ret; 
}


CPluginEntry_i* CPluginManager_i::GetPlugin(LPCTSTR pszPluginName)
{
  U32 ret = ERR_OK_I;

  if(pszPluginName == NULL)
    return NULL;
  
  POSITION pos; 
  CDllMgr* pDllFile = NULL;

  m_strLastError.Empty();

  ret = ERR_PLUGIN_NOT_EXIST_I;
  
  // Iterate through the list in head-to-tail order.
  for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
  {
    pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos );
    if(pDllFile && pDllFile->m_pTemplate && (pDllFile->m_pTemplate->GetPluingName() == pszPluginName))
    { 
      return pDllFile->m_pTemplate;
    }
  }

  m_strLastError.Format(_T("The Plugin '%s' don't exist."), pszPluginName);
  
  return NULL;
}


CPluginEntry_i* CPluginManager_i::GetPlugin(UINT ulPluginId)
{
  POSITION pos; 
  CDllMgr* pDllFile = NULL;
  ULONG ret = ERR_PLUGIN_NOT_EXIST_I;
  
  m_strLastError.Empty();
     
  // Iterate through the list in head-to-tail order.
  for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
  {
    pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos );
    
    if(pDllFile && pDllFile->m_pTemplate && pDllFile->m_pTemplate->GetPluingId() == ulPluginId)
    {  
      return pDllFile->m_pTemplate;
    }
  }  
  
  return NULL;  
}


U32 CPluginManager_i::EnumPlugins(CStringArray& strPluginNameArray,  //
                                  CStringArray& strPluginFileNameArray, // 
                                  CDWordArray& PluginIdArray, // 
                                  CDWordArray& UserDataArray) // 
{
  POSITION pos; 
  CDllMgr* pDllFile = NULL;
  ULONG ret = ERR_PLUGIN_NOT_EXIST_I;
  
  m_strLastError.Empty();
  
  // Iterate through the list in head-to-tail order.
  for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
  {
    pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos );
    if(pDllFile && pDllFile->m_pTemplate)
    { 
      strPluginNameArray.Add(pDllFile->m_pTemplate->GetPluingName());
      strPluginFileNameArray.Add(pDllFile->m_pTemplate->m_strPluginFileName);
      PluginIdArray.Add(pDllFile->m_pTemplate->GetPluingId());
      UserDataArray.Add((DWORD)pDllFile->m_pTemplate->GetPluingUserData());
      pDllFile = NULL;
    }
  } 
  
  return ERR_OK_I;
}


U32 CPluginManager_i::GetPlugin(IN LPCTSTR pszPluginFileName, 
                                OUT CString& strPluginName,  //
                                OUT ULONG& ulPluginId)
{
  POSITION pos; 
  CDllMgr* pDllFile = NULL;
  ULONG ret = ERR_PLUGIN_NOT_EXIST_I;
  
  m_strLastError.Empty();
  
  // Iterate through the list in head-to-tail order.
  for( pos = m_PluginObjList.GetHeadPosition(); pos != NULL; )
  {
    pDllFile = (CDllMgr*)m_PluginObjList.GetNext( pos );
    if(pDllFile && pDllFile->m_pTemplate)
    {
      if(pDllFile->m_pTemplate->m_strPluginFileName.CompareNoCase (pszPluginFileName) == 0)
      {
        strPluginName = pDllFile->m_pTemplate->GetPluingName();
        ulPluginId    = pDllFile->m_pTemplate->GetPluingId();
        break;
      }
      pDllFile = NULL;
    }
  }
  
  return ERR_OK_I;
}


#if 0
CPluginEntry_i* CPluginManager_i::GetActivePlugin(VOID)
{
  U32 ret = ERR_ITS_OK;
  
  POSITION DocTemplatePos = GetFirstDocTemplatePosition();
  
  //while(DocTemplatePos)
  {
    CMultiDocTemplate* pDocTemp = (CMultiDocTemplate*)GetNextDocTemplate(DocTemplatePos);    
    POSITION pos = pDocTemp->GetFirstDocPosition ();
    
    // Find the doc within the same doctemplate.
    while (pos)
    {
      CDocument* pDoc     = pDocTemp->GetNextDoc (pos);
      POSITION ViewPos    = pDoc->GetFirstViewPosition ();
      CView* pView        = pDoc->GetNextView (ViewPos);
      if(pView)
      {
        CFrameWnd* pFrame = (CFrameWnd*)pView->GetParentFrame();
        CFrameWnd* pActiveFrameWnd = GetActiveFrame();
        if(pFrame == pActiveFrameWnd)
        {
          //OutputDebugString("Find!\n");
          pView->SendMessage(ulMsg, wParam, lParam);
        } 
      }
    }
  }
  
  return ret;
}
#endif 
