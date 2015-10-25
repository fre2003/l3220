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
#pragma warning(disable:4273)

//#pragma comment(lib, "../lib/user32.lib")

#define GR_GDIOBJECTS     0       /* Count of GDI objects */
#define GR_USEROBJECTS    1       /* Count of USER objects */

WINUSERAPI
DWORD
WINAPI
GetGuiResources(
    IN HANDLE hProcess,
    IN DWORD uiFlags);

typedef struct _IO_COUNTERS {
    ULONGLONG  ReadOperationCount;
    ULONGLONG  WriteOperationCount;
    ULONGLONG  OtherOperationCount;
    ULONGLONG ReadTransferCount;
    ULONGLONG WriteTransferCount;
    ULONGLONG OtherTransferCount;
} IO_COUNTERS;
typedef IO_COUNTERS *PIO_COUNTERS;

BOOL GetProcessIoCounters(
  HANDLE hProcess,           // handle to process
  PIO_COUNTERS lpIoCounters  // I/O accouting information
);

CProcessInfo_i::CProcessInfo_i(HANDLE hProcess)
{
  if(hProcess == 0)
  {
    m_hProcess = GetCurrentProcess();
  }
  else 
    m_hProcess = hProcess;
  
  m_hModule  = ::LoadLibrary(_T("user32.dll"));
}

CProcessInfo_i::CProcessInfo_i(DWORD dwProcessId)
{
  if(dwProcessId == 0)
  {
    m_hProcess = GetCurrentProcess();
  }
  else 
  {
    m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
  }
  
  m_hModule  = ::LoadLibrary(_T("user32.dll"));
}

CProcessInfo_i::~CProcessInfo_i()
{
  if(m_hProcess)
  {
    CloseHandle(m_hProcess);
    m_hProcess = NULL;
  }
  if (m_hModule)
  {
    FreeLibrary(m_hModule);
    m_hModule = NULL;
  }
}

DWORD CProcessInfo_i::GetProcessHandleQuota(DWORD& dwGdiHandleQuota, DWORD& dwUserHandleQuota)
{
  DWORD dwType = REG_DWORD;
  DWORD dwCbData = sizeof(DWORD)*1;

  SHGetValue(HKEY_LOCAL_MACHINE, 
    _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows"),
    _T("GDIProcessHandleQuota"), &dwType, &dwGdiHandleQuota, &dwCbData);
  
  SHGetValue(HKEY_LOCAL_MACHINE, 
    _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows"),
    _T("USERProcessHandleQuota"), &dwType, &dwUserHandleQuota, &dwCbData);
  
  return 0;
}

#include <tlhelp32.h>

BOOL CProcessInfo_i::GetProcessPath(DWORD dwPID, CString& strFullPath) 
{ 
  BOOL          bRet        = FALSE; 
  BOOL          bFound      = FALSE; 
  HANDLE        hModuleSnap = NULL; 
  MODULEENTRY32 me32        = {0}; 
  
  // Take a snapshot of all modules in the specified process. 
  
  hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
  if (hModuleSnap == INVALID_HANDLE_VALUE) 
    return (FALSE); 
  
  // Fill the size of the structure before using it. 
  
  me32.dwSize = sizeof(MODULEENTRY32); 
  
  // Walk the module list of the process, and find the module of 
  // interest. Then copy the information to the buffer pointed 
  // to by lpMe32 so that it can be returned to the caller. 
  
  if (Module32First(hModuleSnap, &me32)) 
  { 
    do 
    { 
      if (me32.th32ProcessID == dwPID) 
      { 
        strFullPath = me32.szExePath; 
        break;
      } 
    } 
    while (!bFound && Module32Next(hModuleSnap, &me32)); 
    // no longer exists in specified process 
  } 
  else 
    bRet = FALSE;           // could not walk module list 
  
  // Do not forget to clean up the snapshot object. 
  
  CloseHandle (hModuleSnap); 
  
  return (bRet); 
} 


