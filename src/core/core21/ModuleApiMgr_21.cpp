
#include "stdafx.h"
#include "ModuleAPIMgr.h"

APIMgr g_APIMgr;

//************************************
// Method:    APIMgr
// FullName:  APIMgr::APIMgr
// Access:    public 
// Returns:   
// Qualifier:
//************************************
APIMgr::APIMgr()
{}

//************************************
// Method:    ~APIMgr
// FullName:  APIMgr::~APIMgr
// Access:    public 
// Returns:   
// Qualifier:
//************************************
APIMgr::~APIMgr()
{
  // Clear module list
  FreeModules();
}


//************************************
// Method:    LoadModule
// FullName:  APIMgr::LoadModule
// Access:    public 
// Returns:   HMODULE
// Qualifier:
// Parameter: LPCTSTR lpctszModuleName_i
//************************************
HMODULE APIMgr::LoadModule( LPCTSTR lpctszModuleName_i )
{
  HMODULE hMod = LoadLibrary( lpctszModuleName_i );
  if( !hMod )
  {
    return 0;
  }
  
  // Store module
  m_ModuleHandleMap[lpctszModuleName_i] = hMod;
  return hMod;
}


//************************************
// Method:    FreeModules
// FullName:  APIMgr::FreeModules
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void APIMgr::FreeModules()
{
  POSITION Pos = m_ModuleHandleMap.GetStartPosition();
  while( Pos )
  {
    // Key, Value vars
    CString csKey;
    HMODULE hMod = 0;
    
    // Get module at position and free
    m_ModuleHandleMap.GetNextAssoc( Pos, csKey, hMod );
    if( hMod )
    {
      FreeLibrary( hMod );
    }
  }// End while
  
  // Clear module and function pointer map
  m_ModuleHandleMap.RemoveAll();
  m_FuncPtrMap.RemoveAll();
}// End FreeModules


//************************************
// Method:    GetProcAddress
// FullName:  APIMgr::GetProcAddress
// Access:    public 
// Returns:   FARPROC
// Qualifier:
// Parameter: LPCSTR lpcszProcName_i
//************************************
FARPROC APIMgr::GetProcAddress( LPCSTR lpcszProcName_i, LPCTSTR lpctszModuleName_i )
{
  //USES_CONVERSION;
  
  // Convert to unicode format
  LPCTSTR lpctszProcName = (LPCTSTR)(lpcszProcName_i);
  
  FARPROC ProcAddr = FindProcPtrByName( lpctszProcName );
  if( ProcAddr )
  {
    return ProcAddr;
  }
  
  HMODULE hMod = 0;
  if( m_ModuleHandleMap.Lookup( lpctszModuleName_i, hMod ) == FALSE )
  {
    hMod = LoadModule( lpctszModuleName_i );
    
    // Try loading the module
    if( !hMod )
    {
      return 0;
    }
  }// End if
  
  ProcAddr = ::GetProcAddress( hMod, lpcszProcName_i );
  if( !ProcAddr )
  {
    return 0;
  }
  
  // Store procedure address
  m_FuncPtrMap[lpctszProcName] = ProcAddr;
  
  return ProcAddr;
}// End GetProcAddress



#if 0 
tst()
{
  // Typedefs for not so common functions
  typedef BOOL ( WINAPI * TD_IsAppThemed )( VOID );
  typedef int  ( WINAPI * TD_StrCmpLogicalW )( LPCWSTR, LPCWSTR );
  
  //typedef CMap<CString, LPCTSTR, class CPrivilege, const CPrivilege& > PrivilegeMap;
  TD_StrCmpLogicalW m_StrCmpLogicalWFuncPtr = ( TD_StrCmpLogicalW )g_APIMgr.GetProcAddress( "StrCmpLogicalW", _T( "ShlWApi.dll" ));
  const TD_IsAppThemed IsAppThemedPtr = g_APIMgr.GetProcAddress( "IsAppThemed", _T( "UxTheme.dll" ));
}

BOOL GetInstallPath(char szInstallPath[])
{
  DWORD dwType = REG_SZ;
  DWORD dwBuf = MAX_PATH-1;
  
  DWORD ret = SHGetValue(HKEY_CURRENT_USER, "Software\\CoolStudio\\ide", "install_path", 
    &dwType,(LPVOID)szInstallPath, &dwBuf);
  //SHSetValue(HKEY_CURRENT_USER, "Software\\CoolStudio\\ide", "install_path", dwType, "C:\\CoolStudio\\test", dwBuf);
  
  return ((ret == ERROR_SUCCESS) ? TRUE : FALSE);
}

BOOL SetEnvironmentVariable()
{
  #define BUFSIZE 512
  LPTSTR lpszOldValue;
  TCHAR tchBuf[BUFSIZE];
  TCHAR tchSystemDir[256];
  TCHAR tchWindowsDir[256];
  BOOL fSuccess;
  PCSTR pszInstallPath = UTL_GetInstallPath();
  
  //SCString str = pszInstallPath;
  
  // lpszOldValue gets current value of "varname", or NULL if "varname" 
  // environment variable does not exist. Set "varname" to new value, 
  // create child process, then use SetEnvironmentVariable to restore 
  // original value of "varname". If lpszOldValue is NULL, the "varname" 
  // variable will be deleted.
  
  lpszOldValue = ((GetEnvironmentVariable("Path", tchBuf, BUFSIZE) > 0) ? tchBuf : NULL);
  
  tchSystemDir[0]  = '\0';
  tchWindowsDir[0] = '\0';

  GetWindowsDirectory(tchWindowsDir, sizeof(tchWindowsDir));
  GetSystemDirectory(tchSystemDir, sizeof(tchSystemDir));
  
  tchBuf[0] = '\0';
  strcpy(tchBuf, pszInstallPath);
  strcat(tchBuf, DEFAULT_COMPILER_BIN_PATH);strcat(tchBuf, "; ");  
  strcat(tchBuf, DEFAULT_JADE_COMPILER_BIN_PATH);strcat(tchBuf, "; ");  
  strcat(tchBuf, tchSystemDir);strcat(tchBuf, "; ");
  strcat(tchBuf, tchWindowsDir);
  
  // Set a value for the child process to inherit.
  if (!SetEnvironmentVariable("Path", tchBuf))
  {
    return FALSE;
  }
  
#if 0
  if(CheckCygwinRunning())
    KillCygwinProcess();

  if(CheckCygwinRegistered())
    RegDisableCygwin();
#endif // #if 0

#if 0
  lpszOldValue = ((GetEnvironmentVariable("Path", tchWindowsDir, BUFSIZE) > 0) ? tchWindowsDir : NULL);
  WinExec("mips-elf-gcc.exe C:\\CoolStudio\\crosscompiler\\bin\\1.c", SW_SHOW);
#endif // #if 0
  
  return TRUE;
}
#endif 
