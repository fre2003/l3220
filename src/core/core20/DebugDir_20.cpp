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

///////////////////////////////////////////////////////////////////////////////
//
// DebugDir.cpp 
//
// DebugDir example source code 
// 
// Author: Oleg Starodumov (www.debuginfo.com)
//
//


///////////////////////////////////////////////////////////////////////////////
// Include files 
//

#include <stdafx.h>
#include <tchar.h>

#include <crtdbg.h>
#include <stdio.h>
#include <limits.h>

#undef _tprintf
#define _tprintf 
#undef printf
#define printf 

#undef DWORD_PTR
#define  DWORD_PTR DWORD


// Helper macros 
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))


///////////////////////////////////////////////////////////////////////////////
// CodeView debug information structures 
//

#define CV_SIGNATURE_NB10   '01BN'
#define CV_SIGNATURE_RSDS   'SDSR'


// CodeView header 
struct CV_HEADER 
{
	DWORD CvSignature; // NBxx
	LONG  Offset;      // Always 0 for NB10
};

// CodeView NB10 debug information 
// (used when debug information is stored in a PDB 2.00 file) 
struct CV_INFO_PDB20 
{
	CV_HEADER  Header; 
	DWORD      Signature;       // seconds since 01.01.1970
	DWORD      Age;             // an always-incrementing value 
	BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file 
};

// CodeView RSDS debug information 
// (used when debug information is stored in a PDB 7.00 file) 
struct CV_INFO_PDB70 
{
	DWORD      CvSignature; 
	GUID       Signature;       // unique identifier 
	DWORD      Age;             // an always-incrementing value 
	BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file 
};


///////////////////////////////////////////////////////////////////////////////
// Function declarations 
//

LPCTSTR ProcessCmdLine( int argc, TCHAR* argv[] ); 
static bool CheckDosHeader( PIMAGE_DOS_HEADER pDosHeader ); 
static bool CheckNtHeaders( PIMAGE_NT_HEADERS pNtHeaders ); 
static bool CheckSectionHeaders( PIMAGE_NT_HEADERS pNtHeaders ); 
static bool CheckDebugDirectory( PIMAGE_DEBUG_DIRECTORY pDebugDir, DWORD DebugDirSize ); 
static bool IsPE32Plus( PIMAGE_OPTIONAL_HEADER pOptionalHeader, bool& bPE32Plus ); 
static bool GetDebugDirectoryRVA( PIMAGE_OPTIONAL_HEADER pOptionalHeader, DWORD& DebugDirRva, DWORD& DebugDirSize );
static bool GetFileOffsetFromRVA( PIMAGE_NT_HEADERS pNtHeaders, DWORD Rva, DWORD& FileOffset ); 
static void DumpDebugDirectoryEntries( LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir,
                                      DWORD DebugDirSize, OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo); 
static void DumpDebugDirectoryEntry( LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir, 
                                    OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo); 
static void DumpCodeViewDebugInfo( LPBYTE pDebugInfo, DWORD DebugInfoSize, OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo ); 
static void DumpMiscDebugInfo( LPBYTE pDebugInfo, DWORD DebugInfoSize, OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo ); 
static LPCTSTR DebugTypeToStr( DWORD DebugType ); 


///////////////////////////////////////////////////////////////////////////////
// main 
//

CFileMapping::CFileMapping()
{
  m_hFile      = NULL; 
  m_hFileMap   = NULL; 
  m_lpFileMem  = NULL; 
}

CFileMapping::~CFileMapping()
{
  // Cleanup 
  if( m_lpFileMem != 0 ) 
  {
    if( !UnmapViewOfFile( m_lpFileMem ) ) 
    {
      _tprintf( _T("Error: Cannot unmap the file. Error code: %u \n"), GetLastError() );  
    }
  }
  
  if( m_hFileMap != NULL ) 
  {
    if( !CloseHandle( m_hFileMap ) ) 
    {
      _tprintf( _T("Error: Cannot close the file mapping object. Error code: %u \n"), GetLastError() );  
    }
  }
  
  if( ( m_hFile != NULL ) && ( m_hFile != INVALID_HANDLE_VALUE ) ) 
  {
    if( !CloseHandle( m_hFile ) )
    {
      _tprintf( _T("Error: Cannot close the file. Error code: %u \n"), GetLastError() );  
    }
  }
}


