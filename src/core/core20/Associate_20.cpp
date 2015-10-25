
#include "common_20.h"
#include <htmlhelp.h>

#if 0 
//
// AssociateFileIcon_2("prj", "C:\\eside\\etools\\bin\\VCProject.dll,-1", 
// "VC or eSide Project File", "C:\\eSide\\etools\\bin\\PrjDlg.exe");
//
//   .lod
//   str = GetValidFilePath("bin\\VCProject.dll"); str += ",-11"; 
//   tmp = GetValidFilePath("bin\\CodeLoader.exe");
//   AssociateFileIcon("lod", str, "CoolStudio Target Image File", tmp);
// 
BOOL CFileAssoc_i::AssociateFileIcon(LPCTSTR pszFileEx,       // "dsp"
                          LPCTSTR pszIconFile,       //".\\bin\\VCProject.dll,-10"
                          LPCTSTR pszDescription,    // "VC Project File"
                          LPCTSTR pszAppToOpen)      // Can NULL if necessary.  .exe 
{
  HKEY phKey = NULL;
  CString sApp, sExtName;
  
  sApp = pszFileEx;
  sApp += "file";
  sExtName = ".";
  sExtName += pszFileEx;
  
  // .dsp
  RegCreateKey(HKEY_CLASSES_ROOT, sApp /*"dspapp"*/, &phKey);
  if(phKey)
  {
    RegSetValue(phKey, "", REG_SZ,pszDescription /*"eSide or VC Project File"*/, 0);
    RegCloseKey(phKey);
  }
  
  RegCreateKey(HKEY_CLASSES_ROOT, sExtName/*".dsp"*/, &phKey);
  if(phKey)
  {
    RegSetValue(phKey, "", REG_SZ, sApp /*"dspapp"*/, 0);
    RegCloseKey(phKey);
  }
  
  RegCreateKey(HKEY_CLASSES_ROOT,sApp /*"dspapp"*/, &phKey);
  if(phKey)
  {
    if(pszAppToOpen)
    {
      CString app;
      app = pszAppToOpen; 
      app += " %1";
      RegSetValue(phKey, "shell\\open\\command", REG_SZ, app /*"C:\\CoolStudio\\bin\\eSide.exe %1"*/, MAX_PATH);
    }
    RegSetValue(phKey, "DefaultIcon", REG_SZ, pszIconFile/*"C:\\CoolStudio\\bin\\VCProject.dll,-10"*/, MAX_PATH);
    RegCloseKey(phKey);
  }
  
  return TRUE;
}

#endif 