/*
首先，每个进程可以创建的GDI对象数，在理论上为64K，但是在Windows 2000中，
系统将客户可以创建的GDI句柄数硬设置为不能超过16K，而事实上当GDI对象数达到12K之后，系统即不正常；

在Windows 2003中，系统放宽了对GDI对象数的限制，使得每个进程可以使用的GDI对象数接近64K；

其次，每个进程可以创建的GDI对象数，还受限于操作系统中GDI对象总数；在Windows 2000中，
当系统中总的GDI对象达到15900以后的某个值后，进程就不能再创建新的GDI对象了；
在20003中，这个值增加到63700以后的某个值。但是，到底是哪个值，则不固定。    
最后，进程可创建的最大GDI对象数目还受限于注册表中 GDIProcessHandle限额
HKEY_LOCAL_MACHINE\ SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows下的"GDIProcessHandleQuota" 
项所设置的值，该值设置了Windows中每个进程可以创建的最大GDI对象数，在Windows 2000及2003系统中，
其默认均为10000，这也就说明了当程序创建的GDI对象数达到9999之后界面为什么会混乱。
*/
VOID CProcessInfo_i::GetProcessResourceObjects(DWORD & dwGdiObjects, DWORD & dwUserOjects)
{
  if (m_hModule && m_hProcess)
  {
    typedef DWORD (CALLBACK *PROC)(IN HANDLE hProcess,IN DWORD uiFlags); 
    PROC fp = (PROC)GetProcAddress(m_hModule, "GetGuiResources");    
    if (fp)
    {
      dwGdiObjects = (DWORD)fp(m_hProcess, GR_GDIOBJECTS);
      dwUserOjects = (DWORD)fp(m_hProcess, GR_USEROBJECTS);
    }
  }
}

/*
    HANDLE hPro = ::OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,  FALSE, 2980);    
    DWORD resnum = ::GetGuiResources ( hPro, GR_GDIOBJECTS );
    CloseHandle ( hPro );
*/

VOID CProcessInfo_i::GetProcessIoCounters(PROCESS_IO_COUNTERS& io)
{  
  if (m_hModule && m_hProcess)
  {
    typedef DWORD (CALLBACK *PROC)(HANDLE hProcess,PIO_COUNTERS lpIoCounters); 
    HMODULE hModull = GetModuleHandle(_T("Kernel32.dll"));
    PROC fp = (PROC)GetProcAddress(hModull, "GetProcessIoCounters");    
    if (fp)
    {
      IO_COUNTERS procio = {0, };
      fp(m_hProcess, &procio);
      
      io.ReadOperationCount  = procio.ReadOperationCount;
      io.WriteOperationCount = procio.WriteOperationCount;
      io.OtherOperationCount = procio.OtherOperationCount;
      io.ReadTransferCount   = procio.WriteTransferCount;
      io.WriteTransferCount  = procio.WriteTransferCount;
      io.OtherTransferCount  = procio.OtherTransferCount;
    }
  }
}

// NtQuerySystemInformation
// The function copies the system information of the specified type into a buffer
// NTSYSAPI 
// NTSTATUS
// NTAPI
// NtQuerySystemInformation(
//		IN UINT SystemInformationClass,		// information type
//		OUT PVOID SystemInformation,		// pointer to buffer
//		IN ULONG SystemInformationLength,	// buffer size in bytes
//		OUT PULONG ReturnLength OPTIONAL	// pointer to a 32 bit variable that
//											// receives the number of bytes written
//											// to the buffer
// );

typedef LONG (WINAPI *PROCNTQSI) (UINT, PVOID, ULONG, PULONG);

#define SystemBasicInformation		0
#define	SystemPerformanceInformation	2
#define SystemTimeInformation		3

#define Li2Double(x)	((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
	DWORD	dwUnknown1;
	ULONG	uKeMaximumIncrement;
	ULONG	uPageSize;
	ULONG	uMmNumberOfPhysicalPages;
	ULONG	uMmLowestPhysicalPage;
	ULONG	UMmHighestPhysicalPage;
	ULONG	uAllocationGranularity;
	PVOID	pLowestUserAddress;
	PVOID	pMmHighestUserAddress;
	ULONG	uKeActiveProcessors;
	BYTE	bKeNumberProcessors;
	BYTE	bUnknown2;
	WORD	bUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER	liIdleTime;
	DWORD		dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER	liKeBootTime;
	LARGE_INTEGER	liKeSystemTime;
	LARGE_INTEGER	liExpTimeZoneBias;
	ULONG			uCurrentTimeZoneID;
	DWORD			dwReserved;
} SYSTEM_TIME_INFORMATION;


