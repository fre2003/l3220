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


#if 0 
static bool IsKeyRepeated( const UINT uFlags_i )
{
  // For extracting bit 14
  const UINT uBit14 = SCAST( UINT, 1 ) << 14;
  const bool bRepetition = IsValidMask( uFlags_i, uBit14 );
  
  // True if 14th bit is set
  return bRepetition;
}
static bool IsValidMask( const DWORD dwValue_i, const DWORD dwMask_i )
{
  return ( dwValue_i & dwMask_i ) == dwMask_i;
}


// If given key state is pressed then returns true
static inline bool IsKeyDown( const UINT uKey_i )
{
  return IsValidMask( GetKeyState( uKey_i ), 0x8000 );
}

// Control down
static inline bool IsCtrlDown()
{
  return IsKeyDown( VK_CONTROL );
}

// Alt key state
static inline bool IsAltDOwn()
{
  return IsKeyDown( VK_MENU );
}
#endif 
