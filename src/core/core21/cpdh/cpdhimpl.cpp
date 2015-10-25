/*
Module : CPDHIMPL.CPP
Purpose: Defines the implementation for a class to support dynamically loading the PDH dll
Created: PJN / 06-02-1999
History: PJN / 05-09-1999 Fixed a bug in pjPdhState::pjPdhState which was causing an access violation

Copyright (c) 1999 by PJ Naughter.  
All rights reserved.

*/


/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "cpdh.h"
#include "cpdhimpl.h"





/////////////////////////////////  Statics  //////////////////////////////////
static const TCHAR szPdhDLL[] = _T("PDH.DLL");

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif








////////////////////////////////// Implementation /////////////////////////////

//Used to hold the PDH hInstance
class pjPdhState
{
public:
  pjPdhState();
  ~pjPdhState();

  HINSTANCE m_hInstPDH;
};

pjPdhState::pjPdhState()
{
	m_hInstPDH = NULL;
}

pjPdhState::~pjPdhState()
{
	if (m_hInstPDH != NULL)
		::FreeLibrary(m_hInstPDH);
}

pjPdhState _pjPdhState;


inline void PASCAL pjPDHLoad(FARPROC* pProcPtr, LPCSTR pstrEntry)
{
  //Try to load up the "PDH.DLL" if not already loaded up 
  //and if it fails throw an exception
  if (_pjPdhState.m_hInstPDH == NULL)
  {
    _pjPdhState.m_hInstPDH = AfxLoadLibrary(szPdhDLL);
    if (_pjPdhState.m_hInstPDH == NULL)
	  {
		  TRACE1("Error: Couldn't load %s!\n", szPdhDLL);
		  CPdh::ThrowPdhException(GetLastError());
	  }
  }


	ASSERT(_pjPdhState.m_hInstPDH);

	// cache the procedure pointer
	ASSERT(pProcPtr != NULL);
	*pProcPtr = GetProcAddress(_pjPdhState.m_hInstPDH, pstrEntry);
	if (*pProcPtr == NULL)
	{
		TRACE2("Error: Couldn't find %s in %s!\n", pstrEntry, szPdhDLL);
		CPdh::ThrowPdhException(GetLastError());
	}
}

#define PDHLOAD(x) pjPDHLoad((FARPROC*)&_pjPdh.pfn##x, #x)
#ifdef _UNICODE
#define PDHLOADT(x) pjPDHLoad((FARPROC*)&_pjPdh.pfn##x, #x"W")
#else
#undef PDHLOADT 
#define PDHLOADT(x) pjPDHLoad((FARPROC*)&_pjPdh.pfn##x, #x"A")
#endif

PDH_STATUS __stdcall pjThunkPdhAddCounter(HQUERY hQuery, LPCTSTR szFullCounterPath, DWORD dwUserData, HCOUNTER *phCounter)
{
	PDHLOADT(PdhAddCounter);
	return _pjPdh.pfnPdhAddCounter(hQuery, szFullCounterPath, dwUserData, phCounter);
}

PDH_STATUS __stdcall pjThunkPdhBrowseCounters(PPDH_BROWSE_DLG_CONFIG pBrowseDlgData)
{
	PDHLOADT(PdhBrowseCounters);
	return _pjPdh.pfnPdhBrowseCounters(pBrowseDlgData);
}

PDH_STATUS __stdcall pjThunkPdhCalculateCounterFromRawValue(HCOUNTER hCounter, DWORD dwFormat, PPDH_RAW_COUNTER rawValue1, 
                                                            PPDH_RAW_COUNTER rawValue2, PPDH_FMT_COUNTERVALUE fmtValue) 
 
{
	PDHLOAD(PdhCalculateCounterFromRawValue);
	return _pjPdh.pfnPdhCalculateCounterFromRawValue(hCounter, dwFormat, rawValue1, rawValue2, fmtValue);
}

PDH_STATUS __stdcall pjThunkPdhCloseQuery(HQUERY hQuery)
{
	PDHLOAD(PdhCloseQuery);
	return _pjPdh.pfnPdhCloseQuery(hQuery);
}

PDH_STATUS __stdcall pjThunkPdhCollectQueryData(HQUERY hQuery)
{
	PDHLOAD(PdhCollectQueryData);
	return _pjPdh.pfnPdhCollectQueryData(hQuery);
}