typedef struct _tagThreadInfo
{
  FILETIME ftCreationTime;
  DWORD dwUnknown1;
  DWORD dwStartAddress;
  DWORD dwOwningPID;
  DWORD dwThreadID;
  DWORD dwCurrentPriority;
  DWORD dwBasePriority;
  DWORD dwContextSwitches;
  DWORD dwThreadState;
  DWORD dwWaitReason;
  DWORD dwUnknown2[ 5 ];
} THREADINFO, *PTHREADINFO;

#pragma warning( disable:4200 )
typedef struct _tagProcessInfo
{
  DWORD dwOffset;
  DWORD dwThreadCount;
  DWORD dwUnknown1[ 6 ];
  FILETIME ftCreationTime;
  DWORD dwUnknown2[ 5 ];
  WCHAR* pszProcessName;
  DWORD dwBasePriority;
  DWORD dwProcessID;
  DWORD dwParentProcessID;
  DWORD dwHandleCount;
  DWORD dwUnknown3;
  DWORD dwUnknown4;
  DWORD dwVirtualBytesPeak;
  DWORD dwVirtualBytes;
  DWORD dwPageFaults;
  DWORD dwWorkingSetPeak;
  DWORD dwWorkingSet;
  DWORD dwUnknown5;
  DWORD dwPagedPool;
  DWORD dwUnknown6;
  DWORD dwNonPagedPool;
  DWORD dwPageFileBytesPeak;
  DWORD dwPrivateBytes;
  DWORD dwPageFileBytes;
  DWORD dwUnknown7[ 4 ];
  THREADINFO ti[ 0 ];
} _PROCESSINFO, *PPROCESSINFO;

#if  0
VOID CProcessInfo_i::GetProcessInfo(DWORD dwProcessID)
{
  PROCNTQSI NtQuerySystemInformation;
  
  NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"), "NtQuerySystemInformation");
  if (NtQuerySystemInformation)
  {
    PBYTE pbyInfo = ( PBYTE ) malloc( 0x20000 ); 
    NtQuerySystemInformation( 5, pbyInfo, 0x20000, 0 ) ;
    PPROCESSINFO pProcessInfo = ( PPROCESSINFO ) pbyInfo;
    bool bLast = false;
    do 
    {
      if ( pProcessInfo->dwOffset == 0 )
        bLast = true;
      if (pProcessInfo->dwProcessID == dwProcessID)
        break;
      pProcessInfo = ( PPROCESSINFO ) ( ( PBYTE ) pProcessInfo + pProcessInfo->dwOffset );
    } while( bLast == false );
  }
}
#endif 

VOID CProcessInfo_i::GetProcessTimeStart(OUT LARGE_INTEGER& IdleTime,
                                    OUT LARGE_INTEGER& SystemTime)
{
	SYSTEM_BASIC_INFORMATION		SysBaseInfo;
	SYSTEM_TIME_INFORMATION			SysTimeInfo;
	SYSTEM_PERFORMANCE_INFORMATION	SysPerfInfo;
  LONG							status = 0;
  double						dbIdleTime = 0.0;
  double						dbSystemTime = 0.0;
	PROCNTQSI					NtQuerySystemInformation;
  
  NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"), "NtQuerySystemInformation");
  if (NtQuerySystemInformation)
  {
    status = NtQuerySystemInformation(SystemBasicInformation, &SysBaseInfo, sizeof(SysBaseInfo), NULL);
    if (status == NO_ERROR)
    {
      // get system time
      status = NtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo, sizeof(SysTimeInfo), NULL);
      if (status == NO_ERROR)
      {
        // get system idle time
        status = NtQuerySystemInformation(SystemPerformanceInformation,	&SysPerfInfo, sizeof(SysPerfInfo), NULL);
        if (status == NO_ERROR)
        {
          IdleTime   = SysPerfInfo.liIdleTime;
          SystemTime = SysTimeInfo.liKeSystemTime;
        }
      }
    }
  }
}