LPVOID CFileMapping::GetReadOnlyFileBuffer(LPCTSTR pszFileName)
{
		m_hFile = CreateFile( pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ); 
    
    if( ( m_hFile == INVALID_HANDLE_VALUE ) || ( m_hFile == NULL ) ) 
    {
      _tprintf( _T("Error: Cannot open the file. Error code: %u \n"), GetLastError() );  
    }
    
    m_hFileMap = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL ); 
    
    if( m_hFileMap == NULL ) 
    {
      _tprintf( _T("Error: Cannot open the file mapping object. Error code: %u \n"), GetLastError() );  
    }
    
    m_lpFileMem = MapViewOfFile( m_hFileMap, FILE_MAP_READ, 0, 0, 0 ); 
    
    if( m_lpFileMem == 0 ) 
    {
      _tprintf( _T("Error: Cannot map the file. Error code: %u \n"), GetLastError() );  
    }    
    
    return m_lpFileMem;
}
   
//    
// Dump the section table from a PE file or an OBJ    
//    
static void DumpSectionTable(PIMAGE_SECTION_HEADER section,   
                      unsigned cSections,   
                      BOOL IsEXE, 
                      CProcessUtil_i::DEBUG_INFO* pstDebugInfo)   
{   
    printf("Section Table\n");   
    
    for ( unsigned i = 0; i <= 10; i++, section++ )
    {
      pstDebugInfo->szSectionNameArray[i][0] = 0;
    }

    for ( i=1; i <= cSections; i++, section++ )   
    {
      if(cSections < 10)
      {
        strcpy(pstDebugInfo->szSectionNameArray[i-1], (const char*)section->Name);
      }
//         printf( "  %02X %-8.8s  %s: %08X  VirtAddr:  %08X\n",   
//                 i, section->Name,   
//                 IsEXE ? "VirtSize" : "PhysAddr",   
//                 section->Misc.VirtualSize, section->VirtualAddress);   
//         printf( "    raw data offs:   %08X  raw data size: %08X\n",   
//                 section->PointerToRawData, section->SizeOfRawData );   
//         printf( "    relocation offs: %08X  relocations:   %08X\n",   
//                 section->PointerToRelocations, section->NumberOfRelocations );   
//         printf( "    line # offs:     %08X  line #'s:      %08X\n",   
//                 section->PointerToLinenumbers, section->NumberOfLinenumbers );   
//         printf( "    characteristics: %08X\n", section->Characteristics);   
//         
//         printf("\n\n");   
    }   
}   


