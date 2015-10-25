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
///////////////////////////////////////////////////////////////////////////////
// CPluginEntry_i.cpp: implementation of the  class.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPluginEntry_i, CMultiDocTemplate)

CPluginEntry_i::CPluginEntry_i(HINSTANCE hInst, 
                           UINT nIDResource, 
                           CRuntimeClass* pDocClass, 
                           CRuntimeClass* pFrameClass, 
                           CRuntimeClass* pViewClass,
                           CObject* pApp,
                           PVOID pUserData, 
                           UINT nPluginId, 
                           LPCTSTR pszPluginName)
                          :CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
  m_hInst         = hInst;
  m_pUserData     = pUserData;
  m_pFrameClass   = pFrameClass;
  m_pDocClass     = pDocClass;
  m_strPluginName = pszPluginName;
  m_nPluginId     = nPluginId;
  m_pApp          = pApp;
  m_pCustomClass  = NULL;
}

CPluginEntry_i::~CPluginEntry_i()
{
  m_InstanceMap.RemoveAll(); 
}

U32 CPluginEntry_i::NewInstance(U8 nInstaceId)
{
  CDocument* pDoc = OpenDocumentFile(NULL);
  if(pDoc)
  {
    m_InstanceMap.SetAt(nInstaceId, (PVOID)pDoc);
  }
  else
  {
    return ERR_WIN32_ERROR_I;
  }
  
  return ERR_OK_I;
}

bool CPluginEntry_i::IsExistInstance(VOID)
{
  POSITION pos = GetFirstDocPosition();
  if (pos == NULL)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

// Check the instace already exist.
bool CPluginEntry_i::IsExistInstance(U8 nInstaceId)
{
  bool ret = FALSE;
    
  if(nInstaceId == 0)
  {
    return FALSE;
  }
  
  POSITION pos = m_InstanceMap.GetStartPosition();
  U8 nKey = 0; 
  PVOID pValue = NULL;
  while (pos != NULL)
  {
    m_InstanceMap.GetNextAssoc( pos, nKey, pValue );
    if(nKey && (nKey == nInstaceId))
    {
      ret = TRUE;
      break;
    }
  }
  
  return ret;
}


U32 CPluginEntry_i::OpenInstance(U8 nInstaceId)
{ 
  POSITION pos = GetFirstDocPosition();
  if (pos == NULL)
  {
    return ERR_PLUGIN_NOT_EXIST_I;
  }
  
  CDocument* pInstanceDoc = NULL;
  
  POSITION inspos = m_InstanceMap.GetStartPosition();
  U8 nKey = 0; 
  PVOID pValue = NULL;
  while (inspos != NULL)
  {
    m_InstanceMap.GetNextAssoc( inspos, nKey, pValue );
    if(nKey && (nKey == nInstaceId))
    {
      pInstanceDoc = (CDocument*)pValue;
      break;
    }
  }
  
  if(pInstanceDoc)
  {
    CDocument* pDoc       = NULL; 
    CDocument* pTargetDoc = NULL; 
    
    pDoc = GetNextDoc (pos);
    ASSERT_VALID (pDoc); 
    
    if(pDoc == pInstanceDoc)
    {
      pTargetDoc = pDoc;
    }
    else
    {
      while (pos != NULL && pDoc)
      {
        pDoc = GetNextDoc (pos);
        if(pDoc == pInstanceDoc)
        {
          pTargetDoc = pDoc;
          break;
        }
      }
    }
    
    // Get View and Acitive it.
    if(pTargetDoc)
    {
      pos = pDoc->GetFirstViewPosition ();
      ASSERT (pos != NULL);
      
      CView* pView = pDoc->GetNextView (pos);
      ASSERT_VALID (pView);
      
      CFrameWnd* pFrame = pView->GetParentFrame ();
      ASSERT_VALID (pFrame);
      
      CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*) AfxGetMainWnd();
      ASSERT_VALID (pMainFrame);
      
      ::SendMessage (pMainFrame->m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)pFrame->GetSafeHwnd(), 0);
    }
    else
    {
      return ERR_WIN32_ERROR_I;
    } 
  }
  else
  {
    return ERR_PLUGIN_NOT_EXIST_I;
  } 

  return ERR_OK_I;
}


U32 CPluginEntry_i::GetInstanceFrameView(IN U8 nInstaceId, OUT CView* &pView, OUT CFrameWnd* &pFrame)
{
  pView   = NULL;
  pFrame = NULL;
  
  POSITION pos = GetFirstDocPosition();
  if (pos == NULL)
  {
    return ERR_PLUGIN_NOT_EXIST_I;
  }
  
  CDocument* pInstanceDoc = NULL;
  
  POSITION posInstance = m_InstanceMap.GetStartPosition();
  U8 nKey = 0; 
  PVOID pValue = NULL;
  while (posInstance != NULL)
  {
    m_InstanceMap.GetNextAssoc( posInstance, nKey, pValue );
    if(nKey && (nKey == nInstaceId))
    {
      pInstanceDoc = (CDocument*)pValue;
      break;
    }
  }
  
  if(pInstanceDoc)
  {
    CDocument* pDoc       = NULL; 
    CDocument* pTargetDoc = NULL; 
    
    pDoc = GetNextDoc (pos);
    ASSERT_VALID (pDoc);
    
    if(pDoc == pInstanceDoc)
    {
      pTargetDoc = pDoc;
    }
    else
    {
      while (pos != NULL && pDoc)
      {
        pDoc = GetNextDoc (pos);
        if(pDoc == pInstanceDoc)
        {
          pTargetDoc = pDoc;
          break;
        }
      }
    }
    
    // Get View and Acitive it.
    if(pTargetDoc)
    {
      pos = pDoc->GetFirstViewPosition ();
      ASSERT (pos != NULL);
      
      pView = pDoc->GetNextView (pos);
      ASSERT_VALID (pView);
      
      pFrame = pView->GetParentFrame ();
      ASSERT_VALID (pFrame); 
    }
    else
    {
      return ERR_WIN32_ERROR_I;
    } 
  }
  else
  {
    return ERR_PLUGIN_NOT_EXIST_I;
  } 
  
  return ERR_OK_I;
}


