/*
Module : CPDHIMPL.H
Purpose: Defines the implementation for a class to support dynamically loading the PDH dll
Created: PJN / 06-02-1999
History: None

Copyright (c) 1999 by PJ Naughter.  
All rights reserved.

*/

#ifndef __CPDHIMPL_H__
#define __CPDHIMPL_H__


struct pjPdhCall
{
	PDH_STATUS (__stdcall *pfnPdhAddCounter)(HQUERY, LPCTSTR, DWORD, HCOUNTER*);
  PDH_STATUS (__stdcall *pfnPdhBrowseCounters)(PPDH_BROWSE_DLG_CONFIG);
  PDH_STATUS (__stdcall *pfnPdhCalculateCounterFromRawValue)(HCOUNTER, DWORD, PPDH_RAW_COUNTER, PPDH_RAW_COUNTER, PPDH_FMT_COUNTERVALUE);
  PDH_STATUS (__stdcall *pfnPdhCloseQuery)(HQUERY);
  PDH_STATUS (__stdcall *pfnPdhCollectQueryData)(HQUERY);
  PDH_STATUS (__stdcall *pfnPdhComputeCounterStatistics)(HCOUNTER, DWORD, DWORD, DWORD, PPDH_RAW_COUNTER, PPDH_STATISTICS);
  PDH_STATUS (__stdcall *pfnPdhConnectMachine)(LPCTSTR);
  PDH_STATUS (__stdcall *pfnPdhEnumMachines)(LPCTSTR, LPTSTR, LPDWORD);
  PDH_STATUS (__stdcall *pfnPdhEnumObjectItems)(LPCTSTR, LPCTSTR, LPCTSTR, LPTSTR, LPDWORD, LPTSTR, LPDWORD, DWORD, DWORD);
  PDH_STATUS (__stdcall *pfnPdhEnumObjects)(LPCTSTR, LPCTSTR, LPTSTR, LPDWORD, DWORD, BOOL);
  PDH_STATUS (__stdcall *pfnPdhExpandCounterPath)(LPCTSTR, LPTSTR, LPDWORD);
  PDH_STATUS (__stdcall *pfnPdhGetCounterInfo)(HQUERY, BOOLEAN, LPDWORD pdwBufferSize, PPDH_COUNTER_INFO);
  PDH_STATUS (__stdcall *pfnPdhGetDefaultPerfCounter)(LPCTSTR, LPCTSTR, LPCTSTR, LPTSTR, LPDWORD);
  PDH_STATUS (__stdcall *pfnPdhGetDefaultPerfObject)(LPCTSTR, LPCTSTR, LPTSTR, LPDWORD);
  PDH_STATUS (__stdcall *pfnPdhGetFormattedCounterValue)(HCOUNTER, DWORD, LPDWORD, PPDH_FMT_COUNTERVALUE);
  PDH_STATUS (__stdcall *pfnPdhGetRawCounterValue)(HCOUNTER, LPDWORD, PPDH_RAW_COUNTER);
  PDH_STATUS (__stdcall *pfnPdhMakeCounterPath)(PDH_COUNTER_PATH_ELEMENTS*, LPTSTR, LPDWORD, DWORD);
  PDH_STATUS (__stdcall *pfnPdhOpenQuery)(LPVOID, DWORD, HQUERY*);
  PDH_STATUS (__stdcall *pfnPdhParseCounterPath)(LPCTSTR, PDH_COUNTER_PATH_ELEMENTS*, LPDWORD, DWORD);
  PDH_STATUS (__stdcall *pfnPdhParseInstanceName)(LPCTSTR, LPTSTR, LPDWORD, LPTSTR szParentName, LPDWORD pcchParentNameLength, LPDWORD lpIndex);
  PDH_STATUS (__stdcall *pfnPdhRemoveCounter)(HCOUNTER);
  PDH_STATUS (__stdcall *pfnPdhSetCounterScaleFactor)(HCOUNTER, LONG);
  PDH_STATUS (__stdcall *pfnPdhValidatePath)(LPCTSTR);
};



extern pjPdhCall _pjPdh;


/////////////////////////////////////////////////////////////////////////////
// macros for pjPdhCall access

