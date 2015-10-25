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


#include "stdafx.h"
//#include <afxdllx.h>
#include "../core20/common_20.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef HAVE_DLL_INIT_ONCE
static AFX_EXTENSION_MODULE CoreDLL = { NULL, NULL };
#endif 

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
  {    
		Core20Init(hInstance);

		TRACE0("core3220.dll Initialized!\n");
    
#ifdef HAVE_DLL_INIT_ONCE
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(CoreDLL, hInstance))
			return 0;
    
		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(CoreDLL);
#endif 
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{

#ifdef HAVE_DLL_INIT_ONCE
		// Terminate the library before destructors are called
		AfxTermExtensionModule(CoreDLL);
#endif     

    Core20Exit();

		TRACE0("core3220.dll Terminated!\n");
	}

	return 1;   // ok
}