bool CProcessUtil_i::HasDebugInfo(LPCTSTR pszPeFileName, OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo) 
{
  bool ret = FALSE;
  
 	if( pszPeFileName == 0 ) 
    return FALSE; 
  
  CFileMapping ReadOnlyFileMap;  
  LPVOID pFileBuffer = NULL;
  
  do 
  {
    // Open the file and map it into memory 
    pFileBuffer = ReadOnlyFileMap.GetReadOnlyFileBuffer(pszPeFileName);
    
    if(pFileBuffer == NULL)
      break;
    
    // Is it a valid PE executable ? 
    
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer; 
    
    if( !CheckDosHeader( pDosHeader ) )
    {
      _tprintf( _T("Error: File is not a PE executable.\n") ); 
      break; 
    }
    
    PIMAGE_NT_HEADERS pNtHeaders = MakePtr( PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew );
    
    if( !CheckNtHeaders( pNtHeaders ) ) 
    {
      _tprintf( _T("Error: File is not a PE executable.\n") ); 
      break; 
    }
    
    if( !CheckSectionHeaders( pNtHeaders ) ) 
    {
      _tprintf( _T("Error: File is not a PE executable.\n") ); 
      break; 
    }
    
    // Look up the debug directory 
    
    DWORD DebugDirRva   = 0; 
    DWORD DebugDirSize  = 0; 
    
    if( !GetDebugDirectoryRVA( &pNtHeaders->OptionalHeader, DebugDirRva, DebugDirSize ) ) 
    {
      _tprintf( _T("Error: File is not a PE executable.\n") ); 
      break; 
    }
    
    if( ( DebugDirRva == 0 ) || ( DebugDirSize == 0 ) ) 
    {
      _tprintf( _T("Debug directory not found.\n") ); 
      break; 
    }
    
    DWORD DebugDirOffset = 0; 
    
    if( !GetFileOffsetFromRVA( pNtHeaders, DebugDirRva, DebugDirOffset ) ) 
    {
      _tprintf( _T("Debug directory not found.\n") ); 
      break; 
    }
    
    PIMAGE_DEBUG_DIRECTORY pDebugDir = MakePtr( PIMAGE_DEBUG_DIRECTORY, pFileBuffer, DebugDirOffset ); 
    
    if( !CheckDebugDirectory( pDebugDir, DebugDirSize ) ) 
    {
      _tprintf( _T("Error: Debug directory is not valid.\n") ); 
      break; 
    }
    
    // Display information about every entry in the debug directory 
    if(DebugDirSize)
    {
      ret = TRUE;
    }
    
    
    // output debug info.
    if(pstDebugInfo)
    {
      DumpSectionTable( IMAGE_FIRST_SECTION(pNtHeaders), pNtHeaders->FileHeader.NumberOfSections, TRUE, 
        pstDebugInfo);  
      
      DumpDebugDirectoryEntries( (LPBYTE)pFileBuffer, pDebugDir, DebugDirSize, pstDebugInfo);
    }
    
  } while( 0 ); 
  
  return ret; 
}


///////////////////////////////////////////////////////////////////////////////
// Functions 
//

// 
// Process command line and display usage information, if necessary 
// 
// Return value: If command line parameters are correct, the function 
//   returns a pointer to the file name specified by the user. 
//   If command line parameters are incorrect, the function returns null. 
// 
static LPCTSTR ProcessCmdLine( int argc, TCHAR* argv[] ) 
{
	if( argc < 2 ) 
	{
		_tprintf( _T("Usage: %s FileName \n"), argv[0] ); 
		return 0; 
	}

	return argv[1]; 
}


// 
// Check IMAGE_DOS_HEADER and determine whether the file is a PE executable 
// (according to the header contents) 
// 
// Return value: "true" if the header is valid and the file is a PE executable, 
//   "false" otherwise 
// 
static bool CheckDosHeader( PIMAGE_DOS_HEADER pDosHeader ) 
{
	// Check whether the header is valid and belongs to a PE executable 

	if( pDosHeader == 0 ) 
	{
		_ASSERT( 0 ); 
		return false; 
	}

	if( IsBadReadPtr( pDosHeader, sizeof(IMAGE_DOS_HEADER) ) )
	{
		// Invalid header 
		return false; 
	}

	if( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE ) 
	{
		// Not a PE executable 
		return false; 
	}

	return true; 
}

// 
// Check IMAGE_NT_HEADERS and determine whether the file is a PE executable 
// (according to the headers' contents) 
// 
// Return value: "true" if the headers are valid and the file is a PE executable, 
//   "false" otherwise 
// 
static bool CheckNtHeaders( PIMAGE_NT_HEADERS pNtHeaders ) 
{
	// Check the signature 

	if( pNtHeaders == 0 ) 
	{
		_ASSERT( 0 ); 
		return false; 
	}

	if( IsBadReadPtr( pNtHeaders, sizeof(pNtHeaders->Signature) ) ) 
	{
		// Invalid header 
		return false; 
	}

	if( pNtHeaders->Signature != IMAGE_NT_SIGNATURE ) 
	{
		// Not a PE executable 
		return false; 
	}


	// Check the file header 

	if( IsBadReadPtr( &pNtHeaders->FileHeader, sizeof(IMAGE_FILE_HEADER) ) )
	{
		// Invalid header 
		return false; 
	}

	if( IsBadReadPtr( &pNtHeaders->OptionalHeader, pNtHeaders->FileHeader.SizeOfOptionalHeader ) ) 
	{
		// Invalid size of the optional header 
		return false; 
	}


	// Determine the format of the header 

		// If true, PE32+, otherwise PE32
	bool bPE32Plus = false; 

	if( !IsPE32Plus( &pNtHeaders->OptionalHeader, bPE32Plus ) ) 
	{
		// Probably invalid IMAGE_OPTIONAL_HEADER.Magic 
		return false; 
	}


	// Complete 

	return true; 
}

