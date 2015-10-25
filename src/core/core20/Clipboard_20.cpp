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

bool CopyTextToClipboard( HWND hWindow_i, LPCTSTR lpctszText_i )
{
  // Open clipboard
  if( !lpctszText_i || !::OpenClipboard( hWindow_i ))
  {
    return false;
  }
  
  // Clear clipboard
  EmptyClipboard();
  
  const size_t nTotalAllocLen = ( _tcslen( lpctszText_i ) + 1 ) * sizeof( TCHAR ) ;
  HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE, nTotalAllocLen );
  if( !hGlobal )
  {
    CloseClipboard();
    return false;
  }
  
  // Lock allocated buffer for copying
  LPTSTR lptszCopyStr = RCAST_I( LPTSTR, GlobalLock( hGlobal ));
  memcpy( lptszCopyStr, lpctszText_i, nTotalAllocLen );
  GlobalUnlock( hGlobal );
  ASSERT( GetLastError() == NO_ERROR );
  
  // Clipboard format
  UINT uClipBoardFormat = 0;
#ifdef _UNICODE
  uClipBoardFormat = CF_UNICODETEXT;
#else
  uClipBoardFormat = CF_TEXT;
#endif
  
  // Set data to clipboard
  HANDLE hClip = SetClipboardData( uClipBoardFormat, hGlobal );
  CloseClipboard();
  
  // Return status
  return ( hClip ? true : false );
}// End CopyTextToClipboard