// CPU Usage %
double CProcessInfo_i::GetProcessTimeEnd(IN LARGE_INTEGER& IdleTime,
                                         IN LARGE_INTEGER& SystemTime)
{
  SYSTEM_BASIC_INFORMATION		SysBaseInfo = {0, };
  SYSTEM_TIME_INFORMATION			SysTimeInfo = {0, };
  SYSTEM_PERFORMANCE_INFORMATION	SysPerfInfo;
  LONG							status = 0;
  double						dbSystemTime = 0.0;
  PROCNTQSI					NtQuerySystemInformation;
  double						dbIdleTime = 0.0;
  
  NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"), "NtQuerySystemInformation");
  if (NtQuerySystemInformation)
  {
    // get new System time
    status = NtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo,sizeof(SysTimeInfo), NULL);          
    if (status == NO_ERROR)
    {
				// get new system idle time
				status = NtQuerySystemInformation(SystemPerformanceInformation, &SysPerfInfo, sizeof(SysPerfInfo), NULL);
        if (status == NO_ERROR)
        {
          // current value = new value - old value
          dbIdleTime   = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(IdleTime);
          dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(SystemTime);
          
          // currentCpuIdle = IdleTime / SystemTime;
          dbIdleTime = dbIdleTime / dbSystemTime;
          
          // currentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
          dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
        }
    }
  }
  
  return dbIdleTime;
}


 
#define  DWORD_PTR DWORD

#include "Pdh.h"
#include "pdhmsg.h"

#pragma comment(lib, "pdh.lib")


#if 0 
#define MAX_RAW_VALUES    20

//// DEFINES FOR COUNTER NAMES ////
#define CNTR_CPU              "\\Processor(_Total)\\% Processor Time" // % of cpu in use
#define CNTR_MEMINUSE_BYTES   "\\Memory\\Committed Bytes" // mem in use measured in bytes
#define CNTR_MEMAVAIL_BYTES   "\\Memory\\Available Bytes" // mem available measured in bytes
#define CNTR_MEMAVAIL_KB      "\\Memory\\Available KBytes" // mem avail in kilobytes
#define CNTR_MEMAVAIL_MB      "\\Memory\\Available MBytes" // mem avail in megabytes
#define CNTR_MEMINUSE_PERCENT "\\Memory\\% Committed Bytes In Use" // % of mem in use
#define CNTR_MEMLIMIT_BYTES   "\\Memory\\Commit Limit" // commit limit on memory in bytes
#define CNTR_RAS_RECEIVBYTES  "\\RAS Total\\Bytes Received" // ras received bytes
//#define CNTR_RAS_RECEIVBYTES "\\Network Interface(3Com EtherLink PCI)\\Bytes Received/sec" // test received bytes

// NOTE: Find other counters using the function PdhBrowseCounters() (lookup in MSDN).
// This function was not implemented in this class.

typedef struct _tag_PDHCounterStruct {
	  int nIndex;				// The index of this counter, returned by AddCounter()
  	LONG lValue;			// The current value of this counter
  	double dValue;		  	// The current value of this counter
    HCOUNTER hCounter;      // Handle to the counter - given to use by PDH Library
    int nNextIndex;         // element to get the next raw value
    int nOldestIndex;       // element containing the oldes raw value
    int nRawCount;          // number of elements containing raw values
    PDH_RAW_COUNTER a_RawValue[MAX_RAW_VALUES]; // Ring buffer to contain raw values
} PDHCOUNTERSTRUCT, *PPDHCOUNTERSTRUCT;

class CPerfMon
{
public:
	CPerfMon();
	~CPerfMon();

	BOOL Initialize(void);
	void Uninitialize(void);

	int AddCounter(LPCTSTR pszCounterName);
	BOOL RemoveCounter(int nIndex);

