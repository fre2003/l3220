////////////////////////////////////////////////////////////////
// MSDN Magazine -- August 2002
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 6.0 on Windows XP.
//

#pragma warning( disable : 4663 4018 4244 4146)

#include "StdAfx.h"
#include "EnumProc_20.h"

//#if(WINVER >= 0x0500)
#define SMTO_NOTIMEOUTIFNOTHUNG 0x0008
//#endif /* WINVER >= 0x0500 */

#pragma comment(lib, "Psapi.lib")

#pragma warning( disable : 4663 4018 )

////////////////////////////////////////////////////////////////
// CProcessIterator - Iterates all processes
//
CProcessIterator::CProcessIterator()
{
  m_pids = NULL;
}

CProcessIterator::~CProcessIterator()
{
  delete [] m_pids;
}

//////////////////
// Get first process: Call EnumProcesses to init array. Return first one.
//
DWORD CProcessIterator::First()
{
  m_current = (DWORD)-1;
  m_count = 0;
  DWORD nalloc = 1024;

  do 
  {
    delete [] m_pids;
    m_pids = new DWORD [nalloc];
    if (EnumProcesses(m_pids, nalloc*sizeof(DWORD), &m_count))
    {
      m_count /= sizeof(DWORD);
      m_current = 1;						 // skip IDLE process
    }
  } while (nalloc <= m_count);
  
  return Next();
}

////////////////////////////////////////////////////////////////
// CProcessModuleIterator - Iterates all modules in a process
//
CProcessModuleIterator::CProcessModuleIterator(DWORD pid)
{
  m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE, pid);
}

CProcessModuleIterator::~CProcessModuleIterator()
{
  CloseHandle(m_hProcess);
  delete [] m_hModules;
}

//////////////////
// Get first module in process. Call EnumProcesseModules to
// initialize entire array, then return first module.
//
HMODULE CProcessModuleIterator::First()
{
  m_count = 0;
  m_current = (DWORD)-1; 
  m_hModules = NULL;
  
  if (m_hProcess) 
  {
    DWORD nalloc = 1024;
    do
    {
      delete [] m_hModules;
      m_hModules = new HMODULE [nalloc];
      if (EnumProcessModules(m_hProcess, m_hModules, nalloc*sizeof(DWORD), &m_count))
      {
        m_count /= sizeof(HMODULE);
        m_current = 0;
      }
    } while (nalloc <= m_count);
  }
  return Next();
}

////////////////////////////////////////////////////////////////
// CWindowIterator - Iterates all top-level windows (::EnumWindows)
//
CWindowIterator::CWindowIterator(DWORD nAlloc)
: m_current(0), m_count(0)
{
  ASSERT(nAlloc>0);
  m_hwnds = new HWND [nAlloc];
  m_nAlloc = nAlloc;
}

CWindowIterator::~CWindowIterator()
{
  delete [] m_hwnds;
}

// callback passes to virtual fn
BOOL CALLBACK CWindowIterator::EnumProc(HWND hwnd, LPARAM lp)
{
  return ((CWindowIterator*)lp)->OnEnumProc(hwnd);
}

//////////////////
// Virtual enumerator proc: add HWND to array if OnWindows is TRUE.
// You can override OnWindow to filter windows however you like.
//
BOOL CWindowIterator::OnEnumProc(HWND hwnd)
{
  if (OnWindow(hwnd)) {
    if (m_count < m_nAlloc)
      m_hwnds[m_count++] = hwnd;
  }
  return TRUE; // keep looking
}

//////////////////////////////////////////////////////////////////////////
// CMainWindowIterator - Iterates the main windows of a process.
// Overrides CWindowIterator::OnWindow to filter.
//////////////////////////////////////////////////////////////////////////
CMainWindowIterator::CMainWindowIterator(DWORD pid, BOOL bVis,
                                         DWORD nAlloc) :
 CWindowIterator(nAlloc), m_pid(pid), m_bVisible(bVis)
{
}

CMainWindowIterator::~CMainWindowIterator()
{
}