// 
// Lookup the section headers and check whether they are valid 
// 
// Return value: "true" if the headers are valid, "false" otherwise 
// 
static bool CheckSectionHeaders( PIMAGE_NT_HEADERS pNtHeaders ) 
{
	if( pNtHeaders == 0 ) 
	{
		_ASSERT( 0 ); 
		return false; 
	}

	PIMAGE_SECTION_HEADER pSectionHeaders = IMAGE_FIRST_SECTION( pNtHeaders ); 

	if( IsBadReadPtr( pSectionHeaders, pNtHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER) ) ) 
	{
		// Invalid header 
		return false; 
	}

	return true; 
}

// 
// Checks whether the debug directory is valid 
// 
// Return value: "true" if the debug directory is valid, "false" if it is not 
// 
static bool CheckDebugDirectory( PIMAGE_DEBUG_DIRECTORY pDebugDir, DWORD DebugDirSize ) 
{
	if( ( pDebugDir == 0 ) || ( DebugDirSize == 0 ) ) 
	{
		_ASSERT( 0 ); 
		return false; 
	}

	if( IsBadReadPtr( pDebugDir, DebugDirSize ) ) 
	{
		// Invalid debug directory 
		return false; 
	}

	if( DebugDirSize < sizeof(IMAGE_DEBUG_DIRECTORY) ) 
	{
		// Invalid size of the debug directory 
		return false; 
	}

	return true; 
}

// 
// Check whether the specified IMAGE_OPTIONAL_HEADER belongs to 
// a PE32 or PE32+ file format 
// 
// Return value: "true" if succeeded (bPE32Plus contains "true" if the file 
//  format is PE32+, and "false" if the file format is PE32), 
//  "false" if failed 
// 
static bool IsPE32Plus( PIMAGE_OPTIONAL_HEADER pOptionalHeader, bool& bPE32Plus ) 
{
	// Note: The function does not check the header for validity. 
	// It assumes that the caller has performed all the necessary checks. 

	// IMAGE_OPTIONAL_HEADER.Magic field contains the value that allows 
	// to distinguish between PE32 and PE32+ formats 

	if( pOptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC ) 
	{
		// PE32 
		bPE32Plus = false; 
	}
	else if( pOptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC ) 
	{
		// PE32+
		bPE32Plus = true; 
	}
	else 
	{
		// Unknown value -> Report an error 
		bPE32Plus = false; 
		return false; 
	}

	return true; 

}

// 
// Returns (in [out] parameters) the RVA and size of the debug directory, 
// using the information in IMAGE_OPTIONAL_HEADER.DebugDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG]
// 
// Return value: "true" if succeeded, "false" if failed
// 
static bool GetDebugDirectoryRVA( PIMAGE_OPTIONAL_HEADER pOptionalHeader, DWORD& DebugDirRva, DWORD& DebugDirSize ) 
{
	// Check parameters 

	if( pOptionalHeader == 0 ) 
	{
		_ASSERT( 0 ); 
		return false; 
	}


	// Determine the format of the PE executable 

	bool bPE32Plus = false; 

	if( !IsPE32Plus( pOptionalHeader, bPE32Plus ) ) 
	{
		// Probably invalid IMAGE_OPTIONAL_HEADER.Magic
		return false; 
	}


	// Obtain the debug directory RVA and size 

	if( bPE32Plus ) 
	{
		PIMAGE_OPTIONAL_HEADER64 pOptionalHeader64 = (PIMAGE_OPTIONAL_HEADER64)pOptionalHeader; 

		DebugDirRva = pOptionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress; 

		DebugDirSize = pOptionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size; 
	}
	else 
	{
		PIMAGE_OPTIONAL_HEADER32 pOptionalHeader32 = (PIMAGE_OPTIONAL_HEADER32)pOptionalHeader; 

		DebugDirRva = pOptionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress; 

		DebugDirSize = pOptionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size; 
	}

	if( ( DebugDirRva == 0 ) && ( DebugDirSize == 0 ) ) 
	{
		// No debug directory in the executable -> no debug information 
		return true; 
	}
	else if( ( DebugDirRva == 0 ) || ( DebugDirSize == 0 ) )
	{
		// Inconsistent data in the data directory 
		return false; 
	}


	// Complete 

	return true; 

}