	BOOL CollectQueryData(void);
	double GetCounterValue(int nIndex);

protected:
	BOOL UpdateValue(PPDHCOUNTERSTRUCT pCounter);
	BOOL UpdateRawValue(PPDHCOUNTERSTRUCT pCounter);

	HQUERY m_hQuery; // the query to the PDH
	PDHCOUNTERSTRUCT m_RasCounter; //ras counter
	int m_nNextIndex;
};


CPerfMon::CPerfMon()
{
	// m_nNextIndex is a unique value.  It will not be decremented, even if you remove counters.
	m_nNextIndex = 0;
  m_hQuery     = NULL;
}

CPerfMon::~CPerfMon()
{
  m_hQuery = NULL;
}


// Function name	: CPerfMon::Initialize
// Description	    : Initialize the query and memory
// Return type		: BOOL ; true on success; false on fail
//
BOOL CPerfMon::Initialize()
{
	if (PdhOpenQuery(NULL, 1, &m_hQuery) != ERROR_SUCCESS)
		return false;

	return true;
}


// Function name	: CPerfMon::Uninitialize
// Description	    : Closes the query and fress all memory
// Return type		: void
//
void CPerfMon::Uninitialize()
{
	PdhCloseQuery(&m_hQuery);
}


// Function name	: CPerfMon::AddCounter
// Description	    : Adds a counter to the query.
// Return type		: int ; -1 on fail, index to counter on success.
// Argument         : const char *pszCounterName
//
int CPerfMon::AddCounter(LPCTSTR pszCounterName)
{
  PPDHCOUNTERSTRUCT pCounter;
  pCounter = &m_RasCounter;
  
  if (!pCounter) return -1;
  
  // add to current query
  if (PdhAddCounter(m_hQuery, pszCounterName, (DWORD)pCounter, &(pCounter->hCounter)) != ERROR_SUCCESS)
  {
    return -1;
  }
  
  // insert counter into array(s)
  pCounter->nIndex = m_nNextIndex++;
  pCounter->lValue = 0;
  pCounter->nNextIndex = 0;
  pCounter->nOldestIndex = 0;
  pCounter->nRawCount = 0; 
  return pCounter->nIndex;
}


// Function name	: CPerfMon::RemoveCounter
// Description	    : remove a counter from the query based on the index returned from AddCounter
// Return type		: BOOL ; false on fail ; true on success
// Argument         : int nIndex
//
BOOL CPerfMon::RemoveCounter(int nIndex)
{
	PPDHCOUNTERSTRUCT pCounter = &m_RasCounter; //GetCounterStruct(nIndex);
	if (!pCounter) return false;
	
	if (PdhRemoveCounter(pCounter->hCounter) != ERROR_SUCCESS)
		return false;
		
	return true;
}


// Function name	: CPerfMon::CollectQueryData
// Description	    : Collects the data for all the counters added with AddCounter()
// Return type		: BOOL ; false fail ; true success
//
BOOL CPerfMon::CollectQueryData()
{
	if (PdhCollectQueryData(m_hQuery) != ERROR_SUCCESS) return false;

	return true;
}


// Function name	: CPerfMon::UpdateValue
// Description	    : Updates the counter value for the counter in pCounter
// Return type		: BOOL ; false fail ; true success
// Argument         : PPDHCOUNTERSTRUCT pCounter
//
BOOL CPerfMon::UpdateValue(PPDHCOUNTERSTRUCT pCounter)
{
	PDH_FMT_COUNTERVALUE pdhFormattedValue;

	// get the value from the PDH
	//if (PdhGetFormattedCounterValue(pCounter->hCounter, PDH_FMT_LONG, NULL, &pdhFormattedValue) != ERROR_SUCCESS)
	if (PdhGetFormattedCounterValue(pCounter->hCounter, PDH_FMT_DOUBLE, NULL, &pdhFormattedValue) != ERROR_SUCCESS)
		return false;

	// test the value for validity
	if (pdhFormattedValue.CStatus != ERROR_SUCCESS)
		return false;

	// set value
	pCounter->dValue = pdhFormattedValue.doubleValue;

	return true;
}