PDH_STATUS __stdcall pjThunkPdhComputeCounterStatistics(HCOUNTER hCounter, DWORD dwFormat, DWORD dwFirstEntry, 
                                                        DWORD dwNumEntries, PPDH_RAW_COUNTER lpRawValueArray,
                                                        PPDH_STATISTICS data)
{
	PDHLOAD(PdhComputeCounterStatistics);
  return _pjPdh.pfnPdhComputeCounterStatistics(hCounter, dwFormat, dwFirstEntry, dwNumEntries, lpRawValueArray, data);
}

PDH_STATUS __stdcall pjThunkPdhConnectMachine(LPCTSTR szMachineName)
{
	PDHLOADT(PdhConnectMachine);
	return _pjPdh.pfnPdhConnectMachine(szMachineName);
}

PDH_STATUS __stdcall pjThunkPdhEnumMachines(LPCTSTR szReserved, LPTSTR mszMachineNameList, LPDWORD pcchBufferLength)
{
	PDHLOADT(PdhEnumMachines);
	return _pjPdh.pfnPdhEnumMachines(szReserved, mszMachineNameList, pcchBufferLength);
}

PDH_STATUS __stdcall pjThunkPdhEnumObjectItems(LPCTSTR szReserved, LPCTSTR szMachineName, LPCTSTR szObjectName, LPTSTR mszCounterList, LPDWORD pcchCounterListLength, 
                                               LPTSTR mszInstanceList, LPDWORD pcchInstanceListLength, DWORD dwDetailLevel, DWORD dwFlags)
{
	PDHLOADT(PdhEnumObjectItems);
	return _pjPdh.pfnPdhEnumObjectItems(szReserved, szMachineName, szObjectName, mszCounterList, pcchCounterListLength, 
                                      mszInstanceList, pcchInstanceListLength, dwDetailLevel, dwFlags);
}

PDH_STATUS __stdcall pjThunkPdhEnumObjects(LPCTSTR szReserved, LPCTSTR szMachineName, LPTSTR mszObjectList, LPDWORD pcchBufferLength, 
                                           DWORD dwDetailLevel, BOOL bRefresh)
{
	PDHLOADT(PdhEnumObjects);
	return _pjPdh.pfnPdhEnumObjects(szReserved, szMachineName, mszObjectList, pcchBufferLength, dwDetailLevel, bRefresh);
}

PDH_STATUS __stdcall pjThunkPdhExpandCounterPath(LPCTSTR szWildCardPath, LPTSTR mszExpandedPathList, LPDWORD pcchPathListLength)
{
	PDHLOADT(PdhExpandCounterPath);
	return _pjPdh.pfnPdhExpandCounterPath(szWildCardPath, mszExpandedPathList, pcchPathListLength);
}

PDH_STATUS __stdcall pjThunkPdhGetCounterInfo(HQUERY hCounter, BOOLEAN bRetrieveExplainText, LPDWORD pdwBufferSize, PPDH_COUNTER_INFO lpBuffer)
{
	PDHLOADT(PdhGetCounterInfo);
	return _pjPdh.pfnPdhGetCounterInfo(hCounter, bRetrieveExplainText, pdwBufferSize, lpBuffer);
}

PDH_STATUS __stdcall pjThunkPdhGetDefaultPerfCounter(LPCTSTR szReserved, LPCTSTR szMachineName, LPCTSTR szObjectName, 
                                                     LPTSTR szDefaultCounterName, LPDWORD pcchBufferSize)
{
	PDHLOADT(PdhGetDefaultPerfCounter);
	return _pjPdh.pfnPdhGetDefaultPerfCounter(szReserved, szMachineName, szObjectName, szDefaultCounterName, pcchBufferSize);
}

PDH_STATUS __stdcall pjThunkPdhGetDefaultPerfObject(LPCTSTR szReserved, LPCTSTR szMachineName, LPTSTR szDefaultObjectName, LPDWORD pcchBufferSize)
{
	PDHLOADT(PdhGetDefaultPerfObject);
	return _pjPdh.pfnPdhGetDefaultPerfObject(szReserved, szMachineName, szDefaultObjectName, pcchBufferSize);
}

PDH_STATUS __stdcall pjThunkPdhGetFormattedCounterValue(HCOUNTER hCounter, DWORD dwFormat, LPDWORD lpdwType, PPDH_FMT_COUNTERVALUE pValue)
{
	PDHLOAD(PdhGetFormattedCounterValue);
	return _pjPdh.pfnPdhGetFormattedCounterValue(hCounter, dwFormat, lpdwType, pValue);
}

