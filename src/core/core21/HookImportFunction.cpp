#include "stdafx.h"

#define MakePtr(cast, ptr, AddValue) (cast)((DWORD)(ptr)+(DWORD)(AddValue))


static PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(HMODULE hModule, LPCSTR pszModuleName)
{
  if(hModule == NULL)
    hModule = GetModuleHandle(NULL);
  
  // Always check parameters.
  ASSERT(pszModuleName);
  ASSERT(hModule);
  if ((pszModuleName == NULL) || (hModule == NULL))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return NULL;
  }

  // Get the Dos header.
  PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER) hModule;

  // Is this the MZ header?
  if (IsBadReadPtr(pDOSHeader, sizeof(IMAGE_DOS_HEADER)) || (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return NULL;
  }

  // Get the PE header.
  PIMAGE_NT_HEADERS pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader->e_lfanew);

  // Is this a real PE image?
  if (IsBadReadPtr(pNTHeader, sizeof(IMAGE_NT_HEADERS)) || (pNTHeader->Signature != IMAGE_NT_SIGNATURE))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return NULL;
  }

  // If there is no imports section, leave now.
  if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0)
    return NULL;

  // Get the pointer to the imports section.
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, pDOSHeader,
                                                 pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].
                                                 VirtualAddress);

  // Loop through the import module descriptors looking for the module whose name matches szImportMod.
  while (pImportDesc->Name)
  {
    PSTR szCurrMod = MakePtr(PSTR, pDOSHeader, pImportDesc->Name);

    if (stricmp(szCurrMod, pszModuleName) == 0)
      break; // Found it.
    
    // Look at the next one.
    pImportDesc++;
  }

  // If the name is NULL, then the module is not imported.
  if (pImportDesc->Name == NULL)
    return NULL;

  // All OK, Jumpmaster!
  return pImportDesc;
}

static BOOL INLINE IsNT()
{
  OSVERSIONINFO stOSVI;
  memset(&stOSVI, NULL, sizeof(OSVERSIONINFO));
  stOSVI.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  BOOL bRet = GetVersionEx(&stOSVI);
  ASSERT(TRUE == bRet);
  if (FALSE == bRet)
  {
    TRACE0("GetVersionEx failed!\n");
    return FALSE;
  }
  
  // Check the version and call the appropriate thing.
  return (VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId);
}

