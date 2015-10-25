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
#undef WINVER 
#define WINVER  0x0501 

#include "StdAfx.h"
#include "..\inc\SetupAPI.h"

#pragma comment(lib, "setupapi.lib" )
//#pragma comment(lib, "User32.Lib" )


#if 0 
int ITS_GetClassImgIndex(const GUID* pGuid)
{
  int nIdx;
  
  if (SetupDiGetClassImageIndex(&m_imgList, pGuid, &nIdx))
  {
    return nIdx;
  }
  else
  {
    return- 1;
  }
}
#endif 

CString ITS_GetClassDesc(GUID* pGuid)
{
  TCHAR buf[MAX_PATH];
  DWORD size;
  
  if (SetupDiGetClassDescription(pGuid, buf, sizeof(buf), &size))
  {
    return CString(buf);
  }
  else
  {
    return _T("");
  }
}


bool FindDevice(HDEVINFO& hDevInfo, 
							  CString& szDevId, 
							  SP_DEVINFO_DATA& spDevInfoData)
{
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    
	for(int i=0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++) 
  { 
		DWORD nSize=0 ;
    TCHAR buf[MAX_PATH] = {0, };

		if ( !SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, buf, sizeof(buf), &nSize) ) 
		{
			return FALSE;
		} 

		if ( szDevId == buf ) 
		{
			// OK, device found
			return TRUE;
		}
	}

	return FALSE;
}


static void GetDeviceInterfaceInfo(PDEV_BROADCAST_DEVICEINTERFACE pDevInf,
                          WPARAM wParam,
                          CDeviceMgr_i::DEVICE_INFO* pDeviceInfo,
                          LPCTSTR pszGuidFilter)
{
  // pDevInf->dbcc_name: 
  // \\?\USB#Vid_04e8&Pid_503b#0002F9A9828E0F06#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
  // szDevId: USB\Vid_04e8&Pid_503b\0002F9A9828E0F06
  // szClass: USB
  
  ASSERT(lstrlen(pDevInf->dbcc_name) > 4);
  CString szDevId = pDevInf->dbcc_name + 4;
  int idx         = szDevId.ReverseFind(_T('#'));
  
  ASSERT(-1 != idx);
  szDevId = szDevId.Left(idx);
  szDevId.Replace(_T('#'), _T('\\'));
  szDevId.MakeUpper();
  
  StrCpyN(pDeviceInfo->szInstanceId,szDevId, sizeof(pDeviceInfo->szInstanceId)); 
  
  CString szClass;
  
  idx = szDevId.Find(_T('\\'));
  ASSERT(-1 != idx);
  szClass = szDevId.Left(idx); 
  StrCpyN(pDeviceInfo->szClassName, szClass, sizeof(pDeviceInfo->szClassName));

  if (DBT_DEVICEARRIVAL == wParam)
  {
    // szTmp.Format(_T("Adding%s\r\n"), szDevId.GetBuffer());
    
    // TRACE("Adding %s\n", szDevId.GetBuffer());
    //TRACE("Adding %s\n", szClass);
  }
  else if (DBT_DEVICEREMOVECOMPLETE == wParam)
  {
    ;
    //TRACE("Removing %s\n", szClass);
  }
  else
  {
    // szTmp.Format(_T("Removing%s\r\n"), szDevId.GetBuffer());
    
    // TRACE("Removing %s\n", szDevId.GetBuffer());
  }
  
  // seems we should ignore "ROOT" type....
  if (_T("ROOT") == szClass)
  {
    return;
  }
  
  DWORD dwFlag      = DBT_DEVICEARRIVAL != wParam ? DIGCF_ALLCLASSES : (DIGCF_ALLCLASSES | DIGCF_PRESENT);
  HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, szClass, NULL, dwFlag);
  
  if (INVALID_HANDLE_VALUE == hDevInfo)
  {
    return;
  }
  
  SP_DEVINFO_DATA spDevInfoData;
  
  if (FindDevice(hDevInfo, szDevId, spDevInfoData))
  {
    // OK, device found
    DWORD DataT;
    TCHAR buf[512] = {0, };
    DWORD nSize = 0; 

    if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData,
      SPDRP_CLASSGUID, &DataT, (PBYTE)buf, sizeof(buf), &nSize))
    {
        StrCpyN(pDeviceInfo->szGuid, buf, sizeof(pDeviceInfo->szGuid));
    }
    else
    {
      pDeviceInfo->szGuid[0] = _T('\0');
    }
    
    if(pszGuidFilter)
    {
      CString strGuid = pszGuidFilter;
      strGuid.MakeUpper();
      
      if(strGuid != pDeviceInfo->szGuid)
      {
        pDeviceInfo->szGuid[0] = _T('\0');
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return ;
      }
    }

    // get Friendly Name or Device Description
    if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData,
      SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buf, sizeof(buf), &nSize))
    {
      StrCpyN(pDeviceInfo->szFriendName, buf, sizeof(pDeviceInfo->szFriendName));
    }
    else 
    {
      pDeviceInfo->szFriendName[0] = _T('\0');
    }
    
    if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData,
      SPDRP_DEVICEDESC, &DataT, (PBYTE)buf, sizeof(buf), &nSize))
    { 
        StrCpyN(pDeviceInfo->szDescription, buf, sizeof(pDeviceInfo->szDescription));
    }
    else
    { 
      pDeviceInfo->szDescription[0] = _T('\0');
    }

    if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData,
      SPDRP_MFG, &DataT, (PBYTE)buf, sizeof(buf), &nSize))
    { 
        StrCpyN(pDeviceInfo->szManufacturerName, buf, sizeof(pDeviceInfo->szManufacturerName));
    }
    else
    { 
      pDeviceInfo->szManufacturerName[0] = _T('\0');
    }
    
    SetupDiDestroyDeviceInfoList(hDevInfo);
  }
}

 typedef struct _DEV_BROADCAST_PORT {
  DWORD dbcp_size;
  DWORD dbcp_devicetype;
  DWORD dbcp_reserved;
  TCHAR dbcp_name[1]; // A pointer to a null-terminated string specifying the friendly name of the port or the device connected to the port. Friendly names are intended to help the user quickly and accurately identify the device¡ªfor example, "COM1" and "Standard 28800 bps Modem" are considered friendly names.
} DEV_BROADCAST_PORT, *PDEV_BROADCAST_PORT;
typedef struct _DEV_BROADCAST_OEM {
  DWORD dbco_size;
  DWORD dbco_devicetype;
  DWORD dbco_reserved;
  DWORD dbco_identifier;
  DWORD dbco_suppfunc;
} DEV_BROADCAST_OEM, *PDEV_BROADCAST_OEM;

