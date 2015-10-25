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
#include "Winsock2.h"

void byte2Hex(unsigned char bData,unsigned char hex[])
{
    int high=bData/16,low =bData %16;
    hex[0] = (unsigned char)((high <10)?('0'+high):('A'+high-10));
    hex[1] = (unsigned char)((low <10)?('0'+low):('A'+low-10));
}

#include "Iphlpapi.h"
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "Iphlpapi.lib")

BOOL ControlAdapter( DWORD dwStatus );

void formatMACToStr(LPSTR lpHWAddrStr,const unsigned char *HWAddr)
{
  int i;
  short temp;
  char szStr[3];
  
  strcpy(lpHWAddrStr, "");
  for (i=0; i<6; ++i)
  {
    temp = (short)(*(HWAddr + i));
    _itoa(temp, szStr, 16);
    
    if (strlen(szStr) == 1) 
      strcat(lpHWAddrStr, "0");
    strcat(lpHWAddrStr, szStr);
    
    if (i<5)  
      strcat(lpHWAddrStr, "-"); 
  }
}

inline void FormatMACToStr(TCHAR szMacAddr[20], BYTE* pMacAddress, UINT AddressLength)
{
  TCHAR szBuf[64] = {0,};
  
  ZeroMemory(szMacAddr, sizeof(TCHAR)*20);
  
  for (int i = 0; i < (int)AddressLength;i++) 
  {
    if (i == (int)(AddressLength - 1))
      wsprintf(szBuf, _T("%.2X"),(int)pMacAddress[i]);
    else
      wsprintf(szBuf,_T("%.2X-"),(int)pMacAddress[i]);
    lstrcat(szMacAddr, szBuf);
  }
}

VOID GetConnectionName(LPCTSTR pszAdapterName, TCHAR szConnectionName[128])
{
  DWORD dwType = REG_SZ;
  DWORD dwCbData = sizeof(TCHAR)*128;
  TCHAR szSubKey[128] = {0, };

  wsprintf(szSubKey, _T("SYSTEM\\ControlSet001\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection"), pszAdapterName);
 
  // HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318} 
  SHGetValue(HKEY_LOCAL_MACHINE, szSubKey, _T("Name"), &dwType, szConnectionName, &dwCbData);
}

// 
// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318}\{ACA306D0-1D69-4116-BC2B-919B428AD084}\Connection
//
bool IsPhyNetworkCard(LPCTSTR pszAdapterName)
{
  DWORD dwType = REG_SZ;
  DWORD dwCbData = sizeof(TCHAR)*512;
  TCHAR cbData[512+1] = {0, };
  TCHAR szSubKey[128] = {0, };
  
  wsprintf(szSubKey, _T("SYSTEM\\ControlSet001\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection"), pszAdapterName);
 
  // HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318} 
  SHGetValue(HKEY_LOCAL_MACHINE, szSubKey, _T("PnpInstanceID"), &dwType, cbData, &dwCbData);
  if(((int)cbData[0] == _T('P')) && ((int)cbData[1] == _T('C')) && ((int)cbData[2] == _T('I')))
  {
    return TRUE;
  }
  else 
  {
    return FALSE;
  }
}

bool IsWirelessNetworkCard(LPCTSTR pszAdapterName)
{
  DWORD dwType = REG_DWORD;
  DWORD dwCbData = sizeof(DWORD)*1;
  DWORD dwData = 0;
  TCHAR szSubKey[128] = {0, };
  
  wsprintf(szSubKey, _T("SYSTEM\\ControlSet001\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection"), pszAdapterName);
 
  // HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318} 
  SHGetValue(HKEY_LOCAL_MACHINE, szSubKey, _T("MediaSubType"), &dwType, &dwData, &dwCbData);
  if(dwData == 0x01) // 01 : Normal Card 
  {
    return FALSE;
  }
  else if(dwData == 0x02) // 02: Wireless Card.
  {
    return TRUE;
  }

  return FALSE;
}

