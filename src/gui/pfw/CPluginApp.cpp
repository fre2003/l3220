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

CPluginApp_i::CPluginApp_i() 
{
  m_hPluginInst = 0;
}

CPluginApp_i::~CPluginApp_i()
{

}

BOOL CPluginApp_i::InitInstance()
{
	m_hPluginInst = m_hInstance;
	
	return CWinApp::InitInstance();
}

bool CPluginApp_i::IsExistInstaceId(UINT nInstanceId)
{
  if(m_pPluginEntry)
  {
    if(m_pPluginEntry->IsExistInstance(nInstanceId))
    {
      return TRUE;
    }
  }

  return FALSE;
}


// Create a new instance window with the unique instance ID.
// ERR_OK_2
// ERR_PLUGIN_INSTANCE_ALREADY_EXIST_I 
// ERR_PLUGIN_NOT_EXIST_I
// ERR_WIN32_ERROR_I
U32 CPluginApp_i::NewInstance(UINT nInstanceId)
{
  U32 ret = 0;

  if(m_pPluginEntry)
  {
    if(m_pPluginEntry->IsExistInstance(nInstanceId))
    {
      ret = ERR_PLUGIN_INSTANCE_ALREADY_EXIST_I;
    }
    else if(m_pPluginEntry->NewInstance(nInstanceId) == ERR_OK_I)
    {
      ret = ERR_OK_I;
    }
    else
    {
      ret = ERR_WIN32_ERROR_I;
      TRACE_I(_T("Fail to new instance[%d].\n"), nInstanceId);
    }
  }
  else
  {
    ret = ERR_PLUGIN_NOT_EXIST_I;
  }
  
  return ret;
}

U32 CPluginApp_i::OpenInstance(UINT nInstanceId)
{
  U32 ret = 0;

  if(m_pPluginEntry)
  {
    ret = m_pPluginEntry->OpenInstance(nInstanceId);   
  }
  else
  {
    ret = ERR_PLUGIN_NOT_EXIST_I;
  }

  return ret;
}

U32 CPluginApp_i::CloseInstance(UINT nInstanceId)
{
  U32 ret = 0;

  if(m_pPluginEntry)
  {
    if(m_pPluginEntry->IsExistInstance(nInstanceId))
    {
      if(m_pPluginEntry->CloseInstance(nInstanceId) == ERR_OK_I)
      {
         ret = ERR_OK_I;
      }
      else
      {
        ret = ERR_WIN32_ERROR_I;
        TRACE_I(_T("Fail to close instance[%d].\n"), nInstanceId);
      }
    }
    else 
    {
        ret = ERR_PLUGIN_INSTANCE_NOT_EXIST_I;
        TRACE_I(_T("Fail to find instance[%d].\n"), nInstanceId);
    }
  }
  else
  {
    ret = ERR_PLUGIN_NOT_EXIST_I;
  }

  return ret;  
}


CPluginEntry_i* CPluginApp_i::CreatePluginEntry(UINT nPluginId, 
                                                LPCTSTR pszPluginName, 
                                                UINT nIDResource,
                                                CObject* pfnApp,
                                                PVOID pUserData,
                                                CRuntimeClass* pDocClass, 
                                                CRuntimeClass* pFrameClass,
                                                CRuntimeClass* pViewClass)
{
  if(m_hPluginInst == 0)
  {
    return NULL;
  }
  else
  {
    CPluginEntry_i	*pEntry = NULL;
    
    pEntry = new CPluginEntry_i 
      (
      m_hPluginInst,			// the APP's resource instance
      nIDResource,        // menu
      pDocClass,          // this doc 
      pFrameClass,        // custom MDI child frame
      pViewClass,
      pfnApp,
      pUserData,
      nPluginId,
      pszPluginName);
    
    m_pPluginEntry = pEntry;
    
    return pEntry;  
  }
}

// Send to MainFrame
U32 CPluginApp_i::SendMainWndMessage(UINT ulMsg, WPARAM wParam, LPARAM lParam)
{
  U32 ulRet = 0;

  if(m_hPluginInst)
  {
    CWnd* pWnd = AfxGetMainWnd();
    if(pWnd) 
    {
      // calls the window procedure directly and does not return 
      // until that window procedure has processed the message
      pWnd->SendMessage(ulMsg,wParam, lParam);
      ulRet = ERR_OK_I;
    }
    else
    {
      ulRet = ERR_NO_INITIALIZATION_I;
    }
  }
  
  return ulRet;
}

U32 CPluginApp_i::PostMainWndMessage(UINT ulMsg, WPARAM wParam, LPARAM lParam)
{
  U32 ulRet = 0;

  if(m_hPluginInst)
  {
    CWnd* pWnd = AfxGetMainWnd();
    if(pWnd) 
    {
      if(pWnd->PostMessage(ulMsg,wParam, lParam) == FALSE)
      {
        ulRet = ERR_WIN32_ERROR_I;
      }
      else
      {
        ulRet = ERR_OK_I;
      }
    }
    else
    {
      ulRet = ERR_NO_INITIALIZATION_I;
    }
  }
  
  return ulRet;
}

#if 0 
U32 CPluginApp_i::SendMessageToFrame(UINT ulMsg, WPARAM wParam, LPARAM lParam) 
{
#if 0 
  CFrameWnd*  pFrameWnd = GetParentFrame(); 
  if(pFrameWnd)
  {
    pFrameWnd->SendMessage(ulMsg, wParam, lParam);
  }
#endif 

  return 0;
}

// 在 Frame中发消息给View 
U32 CPluginApp_i::SendMessageToView(UINT ulMsg, WPARAM wParam, LPARAM lParam) 
{
  CWinApp* pWinApp = AfxGetApp();
  if(pWinApp)
  {
    POSITION DocTemplatePos = pWinApp->GetFirstDocTemplatePosition();
  
    //while(DocTemplatePos)
    {
	    CMultiDocTemplate* pDocTemp = (CMultiDocTemplate*)pWinApp->GetNextDocTemplate(DocTemplatePos);
    
      //  
 	    POSITION pos = pDocTemp->GetFirstDocPosition ();
  
      // Find the doc within the same doctemplate.
      while (pos)
      {
	      CDocument* pDoc     = pDocTemp->GetNextDoc (pos);
	      POSITION ViewPos    = pDoc->GetFirstViewPosition ();
	      CView* pView        = pDoc->GetNextView (ViewPos);
        if(pView)
        {
          CMDIChildWnd* pFrame = (CMDIChildWnd*)pView->GetParentFrame();
          if(pFrame == this)
          {
            //OutputDebugString("Find!\n");
            pView->SendMessage(ulMsg, wParam, lParam);
          } 
        }
      }
    }
  }
  
  return 0;
}
#endif 
