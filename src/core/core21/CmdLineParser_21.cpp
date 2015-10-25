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

#pragma comment(exestr, "CORE32" "Compiled on " __DATE__ " at " __TIME__)

///////////////////////////////////////////////////////////////////////////////
// 
// METHOD: CStringToken_i(CString str, CString sep) 
//         CStringToken_i Constructor 
// PARAMETERS:
//   str [in] string type that to be tokenized.
//   sep [in] string type that is used as token seperator and default seperator is space
// 
// RETURN:
//   TODO...
// 
// REMARK:
//   1> TODO...
//
// EXAMPLE:
//   TODO...
///////////////////////////////////////////////////////////////////////////////
CStringToken_i::CStringToken_i(CString str, CString sep)
{
  index = 0;
  count = 0;
  
  CString str1 = _T("");
  int seppair  = 0;
  TCHAR sepch = sep.GetAt(0);

  for (int i = 0; i < str.GetLength() && sep.GetLength() == 1; i++)
  {
    TCHAR ch = str.GetAt(i);
 
    if(ch == _T('"'))
    {
      seppair ++;
    }
    
    if ((ch == sepch) && ((seppair&1) == 0))
    {
      seppair = 0;
      if (!str1.IsEmpty())
      {
        elements.Add(str1);
      }
      
      str1 = _T("");
    }
    else
    {
      str1 += ch;
    }
  }
  
  if (!str1.IsEmpty())
  {
    elements.Add(str1);
  }
  
  count = elements.GetSize();
}

///////////////////////////////////////////////////////////////////////////////
// 
// METHOD: CStringToken_i(CString str, CString sep) 
//         CStringToken_i Constructor 
// PARAMETERS:
//   str [in] string type that to be tokenized.
//   sep [in] string type that is used as token seperator and default seperator is space
// 
// RETURN:
//   TODO...
// 
// REMARK:
//   1> TODO...
//
// EXAMPLE:
//   TODO...
///////////////////////////////////////////////////////////////////////////////
CStringToken_i::CStringToken_i(CString str, TCHAR sep)
{
  index = 0;
  count = 0;
  
  CString str1 = _T("");
  int seppair  = 0;
  TCHAR sepch = sep;
  
  if(str.IsEmpty())
  {
    return ;
  }

  for (int i = 0; i < str.GetLength(); i++)
  {
    TCHAR ch = str.GetAt(i);
 
    if(ch == _T('"'))
    {
      seppair ++;
    }
    
    if ((ch == sepch) && ((seppair&1) == 0))
    {
      seppair = 0;
      if (!str1.IsEmpty())
      {
        elements.Add(str1);
      }
      
      str1 = _T("");
    }
    else
    {
      str1 += ch;
    }
  }
  
  if (!str1.IsEmpty())
  {
    elements.Add(str1);
  }
  
  count = elements.GetSize();
}

// Method is used to fetch the tokens.
CString CStringToken_i::GetNextElement()
{
  index++;
  
  if (index > count)
  {
    return CString(_T(""));
  }
  
  return elements.GetAt(index - 1);
}

CString CStringToken_i::GetLastElement(VOID)
{
  if(count >= 1)
  {
    return elements.GetAt(count - 1);
  }
  else 
  {
    return CString(_T(""));
  }
}

// methodused to fetch the count of tokens from the string
int CStringToken_i::GetCount()
{
  return count;
}

// fetchthe elements at given position 
CString CStringToken_i::GetAt(int index)
{
  if (index >= count || index < 0)
  {
    return CString(_T(""));
  }
  else
    return elements.GetAt(index);
}

/*
  CString strTokenIn("/I \"D:\\inc lude\" /Fo\"Debug/\" /arch:arm /GZ /c ");
  //CString strTokenIn("/nologo /Zp4 /MLd /W3 /ZI /I \"D:\\include\" /I \".\\inc\" /D \"ARM\" /D \"DEBUG\" /Fo\"Debug/\" /Fd\"Debug/app.pdb\" /FD /arch:arm /GZ /c ");
  CCmdLineParser_i strToken(strTokenIn, " ");
  CString str;
  
  int cnt = strToken.GetCount();
  for (int i = 0; i< cnt; i++)
  {
    str = strToken.GetAt(i);
    Printf1("%s\n", str);
  }
*/

int CCmdLineParser_i::SplitLine(LPCTSTR pszToken)
{
  m_ArgArray.RemoveAll();
  m_SwitchArray.RemoveAll();
  
  CString strSwitch;
  
  CStringToken_i strToken(pszToken);
  
  int argc = strToken.GetCount();
  
  for (int i = 0; i < argc; i++)
  {
    CString str = strToken.GetAt(i);
    
    // if it's a switch 
    if (IsSwitch(strToken.GetAt(i)))
    {
      strSwitch = strToken.GetAt(i);
      
      CString arg(_T(""));
      int pos = 0;

      // 
      // Support the switch format: '/switch:arg' 
      // 
      if (strSwitch != _T(""))
      {
        pos = strSwitch.Find(_T(':'));
        
        if (pos != -1)
        {
          arg       = strSwitch.Mid(pos + 1);
          strSwitch = strSwitch.Left(pos + 1);
        }
      }

      if(pos == -1)
      {
        // look at next input string to see if it's a switch or an argument
        if (i + 1 < argc)
        {
          if (!IsSwitch(strToken.GetAt(i+1)))
          {
            if(arg == _T(""))
            {
              // it's an argument, not a switch
              arg = strToken.GetAt(i+1);
            
              // skip to next
              i++;
            }
          }
        }
      }
      
      m_ArgArray.Add(arg);
      m_SwitchArray.Add(strSwitch);
    }
    else
    {
      m_ArgArray.Add(str);
      m_SwitchArray.Add(_T(""));
    }
  }
  
  return m_SwitchArray.GetSize();
}