//////////////////
// OnWindow:: Is window's process ID the one i'm looking for?
// Set m_bVisible=FALSE to find invisible windows too.
//
BOOL CMainWindowIterator::OnWindow(HWND hwnd)
{
  if (!m_bVisible || (GetWindowLong(hwnd,GWL_STYLE) & WS_VISIBLE)) {
    DWORD pidwin;
    GetWindowThreadProcessId(hwnd, &pidwin);
    if (pidwin==m_pid)
      return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////
// CFindKillProcess - to find/kill a process by module name.
//
CFindKillProcess::CFindKillProcess()
{
}

CFindKillProcess::~CFindKillProcess()
{
}

//////////////////
// Search for process whose module name matches parameter.
//
DWORD CFindKillProcess::FindProcess(LPCTSTR modname)
{
  CProcessIterator itp;
    
  for (DWORD pid=itp.First(); pid; pid=itp.Next()) 
  {
    TCHAR name[MAX_PATH];
    name[0] = _T('\0');
    CProcessModuleIterator itm(pid);
    HMODULE hModule = itm.First(); // .EXE
    if (hModule) 
    {
      GetModuleBaseName(itm.GetProcessHandle(), hModule, name, MAX_PATH);
      
      if (lstrcmpi(modname,name)==0)
        return pid;
    }
  }
  
  return 0;
}


DWORD CFindKillProcess::FindProcess(LPCTSTR modname, OUT CDWordArray& dwArrayPid)
{
  CProcessIterator itp;
  
  dwArrayPid.RemoveAll();

  for (DWORD pid=itp.First(); pid; pid=itp.Next()) 
  {
    TCHAR name[MAX_PATH];
    name[0] = _T('\0');
    CProcessModuleIterator itm(pid);
    HMODULE hModule = itm.First(); // .EXE
    if (hModule) 
    {
      GetModuleBaseName(itm.GetProcessHandle(), hModule, name, MAX_PATH);
      
      if (lstrcmpi(modname,name)==0)
      {
        dwArrayPid.Add(pid);
      }
    }
  }
  
  return 0;
}

DWORD CFindKillProcess::FindFullPathProcess(LPCTSTR modname, OUT CDWordArray& dwArrayPid)
{
  CProcessIterator itp;
    
  for (DWORD pid=itp.First(); pid; pid=itp.Next()) 
  {
    TCHAR name[MAX_PATH];

    name[0] = _T('\0');
    CProcessModuleIterator itm(pid);
    HMODULE hModule = itm.First(); // .EXE
    if (hModule) 
    {
      GetModuleFileNameEx(itm.GetProcessHandle(),hModule, name, MAX_PATH);
      if (lstrcmpi(modname,name)==0)
      {
        dwArrayPid.Add(pid);
      }
    }
  }
  
  return 0;
}


DWORD CFindKillProcess::FindFullPathProcess(LPCTSTR modname)
{
  CProcessIterator itp;
    
  for (DWORD pid=itp.First(); pid; pid=itp.Next()) 
  {
    TCHAR name[MAX_PATH];
    name[0] = _T('\0');
    CProcessModuleIterator itm(pid);
    HMODULE hModule = itm.First(); // .EXE
    if (hModule) 
    {
      GetModuleFileNameEx(itm.GetProcessHandle(),hModule, name, MAX_PATH);
      if (lstrcmpi(modname,name)==0)
      {
        return pid;
      }
    }
  }
  
  return 0;
}

HANDLE CFindKillProcess::FindProcessHandle(LPCTSTR modname)
{
  CProcessIterator itp;
  //HANDLE hProcess= 0;
  //BOOL bAddExe = FALSE;
    
  for (DWORD pid=itp.First(); pid; pid=itp.Next())
  {
    TCHAR name[MAX_PATH];
    name[0] = _T('\0');
    CProcessModuleIterator itm(pid);
    HMODULE hModule = itm.First(); // .EXE
    if (hModule)
    {
      GetModuleBaseName(itm.GetProcessHandle(), hModule, name, MAX_PATH);
      if (lstrcmpi(modname,name)==0)
        return itm.GetProcessHandle();
    }
  }
  
  return 0;
}

HANDLE CFindKillProcess::FindFullPathProcessHandle(LPCTSTR modname)
{
  CProcessIterator itp;
 // HANDLE hProcess= 0;
  //BOOL bAddExe = FALSE;
    
  for (DWORD pid=itp.First(); pid; pid=itp.Next())
  {
    TCHAR name[MAX_PATH];
    name[0] = _T('\0');
    CProcessModuleIterator itm(pid);
    HMODULE hModule = itm.First(); // .EXE
    if (hModule)
    {
      GetModuleFileNameEx(itm.GetProcessHandle(), hModule, name, MAX_PATH);
      if (lstrcmpi(modname,name)==0)
        return itm.GetProcessHandle();
    }
  }
  
  return 0;
}


DWORD CFindKillProcess::EnumProcess(CDWordArray& dwPidArray, CStringArray& strFullPathArray)
{
  CProcessIterator itp;
    
  for (DWORD pid=itp.First(); pid; pid=itp.Next()) 
  {
    TCHAR name[MAX_PATH];
    name[0] = _T('\0');
    CProcessModuleIterator itm(pid);
    HMODULE hModule = itm.First();
    if (hModule) 
    {
      GetModuleFileNameEx(itm.GetProcessHandle(), hModule, name, MAX_PATH); 
      dwPidArray.Add(pid);
      strFullPathArray.Add(name);
    }
  }
  
  return 0;
}

//
// Kill a process cleanly: Close main windows and wait.
// bZap=TRUE to force kill.
//
BOOL CFindKillProcess::KillProcess(DWORD pid, BOOL bZap, UINT32 nTimeOut)
{
  CMainWindowIterator itw(pid);
  
  for (HWND hwnd = itw.First(); hwnd; hwnd=itw.Next()) 
  {
    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);
    ShowWindow(hwnd, SW_RESTORE);
    
    DWORD bOKToKill = FALSE;
    SendMessageTimeout(hwnd, WM_QUERYENDSESSION, 0, 0, SMTO_ABORTIFHUNG|SMTO_NOTIMEOUTIFNOTHUNG, nTimeOut, &bOKToKill);
    if (!bOKToKill)
      return FALSE;  // window doesn't want to die: abort 
    PostMessage(hwnd, WM_CLOSE, 0, 0);
  }
  
  // I've closed the main windows; now wait for process to die. 
  BOOL bKilled = TRUE;
  HANDLE hp = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,FALSE,pid);
  if (hp)
  {
    if (WaitForSingleObject(hp, nTimeOut) != WAIT_OBJECT_0)
    {
      if (bZap)
      {
        DWORD nExitCode = 0;
        GetExitCodeProcess(hp, &nExitCode);
        // didn't die: force kill it if zap requested
        BOOL bRet = TerminateProcess(hp, nExitCode); //PROCESS_QUERY_INFORMATION
        if(!bRet)
        {
          bKilled = FALSE;
        }
      }
      else
      {
        bKilled = FALSE;
      }
    }
    CloseHandle(hp);
  }
  
  return bKilled;
}