#ifdef PdhAddCounter
#undef PdhAddCounter
#endif
#define PdhAddCounter _pjPdh.pfnPdhAddCounter

#ifdef PdhBrowseCounters
#undef PdhBrowseCounters
#endif
#define PdhBrowseCounters _pjPdh.pfnPdhBrowseCounters

#ifdef PdhCalculateCounterFromRawValue
#undef PdhCalculateCounterFromRawValue
#endif
#define PdhCalculateCounterFromRawValue _pjPdh.pfnPdhCalculateCounterFromRawValue

#ifdef PdhCloseQuery
#undef PdhCloseQuery
#endif
#define PdhCloseQuery _pjPdh.pfnPdhCloseQuery

#ifdef PdhCollectQueryData
#undef PdhCollectQueryData
#endif
#define PdhCollectQueryData _pjPdh.pfnPdhCollectQueryData

#ifdef PdhComputeCounterStatistics
#undef PdhComputeCounterStatistics
#endif
#define PdhComputeCounterStatistics _pjPdh.pfnPdhComputeCounterStatistics

#ifdef PdhConnectMachine
#undef PdhConnectMachine
#endif
#define PdhConnectMachine _pjPdh.pfnPdhConnectMachine

#ifdef PdhEnumMachines
#undef PdhEnumMachines
#endif
#define PdhEnumMachines _pjPdh.pfnPdhEnumMachines

#ifdef PdhEnumObjectItems
#undef PdhEnumObjectItems
#endif
#define PdhEnumObjectItems _pjPdh.pfnPdhEnumObjectItems

#ifdef PdhEnumObjects
#undef PdhEnumObjects
#endif
#define PdhEnumObjects _pjPdh.pfnPdhEnumObjects

#ifdef PdhExpandCounterPath
#undef PdhExpandCounterPath
#endif
#define PdhExpandCounterPath _pjPdh.pfnPdhExpandCounterPath

#ifdef PdhGetCounterInfo
#undef PdhGetCounterInfo
#endif
#define PdhGetCounterInfo _pjPdh.pfnPdhGetCounterInfo

#ifdef PdhGetDefaultPerfCounter
#undef PdhGetDefaultPerfCounter
#endif
#define PdhGetDefaultPerfCounter _pjPdh.pfnPdhGetDefaultPerfCounter

#ifdef PdhGetDefaultPerfObject
#undef PdhGetDefaultPerfObject
#endif
#define PdhGetDefaultPerfObject _pjPdh.pfnPdhGetDefaultPerfObject

#ifdef PdhGetFormattedCounterValue
#undef PdhGetFormattedCounterValue
#endif
#define PdhGetFormattedCounterValue _pjPdh.pfnPdhGetFormattedCounterValue

#ifdef PdhGetRawCounterValue
#undef PdhGetRawCounterValue
#endif
#define PdhGetRawCounterValue _pjPdh.pfnPdhGetRawCounterValue

#ifdef PdhMakeCounterPath
#undef PdhMakeCounterPath
#endif
#define PdhMakeCounterPath _pjPdh.pfnPdhMakeCounterPath

#ifdef PdhOpenQuery
#undef PdhOpenQuery
#endif
#define PdhOpenQuery _pjPdh.pfnPdhOpenQuery

#ifdef PdhParseCounterPath
#undef PdhParseCounterPath
#endif
#define PdhParseCounterPath _pjPdh.pfnPdhParseCounterPath

#ifdef PdhParseInstanceName
#undef PdhParseInstanceName
#endif
#define PdhParseInstanceName _pjPdh.pfnPdhParseInstanceName

#ifdef PdhRemoveCounter
#undef PdhRemoveCounter
#endif
#define PdhRemoveCounter _pjPdh.pfnPdhRemoveCounter

#ifdef PdhSetCounterScaleFactor
#undef PdhSetCounterScaleFactor
#endif
#define PdhSetCounterScaleFactor _pjPdh.pfnPdhSetCounterScaleFactor

#ifdef PdhValidatePath
#undef PdhValidatePath
#endif
#define PdhValidatePath _pjPdh.pfnPdhValidatePath



#endif //__CPDHIMPL_H__