HWND CPluginEntry_i::GetPluginInstanceHwnd(U8 nInstaceId)
{
  CFrameWnd* pFrame = NULL;
  CView* pView = NULL;
  U32 ret = ERR_OK_I;
  
  ret = GetInstanceFrameView(nInstaceId, pView, pFrame);
  if(ret == ERR_OK_I)
  {
    if(pFrame)
    {
      return pFrame->GetSafeHwnd();
    }
  }
  
  return NULL;
}


U8 CPluginEntry_i::GetPluginInstanceCount()
{
  return (U8)m_InstanceMap.GetCount();
}


U8 CPluginEntry_i::EnumPluginInstanceHwnd(CDWordArray& aHwndArray)
{
  POSITION pos = GetFirstDocPosition();
  if (pos == NULL)
  {
    return 0;
  }
  
  aHwndArray.RemoveAll();
  
  CDocument* pInstanceDoc = NULL;  
  POSITION posInstance =  m_InstanceMap.GetStartPosition();
  U8 nKey = 0; 
  PVOID pValue = NULL;
  while (posInstance != NULL)
  {
    m_InstanceMap.GetNextAssoc( posInstance, nKey, pValue );
    if(nKey && pValue)
    {
      pInstanceDoc = (CDocument*)pValue; 
      CDocument* pDoc       = NULL; 
      CDocument* pTargetDoc = NULL; 
        
      pDoc = GetNextDoc (pos);
      ASSERT_VALID (pDoc);
      
      if(pDoc == pInstanceDoc)
      {
        pTargetDoc = pDoc;
      }
      else
      {
        while (pos != NULL && pDoc)
        {
          pDoc = GetNextDoc (pos);
          if(pDoc == pInstanceDoc)
          { 
            pTargetDoc = pDoc;
            break;
          }
        } 
      }
      
      // Get View and Acitive it.
      if(pTargetDoc)
      {
        pos = pDoc->GetFirstViewPosition ();
        ASSERT (pos != NULL);
        
        CView* pView = pDoc->GetNextView (pos);
        ASSERT_VALID (pView);
        
        CFrameWnd*pFrame = pView->GetParentFrame ();
        ASSERT_VALID (pFrame);
        
        aHwndArray.Add((DWORD)pFrame->GetSafeHwnd());
      }  
    }
  } // while 
  
  return aHwndArray.GetSize();
}

U32 CPluginEntry_i::CloseInstance(U8 nInstaceId)
{
  U32 ret = ERR_OK_I; 
  
  if(!m_InstanceMap.RemoveKey(nInstaceId))
  {
    ret = ERR_PLUGIN_NOT_EXIST_I;
  }

  return ret;
}

#if 0 
U32 CPluginEntry_i::LoadResource(UINT nIDResource)
{
  UnloadResource();
  
  if(nIDResource)
  {
    m_nIDResource = nIDResource;
  }
  
  if (m_nResourceId != 0 && m_hMenu == NULL)
  {
    HINSTANCE hInst = AfxFindResourceHandle(
      MAKEINTRESOURCE(m_nIDResource), RT_MENU);
    m_hMenu = ::LoadMenu(hInst, MAKEINTRESOURCE(m_nIDResource));
    m_hAccelTbl = ::LoadAccelerators(hInst, MAKEINTRESOURCE(m_nIDResource));
  }
  
  return ERR_OK_I;
}

U32 CPluginEntry_i::UnloadResource()
{
  ::LoadMenu(NULL, 0);

	// delete shared components
	if (m_hMenu != NULL)
	{
    ::DestroyMenu(m_hMenu);
    m_hMenu = NULL;
  }

	if (m_hAccelTbl != NULL)
	{
    ::FreeResource((HGLOBAL)m_hAccelTbl);
    m_hAccelTbl = NULL;
  }

  return ERR_OK_I;
}
#endif 


// void CPluginEntry_i::LoadTemplate()
// {
// 	HINSTANCE	hOld ;
// 
//   hOld = AfxGetResourceHandle();
// 	AfxSetResourceHandle(m_hInst);
// 	CMultiDocTemplate::LoadTemplate();
//   AfxSetResourceHandle(hOld);
// }
// 
// CDocument* CPluginEntry_i::OpenDocumentFile(LPCTSTR lpszPathName, bool bMakeVisible)
// {
// 	CDocument	*pDoc = NULL ;
// 	HINSTANCE	hOld;
// 
// 	hOld = AfxGetResourceHandle();
// 	AfxSetResourceHandle(m_hInst);
// 	pDoc = CMultiDocTemplate::OpenDocumentFile(lpszPathName, bMakeVisible) ;
// 	AfxSetResourceHandle(hOld);
// 	return pDoc;
// }