// 
// The function walks through the section headers, finds out the section 
// the given RVA belongs to, and uses the section header to determine 
// the file offset that corresponds to the given RVA 
// 
// Return value: "true" if succeeded, "false" if failed 
// 
static bool GetFileOffsetFromRVA( PIMAGE_NT_HEADERS pNtHeaders, DWORD Rva, DWORD& FileOffset ) 
{
	// Check parameters 

	if( pNtHeaders == 0 ) 
	{
		_ASSERT( 0 ); 
		return false; 
	}


	// Look up the section the RVA belongs to 

	bool bFound = false; 

	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION( pNtHeaders ); 

	for( int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeader++ ) 
	{
		DWORD SectionSize = pSectionHeader->Misc.VirtualSize; 

		if( SectionSize == 0 ) // compensate for Watcom linker strangeness, according to Matt Pietrek 
			pSectionHeader->SizeOfRawData; 

		if( ( Rva >= pSectionHeader->VirtualAddress ) && 
		    ( Rva < pSectionHeader->VirtualAddress + SectionSize ) ) 
		{
			// Yes, the RVA belongs to this section 
			bFound = true; 
			break; 
		}
	}

	if( !bFound ) 
	{
		// Section not found 
		return false; 
	}


	// Look up the file offset using the section header 

	INT Diff = (INT)( pSectionHeader->VirtualAddress - pSectionHeader->PointerToRawData ); 

	FileOffset = Rva - Diff; 


	// Complete 

	return true; 

}

// 
// Walk through each entry in the debug directory and display information about it 
// 
static void DumpDebugDirectoryEntries( LPBYTE pImageBase, 
                                      PIMAGE_DEBUG_DIRECTORY pDebugDir, 
                                      DWORD DebugDirSize,  
                                      OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo) 
{
	// Check parameters 
  
	if( !CheckDebugDirectory( pDebugDir, DebugDirSize ) ) 
  { 
		return; 
	}

	if( pImageBase == 0 ) 
  { 
		return; 
	}
  

	// Determine the number of entries in the debug directory 

	int NumEntries = DebugDirSize / sizeof(IMAGE_DEBUG_DIRECTORY); 
  
	if( NumEntries == 0 ) 
  { 
		return; 
	}
  
	_tprintf( _T("Number of entries in debug directory: %d \n"), NumEntries ); 


	// Display information about every entry 

	for( int i = 1; i <= NumEntries; i++, pDebugDir++ ) 
	{
		_tprintf( _T("\nDebug directory entry %d: \n"), i ); 
    
		DumpDebugDirectoryEntry( pImageBase, pDebugDir, pstDebugInfo ); 
	}

}

// 
// Display information about debug directory entry 
// 
static void DumpDebugDirectoryEntry( LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir,
                                    OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo ) 
{
	// Check parameters 

	if( pDebugDir == 0 ) 
	{
		_ASSERT( 0 ); 
		return; 
	}

	if( pImageBase == 0 ) 
	{
		_ASSERT( 0 ); 
		return; 
	}


	// Display information about the entry 

	if( pDebugDir->Type != IMAGE_DEBUG_TYPE_UNKNOWN ) 
	{
		_tprintf( _T("Type: %u ( %s ) \n"), pDebugDir->Type, DebugTypeToStr(pDebugDir->Type) ); 

		_tprintf( _T("TimeStamp: %08x  Characteristics: %x  MajorVer: %u  MinorVer: %u \n"), 
			pDebugDir->TimeDateStamp, pDebugDir->Characteristics, pDebugDir->MajorVersion, pDebugDir->MinorVersion ); 

		_tprintf( _T("Size: %u  RVA: %08x  FileOffset: %08x  \n"), pDebugDir->SizeOfData, 
			pDebugDir->AddressOfRawData, pDebugDir->PointerToRawData ); 
	}
	else 
	{
		_tprintf( _T("Type: Unknown.\n") ); 
	}


	// Display additional information for some interesting debug information types 

	LPBYTE pDebugInfo = pImageBase + pDebugDir->PointerToRawData; 

	if( pDebugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW ) 
	{
		DumpCodeViewDebugInfo( pDebugInfo, pDebugDir->SizeOfData, pstDebugInfo ); 
	}
	else if( pDebugDir->Type == IMAGE_DEBUG_TYPE_MISC ) 
	{
		DumpMiscDebugInfo( pDebugInfo, pDebugDir->SizeOfData, pstDebugInfo ); 
	}

}