typedef struct _DEV_BROADCAST_VOLUME {
  DWORD dbcv_size;
  DWORD dbcv_devicetype;
  DWORD dbcv_reserved;
  DWORD dbcv_unitmask; // The logical unit mask identifying one or more logical units. Each bit in the mask corresponds to one logical drive. Bit 0 represents drive A, bit 1 represents drive B, and so on.
  WORD  dbcv_flags; // DBTF_MEDIA 0x0001 Change affects media in drive. If not set, change affects physical device or drive£» DBTF_NET 0x0002 Indicated logical volume is a network volume. 
} DEV_BROADCAST_VOLUME, *PDEV_BROADCAST_VOLUME;


#define DBT_DEVNODES_CHANGED            0x007
#define DBT_CUSTOMEVENT                 0x8006  // user-defined event

// ON_MESSAGE(WM_DEVICECHANGE, OnDeviceChange)
LRESULT CDeviceMgr_i::DeviceChangeHandler(WPARAM wParam, LPARAM lParam, 
                                          DEV_TYPE_E& eDevType, 
                                          CString& strDevClass, 
                                          CString& strDevDescription, // Friendly Name.
                                          DEV_CHANGE_STATUS_E& eChangedStatus)
{
  eChangedStatus = DEV_CHANGE_STATUS_UNKNOWN; 
  eDevType       = DEV_TYPE_UNKNOWN;
  
  if(DBT_DEVNODES_CHANGED == wParam) // A device has been added to or removed from the system. 
  {
    // Windows 2000/XP: A device has been added to or removed from the system. 
    eChangedStatus = DEV_CHANGE_STATUS_CHANGED_E;
  } 
  else if(DBT_CUSTOMEVENT == wParam)
  {
    eChangedStatus = DEV_CHANGE_STATUS_CUSTOM_E;
    // Windows 98/Me, Windows 2000/XP: A custom event has occurred.
  }
  else   if ((DBT_DEVICEARRIVAL == wParam) ||
             (DBT_DEVICEREMOVEPENDING == wParam) || 
             (DBT_DEVICEQUERYREMOVE == wParam) ||
             (DBT_DEVICEREMOVECOMPLETE == wParam))
  {
    PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
    //PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
    PDEV_BROADCAST_HANDLE pDevHnd;
    PDEV_BROADCAST_OEM pDevOem;
    PDEV_BROADCAST_PORT pDevPort;
    //PDEV_BROADCAST_VOLUME pDevVolume;
    //PDEV_BROADCAST_NET pNet;
    
    // A device has been added to or removed from the system.
    if( DBT_DEVICEARRIVAL == wParam ) 
    {
      // A device has been inserted and is now available.
      eChangedStatus = DEV_CHANGE_STATUS_ADDED_E;
    }
    else if( DBT_DEVICEREMOVECOMPLETE == wParam )
    {
      // A device has been removed.
      eChangedStatus = DEV_CHANGE_STATUS_REMOVED_E;
    }
    else if( DBT_DEVICEREMOVEPENDING == wParam )
    {
      eChangedStatus = DEV_CHANGE_STATUS_REMOVING_E;
    }
    else if( DBT_DEVICEQUERYREMOVEFAILED == wParam )
    {
      eChangedStatus = DEV_CHANGE_STATUS_REMOVING_E;
    }
    else if( DBT_DEVICEQUERYREMOVE == wParam )
    {
      eChangedStatus = DEV_CHANGE_STATUS_REMOVING_E;
    }
    
    if(pHdr)
    {
      switch (pHdr->dbch_devicetype)
      {
      case DBT_DEVTYP_DEVICEINTERFACE:
        {
          //GetDeviceInterfaceInfo(pDevInf, wParam, strDevClass, strDevDescription);
        }
        break;
        
      case DBT_DEVTYP_HANDLE:
        pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
        break;
        
      case DBT_DEVTYP_OEM:
        pDevOem = (PDEV_BROADCAST_OEM)pHdr;
        break;
        
      case DBT_DEVTYP_PORT:
        pDevPort = (PDEV_BROADCAST_PORT)pHdr;
        if(pDevPort)
        {
          strDevDescription = pDevPort->dbcp_name; // friendly name  
          eDevType = DEV_TYPE_COM_PORT_E; // Contains information about a modem, serial, or parallel port.
        }
        break;
      
      case DBT_DEVTYP_VOLUME:
        //pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
        break;
      
      case DBT_DEVTYP_NET:
        //pNet = (PDEV_BROADCAST_NET)pHdr;
        break;
      }

      return 0;
    }
  }
  
  return 1;
}