PDH_STATUS __stdcall pjThunkPdhGetRawCounterValue(HCOUNTER hCounter, LPDWORD lpdwType, PPDH_RAW_COUNTER pValue)
{
	PDHLOAD(PdhGetRawCounterValue);
	return _pjPdh.pfnPdhGetRawCounterValue(hCounter, lpdwType, pValue);
}

PDH_STATUS __stdcall pjThunkPdhMakeCounterPath(PDH_COUNTER_PATH_ELEMENTS *pCounterPathElements, LPTSTR szFullPathBuffer, LPDWORD pcchBufferSize, DWORD dwFlags)
{
	PDHLOADT(PdhMakeCounterPath);
	return _pjPdh.pfnPdhMakeCounterPath(pCounterPathElements, szFullPathBuffer, pcchBufferSize, dwFlags);
}

PDH_STATUS __stdcall pjThunkPdhOpenQuery(LPVOID pReserved, DWORD dwUserData, HQUERY *phQuery)
{
	PDHLOAD(PdhOpenQuery);
	return _pjPdh.pfnPdhOpenQuery(pReserved, dwUserData, phQuery);
}

PDH_STATUS __stdcall pjThunkPdhParseCounterPath(LPCTSTR szFullPathBuffer, PDH_COUNTER_PATH_ELEMENTS *pCounterPathElements, LPDWORD pdwBufferSize, DWORD dwFlags)
{
	PDHLOADT(PdhParseCounterPath);
	return _pjPdh.pfnPdhParseCounterPath(szFullPathBuffer, pCounterPathElements, pdwBufferSize, dwFlags);
}

PDH_STATUS __stdcall pjThunkPdhParseInstanceName(LPCTSTR szInstanceString, LPTSTR szInstanceName, LPDWORD pcchInstanceNameLength,
                                                 LPTSTR szParentName, LPDWORD pcchParentNameLength, LPDWORD lpIndex)
{
	PDHLOADT(PdhParseInstanceName);
	return _pjPdh.pfnPdhParseInstanceName(szInstanceString, szInstanceName, pcchInstanceNameLength,
                                       szParentName, pcchParentNameLength, lpIndex);
}

PDH_STATUS __stdcall pjThunkPdhRemoveCounter(HCOUNTER hCounter)
{
	PDHLOAD(PdhRemoveCounter);
	return _pjPdh.pfnPdhRemoveCounter(hCounter);
}

PDH_STATUS __stdcall pjThunkPdhSetCounterScaleFactor(HCOUNTER hCounter, LONG lFactor)
{
	PDHLOAD(PdhSetCounterScaleFactor);
	return _pjPdh.pfnPdhSetCounterScaleFactor(hCounter, lFactor);
}

PDH_STATUS __stdcall pjThunkPdhValidatePath(LPCTSTR szFullCounterPath)
{
	PDHLOADT(PdhValidatePath);
	return _pjPdh.pfnPdhValidatePath(szFullCounterPath);
}

pjPdhCall _pjPdh =
{
	{ pjThunkPdhAddCounter, },
  { pjThunkPdhBrowseCounters, },
  { pjThunkPdhCalculateCounterFromRawValue, },
  { pjThunkPdhCloseQuery, },
  { pjThunkPdhCollectQueryData, },
  { pjThunkPdhComputeCounterStatistics, },
  { pjThunkPdhConnectMachine, },
  { pjThunkPdhEnumMachines, },
  { pjThunkPdhEnumObjectItems, },
  { pjThunkPdhEnumObjects, },
  { pjThunkPdhExpandCounterPath, },
  { pjThunkPdhGetCounterInfo, },
  { pjThunkPdhGetDefaultPerfCounter, },
  { pjThunkPdhGetDefaultPerfObject, },
  { pjThunkPdhGetFormattedCounterValue, },
  { pjThunkPdhGetRawCounterValue, },
  { pjThunkPdhMakeCounterPath, },
  { pjThunkPdhOpenQuery, },
  { pjThunkPdhParseCounterPath, },
  { pjThunkPdhParseInstanceName, },
  { pjThunkPdhRemoveCounter, },
  { pjThunkPdhSetCounterScaleFactor, },
  { pjThunkPdhValidatePath, },
};