// Function name	: CPerfMon::UpdateRawValue
// Description	    : Update the raw values for the counter in pCounter
// Return type		: BOOL ; false fail ; true success
// Argument         : PPDHCOUNTERSTRUCT pCounter
//
BOOL CPerfMon::UpdateRawValue(PPDHCOUNTERSTRUCT pCounter)
{
    PPDH_RAW_COUNTER ppdhRawCounter;

    // Assign the next value into the array
    ppdhRawCounter = &(pCounter->a_RawValue[pCounter->nNextIndex]);

	if (PdhGetRawCounterValue(pCounter->hCounter, NULL, ppdhRawCounter) != ERROR_SUCCESS)
		return false;
	
    // update raw counter - up to MAX_RAW_VALUES
    pCounter->nRawCount = min(pCounter->nRawCount + 1, MAX_RAW_VALUES);

    // Update next index - rolls back to zero upon reaching MAX_RAW_VALUES
    pCounter->nNextIndex = (pCounter->nNextIndex + 1) % MAX_RAW_VALUES;

    // The Oldest index remains zero until the array is filled.
    // It will now be the same as the 'next' index since it was previously assigned.
    if (pCounter->nRawCount >= MAX_RAW_VALUES)
        pCounter->nOldestIndex = pCounter->nNextIndex;

	return true;
}

double CPerfMon::GetCounterValue(int nIndex)
{
	PPDHCOUNTERSTRUCT pCounter = &m_RasCounter; //GetCounterStruct(nIndex);
	if (!pCounter) return 0;

	// update the value(s)
	if (!UpdateValue(pCounter)) return 0;

	// return the value
	return pCounter->dValue;
}
#else
#include "..\core\core21/cpdh/cpdh.h"
#endif  

#if 0 
CProcessPerformance_i::CProcessPerformance_i(LPCTSTR pszProcess)
{
  DWORD dwVersion = GetVersion();
  
  // Get major and minor version numbers of Windows
  DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
  DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
  if (dwVersion>>31 ==0)                // Windows NT
  {
    m_pPerfMon = new CPerfMon;

    if(m_pPerfMon)
    {
      //add conter here
      if (!m_pPerfMon->Initialize())
      {
        ;
      }
      else
      {
        if(pszProcess)
        {
          TCHAR name[MAX_PATH] = {0, };
          wsprintf(name, _T("\\Process(%s)\\%% Processor Time"), pszProcess);
          m_CounterIndex = m_pPerfMon->AddCounter(_T("\\Process(notepad)\\% Processor Time"));
        }
        else 
        {
          m_CounterIndex = m_pPerfMon->AddCounter(_T("\\Processor(_Total)\\% Processor Time"));
        }
         
         // "\\Process(notepad#1)\\% Processor Time"
         // \\Processor(_Total)\\% Processor Time "\\Thread(cpdh/1)\\% Processor Time"
         // "\\Process(notepad#1)\\% Processor Time" "\\Process(MSDEV#1)\\% Processor Time"
         // "\\Thread(XMsgBoxTest/1)\\% Processor Time"   "\\Processor(_Total)\\% Processor Time"  \\Process(notepad#1)\\% Processor Time 
      }
    }
  }
}
#else;


CProcessPerformance_i::CProcessPerformance_i(LPCTSTR pszProcess)
{
  m_pQueryObject   = NULL;
  m_pCounterObject = NULL;
  
  if(pszProcess == NULL)
  {
    pszProcess = _T("_Total");
  }
  
  TCHAR pszFullCounterPath[MAX_PATH] = {0, };
  //wsprintf(pszFullCounterPath, _T("\\Process(%s)\\%% Processor Time"), pszProcess);
  wsprintf(pszFullCounterPath, _T("\\Process(PerM)\\%% Processor Time"), pszProcess);
  
  if(CPdh::ValidatePath(pszFullCounterPath))
  {
    CPdhQuery* pQuery = new CPdhQuery;
    CPdhCounter*  pCounter = new CPdhCounter;
    m_pQueryObject   = (PVOID)pQuery;
    m_pCounterObject = (PVOID)pCounter;
    
    pCounter->SetFullCounterPath(pszFullCounterPath);
    pQuery->Open();
    pQuery->Add(pCounter);
    pQuery->Collect(); 
    
    CPdhCounterInfo info;
    pCounter->GetInfo(info);
  }
}
#endif 