/*
You can see the current list of device classes and device interface classes at the following registries:

\\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Class 
\\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses

  A list of common device interface class GUIDs is given below:

Device Interface Name GUID 
USB Raw Device {a5dcbf10-6530-11d2-901f-00c04fb951ed} 
Disk Device    {53f56307-b6bf-11d0-94f2-00a0c91efb8b} 
Network Card   {ad498944-762f-11d0-8dcb-00c04fc3358c} 
Human Interface Device (HID) {4d1e55b2-f16f-11cf-88cb-001111000030} 
Palm {784126bf-4190-11d4-b5c2-00c04f687a67} 
USB dbcc_classguid £º{36FC9E60-C465-11CF-8056-444553540000}
*/
u32 CDeviceMgr_i::GetDeviceChangedInfo(WPARAM wParam, LPARAM lParam, bool& bDeviceAdd, 
                                       DEVICE_INFO* pDeviceInfo, LPCTSTR pszGuidFilter)
{
  bDeviceAdd = FALSE;
  
  if(DBT_DEVNODES_CHANGED == wParam) // A device has been added to or removed from the system. 
  {
    ;
  }
  
  if (DBT_DEVICEARRIVAL == wParam ||
      DBT_DEVICEREMOVEPENDING == wParam || 
      DBT_DEVICEQUERYREMOVE == wParam ||
      DBT_DEVICEREMOVECOMPLETE == wParam)
  {
    PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
    //PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
    PDEV_BROADCAST_HANDLE pDevHnd;
    PDEV_BROADCAST_OEM pDevOem;
    PDEV_BROADCAST_PORT pDevPort;
    //PDEV_BROADCAST_VOLUME pDevVolume;
    //PDEV_BROADCAST_NET pNet;
    
    // A device has been added to or removed from the system.
    if( DBT_DEVICEARRIVAL == wParam )
    {
      bDeviceAdd = TRUE;
    }
    else if( DBT_DEVICEREMOVECOMPLETE == wParam )
    {
      bDeviceAdd = FALSE;
    }
    else if( DBT_DEVICEREMOVEPENDING == wParam )
    {
      bDeviceAdd = FALSE;
    }
    else if( DBT_DEVICEQUERYREMOVE == wParam )
    {
      bDeviceAdd = FALSE;
    }

    if(pHdr)
    {
      switch (pHdr->dbch_devicetype)
      {
      case DBT_DEVTYP_DEVICEINTERFACE:
        {
          if(pDeviceInfo)
          {
            //
            // pDevInf->dbcc_name = 
            // \\?\USB#Vid_4146&Pid_d2b5#0005050400044#{a5dcbf10-6530-11d2-901f-00c04fb951ed} 
            // \\?\USB#Vid_05ac&Pid_1299#5eb9b5b5bcd92a0eff3dd918d50970b7b952845c#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
            //
            PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
            
            //PRINT_I("DBCC GUID:%s \n", CStringUtl::GUID2String(pDevInf->dbcc_classguid));
            //PRINT_I("DevName=%s \n", pDevInf->dbcc_name);             
            GetDeviceInterfaceInfo(pDevInf, wParam, pDeviceInfo, pszGuidFilter);
            //PRINT_I("GUID:%s \n", strClassGuid);
          } 
        }
        break;
        
      case DBT_DEVTYP_HANDLE:
        pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
        break;
        
      case DBT_DEVTYP_OEM:
        pDevOem = (PDEV_BROADCAST_OEM)pHdr;
        break;
        
      case DBT_DEVTYP_PORT:
        pDevPort = (PDEV_BROADCAST_PORT)pHdr;
        if(pDeviceInfo)
        {
          pDeviceInfo->DevType = DEV_TYPE_COM_PORT_E;
        }
        break;
        
      case DBT_DEVTYP_VOLUME:
        //pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
        break;
        
      case DBT_DEVTYP_NET:
        //pNet = (PDEV_BROADCAST_NET)pHdr;
        break;
      }
      return 0;
    }
  }
  
  return 1;
}