BOOL CHookFunction_i::HookFunctions(HMODULE hModule, 
                                    LPCTSTR pszModuleName, // the module name loaded in process.
                                    HOOK_FUNC_DESC* pHookInfo,
                                    bool bFullMatch)
{
  if(hModule == NULL)
  {
    // Get Current module at default.
    hModule = GetModuleHandle(NULL);
  }

 // Do the parameter validation for real.
  if (pszModuleName == NULL ||
      (IsBadReadPtr(pHookInfo, sizeof(HOOK_FUNC_DESC)*1)))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return FALSE;
  }

  // Is this a system DLL, which Windows95 will not let you patch
  //  since it is above the 2GB line?
  if (!IsNT() && ((DWORD)hModule >= 0x80000000))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_HANDLE, SLE_ERROR);
    return FALSE;
  }
  
  // TODO TODO
  //  Should each item in the hook array be checked in release builds?

  // Get the specific import descriptor.
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = GetImportDescriptor(hModule, pszModuleName);
  if (pImportDesc == NULL)
    return FALSE; // The requested module was not imported.

  // Get the original thunk information for this DLL.  I cannot use
  //  the thunk information stored in the pImportDesc->FirstThunk
  //  because the that is the array that the loader has already
  //  bashed to fix up all the imports.  This pointer gives us acess
  //  to the function names.
  PIMAGE_THUNK_DATA pOrigThunk = MakePtr(PIMAGE_THUNK_DATA, hModule, pImportDesc->OriginalFirstThunk);

  // Get the array pointed to by the pImportDesc->FirstThunk.  This is
  //  where I will do the actual bash.
  PIMAGE_THUNK_DATA pRealThunk = MakePtr(PIMAGE_THUNK_DATA, hModule, pImportDesc->FirstThunk);
  
  // Loop through and look for the one that matches the name.
  while (pOrigThunk->u1.Function)
  {
    // Only look at those that are imported by name, not ordinal.
    if (IMAGE_ORDINAL_FLAG != (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG))
    {
      // Look get the name of this imported function.
      PIMAGE_IMPORT_BY_NAME pByName = MakePtr(PIMAGE_IMPORT_BY_NAME, hModule, pOrigThunk->u1.AddressOfData);

      // If the name starts with NULL, then just skip out now.
      if (_T('\0') == pByName->Name[0])
        continue;

      // Determines if we do the hook.
      BOOL bDoHook = FALSE;
      UINT hookidx = 0;

      // TODO TODO
      //  Might want to consider bsearch here.
      
      // See if the particular function name is in the import
      //  list.  It might be good to consider requiring the
      //  paHookArray to be in sorted order so bsearch could be
      //  used so the lookup will be faster.  However, the size of
      //  uiCount coming into this function should be rather
      //  small but it is called for each function imported by
      //  szImportMod.
      for (UINT i = 0; i<1; i++)
      {
        if(pHookInfo->szFuncName[0] == _T('\0') || pHookInfo->pHookFunc==NULL)
        {
          continue;
        }

        if(!bFullMatch)
        {
          CString str = pByName->Name;
          if(str.Find(pHookInfo->szFuncName) == -1)
          {
            continue;
          }
          else
          {
            // If the proc is NULL, kick out, otherwise go
            //  ahead and hook it.
            if (pHookInfo->pHookFunc)
            {
              bDoHook = TRUE;
              hookidx = i;
            }
            break;
          }
        }
        else 
        {
          if ((pHookInfo->szFuncName[0] == pByName->Name[0]) &&
            (strcmpi(pHookInfo->szFuncName, (char*)pByName->Name) == 0))
          {
            // If the proc is NULL, kick out, otherwise go
            //  ahead and hook it.
            if (pHookInfo->pHookFunc)
            {
              bDoHook = TRUE;
              hookidx = i;
            }
            break;
          }
        } 
      } // for 
      
      if (bDoHook)
      {
        pHookInfo->pOrigFunc = NULL;
        // I found it.  Now I need to change the protection to
        //  writable before I do the blast.  Note that I am now
        //  blasting into the real thunk area!
        MEMORY_BASIC_INFORMATION mbi_thunk;
        VirtualQuery(pRealThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
        VERIFY(VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect));
        
        // Save the original address if requested.
        pHookInfo->pOrigFunc   = (VOID*)pRealThunk->u1.Function;
        pHookInfo->pszOrigFunc = (char*)(&pByName->Name[0]);
        
        // Do the actual hook.
        pRealThunk->u1.Function = (PDWORD)pHookInfo->pHookFunc;
        
        // Change the protection back to what it was before I blasted.
        DWORD dwOldProtect;
        VERIFY(VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect));
        break;
      }
    }

    // Increment both tables.
    pOrigThunk++;
    pRealThunk++;
  } // while 

  // All OK, JumpMaster!
  SetLastError(ERROR_SUCCESS);
  return TRUE;
}


