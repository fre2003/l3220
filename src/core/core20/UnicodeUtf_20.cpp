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

VOID UTF8ToUnicode(WCHAR* pOut, const char* pText)
{
  char* uchar = (char *)pOut;
  
  uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
  uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
  return;
}

VOID UnicodeToUTF8(char* pOut, WCHAR* pText)
{
  // Low -->High 
  char* pchar = (char *)pText;	
  pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));	
  pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);	
  pOut[2] = (0x80 | (pchar[0] & 0x3F));	
  return; 
} 

void UnicodeToGB2312(char* pOut, unsigned short uData)
{
  WideCharToMultiByte(CP_ACP, NULL,&uData,1, pOut,sizeof(WCHAR),NULL,NULL);
  return;
}

//void ITS_UTF8ToGB2312(CString &pOut, char *pText, int pLen)
BOOL ITS_UTF8ToLocal(PSTR pOut, PCSTR pText, UINT32 nLen)
{
  char * newBuf = new char[nLen+1];
  char Ctemp[4];
  
  memset(newBuf,0,nLen+1);
  memset(Ctemp,0,4);
  int i = 0;
  int j = 0;
  
  while(i < (int)nLen)
  {    
    if(pText[i] > 0)
    {
      newBuf[j++] = pText[i++]; 
    }
    else 
    {
      WCHAR Wtemp;
      UTF8ToUnicode(&Wtemp,pText + i);
      UnicodeToGB2312(Ctemp,Wtemp);
      newBuf[j] = Ctemp[0];
      newBuf[j + 1] = Ctemp[1];
      i += 3; 
      j += 2;
    }
  }
  newBuf[j] = '\0';
  pOut[j] = '\0'; 
  strcpy(pOut, newBuf);
  delete []newBuf;

  return TRUE;
}

BOOL ITS_UTF8ToUnicode(WCHAR* pOut, PCSTR pText, UINT32 nLen)
{
  WCHAR* newBuf = new WCHAR[3*nLen+1];
  
  memset(newBuf,0,nLen+1);
  int i = 0;
  int j = 0;
  
  while(i < (int)nLen)
  {
    if(pText[i] > 0)
    {
      newBuf[j++] = pText[i++]; 
    }
    else 
    {
      WCHAR Wtemp;
      UTF8ToUnicode(&Wtemp, pText + i);
      //UnicodeToGB2312(Ctemp,Wtemp);
      newBuf[j]     = Wtemp;
      //newBuf[j + 1] = Wtemp[1];
      i += 3;
      j += 2;
    }
  }

  newBuf[j] = '\0';
  pOut[j] = '\0';

  wcscpy(pOut, newBuf);
  delete []newBuf;

  return TRUE;
}

BOOL Gb2312ToUnicode(WCHAR* pOut, PCSTR gbBuffer) 
{
  MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);

  return TRUE;
}

BOOL ITS_LocalToUTF8(PSTR pOut, PCSTR pText, UINT32 pLen) 
{
  char buf[4];
  char* rst = new char[pLen + (pLen >> 2) + 2]; 
  
  memset(buf,0,4); 
  memset(rst,0,pLen + (pLen >> 2) + 2); 
  
  UINT32 i = 0; UINT32 j = 0; 
  
  while(i < pLen) 
  {
    //如果是英文直接复制就可以
    if( *(pText + i) >= 0) 
    { 
      rst[j++] = pText[i++]; 
    } 
    else 
    { 
      WCHAR pbuffer;
      Gb2312ToUnicode(&pbuffer, pText+i);
      UnicodeToUTF8(buf,&pbuffer); 
      unsigned short int tmp = 0;
      tmp = rst[j] = buf[0]; 
      tmp = rst[j+1] = buf[1]; 
      tmp = rst[j+2] = buf[2]; 
      j += 3; i += 2; 
    }
  } 
  rst[j] = '\0'; //返回结果 

  strcpy(pOut, rst);
  delete []rst; 

  return TRUE;
} 

BOOL ITS_UnicodeToUTF8(PSTR pOut, WCHAR* pText, UINT32 pLen) 
{
  char buf[4];
  char* rst = new char[2*pLen + (pLen >> 2) + 2]; 
  
  memset(buf,0,4); 
  memset(rst,0,pLen + (pLen >> 2) + 2); 
  
  UINT32 i = 0; UINT32 j = 0; 
  
  while(i < pLen) 
  {
    //如果是英文直接复制就可以
    if( *(pText + i) >= 0) 
    { 
      rst[j++] = (char)pText[i++]; 
    } 
    else 
    { 
      //WCHAR pbuffer;
      //Gb2312ToUnicode(&pbuffer, pText+i);
      UnicodeToUTF8(buf, pText); 
      unsigned short int tmp = 0;
      tmp = rst[j] = buf[0]; 
      tmp = rst[j+1] = buf[1]; 
      tmp = rst[j+2] = buf[2]; 
      j += 3; i += 2; 
    }
  } 
  rst[j] = '\0'; //返回结果 

  strcpy(pOut, rst);
  delete []rst; 

  return TRUE;
} 