// Compute Device Class: this is used to get the tree contrl root icon
static const GUID GUID_DEVCLASS_COMPUTER = 
{ 0x4D36E966, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18} };

// Copy from HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses
static const GUID GUID_DEVINTERFACE_LIST[] = 
{
  // {36FC9E60-C465-11CF-8056-444553540000} 
	{ 0x36FC9E60, 0xC465, 0x11CF, { 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } },
  
    // GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
			 
	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }, 
    
	// GUID_DEVINTERFACE_COMPORT
	{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },

	// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
	{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },

	// GUID_DEVINTERFACE_PARALLEL
	{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },

	// GUID_DEVINTERFACE_PARCLASS
	{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } },

	// GUID_DEVINTERFACE_VOLUME 
	{ 0x53f5630d, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }, 

	// GUID_DEVCLASS_NET network device 
	{ 0x4d36e972, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } }, 

	// GUID_DEVCLASS_ENUM1394
	{ 0xc459df55, 0xdb08, 0x11d1, { 0xb0, 0x09, 0x00, 0xa0, 0xc9, 0x08, 0x1f, 0xf6} },

	// GUID_DEVCLASS_PORTS Serial port 
	{ 0x4d36e978, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } },
    
	// GUID_DEVCLASS_ADAPTER,        
	{ 0x4d36e964, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } }, 

	// GUID_DEVINTERFACE_STORAGEPORT
	{ 0x2accfe60, 0xc130, 0x11d2, { 0xb0, 0x82, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }, 
};

