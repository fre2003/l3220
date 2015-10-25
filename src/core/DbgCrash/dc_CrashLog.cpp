#include "StdAfx.h"
#include "dc_LogFile.h"
#include "dc_utl.h"
#include "TLHELP32.H"
#include "dc_CrashLog.h"

// Translate the exception code into something human readable
CONST TCHAR *GetExceptionDescription(DWORD ExceptionCode)
{
	struct ExceptionNames
	{
		DWORD	ExceptionCode;
		TCHAR *	ExceptionName;
	};

  // STATUS_WAIT_0
	ExceptionNames ExceptionMap[] =
	{
		{0x40010005, _T("a Control-C")},
		{0x40010008, _T("a Control-Break")},
		{0x80000002, _T("a Datatype Misalignment")},
		{0x80000003, _T("a Breakpoint")},
		{0xc0000005, _T("an Access Violation")},
		{0xc0000006, _T("an In Page Error")},
		{0xc0000017, _T("a No Memory")},
		{0xc000001d, _T("an Illegal Instruction")},
		{0xc0000025, _T("a Noncontinuable Exception")},
		{0xc0000026, _T("an Invalid Disposition")},
		{0xc000008c, _T("a Array Bounds Exceeded")},
		{0xc000008d, _T("a Float Denormal Operand")},
		{0xc000008e, _T("a Float Divide by Zero")},
		{0xc000008f, _T("a Float Inexact Result")},
		{0xc0000090, _T("a Float Invalid Operation")},
		{0xc0000091, _T("a Float Overflow")},
		{0xc0000092, _T("a Float Stack Check")},
		{0xc0000093, _T("a Float Underflow")},
		{0xc0000094, _T("an Integer Divide by Zero")},
		{0xc0000095, _T("an Integer Overflow")},
		{0xc0000096, _T("a Privileged Instruction")},
		{0xc00000fD, _T("a Stack Overflow")},
		{0xc0000142, _T("a DLL Initialization Failed")},
		{0xe06d7363, _T("a Microsoft C++ Exception")},
	};

	for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
		if (ExceptionCode == ExceptionMap[i].ExceptionCode)
			return ExceptionMap[i].ExceptionName;

	return _T("an Unknown exception type");
}

LONG DumpExceptionHeader(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile)
{
  LONG ret = 0;
	PEXCEPTION_RECORD Exception = pExceptPtrs->ExceptionRecord;
	PCONTEXT          Context   = pExceptPtrs->ContextRecord;

	TCHAR szCrashModulePathName[MAX_PATH*2];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));

  TCHAR *pszCrashModuleFileName = _T("Unknown");
  
	MEMORY_BASIC_INFORMATION MemInfo;
  
	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the hModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
  if (VirtualQuery((void*)Context->Eip, &MemInfo, sizeof(MemInfo)) &&
    (GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,
    szCrashModulePathName,
    sizeof(szCrashModulePathName)-2) > 0))
  {
    pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
  }
  
  TCHAR szFileName[MAX_PATH*2] = {0, };
  if (GetModuleFileName(NULL, szFileName, COUNTOF(szFileName)-2) <= 0)
  {
  }  
  TCHAR* pszFileName = GetFilePart(szFileName);
  
  dc_OutputDbgStr(_T("%s caused %s (0x%08x) in module %s at %04x:%08x.\r\n"),
				pszFileName, GetExceptionDescription(Exception->ExceptionCode),
				Exception->ExceptionCode,
				pszCrashModuleFileName, Context->SegCs, Context->Eip);
  
  if(hLogFile)
  {
    dc_LogFilePrint(hLogFile, _T("%s caused %s (0x%08x) in module %s at %04x:%08x.\r\n\r\n"),
      pszFileName, GetExceptionDescription(Exception->ExceptionCode),
      Exception->ExceptionCode, 
      pszCrashModuleFileName, Context->SegCs, Context->Eip);
    
    dc_LogFileFlush(hLogFile);
  }
  
  return ret;
}

const int NumCodeBytes = 16;	   // Number of code bytes to record.
const int MaxStackDump = 3072;	 // Maximum number of DWORDS in stack dumps.
const int StackColumns = 4;		   // Number of columns in stack dump.

#define	ONEK		    	1024
#define	SIXTYFOURK		(64*ONEK)
#define	ONEM		   	  (ONEK*ONEK)
#define	ONEG			    (ONEK*ONEK*ONEK)

