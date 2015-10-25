#ifndef _DYNARRAY_H_
#define _DYNARRAY_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma once

template<class TYPE> 
class CDynArray
{
protected:
  struct CNode
  {
    CNode*	pNext;
    CNode*	pPrev;
    TYPE*	  data;
  };

private:
  DWORD m_dwSize;
  CNode* m_pNodeHead;
  CNode* m_pNodeTail;
  
public:
  CDynArray()
  { 
    m_dwSize = 0; 
    m_pNodeTail = m_pNodeHead = NULL;
  }
  
  ~CDynArray() { RemoveAll(); }
  
  void RemoveAll()
  {
    if(m_pNodeHead != NULL)
    {
      CNode *pTmp;
      for(DWORD i = 0; i < m_dwSize; i++)
      {
        delete m_pNodeHead->data;
        m_pNodeHead->data = NULL;
        pTmp = m_pNodeHead;
        m_pNodeHead = m_pNodeHead->pNext;
        delete pTmp;
        pTmp = NULL;
      }
      
      m_dwSize = 0; 
      m_pNodeTail = m_pNodeHead = NULL;
    }
  }
  
  TYPE *operator [](DWORD nIndex)
  {
    if((NULL != m_pNodeHead) && (nIndex < m_dwSize))
    {
      CNode *pTmp = m_pNodeHead;
      
      for(DWORD i = 0; i < nIndex; i++)
      {
        pTmp = pTmp->pNext;
      }
      
      if(pTmp)
      {
        return pTmp->data;
      }
    }
    
    return NULL;
  }
  
  DWORD GetSize() { return m_dwSize; }
  
  bool Add(TYPE *Item)
  {
    if(m_pNodeHead == NULL)
    {
      m_pNodeHead = new CNode;
      m_pNodeTail = m_pNodeHead;
      m_pNodeHead->data = Item;
      m_pNodeHead->pPrev = NULL;
      m_pNodeHead->pNext = NULL;
    }
    else
    {
      CNode *pTmp;
      m_pNodeTail->pNext = new CNode;
      pTmp = m_pNodeTail;
      m_pNodeTail = m_pNodeTail->pNext;
      m_pNodeTail->data = Item;
      m_pNodeTail->pPrev = pTmp;
      m_pNodeTail->pNext = NULL;
    }
    
    m_dwSize++;
    return true;
  }
  
  bool RemoveAt(TYPE *Item)
  {
    bool find = false;
    if(m_dwSize == 0 || Item == NULL)
      return false;
    
    CNode *pTmp = m_pNodeHead;
    if(pTmp)
    {
      if(pTmp->data == Item)
      {
        m_dwSize--;
        if(pTmp->data)
          delete pTmp->data;
        delete pTmp; 
        m_pNodeHead = NULL;
        return true;
      }
      
      for(DWORD i = 0; i < m_dwSize; i++)
      {
        pTmp = pTmp->pNext;
        if(pTmp && (pTmp->data == Item))
        {
          pTmp->pPrev->pNext = pTmp->pNext;
          pTmp->pNext->pPrev = pTmp->pPrev;
          m_dwSize--;
          if(pTmp->data)
            delete pTmp->data;
          delete pTmp;
          find = true;
        }
      }
    }

    return find?true:false;
  }

  bool RemoveAt(DWORD nIndex)
  {
    if((NULL != m_pNodeHead) && (nIndex < m_dwSize))
    {
      if(nIndex == 0)					// if remove the head
      {
        CNode *pTmp = m_pNodeHead;
        m_pNodeHead = m_pNodeHead->pNext;
        
        delete pTmp->data;
        delete pTmp;
        
        if(m_pNodeHead)
          m_pNodeHead->pPrev = NULL;
      }
      else if(nIndex == (m_dwSize - 1))// else the tail
      {
        CNode *pTmp = m_pNodeTail;
        m_pNodeTail = m_pNodeTail->pPrev;
        
        delete pTmp->data;
        delete pTmp;

        if(m_pNodeTail)
          m_pNodeTail->pNext = NULL;
      }
      else							// or any other item
      {
        CNode *pTmp = m_pNodeHead;
        
        for(DWORD i = 0; i < nIndex; i++)
        {
          pTmp = pTmp->pNext;
        }
        if(pTmp)
        {
          pTmp->pPrev->pNext = pTmp->pNext;
          pTmp->pNext->pPrev = pTmp->pPrev;
          delete pTmp->data;
          delete pTmp;     
        }
      }

      m_dwSize--;
      return true;
    }

    return false;
  }
};


/*
    class SkinButton : public SkinObject { ... };
    
    CDynArray< SkinButton > m_Buttons;
    
    SkinButton *tmp = new SkinButton;

    m_Buttons.Add(tmp);

    DWORD size = m_Buttons.GetSize();
    for(DWORD i = 0; i < size; i++)
    (m_Buttons[i])->Enable(wParam);
    
    m_Buttons.RemoveAll();        
*/

#endif // _DYNARRAY_H_