DEFINE_GUID(GUID_CLASS_COMPORT,          0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
DEFINE_GUID(GUID_SERENUM_BUS_ENUMERATOR, 0x4D36E978L, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);

#undef WINVER
#define WINVER 0x0500
#include "WINUSER.H"

bool CDeviceMgr_i::RegisterDeviceNotify( HANDLE hRecipient )
{
#if 0 
	bool b = SetupDiGetClassImageList(&m_imgList);
	ASSERT(b);

	int nRootImg;
	b = SetupDiGetClassImageIndex(&m_imgList, &GUID_DEVCLASS_COMPUTER, &nRootImg);

	ASSERT(b);
	m_ctrlTree.SetImageList(CImageList::FromHandle(m_imgList.ImageList), TVSIL_NORMAL);
	int n = GetClassImgIndex(&GUID_DEVCLASS_COMPUTER);
	TCHAR szName[MAX_PATH];
	DWORD nSize = sizeof(szName);
	b = GetComputerName(szName, &nSize);
	ASSERT(b);
	hRoot = m_ctrlTree.InsertItem(szName, n, n);
#endif 
  
#if 0 
  typedef HDEVNOTIFY WINAPI (*pfn_RegisterDeviceNotification) (HANDLE hRecipient, LPVOID NotificationFilter, DWORD Flags);

  pfn_RegisterDeviceNotification pfn = NULL;
  HMODULE hModule = LoadLibrary(_T("User32.dll"));
  if(hModule)
  {
#ifdef UNICODE
    pfn = (pfn_RegisterDeviceNotification)GetProcAddress(hModule, "RegisterDeviceNotificationW");
#else
    pfn = (pfn_RegisterDeviceNotification)GetProcAddress(hModule, "RegisterDeviceNotificationA");
#endif 
  }
  else 
  {
    return FALSE;
  }

  if(pfn == NULL && hModule)
  {
    FreeLibrary(hModule);
    return FALSE;
  }
#endif 
  
  HDEVNOTIFY hDevNotify;
  DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
  
  ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
  NotificationFilter.dbcc_size       = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
  NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
  
  for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
  {
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
    
    hDevNotify =  RegisterDeviceNotification(hRecipient, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!hDevNotify)
    {
      return FALSE;
    }
  }
  
  //FreeLibrary(hModule);

  return TRUE;
}

#if 0 
int CDeviceMgr_i::tst()
{   
  HDEVINFO   hDevInfo;   
  SP_DEVINFO_DATA   DeviceInfoData;   
  DWORD   i;   
  //   Create   a   HDEVINFO   with   all   present   devices.   
  hDevInfo   =   SetupDiGetClassDevs(NULL,   0,   0,   DIGCF_PRESENT   |   DIGCF_ALLCLASSES   );   
  if   (hDevInfo   ==   INVALID_HANDLE_VALUE)   
  {   
    return   1;   
  }   

  //   Enumerate   through   all   devices   in   Set.   
  int   nComCount   =   0;   
  DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);   
  for   (i=0;SetupDiEnumDeviceInfo(hDevInfo,i,  &DeviceInfoData);i++)   
  {   
    DWORD   DataT;   
    LPTSTR   buffer   =   NULL;   
    DWORD   buffersize   =   0;   
    //   
    //   Call   function   with   null   to   begin   with,     
    //   then   use   the   returned   buffer   size     
    //   to   Alloc   the   buffer.   Keep   calling   until   
    //   success   or   an   unknown   failure.   
    //     
    while   (!SetupDiGetDeviceRegistryProperty(   
      hDevInfo,   &DeviceInfoData,   SPDRP_DEVICEDESC,  
      &DataT,   (PBYTE)buffer,   buffersize,   &buffersize))   
    {   
      if   (GetLastError()  ==  ERROR_INSUFFICIENT_BUFFER)   
      {   
        //   Change   the   buffer   size.   
        if   (buffer)   LocalFree(buffer);   
        buffer  =  (char*)LocalAlloc(LPTR,buffersize);   
      }   
      else   
      {   
        //   Insert   error   handling   here.   
        break;   
      } 
    }     
    if(strcmp(buffer,"Communications   Port")==0)   
      nComCount++;   
    
    TRACE_I("SPDRP_DEVICEDESC:[%s]\n",buffer);
    if (buffer) 
      LocalFree(buffer);   
    continue;   
  }   
  if   (   GetLastError()!=NO_ERROR   &&GetLastError()!=ERROR_NO_MORE_ITEMS   )   
  {   
    //   Insert   error   handling   here.   
    return   1;   
  }   
  TRACE("Communications   Port   count:%d   by   masterz\n"  ,nComCount   );   

  //     Cleanup   
  SetupDiDestroyDeviceInfoList(hDevInfo);   
  return   0;   
}     
#endif 
