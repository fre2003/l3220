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
#include "Dbghelp.h"

#pragma comment(lib , "Dbghelp.lib")

bool SymInit_i(PSTR UserSearchPath)
{
  return SymInitialize(GetCurrentProcess(), UserSearchPath, FALSE);
}

bool SymFromAddr_i(U32 ulAddress, CHAR Name[64])
{
  DWORD Displacement = 0;
  IMAGEHLP_SYMBOL Symbol = {0, };
  Symbol.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
  
  bool ret = SymGetSymFromAddr(GetCurrentProcess(), ulAddress, &Displacement, &Symbol);
  strcpy(Name, Symbol.Name);

  return ret;
}