// 
// Display information about CodeView debug information block 
// 
static void DumpCodeViewDebugInfo( LPBYTE pDebugInfo, DWORD DebugInfoSize, 
                                  OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo ) 
{
	// Check parameters 

	if( ( pDebugInfo == 0 ) || ( DebugInfoSize == 0 ) ) 
		return; 

	if( IsBadReadPtr( pDebugInfo, DebugInfoSize ) ) 
		return; 

	if( DebugInfoSize < sizeof(DWORD) ) // size of the signature 
		return; 


	// Determine the format of the information and display it accordingly 

	DWORD CvSignature = *(DWORD*)pDebugInfo; 

	if( CvSignature == CV_SIGNATURE_NB10 ) 
	{
		// NB10 -> PDB 2.00 
    
		CV_INFO_PDB20* pCvInfo = (CV_INFO_PDB20*)pDebugInfo; 

		if( IsBadReadPtr( pDebugInfo, sizeof(CV_INFO_PDB20) ) ) 
			return;
		
		if( IsBadStringPtrA( (CHAR*)pCvInfo->PdbFileName, UINT_MAX ) ) 
			return; 

		_tprintf( _T("CodeView format: NB10 \n") ); 

    printf( "Signature: %08x  Age: %u  \n", pCvInfo->Signature, pCvInfo->Age ); 
		printf( "PDB File: %s \n", pCvInfo->PdbFileName ); 
    
    if(pstDebugInfo)
    {
      pstDebugInfo->Age       =  pCvInfo->Age;
      lstrcpy(pstDebugInfo->szPDBVersion, _T("PDB 2.00"));
      if(pCvInfo->PdbFileName[0])
      {
        lstrcpyn(pstDebugInfo->szPdbFile, (LPCTSTR)pCvInfo->PdbFileName, COUNTOF_I(pstDebugInfo->szPdbFile));
      }
    }
	}
	else if( CvSignature == CV_SIGNATURE_RSDS ) 
	{
		// RSDS -> PDB 7.00 
    
		CV_INFO_PDB70* pCvInfo = (CV_INFO_PDB70*)pDebugInfo; 

		if( IsBadReadPtr( pDebugInfo, sizeof(CV_INFO_PDB70) ) ) 
			return;
		
		if( IsBadStringPtrA( (CHAR*)pCvInfo->PdbFileName, UINT_MAX ) ) 
			return; 

		_tprintf( _T("CodeView format: RSDS \n") ); 

		_tprintf( _T("Signature: ") ); 
		//DumpGuid( pCvInfo->Signature ); 
    
		printf( "  Age: %u  \n", pCvInfo->Age ); 

		printf( "PdbFile: %s \n", pCvInfo->PdbFileName ); 
    
    if(pstDebugInfo)
    {
      lstrcpy(pstDebugInfo->szPDBVersion, _T("PDB 7.00"));
      pstDebugInfo->Age       =  pCvInfo->Age;
      if(pCvInfo->PdbFileName[0])
      {
        lstrcpyn(pstDebugInfo->szPdbFile, (LPCTSTR)pCvInfo->PdbFileName, COUNTOF_I(pstDebugInfo->szPdbFile));
      }
    }
	}
	else 
	{
		// Other CodeView format 
    
		CHAR* pSig = (CHAR*)&CvSignature; 
    
		printf( "CodeView signature: %c%c%c%c \n", pSig[0], pSig[1], pSig[2], pSig[3] ); 
        
		if( ( pSig[0] == 'N' ) && ( pSig[1] == 'B' ) ) // One of NBxx formats 
		{
			CV_HEADER* pCvHeader = (CV_HEADER*)pDebugInfo; 

			_tprintf( _T("CodeView information offset: %08x\n"), pCvHeader->Offset ); 
		}

    if(pstDebugInfo)
    {
      memcpy(pstDebugInfo->szPDBVersion, pSig, 4);
      pstDebugInfo->szPDBVersion[4] = 0;
      pstDebugInfo->Age          =  0;
      pstDebugInfo->szPdbFile[0] = 0;
    }
	}
}

