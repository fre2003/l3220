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
typedef void (__stdcall * IMPORTCALLBACK)(LPSTR lpLibrary, 
							  LPSTR lpFunction,
							  u32	ordinal,
							  u32   dwRVA);

typedef struct
{
	char*	lpLibrary;		// DLL name
	char*	lpFunction;		// function name
	u32		ordinal;		  // Ordinal of function
	u32		RVA;			    // RVA of entry in FirstThunk table
} API_HOOK_INFO;


API_HOOK_INFO hookTable[] = 
{
	{"ws2_32.dll","socket",0x17,0},
	{"ws2_32.dll","recv",  0x10,0},
	{"ws2_32.dll","send",  0x13,0},
	{"ws2_32.dll","closesocket",0x03,0},
	{"ws2_32.dll","accept",0x01,0},
	{"ws2_32.dll","WSASocketA",0x4E,0},
	{"ws2_32.dll","WSASocketW",0x4F,0},
	{"ws2_32.dll","WSASend",0x48,0},
	{"ws2_32.dll","WSAAccept",0x1A,0},
	{"ws2_32.dll","WSARecv",0x43,0},
	
	{"wsock32.dll","accept",0x01,0},
	{"wsock32.dll","closesocket",0x03,0},
	{"wsock32.dll","recv",0x10,0},
	{"wsock32.dll","send",0x13,0},
	{"wsock32.dll","socket",0x17,0}
};

void CALLBACK ImportCallBack(LPSTR lpLibrary, LPSTR lpFunction, u32 ordinal, u32 dwRVA)
{
  int i;
    
  for (i=0;i<arrcount(hookTable);i++)
  {
    if (lpFunction!=0) // import by name
    {
      // See if name & DLL name matches
      if (lstrcmp(hookTable[i].lpFunction, lpFunction)==0 &&
        lstrcmpi(hookTable[i].lpLibrary, lpLibrary)==0)
      {
        // We got a match
        //		printf("match: %s\n", lpFunction);
        hookTable[i].RVA = dwRVA;
      }
    }
    else	// import by ordinal
    {
      if (hookTable[i].ordinal==ordinal &&
        lstrcmpi(hookTable[i].lpLibrary, lpLibrary)==0)
      {
        // We got a match
        //	printf("match: 0x%X\n", ordinal);
        hookTable[i].RVA = dwRVA;
        
      }
    }
  }
}

bool EnumImports(LPSTR lpImageFile,
                 IMPORTCALLBACK lpCallBackFunc)
{
  HANDLE	hFile, hMap;
  void	*pMap, *curPtr;
  
  // Load DLL
  if (!(hFile = CreateFile(lpImageFile,GENERIC_READ,
				FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,NULL,NULL)))
        return false;
  
  // Create file mapping
  if (!(hMap = CreateFileMapping(hFile,NULL,PAGE_READONLY|SEC_COMMIT,
				0,0,NULL)))
  {
    CloseHandle(hFile);
    return false;
  }
  
  if(!(pMap = MapViewOfFile(hMap,FILE_MAP_READ,0,0,0)))
  {
    CloseHandle(hFile);
    CloseHandle(hMap);
    return false;
  }
  
  curPtr = pMap;
  
  if (((IMAGE_DOS_HEADER*)pMap)->e_magic!=IMAGE_DOS_SIGNATURE)
  {
    MessageBeep(0);
  }
  
  curPtr = (void*)((long)curPtr + ((IMAGE_DOS_HEADER*)pMap)->e_lfanew);
  
  if (((IMAGE_NT_HEADERS*)curPtr)->Signature!=IMAGE_NT_SIGNATURE)
  {
    MessageBeep(0);
  }
  
  //	curPtr points to NT header
  
  IMAGE_IMPORT_DESCRIPTOR *pImportDescriptor;
  
  pImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)
    RVAToOffset(pMap,
    ((IMAGE_NT_HEADERS*)curPtr)->OptionalHeader.DataDirectory[1].VirtualAddress);
  if (pImportDescriptor==0)
  {
    UnmapViewOfFile(pMap);
    CloseHandle(hMap);
    CloseHandle(hFile);
    return true;  // no import table, no imports 
  }
  
  pImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)((long)pImportDescriptor + (long)pMap);
  
  
  while(!(pImportDescriptor->OriginalFirstThunk==0 &&
    pImportDescriptor->TimeDateStamp==0 &&
    pImportDescriptor->ForwarderChain==0 &&
    pImportDescriptor->Name==0 &&
    pImportDescriptor->FirstThunk==0))
  {
    //DLL name:
    char *curDLLName = (char*)((long)pMap + (long)RVAToOffset(pMap, pImportDescriptor->Name));
    PIMAGE_THUNK_DATA pThunkData = (pImportDescriptor->OriginalFirstThunk==0) ?
						(PIMAGE_THUNK_DATA)pImportDescriptor->FirstThunk :
    (PIMAGE_THUNK_DATA)pImportDescriptor->OriginalFirstThunk;
    
    pThunkData = (PIMAGE_THUNK_DATA)
					 (RVAToOffset(pMap,(long)pThunkData) + (long)pMap);
    
    
    // curImportRVA is the RVA of the DWORD that contains the
    // address of the imported function when the image is loaded.
    
    u32 curImportRVA = pImportDescriptor->FirstThunk;
    
    
    while(pThunkData->u1.Ordinal!=0)
    {
      if (pThunkData->u1.Ordinal&IMAGE_ORDINAL_FLAG32)
      {
        //import by ordinal
        //ordinal is 
        lpCallBackFunc(curDLLName, NULL, pThunkData->u1.Ordinal&(0xFFFF),curImportRVA);
      }
      else
      {
        char *funcName = (char*)((long)pMap + RVAToOffset(pMap,(long)((PIMAGE_IMPORT_BY_NAME)(pThunkData->u1.AddressOfData))->Name));
        lpCallBackFunc(curDLLName, funcName, 0,curImportRVA);
        
        //MessageBox(0,(char*)
        //	((long)pMap + RVAToOffset(pMap,(long)pThunkData->u1.AddressOfData->Name)),0,0);
      }
      pThunkData++;
      
      curImportRVA += 4;
    }
    
    pImportDescriptor++;
  }
  
  
  
  UnmapViewOfFile(pMap);
  CloseHandle(hMap);
  
  CloseHandle(hFile);
  
  
  return true;
}

#endif 