//
// Record information about the user's system, such as processor type, amount of memory, etc.
// 
static void DumpSystemInformation(HANDLE hLogFile)
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	TCHAR szTimeBuffer[100];
	FormatTime(szTimeBuffer, CurrentTime);

	dc_LogFilePrint(hLogFile, _T("*** Error occurred at %s.\r\n"), szTimeBuffer);

	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, COUNTOF(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));

	TCHAR szUserName[200];
	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = COUNTOF(szUserName)-2;
	if (!GetUserName(szUserName, &UserNameSize))
		lstrcpy(szUserName, _T("Unknown"));

	dc_LogFilePrint(hLogFile, _T("%s, run by %s.\r\n"), szModuleName, szUserName);

	// Print out operating system Info.
	TCHAR szWinVer[50], szMajorMinorBuild[50];
  GetWinOSVer(szWinVer, szMajorMinorBuild); // Operating system:  Windows XP (5.1.2600). 
	dc_LogFilePrint(hLogFile, _T("Operating system:  %s (%s).\r\n"), szWinVer, szMajorMinorBuild);

	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	dc_LogFilePrint(hLogFile, _T("%d processor(s), type %d.\r\n"),SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);
    
	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	dc_LogFilePrint(hLogFile, _T("%d%% memory in use.\r\n"), MemInfo.dwMemoryLoad);
	dc_LogFilePrint(hLogFile, _T("%d MBytes physical memory.\r\n"), (MemInfo.dwTotalPhys + ONEM - 1) / ONEM);
	dc_LogFilePrint(hLogFile, _T("%d MBytes physical memory free.\r\n"), (MemInfo.dwAvailPhys + ONEM - 1) / ONEM);
	dc_LogFilePrint(hLogFile, _T("%d MBytes paging file.\r\n"), (MemInfo.dwTotalPageFile + ONEM - 1) / ONEM);
	dc_LogFilePrint(hLogFile, _T("%d MBytes paging file free.\r\n"), (MemInfo.dwAvailPageFile + ONEM - 1) / ONEM);
	dc_LogFilePrint(hLogFile, _T("%d MBytes user address space.\r\n"), (MemInfo.dwTotalVirtual + ONEM - 1) / ONEM);
	dc_LogFilePrint(hLogFile, _T("%d MBytes user address space free.\r\n"), (MemInfo.dwAvailVirtual + ONEM - 1) / ONEM);
}

////////////////////////////////////////////////////////////////////////////////
// PrintAccessViolationInfo().
////////////////////////////////////////////////////////////////////////////////
LONG PrintAccessViolationInfo(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile)
{
  //
  // If the exception was an access violation, print out some additional
  // information, to the error log and the debugger.
  //
  if (pExceptPtrs->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION &&
				pExceptPtrs->ExceptionRecord->NumberParameters >= 2)
  {
    const TCHAR* readwrite = _T("Read from");
    if (pExceptPtrs->ExceptionRecord->ExceptionInformation[0])
      readwrite = _T("Write to");
    
    dc_LogFilePrint(hLogFile, _T("%s location %08x caused an access violation.\r\n"),
      readwrite, pExceptPtrs->ExceptionRecord->ExceptionInformation[1]);
    
    //dc_OutputDbgStr(_T("Exception handler: %s location %08x caused an access violation.\r\n"),
    //  readwrite, pExceptPtrs->ExceptionRecord->ExceptionInformation[1]);
   
  }
  
  return 0;
}

static void DumpThreads(DWORD dwProcessId, HANDLE hLogFile)
{
  SYSTEMTIME st;  
  SYSTEMTIME st1;  
  FILETIME CreationTime, ExitTime,KernelTime,UserTime; 
  
  HANDLE hProcess = OpenProcess(
    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE, dwProcessId);

  if (hProcess == NULL)
    return;
  
  dc_LogFilePrint(hLogFile, _T("\r\nDump Threads:\r\n"));
  
  GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);

  FILETIME  tfUserTime, tfCreateUserTime;
  FileTimeToLocalFileTime(&UserTime, &tfUserTime);
  FileTimeToLocalFileTime(&CreationTime, &tfCreateUserTime);
  FileTimeToSystemTime(&tfUserTime, &st);
  FileTimeToSystemTime(&tfCreateUserTime, &st1);
  
  dc_LogFilePrint(hLogFile, _T("Threads in Process(0x%04x), Create Time:User Time:%02d:%02d:%02d, User Time:%02d:%02d:%02d.%03d : \r\n"), 
    dwProcessId, st1.wHour, st1.wMinute, st1.wSecond, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  
  dc_LogFilePrint(hLogFile, _T("Current Thread ID: 0x%x\r\n"), GetCurrentThreadId());
  
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, dwProcessId);
  if (hSnap == INVALID_HANDLE_VALUE)
    return;
  
  THREADENTRY32 te;
  memset(&te, 0, sizeof(te));
  te.dwSize = sizeof(te);
  if (Thread32First(hSnap, &te) == FALSE)
  {
    CloseHandle(hSnap);
    return;
  }
  