CProcessPerformance_i::~CProcessPerformance_i()
{
  if(m_pQueryObject && m_pCounterObject)
  {
    CPdhQuery* pQuery = (CPdhQuery*)m_pQueryObject;
    CPdhCounter* pCounter = (CPdhCounter*)m_pCounterObject;
    pQuery->Remove(*pCounter);
    delete pQuery;
    delete pCounter;
  }
}

double CProcessPerformance_i::GetProcessUsage()
{
  CPdhQuery* pQuery = (CPdhQuery*)m_pQueryObject;
  CPdhCounter* pCounter = (CPdhCounter*)m_pCounterObject;
  if(pQuery && pCounter)
  {
    pQuery->Collect();
    PDH_FMT_COUNTERVALUE fv;
    
    pCounter->GetFormattedValue(PDH_FMT_DOUBLE, fv);
    return fv.doubleValue;
  }
  else 
  {
    return 0.0;
  }  
}

#include "Pdh.h"
#pragma comment ( lib , "Pdh.lib" )
// 　　要使用性能计数器的基本步骤是:
// 　　1.打开计数器PdhOpenQuery；
// 　　2.为计数器句柄分配空间；
// 　　3.把感兴趣的计数器添加进来PdhAddCounter；
// 　　4.收集数据PdhCollectQueryData
// 　　5.得到计数器的数值PdhGetFormattedCounterValue；
// 　　6.关闭计数器PdhCloseQuery。

#if 0 
BOOL CPdh::Initialize()
{
  typedef long (__stdcall *PDHOpenQuery) (LPCSTR, DWORD_PTR, HQUERY);
  PDHOpenQuery pfnPdhOpenQuery;
  
  m_hPdhModule = LoadLibrary(_T("PDH.DLL"));
  if(m_hPdhModule == NULL)
    return FALSE;
  // 这个地方ms弄错了，pdh.h里声明的是PdhOpenQueryA，但pdh.dll里导出的却是PdhOpenQuery。
  pfnPdhOpenQuery = (PDHOpenQuery)GetProcAddress(m_hPdhModule, "PdhOpenQuery");
  if(pfnPdhOpenQuery == NULL)
    return FALSE;
  
  return ERROR_SUCCESS == pfnPdhOpenQuery(NULL, 1, &m_hQuery);
}
#endif 

U32 CProcessPef_i::Init()
{
  PDH_STATUS pdhStatus = PdhOpenQuery(NULL, 1, (PDH_HQUERY*)&m_hQuery);
  
  return 0;
}

LONG CProcessPef_i::QueryData(LPCTSTR lpCounterName)
{
  HCOUNTER hCounter;
  PDH_FMT_COUNTERVALUE pdhFormattedValue;
  if(ERROR_SUCCESS != PdhAddCounter(m_hQuery, lpCounterName, NULL, (PDH_HQUERY*)&hCounter))
    return -1;
  
  if(ERROR_SUCCESS != PdhCollectQueryData((PDH_HQUERY)m_hQuery)
    || ERROR_SUCCESS != PdhGetFormattedCounterValue( 
    hCounter,
    PDH_FMT_LONG,
    NULL,
    &pdhFormattedValue )
    || ERROR_SUCCESS != pdhFormattedValue.CStatus)
  {
    pdhFormattedValue.longValue = -1;
  }
  
  PdhRemoveCounter(hCounter);
  return pdhFormattedValue.longValue;
}


DWORD CProcessPef_i::GetThreadState(LPCTSTR szThreadName)
{
  TCHAR szCounterName[256];
  _stprintf(szCounterName, _T("\\Thread(%s)\\Thread State"),szThreadName);

  return QueryData(szCounterName);
}
 