// parsethe command line into switches and arguments
// returnsnumber of switches found
int CCmdLineParser_i::SplitLine(int argc, TCHAR **argv)
{
   m_ArgArray.RemoveAll();
   m_SwitchArray.RemoveAll();
   
   CString strSwitch;
   
   // skip the exe name (start with i = 1)
   for (int i = 1; i < argc; i++)
   {
      //printf("argv[%d]:%s ", i, argv[i]);
      // if it's a switch 
      if (IsSwitch(argv[i]))
      {
         strSwitch = argv[i];
         
         CString arg(_T(""));
         int pos = 0;

         // 
         // Support the format: '/switch:arg' 
         // 
         if (strSwitch != _T(""))
         {
           pos = strSwitch.Find(_T(':'));
           if (pos != -1)
           {
             arg       = strSwitch.Mid(pos + 1);
             strSwitch = strSwitch.Left(pos + 1);
           }
         }

         // look at next input string to see if it's a switch or an argument
         if(pos == -1)
         {
           if (i + 1 < argc)
           {
              if (!IsSwitch(argv[i + 1]))
              {
                if(arg == _T(""))
                {
                  // it's an argument, not a switch
                  arg = argv[i + 1];
                  // skip to next
                  i++;
                }
              }
           }
         }

         m_SwitchArray.Add(strSwitch);
         m_ArgArray.Add(arg);
      }
      else
      {
        m_SwitchArray.Add(_T(""));
        m_ArgArray.Add(argv[i]);
      }
   }

   return m_SwitchArray.GetSize();
}

//  protected member function
//  test a parameter to see if it's a switch :
// 
//  switches are of the form : -x
//  where 'x' is one or more characters.
//  the first character of a switch must be non-numeric!
bool CCmdLineParser_i::IsSwitch(LPCTSTR pszParam)
{
   if (pszParam == NULL)
   {
     return FALSE;
   }
   
   // switches must non-empty
   // must have at least one character after the '-'
   int len = lstrlen(pszParam);
   if (len <= 1)
   {
      return FALSE;
   }

   // switches always start with '-'
   if (pszParam[0]==_T('-') || pszParam[0]==_T('/'))
   {
      // allow negative numbers as arguments.
      // ie., don't count them as switches
      return (!isdigit(pszParam[1]));
   }
   else
   {
      return FALSE;
   }
}

CString CCmdLineParser_i::GetArgument(LPCTSTR pSwitch, LPCTSTR pSwitchReplace,
                                      bool bSwitchAdd, bool bAddQuotationMarks)
{
  CString strArgument(_T(""));
  
  int cnt = m_ArgArray.GetSize();
  
  cnt = m_SwitchArray.GetSize();
  
  for (int i = 0; i < cnt; i++)
  {
    CString str = m_SwitchArray[i];
    
    if (str == pSwitch)
    {
      if (bSwitchAdd)
      {
        CString strSwitch;
        
        if (pSwitchReplace)
        {
          strSwitch = pSwitchReplace;
        }
        else
        {
          strSwitch = pSwitch;
        }
        
        if (bAddQuotationMarks)
        {
          strArgument = strSwitch + _T(" \"") + m_ArgArray[i] + _T("\" ");
        }
        else
        {
          strArgument = strSwitch + m_ArgArray[i] + _T(" ");
        }
        break;
      }
      else
      {
        strArgument = m_ArgArray[i];
        break;
      }
    }
  }
  
  return strArgument;
}


BOOL CCmdLineParser_i::IsValidSwitch(LPCTSTR pszSwitch)
{
  BOOL ret = FALSE;
  
  int cnt = m_SwitchArray.GetSize();
  CString strSwt  = pszSwitch;
  
  strSwt.MakeUpper();
  
  for (int i = 0; i < cnt; i++)
  {
    CString str = m_SwitchArray[i];
    str.MakeUpper();
    if (str == strSwt)
    {
      ret = TRUE;
      break;
    }
  }
  
  return ret;
}


CString CCmdLineParser_i::GetAllArgument(LPCTSTR pSwitch,
                                 LPCTSTR pSwitchReplace,
                                 bool bSwitchAdd,
                                 bool bAddQuotationMarks)
{
  CString strAllArgument(_T(""));
  
  int cnt = m_ArgArray.GetSize();
  
  cnt = m_SwitchArray.GetSize();
  
  for (int i = 0; i < cnt; i++)
  {
    CString str = m_SwitchArray[i];
    
    if (str == pSwitch)
    {
      if (bSwitchAdd)
      {
        CString strSwitch;
        
        if (pSwitchReplace)
        {
          strSwitch = pSwitchReplace;
        }
        else
        {
          strSwitch = pSwitch;
        }
        if (bAddQuotationMarks)
        {
          strAllArgument += strSwitch + _T("\"") + m_ArgArray[i] + _T("\" ");
        }
        else
        {
          strAllArgument += strSwitch + _T(" ") + m_ArgArray[i] + _T(" ");
        }
      }
      else
      {
        strAllArgument += m_ArgArray[i] + _T(" ");
      }
    }
  }
  
  return strAllArgument;
}