#if 0 
  // Initialize StackWalker...
  MyStackWalker sw(dwProcessId, hProcess, "E:\\Build\\sym", "D:\\RISS\\ITS\\deploy\\bin\\dbghelp.dll");
  sw.LoadModules();
#endif 

  int cnt = 0;

  // now enum all thread for this processId
  do
  {
    if (te.th32OwnerProcessID != dwProcessId)
    {
      continue;
    }

    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
    if (hThread == NULL)
    {
      continue;
    }
    
#if 0 
    char szTemp[100];
    sprintf(szTemp, "\r\nThreadID: %d\r\n", te.th32ThreadID);
    sw.OnOutput(szTemp);  // output the threadId
    sw.ShowCallstack(hThread);
#endif 
    
    FILETIME CreationTime, ExitTime,KernelTime,UserTime; 
    GetThreadTimes(hThread, &CreationTime, &ExitTime, &KernelTime, &UserTime);
    FILETIME  tfUserTime, tfCreateUserTime;
    FileTimeToLocalFileTime(&UserTime, &tfUserTime);
    FileTimeToLocalFileTime(&CreationTime, &tfCreateUserTime);
    FileTimeToSystemTime(&tfUserTime, &st);
    FileTimeToSystemTime(&tfCreateUserTime, &st1); 

    //printf("ThreadID:%04x, user time:%s\n", te.th32ThreadID, lpszString);
    dc_LogFilePrint(hLogFile, _T("ThreadID[%d]:%04x, Create Time:%02d:%02d:%02d, User Time:%02d:%02d:%02d.%03d \r\n"), ++cnt, 
      te.th32ThreadID, st1.wHour, st1.wMinute, st1.wSecond, st1.wHour, st1.wMinute, st1.wSecond, st1.wMilliseconds);
    
    CloseHandle(hThread);
  } while(Thread32Next(hSnap, &te) != FALSE);
  
  CloseHandle(hSnap);
}

//////////////////////////////////////////////////////////////////////////
// DumpRegisters
//////////////////////////////////////////////////////////////////////////
static void DumpRegisters(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile)
{
  PCONTEXT Context = pExceptPtrs->ContextRecord;

  // Print out the register values in an XP error window compatible format.
  dc_LogFilePrint(hLogFile, _T("\r\n"));
  dc_LogFilePrint(hLogFile, _T("Context:\r\n"));
  dc_LogFilePrint(hLogFile, _T("EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n"),
				Context->Edi, Context->Esi, Context->Eax);
  dc_LogFilePrint(hLogFile, _T("EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n"),
				Context->Ebx, Context->Ecx, Context->Edx);
  dc_LogFilePrint(hLogFile, _T("EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n"),
				Context->Eip, Context->Ebp, Context->SegCs);
  dc_LogFilePrint(hLogFile, _T("EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n"),
				Context->EFlags, Context->Esp, Context->SegSs);
}

//////////////////////////////////////////////////////////////////////////
// DumpEIP
//////////////////////////////////////////////////////////////////////////
static void DumpEIP(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile)
{
  PCONTEXT Context = pExceptPtrs->ContextRecord;

	// Print out the bytes of code at the instruction pointer. Since the
	// crash may have been caused by an instruction pointer that was bad,
	// this code needs to be wrapped in an exception handler, in case there
	// is no memory to read. If the dereferencing of code[] fails, the
	// exception handler will print '??'.

	dc_LogFilePrint(hLogFile, _T("\r\nBytes at CS:EIP:\r\n"));
	BYTE * code = (BYTE *)Context->Eip;
	for (int codebyte = 0; codebyte < NumCodeBytes; codebyte++)
	{
		__try
		{
			dc_LogFilePrint(hLogFile, _T("%02x "), code[codebyte]);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			dc_LogFilePrint(hLogFile, _T("?? "));
		}
	}
}