BOOL CHookFunction_i::UnHookFunctions(HMODULE hModule, LPCSTR pszModuleName, 
                                    HOOK_FUNC_DESC* pHookInfo,
                                    bool bFullMatch)
{
  if(hModule == NULL)
  {
    // Get Current module at default.
    hModule = GetModuleHandle(NULL);
  }

  // Do the parameter validation for real.
  if ( (pszModuleName == NULL) ||
      (IsBadReadPtr(pHookInfo, sizeof(HOOK_FUNC_DESC)*1)))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return FALSE;
  }

  // Is this a system DLL, which Windows95 will not let you patch
  //  since it is above the 2GB line?
  if (!IsNT() && ((DWORD)hModule >= 0x80000000))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_HANDLE, SLE_ERROR);
    return FALSE;
  }
  
  // TODO TODO
  //  Should each item in the hook array be checked in release builds?

  // Get the specific import descriptor.
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = GetImportDescriptor(hModule, pszModuleName);
  if (pImportDesc == NULL)
    return FALSE; // The requested module was not imported.

  // Get the original thunk information for this DLL.  I cannot use
  //  the thunk information stored in the pImportDesc->FirstThunk
  //  because the that is the array that the loader has already
  //  bashed to fix up all the imports.  This pointer gives us acess
  //  to the function names.
  PIMAGE_THUNK_DATA pOrigThunk = MakePtr(PIMAGE_THUNK_DATA, hModule, pImportDesc->OriginalFirstThunk);

  // Get the array pointed to by the pImportDesc->FirstThunk.  This is
  //  where I will do the actual bash.
  PIMAGE_THUNK_DATA pRealThunk = MakePtr(PIMAGE_THUNK_DATA, hModule, pImportDesc->FirstThunk);

  // Loop through and look for the one that matches the name.
  while (pOrigThunk->u1.Function)
  {
    // Only look at those that are imported by name, not ordinal.
    if (IMAGE_ORDINAL_FLAG != (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG))
    {
      // Look get the name of this imported function.
      PIMAGE_IMPORT_BY_NAME pByName = MakePtr(PIMAGE_IMPORT_BY_NAME, hModule, pOrigThunk->u1.AddressOfData);

      // If the name starts with NULL, then just skip out now.
      if (_T('\0') == pByName->Name[0])
        continue;

      if(pHookInfo->pHookFunc == pOrigThunk->u1.Function)
      {
        // I found it.  Now I need to change the protection to
        //  writable before I do the blast.  Note that I am now
        //  blasting into the real thunk area!
        MEMORY_BASIC_INFORMATION mbi_thunk;
        VirtualQuery(pRealThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
        VERIFY(VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect));
        
        // Do the actual hook.
        pRealThunk->u1.Function = (PDWORD)pHookInfo->pOrigFunc;
        pHookInfo->pHookFunc = NULL;
        
        // Change the protection back to what it was before I blasted.
        DWORD dwOldProtect;
        VERIFY(VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect));
        break;
      }
    } // if 

    // Increment both tables.
    pOrigThunk++;
    pRealThunk++;
  } // while 

  // All OK, JumpMaster!
  SetLastError(ERROR_SUCCESS);

  return TRUE;
}



#if 0 

TESTDLL_API void InstallHook();
TESTDLL_API void RemoveHook();
struct SFunctionHook
{
    char* Name;         // Function name, e.g. "DirectDrawCreateEx".
    void* HookFn;       // Address of your function.
    void* OrigFn;       // Stored by HookAPICalls, the address of the original function.
};

struct SDLLHook
{
    // Name of the DLL, e.g. "DDRAW.DLL"
    char* Name;

    // Set true to call the default for all non-hooked functions before they are executed.
    bool UseDefault;
    void* DefaultFn;

    // Function hook array.  Terminated with a NULL Name field.
    SFunctionHook Functions[];
};

// Hook functions one or more DLLs.
TESTDLL_API bool HookAPICalls( SDLLHook* Hook ); 


/*--------------------------------------------------------------------------------------------------------
  APIHIJACK.H - Based on DelayLoadProfileDLL.CPP, by Matt Pietrek for MSJ February 2000.
  http://msdn.microsoft.com/library/periodic/period00/hood0200.htm
  Adapted by Wade Brainerd, wadeb@wadeb.com
--------------------------------------------------------------------------------------------------------*/
#ifndef APIHIJACK_H
#define APIHIJACK_H

#pragma warning(disable:4200)

// Macro for convenient pointer addition.
// Essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr(cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))

