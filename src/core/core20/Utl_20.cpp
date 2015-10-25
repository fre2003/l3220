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
int STR_TO_INT(const CString& str)
{
	int ret = 0;
	sscanf(str, "%d", &ret);
	return ret;
}

char buffer[10];
char* INT_TO_STR(int i)
{
	return itoa(i, buffer, 10);
}

const CString GetExeDir()
{
	char bufferName[128] = {0};
	char bufferDir[128] = {0};
	char bufferDriver[128] = {0};
	GetModuleFileName(NULL, bufferName, 128);
	_splitpath(bufferName, bufferDriver, bufferDir, NULL, NULL);
	CString strExeDir = CString::CString(bufferDriver) + bufferDir;
	return strExeDir;
}
#endif 