// DumpStack
static void DumpStack(PEXCEPTION_POINTERS pExceptPtrs,HANDLE hLogFile)
{
  PCONTEXT Context = pExceptPtrs->ContextRecord;

	// Esp contains the bottom of the stack, or at least the bottom of
	// the currently used area
	DWORD* pStack = (DWORD *)Context->Esp;

	dc_LogFilePrint(hLogFile, _T("\r\nStack:\r\n"));
  
	__try
	{
		// Esp contains the bottom of the stack, or at least the bottom of
		// the currently used area.
		DWORD* pStackTop;

		__asm
		{
			// Load the top (highest address) of the stack from the
			// thread information block. It will be found there in
			// Win9x and Windows NT.
			mov	eax, fs:[4]
			mov pStackTop, eax
		}

		if (pStackTop > pStack + MaxStackDump)
			pStackTop = pStack + MaxStackDump;

		int Count = 0;

		DWORD* pStackStart = pStack;

		int nDwordsPrinted = 0;

		while (pStack + 1 <= pStackTop)
		{
			if ((Count % StackColumns) == 0)
			{
				pStackStart = pStack;
				nDwordsPrinted = 0;
				dc_LogFilePrint(hLogFile, _T("0x%08x: "), pStack);
			}

			if ((++Count % StackColumns) == 0 || pStack + 2 > pStackTop)
			{
				dc_LogFilePrint(hLogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;

				int n = nDwordsPrinted;
				while (n < 4)
				{
					dc_LogFilePrint(hLogFile, _T("         "));
					n++;
				}

				for (int i = 0; i < nDwordsPrinted; i++)
				{
					DWORD dwStack = *pStackStart;
					for (int j = 0; j < 4; j++)
					{
						char c = (char)(dwStack & 0xFF);
						if (c < 0x20 || c > 0x7E)
							c = '.';
#ifdef _UNICODE
						WCHAR w = (WCHAR)c;
						dc_LogFilePrint(hLogFile, _T("%c"), w);
#else
						dc_LogFilePrint(hLogFile, _T("%c"), c);
#endif
						dwStack = dwStack >> 8;
					}
					pStackStart++;
				}

				dc_LogFilePrint(hLogFile, _T("\r\n"));
			}
			else
			{
				dc_LogFilePrint(hLogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;
			}
			pStack++;
		}
		dc_LogFilePrint(hLogFile, _T("\r\n"));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		dc_LogFilePrint(hLogFile, _T("Exception encountered during stack dump.\r\n"));
	}
}

//
// Print information about a code module (DLL or EXE) such as its size,
// location, time stamp, etc.
//
static bool DumpModuleInfo(HANDLE hLogFile, HINSTANCE hModuleHandle, int nModuleNo)
{
  bool rc = false;
  TCHAR szModName[MAX_PATH*2];

  ZeroMemory(szModName, sizeof(szModName));
  
  __try
  {
    if (GetModuleFileName(hModuleHandle, szModName, sizeof(szModName)-2) > 0)
    {
      // If GetModuleFileName returns greater than zero then this must
      // be a valid code module address. Therefore we can try to walk
      // our way through its structures to find the link time stamp.
      IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER*)hModuleHandle;
      if (IMAGE_DOS_SIGNATURE != DosHeader->e_magic)
        return false;
      
      IMAGE_NT_HEADERS *NTHeader = (IMAGE_NT_HEADERS*)((TCHAR *)DosHeader + DosHeader->e_lfanew);
      if (IMAGE_NT_SIGNATURE != NTHeader->Signature)
        return false;      
      
      dc_LogFilePrint(hLogFile, _T("Module %d\r\n"), nModuleNo);
      dc_LogFilePrint(hLogFile, _T("%s\r\n"), szModName);
      dc_LogFilePrint(hLogFile, _T("Image Base: 0x%08x  Image Size: 0x%08x\r\n"), 
        NTHeader->OptionalHeader.ImageBase, NTHeader->OptionalHeader.SizeOfImage);
      
      dc_LogFilePrint(hLogFile, _T("Checksum:   0x%08x  Time Stamp: 0x%08x\r\n"),
        NTHeader->OptionalHeader.CheckSum, NTHeader->FileHeader.TimeDateStamp);
      
      {
        TCHAR szTimeBuffer[100];
        DWORD FileSize = 0;
        szTimeBuffer[0] = _T('\0');
        
        GetFileSizeTime(szModName, szTimeBuffer, &FileSize);
        dc_LogFilePrint(hLogFile, _T("File Size:  %-10d  File Time:  %s\r\n"), FileSize, szTimeBuffer);      
      }
      dc_LogFilePrint(hLogFile, _T("Version Information:\r\n"));
      
      // Dump the File Version Information.
      {
        TCHAR szBuf[200];
        WORD dwBuf[4];
        
        dc_GetCompanyName(szModName,szBuf, sizeof(szBuf)-1);
        dc_LogFilePrint(hLogFile, _T("   Company:    %s\r\n"), szBuf);
        
        dc_GetProductName(szModName,szBuf, sizeof(szBuf)-1);
        dc_LogFilePrint(hLogFile, _T("   Product:    %s\r\n"), szBuf);
        
        dc_GetFileDescription(szModName,szBuf, sizeof(szBuf)-1);
        dc_LogFilePrint(hLogFile, _T("   FileDesc:   %s\r\n"), szBuf);
        
        dc_GetFileVersion(szModName,dwBuf);
        dc_LogFilePrint(hLogFile, _T("   FileVer:    %d.%d.%d.%d  "), dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);
        
        dc_GetProductVersion(szModName,dwBuf);
        dc_LogFilePrint(hLogFile, _T("   ProdVer:    %d.%d.%d.%d\r\n"), dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);
        
        dc_LogFilePrint(hLogFile, _T("\r\n"));
      }
      
      rc = true;
    }
  }
  __except(EXCEPTION_EXECUTE_HANDLER)	// Handle any exceptions by continuing from this point.
  {
    
  }
  
  return rc;
}

//
// Scan memory looking for code modules (DLLs or EXEs). VirtualQuery is used
// to find all the blocks of address space that were reserved or committed,
// and ShowModuleInfo will display module information if they are code
// modules.
//
static void DumpModuleList(HANDLE hLogFile)
{
  SYSTEM_INFO	SystemInfo;
  GetSystemInfo(&SystemInfo);
  
  const size_t PageSize = SystemInfo.dwPageSize;
  
  // Set NumPages to the number of pages in the 4GByte address space,
  // while being careful to avoid overflowing ints
  const size_t NumPages = 4 * size_t(ONEG / PageSize);
  size_t pageNum = 0;
  void *LastAllocationBase = 0;
  
  int nModuleNo = 1;
  
  while (pageNum < NumPages)
  {
    MEMORY_BASIC_INFORMATION MemInfo;
    if (VirtualQuery((void *)(pageNum * PageSize), &MemInfo,
      sizeof(MemInfo)))
    {
      if (MemInfo.RegionSize > 0)
      {
        // Adjust the page number to skip over this block of memory
        pageNum += MemInfo.RegionSize / PageSize;
        if (MemInfo.State == MEM_COMMIT && MemInfo.AllocationBase >
          LastAllocationBase)
        {
          // Look for new blocks of committed memory, and try
          // recording their module names - this will fail
          // gracefully if they aren't code modules
          LastAllocationBase = MemInfo.AllocationBase;
          if (DumpModuleInfo(hLogFile, 
									   (HINSTANCE)LastAllocationBase, 
                     nModuleNo))
          {
            nModuleNo++;
          }
        }
      }
      else
        pageNum += SIXTYFOURK / PageSize;
    }
    else
      pageNum += SIXTYFOURK / PageSize;
    
    // If VirtualQuery fails we advance by 64K because that is the
    // granularity of address space doled out by VirtualAlloc()
  }
}

LONG DumpCrashHeader(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile)
{
  LONG ret = 0;
  
  DumpExceptionHeader(pExceptPtrs, hLogFile);
  
  //dc_OutputDbgStr(_T("Dump SystemInformation\n"));
  DumpSystemInformation(hLogFile);
  DumpThreads(GetCurrentProcessId(), hLogFile);
  
  PrintAccessViolationInfo(pExceptPtrs, hLogFile);
  DumpRegisters(pExceptPtrs, hLogFile);
  DumpEIP(pExceptPtrs, hLogFile);
    
  return ret;
}


VOID DumpCrashStack(PEXCEPTION_POINTERS pExceptPtrs, HANDLE hLogFile)
{
  DumpStack(pExceptPtrs, hLogFile);
}

VOID DumpModuleListLog(HANDLE hLogFile)
{
  DumpModuleList(hLogFile);
}

