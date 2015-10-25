#include "StdAfx.h"
#define WINTRUST_H 
#include "imagehlp.h"

DWORD RVA2RawAdr(IMAGE_SECTION_HEADER* pSections, DWORD dwRVA)
{
    DWORD dwRawRVAAdr(0);
	
    if(!pSections)
    {
        return dwRawRVAAdr;
    }
	
    while(pSections->VirtualAddress != 0)
    {
        if(dwRVA >= pSections->VirtualAddress &&
			dwRVA < pSections->VirtualAddress + pSections->SizeOfRawData)
        {
            dwRawRVAAdr = (dwRVA - pSections->VirtualAddress) + pSections->PointerToRawData;
            break;
        }
		pSections++;
    }
	
    return dwRawRVAAdr;
}

u32 CHookFunction_i::GetExportFunctionInfo(CArray<EXPORT_FUN_INFO, EXPORT_FUN_INFO&>& InfoArray, 
                                           const char* pszDllFile)
{
    if(!pszDllFile)
    {
        return 0;
    }
	  
    PLOADED_IMAGE pImage = ::ImageLoad((char*)pszDllFile, NULL);
    if(!pImage)
    {
        return 0;
    }
	
    DWORD dwBase = (DWORD)pImage->MappedAddress;
    PIMAGE_SECTION_HEADER pSections    = (PIMAGE_SECTION_HEADER)((DWORD)pImage->FileHeader + sizeof(IMAGE_NT_HEADERS));
    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)(RVA2RawAdr(pSections, pImage->FileHeader->OptionalHeader.DataDirectory[0].VirtualAddress) + dwBase);
	
    InfoArray.RemoveAll();
	  
    EXPORT_FUN_INFO info;
	
    DWORD* pFunOffsetAdr = (DWORD*)(RVA2RawAdr(pSections, pExportDir->AddressOfFunctions) + dwBase);
    for(DWORD i = 0; i < pExportDir->NumberOfFunctions; i++)
    {
        info.dwOffset = 0;
        info.strFunName = "";
        info.dwOrdinal = 0;
        
        info.dwOffset = *(pFunOffsetAdr + i);
        info.dwOrdinal =  i + 1;
        InfoArray.Add(info);
    }
	
    DWORD* pFunNamesAdr       = (DWORD*)(RVA2RawAdr(pSections, pExportDir->AddressOfNames) + dwBase);
    WORD* pFunNamesOrdinalAdr = (WORD*)(RVA2RawAdr(pSections, pExportDir->AddressOfNameOrdinals) + dwBase);
	  
    char* pName = NULL;
    for(i = 0; i < pExportDir->NumberOfNames; i++)
    {
        pName = (char*)(RVA2RawAdr(pSections, *(pFunNamesAdr+i)) + dwBase);
        int idx = *(pFunNamesOrdinalAdr+i);
        if(idx < InfoArray.GetSize())
        {
          EXPORT_FUN_INFO& tmpInfo = InfoArray.ElementAt(idx);
          tmpInfo.strFunName = pName;
        }
    }
	  
    ImageUnload(pImage);
    
    return InfoArray.GetSize();
}

// CString strDllFile(_T("E:\\DevTools\\VC6\\VC6.0.Full.G\\VC98\\Bin\\C1.DLL")); 
u32 CHookFunction_i::ViewExportFunctionInfo(LPCTSTR pszDllFile, LPCTSTR pszOutFile, bool bAutoOpenFile) // _T("core32@20D.dll")
{
  u32 ret = ERR_OK_I;
  
  CArray<EXPORT_FUN_INFO, EXPORT_FUN_INFO&> InfoArray;
  
#ifdef HAVE_UNICODE_I 
  CHAR file[MAX_PATH] = {0, };
  CStringUtl::wcs2mbsz(file, wcslen(pszDllFile), pszDllFile);
  if(GetExportFunctionInfo(InfoArray, file) > 0)
#else
    if(GetExportFunctionInfo(InfoArray, pszDllFile) > 0)
#endif
    {
      CFile fileTmp;
      if(fileTmp.Open(pszOutFile, CFile::modeCreate | CFile::modeReadWrite))
      {
        CString strTmp;
        strTmp.Format("%s Export Information:", pszDllFile);
        fileTmp.Write(strTmp, strTmp.GetLength());
        
        for (int i=0;i < InfoArray.GetSize();i++)
        {
          EXPORT_FUN_INFO& r = InfoArray.ElementAt(i); // return the reference.
          
          if(r.dwOffset == 0)
            continue;
          
          strTmp.Format("\r\n%3d   0x%p   %s", r.dwOrdinal, r.dwOffset, r.strFunName);
          fileTmp.Write(strTmp, strTmp.GetLength());
        }
        
        fileTmp.Close();
        if(bAutoOpenFile)
        {
          ShellExecute(NULL, "open", "notepad.exe", pszOutFile, NULL, SW_NORMAL);
        }
      }
    }
    
    return ret;
}