BOOL IsFullPath(LPCTSTR pszFile)
{
  CString str = PathSplitLast_i(CString(pszFile));
  if(!str.IsEmpty())
    return TRUE;
  else 
    return FALSE;
}

U32 CProcessUtil_i::IsProcessRunning(LPCTSTR pszProcFile)
{
  CFindKillProcess fkp;
  DWORD pid = 0; 

  if(pszProcFile == NULL)
    return 0; 
  
  if(IsFullPath(pszProcFile))
    pid = fkp.FindFullPathProcess(pszProcFile);
  else
    pid = fkp.FindProcess(pszProcFile);

  if (pid) 
  {
    return pid;
  }
  else
    return 0;
}

BOOL KillOneProcess(DWORD pid, DWORD dwTimeOut) // like a.exe
{
  CFindKillProcess fkp;

  if (pid)
  {
      if(fkp.KillProcess(pid, TRUE, dwTimeOut)) // INFINITE force to kill it
        return TRUE;
  }
  
  return FALSE;
}

bool CProcessUtil_i::KillProcess(U32 ulPid, DWORD dwTimeOut/*500*/)
{
    return KillOneProcess(ulPid, dwTimeOut);
}

bool CProcessUtil_i::KillProcess(LPCTSTR pszProcFile, DWORD dwTimeOut/*500*/)
{
  int cnt = 0;
  DWORD pid = 0;

  pid = IsProcessRunning(pszProcFile);
  while(pid)
  {
    KillOneProcess(pid, dwTimeOut);
    Sleep(0);

    if(cnt++ > 5)
    {
      cnt = 0;

      return FALSE;

#if  0
      MessageBox(NULL, "", "You SHOULD Kill '%s' manually in 'Windows Task Manager'('Processes' tab) before running Emulator.",pszName,pszName);
      ShellExecute(NULL, "open", "taskmgr /svc", NULL, NULL, SW_NORMAL);
      Sleep(2000);
#endif 
    }
    pid = IsProcessRunning(pszProcFile);
  }
  
  return TRUE;
}


DWORD CProcessUtil_i::FindProcessId(LPCTSTR pszProcFileName)
{
  CFindKillProcess findproc;
  DWORD dwPid = 0;
   
  if(IsFullPath(pszProcFileName))
    dwPid = findproc.FindFullPathProcess(pszProcFileName);
  else
    dwPid = findproc.FindProcess(pszProcFileName);
  
  return dwPid;
}

DWORD CProcessUtil_i::FindProcessId(LPCTSTR pszProcFileName, OUT CDWordArray& dwArrayPid)
{
  CFindKillProcess findproc;
  DWORD ret = 0;
  
  if(IsFullPath(pszProcFileName))
    ret = findproc.FindFullPathProcess(pszProcFileName, dwArrayPid);
  else
    ret = findproc.FindProcess(pszProcFileName, dwArrayPid);
  
  return ret;
}

HANDLE CProcessUtil_i::FindProcessHandle(LPCTSTR pszProcFileName, OUT CDWordArray& dwArrayHandle)
{
  CFindKillProcess findproc;
  HANDLE hProcess = 0;
  CDWordArray dwPidArray;

  FindProcessId(pszProcFileName, dwPidArray);
  for (int i = 0; i< dwPidArray.GetSize(); i++)
  {
    DWORD pid = dwPidArray.GetAt(i);
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid); 
    dwArrayHandle.Add((DWORD)hProcess);
  }
  
  return hProcess;
}

HANDLE CProcessUtil_i::FindProcessHandle(LPCTSTR pszProcFileName)
{
  CFindKillProcess findproc;
  HANDLE hProcess = 0;
  
  DWORD pid = FindProcessId(pszProcFileName);
  if(pid)
  {
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid); 
  }
  
  return hProcess;
}

DWORD CProcessUtil_i::EnumProcess(CDWordArray& dwPidArray, CStringArray& strFullPathArray)
{
  CFindKillProcess findproc;

  return  findproc.EnumProcess(dwPidArray, strFullPathArray);
}