bool IsConnect(TCHAR szIPAddr[10][16], S32 cnt) // unsigned char  
{ 
  for (S32 i = 0; i< cnt; i++)
  {
    for (S32 j = 0; j<16; j++)
    {
      if(szIPAddr[i][j] == '\0')
        break;
      
      if(szIPAddr[i][j] == '0' || szIPAddr[i][j] == '.' || szIPAddr[i][j] == ':')
      {
        continue;
      }
      else 
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}

void FillAdapterInfo( CDeviceMgr_i::NET_ADAPTER_INFOV4* pAdapterList, PIP_ADAPTER_INFO pAdapter )
{
  //TCHAR Name[MAX_ADAPTER_NAME_LENGTH + 4] = {0, };
  
#if defined(HAVE_UNICODE_I)
  Name[0] = 0; Name[1] = 0;
  CStringUtl::mbs2wcsz(Name, sizeof(Name), pAdapter->AdapterName);
  //lstrcpyW(pAdapterList->szAdapterName, Name ); 
  StrCpyN(pAdapterList->szAdapterName, Name, sizeof(pAdapterList->szAdapterName));
  
  Name[0] = 0; Name[1] = 0;
  CStringUtl::mbs2wcsz(Name, sizeof(Name), pAdapter->Description);
  StrCpyN(pAdapterList->szDescription, Name, sizeof(pAdapterList->szDescription));
#else
  StrCpyN(pAdapterList->szAdapterName, pAdapter->AdapterName, sizeof(pAdapterList->szAdapterName));
  //lstrcpyA(pAdapterList->szAdapterName, pAdapter->AdapterName);
  StrCpyN(pAdapterList->szDescription, pAdapter->Description, sizeof(pAdapterList->szDescription));
  //lstrcpyA(pAdapterList->szDescription, pAdapter->Description);
#endif 
  
  //TCHAR szAddr[16] = {0, };
  
  IP_ADDR_STRING* pIpAddr = &(pAdapter->IpAddressList);
  U32 idx = 0;
  while (pIpAddr && (idx < 10))
  {
#if defined(HAVE_UNICODE_I)
    szAddr[0] = 0; szAddr[1] = 0;
    CStringUtl::mbs2wcsz(szAddr, sizeof(szAddr), pIpAddr->IpAddress.String);
    //lstrcpyW(pAdapterList->szIPAddr[idx], szAddr ); 
    StrCpyN(pAdapterList->szIPAddr[idx], szAddr, 16);
    
    szAddr[0] = 0; szAddr[1] = 0;
    CStringUtl::mbs2wcsz(szAddr, sizeof(szAddr), pIpAddr->IpMask.String);
    StrCpyN(pAdapterList->szIPMask[idx], szAddr, 16);
    //lstrcpyW(pAdapterList->szIPMask[idx], szAddr ); 
#else
    StrCpyN(pAdapterList->szIPAddr[idx], pIpAddr->IpAddress.String, 16);
    //lstrcpyA(pAdapterList->szIPAddr[idx], pIpAddr->IpAddress.String ); 
    StrCpyN(pAdapterList->szIPMask[idx], pIpAddr->IpMask.String, 16);
    //lstrcpyA(pAdapterList->szIPMask[idx], pIpAddr->IpMask.String ); 
#endif 
    
    idx ++;
    pAdapterList->IPAddrCnt = (U8)idx;
    pAdapterList->IPMaskCnt = (U8)idx;
    
    pIpAddr = pIpAddr->Next;
  }
  
  pIpAddr = &(pAdapter->GatewayList);
  idx = 0;      
  while (pIpAddr && (idx < 10))
  {
#if defined(HAVE_UNICODE_I)
    szAddr[0] = 0; szAddr[1] = 0;
    CStringUtl::mbs2wcsz(szAddr, sizeof(szAddr), pIpAddr->IpAddress.String);
    StrCpyN(pAdapterList->szGatewayAddr[idx++], szAddr, 16);
    //lstrcpyW(pAdapterList->szGatewayAddr[idx++], szAddr );
#else
    StrCpyN(pAdapterList->szGatewayAddr[idx++], pIpAddr->IpAddress.String, 16);
    //lstrcpyA(pAdapterList->szGatewayAddr[idx++], pIpAddr->IpAddress.String ); 
#endif
    
    pAdapterList->GatewayAddrCnt = (U8)idx;
    pIpAddr = pIpAddr->Next;
  }
  
  pIpAddr = &(pAdapter->DhcpServer);
  idx = 0;      
  while (pIpAddr && (idx < 10))
  {
#if defined(HAVE_UNICODE_I)
    szAddr[0] = 0; szAddr[1] = 0;
    CStringUtl::mbs2wcsz(szAddr, sizeof(szAddr), pIpAddr->IpAddress.String);
    StrCpyN(pAdapterList->szDhcpServer[idx++], szAddr, 16);
    //lstrcpyW(pAdapterList->szDhcpServer[idx++], szAddr );
#else
    StrCpyN(pAdapterList->szDhcpServer[idx ++], pIpAddr->IpAddress.String, 16);
    //lstrcpyA(pAdapterList->szDhcpServer[idx ++], pIpAddr->IpAddress.String ); 
#endif 
    pAdapterList->DhcpServerCnt = (U8)idx;
    pIpAddr = pIpAddr->Next;
  }
  
  memcpy(pAdapterList->uMacAddress, pAdapter->Address, sizeof(pAdapter->Address));
  pAdapterList->uMacAddressLen = (u8)(pAdapter->AddressLength&0x0FF);
  
  FormatMACToStr(pAdapterList->szMacAddr, pAdapter->Address, pAdapter->AddressLength); 
  
  pAdapterList->bPhysicalCard = IsPhyNetworkCard(pAdapterList->szAdapterName);
  pAdapterList->bWirelessCard = IsWirelessNetworkCard(pAdapterList->szAdapterName);
  pAdapterList->bEthenetCard  = (pAdapter->Type == MIB_IF_TYPE_ETHERNET) ? TRUE : FALSE;
  pAdapterList->bConnected    = IsConnect(pAdapterList->szIPAddr, idx);
  pAdapterList->bDhcpEnabled  = pAdapter->DhcpEnabled==TRUE?TRUE:FALSE;
  
  GetConnectionName(pAdapterList->szAdapterName, pAdapterList->szConnectionName);
}


#define MIB_IF_TYPE_OTHER               1
#define MIB_IF_TYPE_ETHERNET            6  // Ethenet 
#define MIB_IF_TYPE_TOKENRING           9
#define MIB_IF_TYPE_FDDI                15
#define MIB_IF_TYPE_PPP                 23
#define MIB_IF_TYPE_LOOPBACK            24
#define MIB_IF_TYPE_SLIP                28


// typedef DWORD  (__stdcall *PGAINFO)(PIP_ADAPTER_INFO,PULONG);
  // Return Value: 
  // ERR_OK_I
  // ERR_INVALID_PARAM_I
  // ERR_WIN32_ERROR_I

// ERR_NO_ADAPTER_I: No adapter information exists for the local computer. 
U32 CDeviceMgr_i::GetAdapterInfoV4(NET_ADAPTER_INFOV4 AdapterList[10],U8& AdapterListCnt) 
{
  U32 ret                             = ERR_OK_I;
  ULONG uListSize                     = sizeof(IP_ADAPTER_INFO) * 10; // max. 10 adpater
  int nAdapterIndex                   = 0;
  bool bDeleteBuffer                  = FALSE;
  PIP_ADAPTER_INFO pAdapter           = NULL;
  IP_ADAPTER_INFO pAdpBuffer[10];
  PIP_ADAPTER_INFO pAdapterListBuffer = pAdpBuffer;
  
  AdapterListCnt = 0;

  DWORD dwRet = GetAdaptersInfo(pAdapterListBuffer, &uListSize); 
  if (dwRet == ERROR_NO_DATA) // No adapter information exists for the local computer. 
  { 
    OutputDbgStr(_T("** No adapter information exists for the local computer!\n"));
    ret = ERR_NO_ADAPTER_I;
  }
  else if (dwRet == ERROR_BUFFER_OVERFLOW)
  {
    pAdapterListBuffer = (PIP_ADAPTER_INFO)HEAP_ALLOC(uListSize);
    if(pAdapterListBuffer == NULL)
    {
      return ERR_NO_MORE_MEMORY_I;
    }

    bDeleteBuffer      = TRUE;
    dwRet              = GetAdaptersInfo(pAdapterListBuffer, &uListSize);
  }
  else if(dwRet == ERROR_INVALID_PARAMETER)
  {
    ret = ERR_INVALID_PARAM_I;
  }
  else 
  {
    ret = ERR_WIN32_ERROR_I;
  }
  
  if (dwRet == ERROR_SUCCESS)
  {
    pAdapter = pAdapterListBuffer;
    while (pAdapter) // 枚举网卡
    {
      FillAdapterInfo(&AdapterList[nAdapterIndex++], pAdapter);
      
      pAdapter = pAdapter->Next;
      AdapterListCnt = (u8)(nAdapterIndex&0xFF);
    }
  }
  else if (dwRet == ERROR_NO_DATA) // No adapter information exists for the local computer. 
  {
    OutputDbgStr(_T("*** No adapter information exists for the local computer!\n"));
    ret = ERR_NO_ADAPTER_I;
  }
  else if (dwRet == ERROR_BUFFER_OVERFLOW)
  {
    ret = ERR_UNKNOWN_I;
  }
  else if(dwRet == ERROR_INVALID_PARAMETER)
  {
    ret = ERR_INVALID_PARAM_I;
  }
  else 
  {
    ret = ERR_WIN32_ERROR_I;
  }
  
  if(bDeleteBuffer && pAdapterListBuffer)
  {
    HEAP_FREE(pAdapterListBuffer);
  }
  
  return ret;
}


bool IsValidIpAddress(IP_ADDR_STRING* pIpAddr)
{
  bool ret = FALSE;
  
  while (pIpAddr)
  {
    char * pszIpAddr = pIpAddr->IpAddress.String;
    for (S32 j = 0; j < 16; j++)
    {
      if (pszIpAddr[j] == '\0')
        break;
      
      if (pszIpAddr[j] == '0' || pszIpAddr[j] == '.' || pszIpAddr[j] == ':')
      {
        continue;
      }
      else
      {
        ret = TRUE;
        break;
      }
    }
    
    if (ret == TRUE)
      break;
    
    pIpAddr = pIpAddr->Next;
  } // while 

  return ret;
}

bool CDeviceMgr_i::IsAdapterConnectV4(BYTE uMacAddress[8], NET_ADAPTER_INFOV4* pConnectedAdpInfo)
{
  U32 ret                         = FALSE;
  int nAdapterIndex                = 0;
  DWORD uListSize                  = sizeof(IP_ADAPTER_INFO) * 10;
  IP_ADAPTER_INFO pAdapterListBuffer[10] = {0, };
  PIP_ADAPTER_INFO pAdapter = NULL;
  bool bConnect             = FALSE;

  DWORD dwRet = GetAdaptersInfo(pAdapterListBuffer, &uListSize);
  if (dwRet == ERROR_SUCCESS)
  {
    pAdapter = pAdapterListBuffer;
    while (pAdapter) // Enum All Adapters 
    {
      if(memcmp(pAdapter->Address, uMacAddress, sizeof(uMacAddress)) == 0)
      {
        IP_ADDR_STRING* pIpAddr = &(pAdapter->IpAddressList);
        if(IsValidIpAddress(pIpAddr))
        {
          bConnect = TRUE;
        }
        if(pConnectedAdpInfo)
        {
          FillAdapterInfo(pConnectedAdpInfo, pAdapter);
        }
        break; // find it 
      } // if 
      
      pAdapter = pAdapter->Next;      
      nAdapterIndex ++;
    }
  }
  else if (dwRet == ERROR_NO_DATA) // No adapter information exists for the local computer. 
  {
    OutputDbgStr(_T("*** No adapter information exists for the local computer!\n"));
    ret = ERR_NO_ADAPTER_I;
  }
  else  if (dwRet == ERROR_BUFFER_OVERFLOW)
  {
    PIP_ADAPTER_INFO pAdapterList = NULL;
    uListSize          = sizeof(IP_ADAPTER_INFO) * 400;
    pAdapterList       = (PIP_ADAPTER_INFO)new (char[uListSize]);
    dwRet              = GetAdaptersInfo(pAdapterList, &uListSize);
    if (dwRet == ERROR_SUCCESS)
    {
      pAdapter = pAdapterList;
      while (pAdapter)    // Enum All Adapters
      {
        IP_ADDR_STRING* pIpAddr = &(pAdapter->IpAddressList);

        // Find adapter.
        if (memcmp(pAdapter->Address, uMacAddress, sizeof(uMacAddress)) == 0)
        {
          if(IsValidIpAddress(pIpAddr))
          {
            bConnect = TRUE;
            
          }  
          if(pConnectedAdpInfo)
          {
            FillAdapterInfo(pConnectedAdpInfo, pAdapter);
          }      
          break; // find it.
        }
        
        pAdapter = pAdapter->Next;
        nAdapterIndex++;
      }
    }
  }
  
  return bConnect;
}

#if 0
u32 CDeviceMgr_i::GetMacAddress(U32 ulIpAddress)
{
  ;
}

u32 CDeviceMgr_i::GetMacAddress(U32 ulIpAddress[16])
{
  ;
}
#endif 

U32 CDeviceMgr_i::NotifyAddrChange(PHANDLE Handle, LPOVERLAPPED overlapped, BOOL bRoute)
{
  U32 ret = ERR_OK_I;
  
  if(bRoute)
    ::NotifyRouteChange(Handle, overlapped);
  ::NotifyAddrChange(Handle, overlapped);

  return ret;
} 

U32 CDeviceMgr_i::GetMacAddress(IN CONST TCHAR* szIpAddress, 
                                OUT TCHAR szMacAddr[20],  // string formt
                                OUT BYTE uMacAddress[8],  // hex format
                                OUT BYTE& uMacAddreLen)
{
  U32 ret = ERR_OK_I;

#if defined(HAVE_UNICODE_I)
  TCHAR ad[64] = {0, 0, };
  CStringUtl::wcs2mbsz(ad, sizeof(ad), szIpAddress);
  int nRemoteAddr =  inet_addr(ad);   
#else
  int nRemoteAddr =  inet_addr(szIpAddress);   
#endif
  
  uMacAddress[0] = _T('\0');
  
  ULONG   macAddLen = 8;   
  uMacAddreLen = 8;
  
  ret = SendARP(nRemoteAddr, (unsigned long)NULL,(PULONG)uMacAddress, (PULONG)&macAddLen);   
  if (ret == NO_ERROR)   
  {
    uMacAddreLen = (u8)(macAddLen&0xFF);
    FormatMACToStr(szMacAddr, uMacAddress, uMacAddreLen);
    ret = ERR_OK_I;
  }
  else
  {
    szMacAddr[0]   = _T('\0');
    uMacAddress[0] = 0;
    uMacAddress[1] = 0;
    uMacAddreLen   = 0;
    
    //CString str = GetLastErrorMsg_i(ret);
    
    if(ret == ERROR_GEN_FAILURE) // A device attached to the system is not functioning.
    {
      ret = ERR_GEN_FAILURE_I; // IP Address input is invalid or not existing.
    }
    else 
    {
      ret = ERR_WIN32_ERROR_I;
    }
  }
  
  return ret;
}

bool ITS_QueryDevice(IN LPTSTR HardwareId)
{
  //TCHAR buf[1024] = {0, };
  //QueuryDosDevice(NULL, buf, 1024);

  return TRUE;
}

#define ULONG_PTR    ULONG 
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
#include <SetupAPI.h>
#pragma comment(lib,"setupapi.lib")


static BOOL GetDeviceRegistryProperty( HDEVINFO hDevInfo, PSP_DEVINFO_DATA pDeviceInfoData, DWORD dwProperty, CString& strProperty )
{
    BOOL bRet = TRUE;
    DWORD dwDataType = 0;
    LPTSTR pszBuf = NULL;
    DWORD dwBuffersize = 0;
    
    while ( !SetupDiGetDeviceRegistryProperty( hDevInfo, pDeviceInfoData, dwProperty, &dwDataType, 
      ( PBYTE )pszBuf, dwBuffersize, &dwBuffersize ) )
    {
        if ( ERROR_INVALID_DATA == GetLastError() )
        {
            // 不存在 Device desc
            bRet = FALSE;
            break;
        }
        else if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
        {
            // buffer size 不对
            if ( NULL != pszBuf )
            {
                LocalFree( LocalHandle( pszBuf ) );
                pszBuf = NULL;
            }

            pszBuf = ( LPTSTR )LocalAlloc( LPTR, dwBuffersize );
            if ( NULL == pszBuf )
            {
                bRet = FALSE;
                break;
            }
        }
        else
        {
            // 未知错误
            bRet = FALSE;
            break;
        }
    }

    if ( bRet )
    {
        strProperty = pszBuf;
    }

    if ( NULL != pszBuf )
    {
        LocalFree( LocalHandle( pszBuf ) );
        pszBuf = NULL;
    }

    return bRet;
}

#define C0C_REGSTR_VAL_PORT_NUM     TEXT("Device Parameters\\PortName")

PRIVATE INLINE int GetPortNum(
    HDEVINFO hDevInfo,
    PSP_DEVINFO_DATA pDevInfoData)
{
  HKEY hKey;
  TCHAR szName[80] = {0, };
  
  hKey = SetupDiOpenDevRegKey(hDevInfo, pDevInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

  if (hKey == INVALID_HANDLE_VALUE)
    return -1;

  int num;
  DWORD len = sizeof(szName)-1; 
    
  if (RegQueryValueEx(hKey, C0C_REGSTR_VAL_PORT_NUM, NULL, NULL, (PBYTE)szName, &len) == ERROR_SUCCESS)
  {
      printf("PortName=%d \n", szName);
  }
  else
    num = -1;

  RegCloseKey(hKey);

  return num;
}

PRIVATE INLINE CString GetPorName(LPCTSTR pszDevInstancedId)
{
  HKEY hKey;
  TCHAR szName[256] = {0, };
  DWORD dwBufLen = sizeof(szName)-1;
  LONG lRet;
  DWORD dwType = REG_SZ;
  CString strPortName;
  
  CString strKeyName;
 
  strKeyName.Format(_T("SYSTEM\\ControlSet001\\Enum\\%s\\Device Parameters"), pszDevInstancedId);
   lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyName, 0, KEY_READ, &hKey);
  if (lRet == ERROR_SUCCESS)
  { 
    DWORD dwType = 0;
    DWORD dwSize = sizeof(DWORD);
    lRet = ::RegQueryValueEx(hKey, _T("PortName"), 0, &dwType, (LPBYTE) szName, &dwBufLen);
     strPortName = szName;
    ::RegCloseKey(hKey);     
  }
  
  return strPortName;
}



#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define FILE_DEVICE_BATTERY             0x00000029
#define FILE_DEVICE_BUS_EXTENDER        0x0000002a
#define FILE_DEVICE_MODEM               0x0000002b
#define FILE_DEVICE_VDM                 0x0000002c
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define FILE_DEVICE_SMB                 0x0000002e
#define FILE_DEVICE_KS                  0x0000002f
#define FILE_DEVICE_CHANGER             0x00000030
#define FILE_DEVICE_SMARTCARD           0x00000031
#define FILE_DEVICE_ACPI                0x00000032
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_FULLSCREEN_VIDEO    0x00000034
#define FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#define FILE_DEVICE_DFS_VOLUME          0x00000036


bool FindExistingDevice(IN LPTSTR HardwareId)
{
  HDEVINFO DeviceInfoSet;
  SP_DEVINFO_DATA DeviceInfoData;
  DWORD i, err;
  bool Found;
  
  // 
  // Create a Device Information Set with all present devices.
  // 
  DeviceInfoSet = SetupDiGetClassDevs(NULL,   // All Classes
    0, 0, DIGCF_ALLCLASSES | DIGCF_PRESENT);    // All devices present on system
  if (DeviceInfoSet == INVALID_HANDLE_VALUE)
  {
     //TRACE(TEXT("GetClassDevs(All Present Devices)"));
     return FALSE;
  }
  
  //TRACE(TEXT("Search for Device ID: [%s]\n"), HardwareId);
  
  // 
  //  Enumerate through all Devices.
  // 
  Found                 = FALSE;
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  for (i = 0; SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData); i++)
  {
    DWORD DataT;
    LPTSTR p, buffer = NULL;
    DWORD buffersize = 0;
    
    // 
    // We won't know the size of the HardwareID buffer until we call
    // this function. So call it with a null to begin with, and then 
    // use the required buffer size to Alloc the nessicary space.
    // Keep calling we have success or an unknown failure.
    // 
    while (!SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
      &DeviceInfoData,
      SPDRP_HARDWAREID, &DataT, (PBYTE)buffer, buffersize, &buffersize))
    {
      if (GetLastError() == ERROR_INVALID_DATA)
      {
        // 
        // May be a Legacy Device with no HardwareID. Continue.
        // 
        break;
      }
      else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        // 
        // We need to change the buffer size.
        // 
        if (buffer)
          LocalFree(buffer);
        buffer = (LPTSTR) LocalAlloc(LPTR, buffersize);
      }
      else
      {
        // 
        // Unknown Failure.
        // 
        //TRACE(TEXT("GetDeviceRegistryProperty"));
        goto cleanup_DeviceInfo;
      }
    }
    
    if (GetLastError() == ERROR_INVALID_DATA)
      continue;
    
    // 
    // Compare each entry in the buffer multi-sz list with our HardwareID.
    // 
    for (p = buffer; *p && (p < &buffer[buffersize]); p += lstrlen(p) + sizeof(TCHAR))
    {
      //TRACE_I(TEXT("Device ID: [%s]\n"), p);
      
      //if (!_tcscmp(HardwareId, p))
      {
        // TRACE(TEXT("Found![%s]\n"), p);
        //Found = TRUE;
        //break;
      }
    }
      
    TRACE_I(TEXT("Device ID:  %s \n"), buffer);
    
    if (buffer)
      LocalFree(buffer);

    if (Found)
      break;
    
    TCHAR name[80] = {0, };
    if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, 
      NULL, (PBYTE)name, sizeof(name), NULL)) 
    {
        //printf("DEVICE_OBJECT_NAME: %s \n", name);
    }

    TCHAR szEnumName[80] = {0, };
    if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_ENUMERATOR_NAME , 
      NULL, (PBYTE)szEnumName, sizeof(szEnumName), NULL)) 
    {
        //printf("SPDRP_ENUMERATOR_NAME: %s \n", szEnumName);
    }
    
    TCHAR szInstanceId[512] = {0, };
    DWORD dwDevType = 0;
    CString strPortName;
    
    if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_DEVTYPE, 
      NULL, (PBYTE)&dwDevType, sizeof(dwDevType), NULL)) 
    {
      if((dwDevType == FILE_DEVICE_MODEM) || 
        (dwDevType == FILE_DEVICE_SERIAL_PORT))
      {
        if (SetupDiGetDeviceInstanceId(DeviceInfoSet, &DeviceInfoData, (PTSTR)szInstanceId, sizeof(szInstanceId), NULL)) 
        {
          strPortName = GetPorName(szInstanceId);
          printf("szInstanceId: %s, PortName=%s \n", szInstanceId, strPortName);
        }
      }
    }   
        
    CString strObjName = name;

    // 获取设备注册表项描述
    CString strClassName;
    CString strDescName;
    CString strFriendName;
    CString strManufacturerName; 
    CString strServiceName;
    CString strUiNum;
    
    if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, SPDRP_CLASS, strClassName ) )
    {
      // continue;
    }
    
    if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, SPDRP_DEVICEDESC, strDescName ) )
    {
      // continue;
    }

    if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, strFriendName ) )
    {
      // continue;
    }
    
    if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, SPDRP_MFG, strManufacturerName ) )
    {
      // continue;
    }

    if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, SPDRP_SERVICE, strServiceName ) )
    {
      // continue;
    }

    if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, SPDRP_UI_NUMBER_DESC_FORMAT, strUiNum ) )
    {
      // continue;
    }
    
    if(CStringUtl::GUID2String(DeviceInfoData.ClassGuid) == _T("DF799E12-3C56-421B-B298-B6D3642BC878"))
    {
       GetPortNum(DeviceInfoSet, &DeviceInfoData); 
    }
    
    TRACE_I("OBJ=%s \n", strObjName);
    
   // if(strObjName == _T("Winachsf0") || strObjName == _T("\\Device\\Serial0"))
    {
      TRACE_I(_T("\nDevice ClassName='%s'\n DescName='%s'\n FriendName='%s'\n ManufacturerName='%s'\n")
        _T(" ServiceName='%s', UINumber='%s'\n, ObjName='%s'\n ENumName='%s'\n InstacneId='%s'\n PortName=%s, GUID: %s \n\n"), strClassName, 
        strDescName, strFriendName, strManufacturerName, strServiceName, strUiNum, 
        strObjName, szEnumName, szInstanceId, strPortName, 
        CStringUtl::GUID2String(DeviceInfoData.ClassGuid)); 
    }
  }
  
  if (GetLastError() != NO_ERROR)
  {
    TRACE_I(TEXT("EnumDeviceInfo, err=%d \n"), GetLastError());
  }
  
  // 
  //  Cleanup.
  //    