// 
// Display information about Misc debug information block 
// 
static void DumpMiscDebugInfo( LPBYTE pDebugInfo, DWORD DebugInfoSize, 
                              OUT CProcessUtil_i::DEBUG_INFO* pstDebugInfo) 
{
  // Check parameters 
  
  if( ( pDebugInfo == 0 ) || ( DebugInfoSize == 0 ) ) 
    return; 
  
  if( IsBadReadPtr( pDebugInfo, DebugInfoSize ) ) 
    return; 
  
  if( DebugInfoSize < sizeof(IMAGE_DEBUG_MISC) ) 
    return;   
  
  // Display information 
  
  PIMAGE_DEBUG_MISC pMiscInfo = (PIMAGE_DEBUG_MISC)pDebugInfo; 
  
  _tprintf( _T("Data type: %u  Length: %u  Format: %s \n"), pMiscInfo->DataType, 
    pMiscInfo->Length, pMiscInfo->Unicode ? _T("Unicode") : _T("ANSI") ); 
  
  if( pMiscInfo->DataType == IMAGE_DEBUG_MISC_EXENAME ) 
  {
    // Yes, it should refer to a DBG file 
    
    if( pMiscInfo->Unicode ) 
    {
      if( !IsBadStringPtrW( (WCHAR*)pMiscInfo->Data, UINT_MAX ) ) 
        wprintf( L"File: %s \n", (WCHAR*)pMiscInfo->Data ); 
    }
    else // ANSI 
    {
      if( !IsBadStringPtrA( (CHAR*)pMiscInfo->Data, UINT_MAX ) ) 
        printf( "File: %s \n", (CHAR*)pMiscInfo->Data ); 
    }
    
    if(pstDebugInfo)
    { 
      lstrcpyn(pstDebugInfo->szPdbFile, (LPCTSTR)pMiscInfo->Data, COUNTOF_I(pstDebugInfo->szPdbFile));
    }
  }
}

static LPCTSTR DebugTypeToStr( DWORD DebugType ) 
{
	switch( DebugType ) 
	{
		case IMAGE_DEBUG_TYPE_UNKNOWN: 
			return _T("Unknown"); 
			break; 

		case IMAGE_DEBUG_TYPE_COFF: 
			return _T("COFF"); 
			break; 

		case IMAGE_DEBUG_TYPE_CODEVIEW: 
			return _T("CodeView"); 
			break; 

		case IMAGE_DEBUG_TYPE_FPO: 
			return _T("FPO"); 
			break; 

		case IMAGE_DEBUG_TYPE_MISC: 
			return _T("MISC"); 
			break; 

		case IMAGE_DEBUG_TYPE_EXCEPTION: 
			return _T("Exception"); 
			break; 

		case IMAGE_DEBUG_TYPE_FIXUP: 
			return _T("Fixup"); 
			break; 

		case IMAGE_DEBUG_TYPE_OMAP_TO_SRC: 
			return _T("OMAP-to-SRC"); 
			break; 

		case IMAGE_DEBUG_TYPE_OMAP_FROM_SRC: 
			return _T("OMAP-from-SRC"); 
			break; 

		case IMAGE_DEBUG_TYPE_BORLAND: 
			return _T("Borland"); 
			break; 

		default: 
			return _T("Unknown"); 
			break; 
	}

	_ASSERT( 0 ); 

	return _T("Unknown"); 

}

// 
// CString DumpGuid( GUID& Guid ) 
// {
//   CString str;
// 	str.Format( _T("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}"), 
// 		Guid.Data1, Guid.Data2, Guid.Data3, Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], 
// 		Guid.Data4[3], Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7] ); 
//   
//   return str;
// }

