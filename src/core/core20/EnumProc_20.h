////////////////////////////////////////////////////////////////
// MSDN Magazine -- August 2002
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 6.0 on Windows XP.
//

#define VC_EXTRALEAN							 // Exclude rarely-used stuff from headers
#include <windows.h>
#include <winuser.h>
#include <psapi.h>

#pragma warning(disable:4786)				 // disable annoying C4786
#include <string>								 // STL string class
#include <list>								 // STL list class

using namespace std;							 // use STL

#pragma once

//////////////////
// Process iterator -- iterator over all system processes
// Always skips the first (IDLE) process with PID=0.
//
class CProcessIterator {
protected:
	DWORD*	m_pids;			// array of procssor IDs
	DWORD		m_count;			// size of array
	DWORD		m_current;		// next array item
	
public:
	CProcessIterator();
	~CProcessIterator();

	DWORD First();
	DWORD Next() {
		return m_pids && m_current < m_count ? m_pids[m_current++] : 0;
	}
	DWORD GetCount() {
		return m_count;
	}
};

//////////////////////////////////////////////////////////////////////////
// Iterate the modules in a process. Note that the first module is the
// main EXE that started the process.
//////////////////////////////////////////////////////////////////////////
class CProcessModuleIterator
{
protected:
  HANDLE	m_hProcess;				// process handle
  HMODULE*	m_hModules;				// array of module handles
  DWORD		m_count;					// size of array
  DWORD		m_current;				// next module handle
  
public:
  CProcessModuleIterator(DWORD pid);
  ~CProcessModuleIterator();
  
  HMODULE First();
  HMODULE Next() 
  {
    return m_hProcess && m_current < m_count ? m_hModules[m_current++] : 0;
  }
  DWORD GetCount() {
    return m_count;
  }
  HANDLE GetProcessHandle() 
  {
    return m_hProcess;
  }
};

//////////////////////////////////////////////////////////////////////////
// Iterate the top-level windows
//////////////////////////////////////////////////////////////////////////
class CWindowIterator 
{
protected:
	HWND*	m_hwnds;				// array of hwnds for this PID
	DWORD m_nAlloc;			// size of array
	DWORD m_count;				// number of HWNDs found
	DWORD m_current;			// current HWND
	
  static BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lp);

	// virtual enumerator
	virtual BOOL OnEnumProc(HWND hwnd);

	// override to filter different kinds of windows
	virtual BOOL OnWindow(HWND hwnd) 
  {
		return TRUE;
	}

public:
	CWindowIterator(DWORD nAlloc=1024);
	~CWindowIterator();
	
	HWND Next() 
  {
		return m_hwnds && m_current < m_count ? m_hwnds[m_current++] : NULL;
	}
	
  HWND First() 
  {
		::EnumWindows(EnumProc, (LPARAM)this);
		m_current = 0;
		return Next();
	}
	DWORD GetCount() 
  {
		return m_count;
	}
};


//////////////////////////////////////////////////////////////////////////
// Iterate the top-level windows in a process
//////////////////////////////////////////////////////////////////////////
class CMainWindowIterator : public CWindowIterator  
{
protected:
	DWORD m_pid;				// process id
	DWORD m_bVisible;			// show only visible windows

	virtual BOOL OnWindow(HWND hwnd);

public:
	CMainWindowIterator(DWORD pid, BOOL bVis=TRUE, DWORD nAlloc=1024);
	~CMainWindowIterator();
};

//////////////////////////////////////////////////////////////////////////
// Handy class to facilitate finding and killing a process by name.
//////////////////////////////////////////////////////////////////////////
class CFindKillProcess 
{
public:
	CFindKillProcess();
	~CFindKillProcess();
	DWORD FindProcess(LPCTSTR pszModname);
	DWORD FindFullPathProcess(LPCTSTR pszModname);
	DWORD FindProcess(LPCTSTR pszModname, OUT CDWordArray& dwArrayPid);
	DWORD FindFullPathProcess(LPCTSTR pszModname, OUT CDWordArray& dwArrayPid);
  HANDLE FindProcessHandle(LPCTSTR modname);
  HANDLE FindFullPathProcessHandle(LPCTSTR modname);
	BOOL  KillProcess(DWORD pid, BOOL bZap, UINT32 nTimeOut=5000);
  DWORD EnumProcess(CDWordArray& dwPidArray, CStringArray& strFullPathArray);
};