// Default Hook Stub Structure: Contains data about the original function, Name/Ordinal, Address 
// and a Count field.  This is actually a block of assembly code.
#pragma pack( push, 1 )
struct DLPD_IAT_STUB
{
    BYTE    instr_CALL;
    DWORD   data_call;
    BYTE    instr_JMP;
    DWORD   data_JMP;
    DWORD   count;
    DWORD   pszNameOrOrdinal;

    DLPD_IAT_STUB() : instr_CALL( 0xE8 ), instr_JMP( 0xE9 ), count( 0 ) {}
};
#pragma pack( pop )

// Example DefaultHook procedure, called from the DLPD_IAT_STUB stubs.  
// Increments "count" field of the stub.
// See the implementation for more information.
void __cdecl DefaultHook( PVOID dummy );

#endif

/*--------------------------------------------------------------------------------------------------------
    APIHIJACK.CPP - Based on DelayLoadProfileDLL.CPP, by Matt Pietrek for MSJ February 2000.
    http://msdn.microsoft.com/library/periodic/period00/hood0200.htm
    Adapted by Wade Brainerd, wadeb@wadeb.com
--------------------------------------------------------------------------------------------------------*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "apihijack.h"
#include "TestDLL\testdll.h"

//===========================================================================
// Called from the DLPD_IAT_STUB stubs.  Increments "count" field of the stub

void __cdecl DefaultHook( PVOID dummy )
{
    __asm   pushad  // Save all general purpose registers

    // Get return address, then subtract 5 (size of a CALL X instruction)
    // The result points at a DLPD_IAT_STUB

    // pointer math!  &dummy-1 really subtracts sizeof(PVOID)
    PDWORD pRetAddr = (PDWORD)(&dummy - 1);

    DLPD_IAT_STUB * pDLPDStub = (DLPD_IAT_STUB *)(*pRetAddr - 5);

    pDLPDStub->count++;

    #if 0
    // Remove the above conditional to get a cheezy API trace from
    // the loader process.  It's slow!
    if ( !IMAGE_SNAP_BY_ORDINAL( pDLPDStub->pszNameOrOrdinal) )
    {
        OutputDebugString( "Called hooked function: " );
        OutputDebugString( (PSTR)pDLPDStub->pszNameOrOrdinal );
        OutputDebugString( "\n" );
    }
    #endif

    __asm   popad   // Restore all general purpose registers
}

// This function must be __cdecl!!!
void __cdecl DelayLoadProfileDLL_UpdateCount( PVOID dummy );

PIMAGE_IMPORT_DESCRIPTOR g_pFirstImportDesc;

//===========================================================================
// Given an HMODULE, returns a pointer to the PE header

PIMAGE_NT_HEADERS PEHeaderFromHModule(HMODULE hModule)
{
    PIMAGE_NT_HEADERS pNTHeader = 0;
    
    __try
    {
        if ( PIMAGE_DOS_HEADER(hModule)->e_magic != IMAGE_DOS_SIGNATURE )
            __leave;

        pNTHeader = PIMAGE_NT_HEADERS(PBYTE(hModule)
                    + PIMAGE_DOS_HEADER(hModule)->e_lfanew);
        
        if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
            pNTHeader = 0;
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {       
    }

    return pNTHeader;
}

//===========================================================================
// Builds stubs for and redirects the IAT for one DLL (pImportDesc)

bool RedirectIAT( SDLLHook* DLLHook, PIMAGE_IMPORT_DESCRIPTOR pImportDesc, PVOID pBaseLoadAddr )
{
    PIMAGE_THUNK_DATA pIAT;     // Ptr to import address table
    PIMAGE_THUNK_DATA pINT;     // Ptr to import names table
    PIMAGE_THUNK_DATA pIteratingIAT;

    // Figure out which OS platform we're on
    OSVERSIONINFO osvi; 
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionEx( &osvi );

    // If no import names table, we can't redirect this, so bail
    if ( pImportDesc->OriginalFirstThunk == 0 )
        return false;

    pIAT = MakePtr( PIMAGE_THUNK_DATA, pBaseLoadAddr, pImportDesc->FirstThunk );
    pINT = MakePtr( PIMAGE_THUNK_DATA, pBaseLoadAddr, pImportDesc->OriginalFirstThunk );

    // Count how many entries there are in this IAT.  Array is 0 terminated
    pIteratingIAT = pIAT;
    unsigned cFuncs = 0;
    while ( pIteratingIAT->u1.Function )
    {
        cFuncs++;
        pIteratingIAT++;
    }

    if ( cFuncs == 0 )  // If no imported functions, we're done!
        return false;

    // These next few lines ensure that we'll be able to modify the IAT,
    // which is often in a read-only section in the EXE.
    DWORD flOldProtect, flNewProtect, flDontCare;
    MEMORY_BASIC_INFORMATION mbi;
    
    // Get the current protection attributes                            
    VirtualQuery( pIAT, &mbi, sizeof(mbi) );
    
    // remove ReadOnly and ExecuteRead attributes, add on ReadWrite flag
    flNewProtect = mbi.Protect;
    flNewProtect &= ~(PAGE_READONLY | PAGE_EXECUTE_READ);
    flNewProtect |= (PAGE_READWRITE);
    
    if ( !VirtualProtect(   pIAT, sizeof(PVOID) * cFuncs,
                            flNewProtect, &flOldProtect) )
    {
        return false;
    }

    // If the Default hook is enabled, build an array of redirection stubs in the processes memory.
    DLPD_IAT_STUB * pStubs = 0;
    if ( DLLHook->UseDefault )
    {
        // Allocate memory for the redirection stubs.  Make one extra stub at the
        // end to be a sentinel
        pStubs = new DLPD_IAT_STUB[ cFuncs + 1];
        if ( !pStubs )
            return false;
    }

    // Scan through the IAT, completing the stubs and redirecting the IAT
    // entries to point to the stubs
    pIteratingIAT = pIAT;

    while ( pIteratingIAT->u1.Function )
    {
        void* HookFn = 0;  // Set to either the SFunctionHook or pStubs.

        if ( !IMAGE_SNAP_BY_ORDINAL( pINT->u1.Ordinal ) )  // import by name
        {
            PIMAGE_IMPORT_BY_NAME pImportName = MakePtr( PIMAGE_IMPORT_BY_NAME, pBaseLoadAddr, pINT->u1.AddressOfData );

            // Iterate through the hook functions, searching for this import.
            SFunctionHook* FHook = DLLHook->Functions;
            while ( FHook->Name )
            {
                if ( lstrcmpi( FHook->Name, (char*)pImportName->Name ) == 0 )
                {
                    OutputDebugString( "Hooked function: " );
                    OutputDebugString( (char*)pImportName->Name );
                    OutputDebugString( "\n" );
                    
                    // Save the old function in the SFunctionHook structure and get the new one.
                    FHook->OrigFn = pIteratingIAT->u1.Function;
                    HookFn = FHook->HookFn;
                    break;
                }

                FHook++;
            }

            // If the default function is enabled, store the name for the user.
            if ( DLLHook->UseDefault )
                pStubs->pszNameOrOrdinal = (DWORD)&pImportName->Name;
        }
        else
        {
            // If the default function is enabled, store the ordinal for the user.
            if ( DLLHook->UseDefault )
                pStubs->pszNameOrOrdinal = pINT->u1.Ordinal;
        }

        // If the default function is enabled, fill in the fields to the stub code.
        if ( DLLHook->UseDefault )
        {
            pStubs->data_call = (DWORD)(PDWORD)DLLHook->DefaultFn
                                - (DWORD)(PDWORD)&pStubs->instr_JMP;
            pStubs->data_JMP = *(PDWORD)pIteratingIAT - (DWORD)(PDWORD)&pStubs->count;

            // If it wasn't manually hooked, use the Stub function.
            if ( !HookFn )
                HookFn = (void*)pStubs;
        }

        // Replace the IAT function pointer if we have a hook.
        if ( HookFn )
        {
            // Cheez-o hack to see if what we're importing is code or data.
            // If it's code, we shouldn't be able to write to it
            if ( IsBadWritePtr( (PVOID)pIteratingIAT->u1.Function, 1 ) )
            {
                pIteratingIAT->u1.Function = (PDWORD)HookFn;
            }
            else if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
            {
                // Special hack for Win9X, which builds stubs for imported
                // functions in system DLLs (Loaded above 2GB).  These stubs are
                // writeable, so we have to explicitly check for this case
                if ( pIteratingIAT->u1.Function > (PDWORD)0x80000000 )
                    pIteratingIAT->u1.Function = (PDWORD)HookFn;
            }
        }

        if ( DLLHook->UseDefault )
            pStubs++;           // Advance to next stub

        pIteratingIAT++;    // Advance to next IAT entry
        pINT++;             // Advance to next INT entry
    }

    if ( DLLHook->UseDefault )
        pStubs->pszNameOrOrdinal = 0;   // Final stub is a sentinel

    // Put the page attributes back the way they were.
    VirtualProtect( pIAT, sizeof(PVOID) * cFuncs, flOldProtect, &flDontCare);
    
    return true;
}

//===========================================================================
// Top level routine to find the EXE's imports, and redirect them
bool HookAPICalls( SDLLHook* Hook )
{
    if ( !Hook )
        return false;

    HMODULE hModEXE = GetModuleHandle( 0 );

    PIMAGE_NT_HEADERS pExeNTHdr = PEHeaderFromHModule( hModEXE );
    
    if ( !pExeNTHdr )
        return false;

    DWORD importRVA = pExeNTHdr->OptionalHeader.DataDirectory
                        [IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if ( !importRVA )
        return false;

    // Convert imports RVA to a usable pointer
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MakePtr( PIMAGE_IMPORT_DESCRIPTOR,
                                                    hModEXE, importRVA );

    // Save off imports address in a global for later use
    g_pFirstImportDesc = pImportDesc;   

    // Iterate through each import descriptor, and redirect if appropriate
    while ( pImportDesc->FirstThunk )
    {
        PSTR pszImportModuleName = MakePtr( PSTR, hModEXE, pImportDesc->Name);

        if ( lstrcmpi( pszImportModuleName, Hook->Name ) == 0 )
        {
            OutputDebugString( "Found " );
            OutputDebugString( Hook->Name );
            OutputDebugString( "...\n" );

            RedirectIAT( Hook, pImportDesc, (PVOID)hModEXE );
        }
        
        pImportDesc++;  // Advance to next import descriptor
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////
// Test Code:
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "..\testdll\testdll.h"

// Function pointer types.
typedef int (WINAPI* MessageBoxW_Type)(
    HWND hWnd ,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType);

HRESULT WINAPI MyMessageBox( HWND hWnd ,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType);

SDLLHook g_DllHook = 
{
  "user32.dll",
  false, NULL,		// Default hook disabled, NULL function pointer.
  {
    { "MessageBoxA", MyMessageBox },
    { NULL, NULL }
  }
};


// Hook function.
HRESULT WINAPI MyMessageBox( HWND hWnd ,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType)
{
    OutputDebugString( "TESTDLL: MyDirectDrawCreateEx called.\n" );
    
    MessageBoxW_Type OldFn = (MessageBoxW_Type)g_DllHook.Functions[0].OrigFn;
    
    return OldFn( hWnd, lpText, lpCaption, uType );
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    HookAPICalls(&g_DllHook);
    
    InstallHook();
    
    MessageBox( NULL, "Hook installed.  Press OK to remove and exit.", "TestLauncher", MB_OK );

    RemoveHook();

    MessageBox( NULL, "Hook installed.  Press OK to remove and exit.", "TestLauncher", MB_OK );
   
    return 0;
}


#endif 