cleanup_DeviceInfo:
  err = GetLastError();
  SetupDiDestroyDeviceInfoList(DeviceInfoSet);
  SetLastError(err);
  
  return err == NO_ERROR; // ???
}

BOOL GetListIPAddresses(CStringArray* psIPAddressList, 
                        BOOL bPrimary /* = FALSE */, 
                        LPCSTR pszHostName/*=NULL*/)  
{
	//	--- In:			BOOL bPrimary :		TRUE if only primary IP must be 
	//										retrieved
	//					LPCSTR pszHostName:	Host name for which IP address 
	//										will be retrieved
	//	--- Out:		CStringArray* psIPAddressList: List of IP addresses
	//	--- Returns:	BOOL - TRUE if success
	//	---	Effect:		Retrieves the list of network IP address of a 
	//					multi-homed computer

	ASSERT(psIPAddressList!=NULL);

	CHAR		buffer[255];
	WORD		wVersionRequested;
	WSADATA		wsaData;
	int			err;
  
	HINSTANCE hLib = LoadLibrary(_T("WS2_32.DLL"));
	if (hLib != NULL)
	{
		// Loaded WS2_32.DLL...
		typedef int (WINAPI* WSASTARTUP) (WORD wVersionRequested, LPWSADATA lpWSAData);
		WSASTARTUP dWSAStartup = (WSASTARTUP) GetProcAddress(hLib, "WSAStartup");
		
		typedef int (WINAPI* GETHOSTNAME) (char FAR* name, int namelen);
		GETHOSTNAME dgethostname = (GETHOSTNAME) GetProcAddress(hLib, "gethostname");

		typedef struct hostent FAR* (WINAPI* GETHOSTBYNAME) (const char FAR* name);
		GETHOSTBYNAME dgethostbyname = (GETHOSTBYNAME) GetProcAddress(hLib, "gethostbyname");

		typedef int (WINAPI* WSACLEANUP) ();
		WSACLEANUP dWSACleanup = (WSACLEANUP) GetProcAddress(hLib, "WSACleanup");

		typedef char FAR* (WINAPI* INET_NTOA) (struct in_addr in);
		INET_NTOA dinet_ntoa = (INET_NTOA) GetProcAddress(hLib, "inet_ntoa");

		// Verify all dll entries
		if (dWSAStartup == NULL || dgethostname == NULL ||
			dgethostbyname == NULL || dWSACleanup == NULL || dinet_ntoa == NULL)
		{
			TRACE(_T("COXSysInfo::GetListIPAddresses - GetProcAddress() failed\n"));
			// Free the DLL
			FreeLibrary(hLib);
			return (FALSE);
		}

		wVersionRequested = MAKEWORD(1, 1);
		err = dWSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
		{
			TRACE(_T("COXSysInfo::GetListIPAddresses - WSAStartup() failed\n"));
			// Free the DLL
			FreeLibrary(hLib);
			return (FALSE);
		}

		BOOL bSuccess=TRUE;
		if(pszHostName==NULL)
		{
			bSuccess=(dgethostname(buffer, sizeof(buffer))==0);
			pszHostName=(LPCSTR)buffer;
		}
		if (bSuccess)
		{
			hostent* pHostInfo = dgethostbyname(pszHostName);
			if (pHostInfo != NULL)
			{
				ASSERT(pHostInfo->h_addrtype == PF_INET);
				ASSERT(pHostInfo->h_length == 4);
	
				// Iterate all the address of this host
				// and put them in stringarray
				int i = 0;
				in_addr address;
				while (pHostInfo->h_addr_list[i] != NULL)
				{
					PBYTE pAddress = (PBYTE)pHostInfo->h_addr_list[i];

					// ... Put the seperate bytes in a single long
					address.S_un.S_addr = (u_long)pAddress[0];
					address.S_un.S_addr += ((u_long)pAddress[1]) << 8;
					address.S_un.S_addr += ((u_long)pAddress[2]) << 16;
					address.S_un.S_addr += ((u_long)pAddress[3]) << 24;

					// ... Convert long to string (4 numbers seperated by dots)
					psIPAddressList->Add(CString(dinet_ntoa(address)));

					if (bPrimary)
						break;
				
					i++;
				}

				// Free the Winsock resources
				dWSACleanup();
				// Free the DLL
				FreeLibrary(hLib);
				return TRUE;
			}
		}

		TRACE(_T("COXSysInfo::GetListIPAddresses - gethostname() or gethostbyname() failed\n"));
		// Free the Winsock resources
		dWSACleanup();
		// Free the DLL
		FreeLibrary(hLib);
		return FALSE;
	}

	TRACE(_T("COXSysInfo::GetListIPAddresses - LoadLibrary of WS2_32.DLL failed\n"));
	return FALSE;
}


BOOL GetPrimaryIPAddress(CString *psIPAddress, 
                         LPCSTR pszHostName/*=NULL*/)  
{
	//	--- In:			LPCSTR pszHostName:		Host name for which primary 
	//											IP address will be retrieved
	//	--- Out:		CString *psIPAddress:	Current IP address (0.0.0.0 
	//											if undefined)
	//	--- Returns:	BOOL - TRUE if success
	//	---	Effect:		Retrieves the current network IP address

	ASSERT(psIPAddress != NULL);

	CStringArray IPArray;
	if(!GetListIPAddresses(&IPArray,TRUE,pszHostName))
		return FALSE;

	*psIPAddress=IPArray[0];
	return (TRUE);
}


