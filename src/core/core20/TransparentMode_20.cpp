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

DWORD SetWindowTransparentMode_i(HWND hwnd, INT iAlphaPercent)
{
  WNDPROC fp = NULL;
  //int  iAlphaPercent;
  BYTE bAlpha = 0;

  if (iAlphaPercent != 0)
  {
    fp = (WNDPROC)GetProcAddress((HMODULE)GetModuleHandle(_T("User32")), ("SetLayeredWindowAttributes"));
    if (fp) 
    {
      SetWindowLong(hwnd,GWL_EXSTYLE, GetWindowLong(hwnd,GWL_EXSTYLE) | /*WS_EX_LAYERED*/0x00080000);

      // get opacity level
      if (iAlphaPercent < 0 || iAlphaPercent > 100)
        iAlphaPercent = 75;
      bAlpha = (BYTE)(iAlphaPercent * 255 / 100);
     
      fp(hwnd,0,bAlpha,/*LWA_ALPHA*/0x00000002);
    }
    else
      return 1;
  }
  else
  {
    SetWindowLong(hwnd,GWL_EXSTYLE, GetWindowLong(hwnd,GWL_EXSTYLE) & ~/*WS_EX_LAYERED*/0x00080000);
		//::SetWindowLong(hwnd, GWL_EXSTYLE,  ::GetWindowLong(hwnd, GWL_EXSTYLE) & ~/*WS_EX_LAYERED*/0x00080000);
  }

  return 0;
}
