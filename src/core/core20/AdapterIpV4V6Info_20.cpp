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
#include <stdafx.h>
#include <winsock2.h>
#include "../inc/iptypes.h"
#include "../inc/iphlpapi.h"
#include "core_common.h"
#include "../inc/Setupapi.h"
#include <Rpc.h>
#include <regstr.h>
#include <Cfgmgr32.h>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib")
//#pragma comment(lib, "Cfgmgr32.lib")

#define ETH_GUID_STRING  _T("4D36E972-E325-11CE-BFC1-08002BE10318") 
 
BOOL ControlAdapter( DWORD dwStatus, CONST TCHAR* GUIDString );

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

inline INT wcs2mbsz(CHAR* mbstr, INT cchMultiByte, CONST WCHAR* wcstr)
{
	if (cchMultiByte == 0 && mbstr == NULL)
		return 0;

	INT result = (INT)::WideCharToMultiByte(CP_ACP, 0, wcstr, -1, mbstr, cchMultiByte, NULL, NULL);

  if (result > 0)
		mbstr[result-1] = 0;

	return result;
}

inline INT mbs2wcsz(WCHAR* wcstr, INT cchWideChar, CONST CHAR* mbstr)
{
  INT result = (INT)::MultiByteToWideChar(CP_ACP, 0, mbstr, -1, wcstr, cchWideChar);
  if (result > 0)
    wcstr[result-1] = 0;
  
  return result;
}

inline bool IsWirelessCard(LPCTSTR pszAdapterName)
{
  DWORD dwType = REG_DWORD;
  DWORD dwCbData = sizeof(DWORD)*1;
  DWORD dwData = 0;
  TCHAR szSubKey[128] = {0, };
  
  wsprintf(szSubKey, _T("SYSTEM\\ControlSet001\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection"), pszAdapterName);
 
  // HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318} 
  SHGetValue(HKEY_LOCAL_MACHINE, szSubKey, _T("MediaSubType"), &dwType, &dwData, &dwCbData);
  if(dwData == 0x01) // 01-Normal 
  {
    return FALSE;
  }
  else if(dwData == 0x02) // 02-Wireless Crad
  {
    return TRUE;
  }

  return FALSE;
}

static BYTE g_buffer[1024*4*2];

BOOL FillIpAddr(TCHAR * buff,
                SOCKET_ADDRESS& Address, 
                int i, CDeviceMgr_i::IPADDR IpAddr[10] )
{
  DWORD bufflen = 256;
  buff[0] = 0;

  WSAAddressToString(Address.lpSockaddr, Address.iSockaddrLength, NULL, buff,&bufflen);
  if(i<10)
  {
    StrCpyN(IpAddr[i].szAddr, buff, MAX_IP_ADDR_CNT);
    IpAddr[i].sa_family = Address.lpSockaddr->sa_family;
    if(Address.lpSockaddr->sa_family == AF_INET)
    { 
      IpAddr[i].IsIpV6 = 0;
    }
    else if(Address.lpSockaddr->sa_family == AF_INET6)
    {
      IpAddr[i].IsIpV6 = 1;
    }
  }
  else 
  {
    return FALSE;
  }

  return TRUE;
}

// IpType : 0 : IPV6 and IPV4; 4:IPV4; 6:IPV6
U32 CDeviceMgr_i::GetAdapterInfo(NET_ADAPTER_INFO AdapterList[10],U8& AdapterListCnt, U8 IpType)
{
  U32 ret                             = ERR_OK_I;
  //DWORD dwSize = 0;
  //DWORD dwRetVal = 0;
  int i = 0;
  
  // Set the flags to pass to GetAdaptersAddresses
  ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
  
  // default to unspecified address family (both)
  ULONG family = AF_UNSPEC;
  
  //LPVOID lpMsgBuf = NULL;
  
  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  //ULONG outBufLen = 0;
  
  PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
  PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
  PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
  PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
  IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
  IP_ADAPTER_PREFIX *pPrefix = NULL;

  if (IpType == 4)
    family = AF_INET;
  else if (IpType == 6)
    family = AF_INET6;
  else 
    family = AF_UNSPEC;
  
  ULONG uListSize                     = sizeof(g_buffer); // max. 10 adpater
  //int nAdapterIndex                   = 0;
  bool bDeleteBuffer                  = FALSE;
  pAddresses = (PIP_ADAPTER_ADDRESSES)g_buffer;

  AdapterListCnt = 0;
  
  DWORD dwRet = GetAdaptersAddresses(family, flags, NULL, pAddresses, &uListSize);
  if (dwRet == ERROR_NO_DATA) // No adapter information exists for the local computer. 
  {
    TRACE_I(_T("** No adapter information exists for the local computer!\n"));
    ret = ERR_NO_ADAPTER_I;
  }
  else if (dwRet == ERROR_BUFFER_OVERFLOW)
  {
    pAddresses = (PIP_ADAPTER_ADDRESSES)HEAP_ALLOC(uListSize);
    bDeleteBuffer      = TRUE;
    dwRet              = GetAdaptersAddresses(family, flags, NULL, pAddresses, &uListSize);
    if(pAddresses == NULL)
      return ERR_NO_MORE_MEMORY_I;
  }
  else if(dwRet == ERROR_INVALID_PARAMETER)
  {
    ret = ERR_INVALID_PARAM_I;
  }
  else 
  {
    ret = ERR_WIN32_ERROR_I;
  }
  
  TCHAR buff[256];
  //DWORD bufflen = 256;
  
  if (dwRet == ERROR_SUCCESS)
  {
    ret = ERR_OK_I;
    
    pCurrAddresses = pAddresses;
    U8 ulIdx = 0;
    while (pCurrAddresses) 
    {
      if(ulIdx >= 10)
        break;
      
      NET_ADAPTER_INFO* pAdapterList = (NET_ADAPTER_INFO*)&AdapterList[ulIdx++];
      AdapterListCnt = ulIdx;
      
      //printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n", pCurrAddresses->Length);
      pAdapterList->IfIndex = pCurrAddresses->IfIndex;
      //printf("\tIfIndex (IPv4 interface): %u\n", pCurrAddresses->IfIndex); // Specifies the index of the interface with which these addresses are associated. 
      
#ifdef HAVE_UNICODE_I
      mbs2wcsz(buff, sizeof(buff),  pCurrAddresses->AdapterName);
      StrCpyN(pAdapterList->szAdapterName,buff, sizeof(pAdapterList->szAdapterName));
#else
      StrCpyN(pAdapterList->szAdapterName, pCurrAddresses->AdapterName, sizeof(pAdapterList->szAdapterName));
#endif
      //printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

      pAdapterList->bWirelessCard = IsWirelessCard(pAdapterList->szAdapterName);

#ifdef HAVE_UNICODE_I
      StrCpyN(pAdapterList->szDnsSuffix, pCurrAddresses->DnsSuffix, sizeof(pAdapterList->szDnsSuffix));
#else
      wcs2mbsz(buff, sizeof(buff),  pCurrAddresses->DnsSuffix);
      //printf("\tDNS Suffix: %s\n", buff);
      StrCpyN(pAdapterList->szDnsSuffix, buff, sizeof(pAdapterList->szDnsSuffix));
#endif
      
#ifdef HAVE_UNICODE_I
      StrCpyN(pAdapterList->szDescription, pCurrAddresses->Description, sizeof(pAdapterList->szDescription));
#else
      wcs2mbsz(buff, sizeof(buff),  pCurrAddresses->Description);
      StrCpyN(pAdapterList->szDescription, buff, sizeof(pAdapterList->szDescription));
#endif      
      //_tprintf(_T("\tDescription: %s\n"), pAdapterList->szDescription);

#ifdef HAVE_UNICODE_I
      StrCpyN(pAdapterList->szConnectionName, pCurrAddresses->FriendlyName, sizeof(pAdapterList->szConnectionName));
#else
      wcs2mbsz(buff, sizeof(buff),  pCurrAddresses->FriendlyName);
      StrCpyN(pAdapterList->szConnectionName, buff, sizeof(pAdapterList->szConnectionName));
#endif
      //printf("\tFriendly name: %s\n", buff);
      
      pUnicast = pCurrAddresses->FirstUnicastAddress;
      if (pUnicast != NULL)
      {
        for (i = 0; pUnicast != NULL; i++)
        {
          if(i>=0xFF)
            break;

          if(FillIpAddr(buff, pUnicast->Address, i, pAdapterList->UnicastAddr) == FALSE)
            break;
         //printf("\tIP Address(Unicast Addresses): %s \n", buff);
          pUnicast = pUnicast->Next;
        }
        pAdapterList->UnicastAddrCnt = (u8)(i&0x0FF);
        //printf("\tNumber of Unicast Addresses: %d\n", i);
      }
      else
      {
        pAdapterList->UnicastAddrCnt = 0;
        //printf("\tNo Unicast Addresses\n");
      }
      
      pAnycast = pCurrAddresses->FirstAnycastAddress;
      if (pAnycast)
      {              
        for (i = 0; pAnycast != NULL; i++)
        {
          if(i>=0xFF)
            break;
          
          if(FillIpAddr(buff, pAnycast->Address, i, pAdapterList->AnycastAddr) == FALSE)
            break;
          //printf("\tIP Address(Anycast Addresses): %s \n", buff);
          pAnycast = pAnycast->Next;  
        }
        //printf("\tNumber of Anycast Addresses: %d\n", i);
        pAdapterList->AnycastAddrCnt = (u8)(i&0x0FF);
      } 
      else
      {
        pAdapterList->AnycastAddrCnt  = 0;
        // printf("\tNo Anycast Addresses\n");
      }
      
      pMulticast = pCurrAddresses->FirstMulticastAddress;
      if (pMulticast)
      {
        for (i = 0; pMulticast != NULL; i++)
        {
          if(i>=0xFF)
            break;

          if(FillIpAddr(buff, pMulticast->Address, i, pAdapterList->MulticastAddr) == FALSE)
            break;
          //printf("\tIP Address(Multicast Addresses): %s \n", buff);
          pMulticast = pMulticast->Next;
        }
        pAdapterList->MulticastAddrCnt = (u8)(i&0x0FF);
        //printf("\tNumber of Multicast Addresses: %d\n", i);
      } 
      else
      {
        //printf("\tNo Multicast Addresses\n");
        pAdapterList->MulticastAddrCnt = 0;
      }
      
      pDnServer = pCurrAddresses->FirstDnsServerAddress;
      if (pDnServer) 
      {
        for (i = 0; pDnServer != NULL; i++)
        {
          if(i>=0xFF)
            break;

          if(FillIpAddr(buff, pDnServer->Address, i, pAdapterList->DnsServer) == FALSE)
            break;
          //printf("\tIP Address(DNS Server): %s \n", buff);
          pDnServer = pDnServer->Next;
        }
        pAdapterList->DnsServerCnt = (u8)(i&0x0FF);
        //printf("\tNumber of DNS Server Addresses: %d\n", i);
      } 
      else
      {
        //printf("\tNo DNS Server Addresses\n");
        pAdapterList->DnsServerCnt = 0;
      }
      
      pPrefix = pCurrAddresses->FirstPrefix;
      if (pPrefix) 
      {
        for (i = 0; pPrefix != NULL; i++)
        {
          if(i>=0xFF)
            break;

          if(FillIpAddr(buff, pPrefix->Address, i, pAdapterList->PrefixAddr) == FALSE)
            break;
          pPrefix = pPrefix->Next;
        }
        pAdapterList->PrefixAddrCnt = (u8)(i&0x0FF);
      } 
      else
      {
        pAdapterList->PrefixAddrCnt = 0;
        //printf("\tNo IP Adapter Prefix entries\n");
      }
      
      pAdapterList->uMacAddressLen = 0;
      pAdapterList->uMacAddress[0] = 0;
      if (pCurrAddresses->PhysicalAddressLength != 0) 
      {
        pAdapterList->uMacAddressLen = (u8)(pCurrAddresses->PhysicalAddressLength);
        memcpy(pAdapterList->uMacAddress, pCurrAddresses->PhysicalAddress, pAdapterList->uMacAddressLen);
        FormatMACToStr(pAdapterList->szMacAddr, pCurrAddresses->PhysicalAddress, pCurrAddresses->PhysicalAddressLength);
      }
      
      /*
      IP_ADAPTER_DDNS_ENABLED(0x01) Dynamic DNS is enabled on this adapter. 
      IP_ADAPTER_REGISTER_ADAPTER_SUFFIX (0x02) Register the DNS suffix for this adapter. 
      IP_ADAPTER_DHCP_ENABLED (0x04) Dynamic Host Configuration Protocol is enabled on this adapter. 
      */
      //printf("\tFlags: %ld\n", pCurrAddresses->Flags);
      pAdapterList->Flags = pCurrAddresses->Flags;
      pAdapterList->bDhcpEnabled = (pCurrAddresses->Flags&IP_ADAPTER_DHCP_ENABLED) ? TRUE:FALSE;
      pAdapterList->bDDNSEnabled = (pCurrAddresses->Flags&IP_ADAPTER_DDNS_ENABLED) ? TRUE:FALSE;
      pAdapterList->Mtu = pCurrAddresses->Mtu;
      //printf("\tMtu: %lu Bytes\n", pCurrAddresses->Mtu); // Specifies the maximum transmission unit (MTU), in bytes. 
      
      /*
      #define MIB_IF_TYPE_OTHER               1
      #define MIB_IF_TYPE_ETHERNET            6
      #define MIB_IF_TYPE_TOKENRING           9
      #define MIB_IF_TYPE_FDDI                15
      #define MIB_IF_TYPE_PPP                 23
      #define MIB_IF_TYPE_LOOPBACK            24
      #define MIB_IF_TYPE_SLIP                28
      */
      pAdapterList->IfType = pCurrAddresses->IfType;
      //printf("\tIfType: %ld\n", pCurrAddresses->IfType);  // Specifies the type of interface using the values defined by the Internet Assigned Numbers Authority (IANA). 
      
      if(pCurrAddresses->IfType == MIB_IF_TYPE_ETHERNET)
      {
        pAdapterList->bEthernetCard  = TRUE;
        pAdapterList->bPhysicalCard = TRUE;
      }
      else 
      {
        pAdapterList->bEthernetCard  = FALSE;
        pAdapterList->bPhysicalCard = FALSE;
      }
      
      /*
      // OperStatus values from RFC 2863
      typedef enum {
      IfOperStatusUp = 1,
      IfOperStatusDown,
      IfOperStatusTesting,
      IfOperStatusUnknown,
      IfOperStatusDormant,
      IfOperStatusNotPresent,
      IfOperStatusLowerLayerDown
      } IF_OPER_STATUS; */
      
      pAdapterList->OperStatus = pCurrAddresses->OperStatus;
      if(pCurrAddresses->OperStatus==IfOperStatusUp)
      {
        pAdapterList->bConnected = TRUE;
      }
      else if(pCurrAddresses->OperStatus==IfOperStatusDown)
      {
        pAdapterList->bConnected = FALSE;
      }
      
      //printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
      // Specifies the operational status of the interface 
      
      pAdapterList->Ipv6IfIndex = pCurrAddresses->Ipv6IfIndex;

      //printf("\tIpv6IfIndex (IPv6 interface): %u\n", pCurrAddresses->Ipv6IfIndex);      
      //       printf("\tZoneIndices (hex): ");
      
      //       for (i = 0; i < 16; i++)
      //       {
      //         printf("%lx ", pCurrAddresses->ZoneIndices[i]);
      //       }
      //       printf("\n");
      
      pCurrAddresses = pCurrAddresses->Next;
    } // while 
  }
  else if (dwRet == ERROR_NO_DATA) // No adapter information exists for the local computer. 
  {
    TRACE_I(_T("*** No adapter information exists for the local computer!\n"));
    ret = ERR_NO_ADAPTER_I;
  }
  else if (dwRet == ERROR_BUFFER_OVERFLOW)
  {
    ret = ERR_NO_MORE_MEMORY_I;
  }
  else if(dwRet == ERROR_INVALID_PARAMETER)
  {
    ret = ERR_INVALID_PARAM_I;
  }
  else 
  {
    ret = ERR_WIN32_ERROR_I;
  }
  
  if(bDeleteBuffer && pAddresses)
  {
    HEAP_FREE(pAddresses);
  }
  
  return ret;
}

U32 CDeviceMgr_i::SetIP(LPCTSTR pszAdapterName, LPCTSTR pszIPAddress, LPCTSTR pszNetMask )
{
    BOOL bRet = TRUE;
    HKEY hKey;
    CString strKeyName = _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\");
    
    strKeyName += pszAdapterName;
    if ( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKeyName, 0, KEY_WRITE, &hKey ) )
    {
        TRACE( _T("\nRegOpenKeyEx failed\n") );
        return FALSE;
    }

    const int MAX_SIZE = 100;
    TCHAR mszIPAddress[ MAX_SIZE ];
    TCHAR mszNetMask[ MAX_SIZE ];

    StrCpyN( mszIPAddress, pszIPAddress, MAX_SIZE);
    StrCpyN( mszNetMask, pszNetMask, MAX_SIZE);

    mszIPAddress[MAX_SIZE-1] = 0;
    mszNetMask[MAX_SIZE-1] = 0;

    int nIP = 0, nMask = 0;
    nIP   = lstrlen( mszIPAddress );
    nMask = lstrlen( mszNetMask );
    
    *( mszIPAddress + nIP + 1 ) = 0x00;         // REG_MULTI_SZ数据需要在后面再加个0
    nIP += 2;
   
    *( mszNetMask + nMask + 1 ) = 0x00;
    nMask += 2;
    
    nIP   = nIP * sizeof( TCHAR );
    nMask = nMask * sizeof( TCHAR );
   
    if ( ERROR_SUCCESS != RegSetValueEx( hKey, _T("IPAddress"), 0, REG_MULTI_SZ, ( unsigned char* )mszIPAddress, nIP ) )
    {
        bRet = FALSE;
    }

    if ( ERROR_SUCCESS != RegSetValueEx( hKey, _T("SubnetMask"), 0, REG_MULTI_SZ, ( unsigned char* )mszNetMask, nMask ) )
    {
        bRet = FALSE;
    }
    
    RegCloseKey( hKey );
    
    return bRet;
}

U32 CDeviceMgr_i::SetMAC(LPCTSTR pszAdapterName, LPCTSTR pszMACAddress)
{
  U32 ret = ERR_OK_I;
    BOOL bRet = FALSE;
    HKEY hKey;
    CString strKeyName = _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\");
    if ( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKeyName, 0, 
        KEY_WRITE | KEY_READ, &hKey ) ) 
    {
        TRACE( _T("\nRegOpenKeyEx failed.\n") );
        return FALSE;
    }
  
    const int BUFSIZE = 255;
    TCHAR szSubKeyName[ BUFSIZE ];
    DWORD dwIndex = 0, dwSubKeyName = BUFSIZE;
    FILETIME fileTime;
    
    while ( ERROR_SUCCESS == RegEnumKeyEx( hKey, dwIndex, szSubKeyName, 
        &dwSubKeyName, NULL, NULL, NULL, &fileTime ) )
    {
        dwIndex ++;
        dwSubKeyName = BUFSIZE;

        HKEY hSubKey;
        CString strSubKeyName;
        strSubKeyName = strKeyName + CString( szSubKeyName ) + _T("\\");
        if ( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, strSubKeyName, 0, 
            KEY_WRITE | KEY_READ, &hSubKey ) )
        {
            continue;
        }

        TCHAR szValue[ BUFSIZE ];
        DWORD dwType = 0, dwValueSize = sizeof( szValue );
        if ( ERROR_SUCCESS == RegQueryValueEx( hSubKey, _T("NetCfgInstanceId"), NULL, 
            &dwType, ( unsigned char* )szValue, &dwValueSize ) )
        {
            CString strValueText( szValue );
            if ( strValueText == pszAdapterName )
            {
                StrCpyN( szValue, pszMACAddress, BUFSIZE );
                DWORD dwDataSize = ( lstrlen(pszMACAddress) + 1 ) * sizeof( TCHAR );
                
                if ( ERROR_SUCCESS == RegSetValueEx( hSubKey, _T("NetworkAddress"), 0, 
                    REG_SZ, ( unsigned char* )szValue, dwDataSize ) )
                {
                    bRet = TRUE;
                }
                RegCloseKey( hSubKey );
                break;
            }
        }

        RegCloseKey( hSubKey );
    }

    RegCloseKey( hKey );

    if ( !ControlAdapter( DICS_PROPCHANGE, ETH_GUID_STRING ) )
    {
        bRet = FALSE;
    }

    return ret;
}


static BOOL ChangeStatus( DWORD dwNewStatus, DWORD dwSelectedItem, HDEVINFO hDevInfo )
{
    BOOL bRet = TRUE;

    TRY 
    {
        SP_DEVINFO_DATA DeviceInfoData;
        ZeroMemory( &DeviceInfoData, sizeof( SP_DEVINFO_DATA ) );
        DeviceInfoData.cbSize = sizeof( SP_DEVINFO_DATA );
        
        // Get a handle to the Selected Item
        if ( !SetupDiEnumDeviceInfo( hDevInfo, dwSelectedItem, &DeviceInfoData ) )
        {
            bRet = FALSE;
        }
        else
        {
            SP_PROPCHANGE_PARAMS PropChangeParams;
            ZeroMemory( &PropChangeParams, sizeof( SP_PROPCHANGE_PARAMS ) );
            PropChangeParams.ClassInstallHeader.cbSize = sizeof( SP_CLASSINSTALL_HEADER );

            // Set the PropChangeParams structure
            PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
            PropChangeParams.Scope = DICS_FLAG_GLOBAL;
            PropChangeParams.StateChange = dwNewStatus;
            
            if ( !SetupDiSetClassInstallParams( hDevInfo, &DeviceInfoData, ( SP_CLASSINSTALL_HEADER* )&PropChangeParams, sizeof( PropChangeParams ) ) )
            {
                bRet = FALSE;
            }
            else
            {
                // Call the ClassInstaller and perform the change
                if ( !SetupDiCallClassInstaller( DIF_PROPERTYCHANGE, hDevInfo, &DeviceInfoData ) )
                {
                    bRet = FALSE;
                }
                else
                {
                    //
                    // Is Need to restart computer 
                    //
                    SP_DEVINSTALL_PARAMS devParams;
                    ZeroMemory( &devParams, sizeof( SP_DEVINSTALL_PARAMS ) );
                    devParams.cbSize = sizeof( SP_DEVINSTALL_PARAMS );

                    if ( !SetupDiGetDeviceInstallParams( hDevInfo, &DeviceInfoData, &devParams ) )
                    {
                        bRet = FALSE;
                    }
                    else
                    {
                        if ( devParams.Flags & ( DI_NEEDRESTART | DI_NEEDREBOOT ) )
                        {
                            TRACE( _T("\nNeed Restart Computer\n") );
                        }
                    }
                }
            }
        }
    }
    CATCH_ALL( e )
    {
    	e->ReportError();
        bRet = FALSE;
    }
    END_CATCH_ALL

    return bRet;
}

static BOOL GetDeviceRegistryProperty( HDEVINFO hDevInfo, PSP_DEVINFO_DATA pDeviceInfoData, DWORD dwProperty, CString& strProperty )
{
    BOOL bRet = TRUE;
    DWORD dwDataType = 0;
    LPTSTR pszBuf = NULL;
    DWORD dwBuffersize = 0;
    
    while ( !SetupDiGetDeviceRegistryProperty( hDevInfo, pDeviceInfoData, dwProperty, &dwDataType, ( PBYTE )pszBuf, dwBuffersize, &dwBuffersize ) )
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

//
// After the adapter parameter is changed, should call this function to active it.
//
BOOL ControlAdapter( DWORD dwStatus, CONST TCHAR* GUIDString) // _T("4D36E972-E325-11CE-BFC1-08002BE10318"); 
{
    BOOL bRet = FALSE;
    
    if ( 0 == dwStatus )
    {
        return FALSE;
    }
    
    //TCHAR* GUIDString = _T("4D36E972-E325-11CE-BFC1-08002BE10318");
    GUID guid;
    ZeroMemory( &guid, sizeof( GUID ) );
    if ( RPC_S_OK != UuidFromString( (unsigned char*)GUIDString, &guid ) )
    {
        bRet = FALSE;
    }
    else
    {
        HDEVINFO hDevInfo = NULL;
        hDevInfo = SetupDiGetClassDevs( &guid, REGSTR_KEY_PCIENUM, NULL, DIGCF_PRESENT );
        if ( INVALID_HANDLE_VALUE == hDevInfo )
        {
            bRet = FALSE;
        }
        else
        {
            DWORD i = 0;
            SP_DEVINFO_DATA DeviceInfoData;
            ZeroMemory( &DeviceInfoData, sizeof( SP_DEVINFO_DATA ) );
            DeviceInfoData.cbSize = sizeof( SP_DEVINFO_DATA );

            for ( i = 0; SetupDiEnumDeviceInfo( hDevInfo, i, &DeviceInfoData ); ++i )
            {
                // 获得设备的状态
                DWORD dwProblem = 0, dwDeviceStatus = 0;
                if ( CR_SUCCESS != CM_Get_DevNode_Status( &dwDeviceStatus, &dwProblem, DeviceInfoData.DevInst, 0 ) )
                {
                    continue;
                }

                // 获取设备注册表项描述
                CString strText;
                if ( !GetDeviceRegistryProperty( hDevInfo, &DeviceInfoData, SPDRP_CLASS, strText ) )
                {
                    continue;
                }

                TRACE( _T("\n The %d device instance handle : %d, Class : %s\n"), i, DeviceInfoData.DevInst, strText );
                
                if ( 0 == lstrcmp( strText, _T("Net") ) )
                {
                    TRACE( _T("This is the adapter device that I want.\n") );

                    //////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
                    if ( GetDeviceRegistryProperty( hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, strText ) )
                    {
                        TRACE( _T("SPDRP_DEVICEDESC : %s\n"), strText );
                    }
#endif
                    //////////////////////////////////////////////////////////////////////////

                    if ( ChangeStatus( dwStatus, i, hDevInfo ) )
                    {
                        bRet = TRUE;
                    }
                }
            }

            // 释放 device information set
            bRet = SetupDiDestroyDeviceInfoList( hDevInfo );
        }
    }

    return bRet;
}

// Set IP, DNS, 
u32 CDeviceMgr_i::SetAdapterIpInfo(LPCTSTR pszConnectionName, 
                                   NET_ADAPTER_IP_INFO* pIpInfoArray, u8 ulIpInfoArrayCnt, 
                                   u32 nTimeOut)
{
  u32 ret = ERR_OK_I;

  if(pszConnectionName == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
#define IPINFO_FILE_NAME _T("C:\\l3230_ipinfo.txt") 

  FILE* f = _tfopen(IPINFO_FILE_NAME, _T("wt"));
  if (f)
  {
    fprintf(f, _T("pushd interface ip\n\n"));
    
    fprintf(f, _T("# Interface IP Configuration for \"%s\"\n\n"), pszConnectionName);
    
    if((pIpInfoArray == NULL) || (ulIpInfoArrayCnt == 0))
    {
      fprintf(f, 
        _T("set address name=\"%s\" source=dhcp\n")
        _T("set dns name=\"%s\" source=dhcp register=PRIMARY\n")
        _T("set wins name=\"%s\" source=dhcp\n"),
        pszConnectionName,pszConnectionName,pszConnectionName); 
    }
    else 
    {
      if(ulIpInfoArrayCnt == 1)
      {
        if(pIpInfoArray->pszIpAdd)
        {
          fprintf(f, 
            _T("set address name=\"%s\" source=static addr=%s mask=%s\n"), 
            pszConnectionName, pIpInfoArray->pszIpAdd, 
            pIpInfoArray->pszIpMask?pIpInfoArray->pszIpMask:_T("255.255.255.0")
            );
        }
        
        if(pIpInfoArray->pszGateway)
        {
          fprintf(f,  
            _T("set address name=\"%s\" gateway=%s gwmetric=0\n"), 
             pszConnectionName, pIpInfoArray->pszGateway
            ); 
        }
        
        if(pIpInfoArray->pszDnsAdd)
        {
          fprintf(f,
            _T("set dns name=\"%s\" source=static addr=%s register=PRIMARY\n"), 
             pszConnectionName, pIpInfoArray->pszDnsAdd
            );
        }
      }
      else 
      {
        for (s32 i = 0; i< ulIpInfoArrayCnt; i++)
        {  
          if(i == 0)
          {
            if(pIpInfoArray->pszIpAdd)
            {
              fprintf(f, 
                _T("set address name=\"%s\" source=static addr=%s mask=%s\n"),
                pszConnectionName, pIpInfoArray->pszIpAdd,
                pIpInfoArray->pszIpMask?pIpInfoArray->pszIpMask:_T("255.255.255.0")
                );
            }
            
            if(pIpInfoArray->pszGateway)
            {
              fprintf(f,  
                _T("set address name=\"%s\" gateway=%s gwmetric=0\n") ,
                pszConnectionName, pIpInfoArray->pszGateway
                ); 
            }
            
            if(pIpInfoArray->pszDnsAdd)
            {
              fprintf(f,
                _T("set dns name=\"%s\" source=static addr=%s register=PRIMARY\n"), 
                pszConnectionName, pIpInfoArray->pszDnsAdd
                );
            }
          }
          else 
          {
            if(pIpInfoArray->pszIpAdd)
            {
              fprintf(f, 
                _T("add address name=\"%s\" addr=%s mask=%s\n"),
                pszConnectionName, pIpInfoArray->pszIpAdd, 
                pIpInfoArray->pszIpMask?pIpInfoArray->pszIpMask:_T("255.255.255.0") 
                );
            }
        
            if(pIpInfoArray->pszGateway)
            {
              fprintf(f,  
                _T("add address name=\"%s\" gateway=%s gwmetric=0\n") ,
                 pszConnectionName, pIpInfoArray->pszGateway
                ); 
            }
        
            if(pIpInfoArray->pszDnsAdd)
            {
              fprintf(f,
                _T("add dns name=\"%s\" addr=%s  index=%d\n"), 
                 pszConnectionName, pIpInfoArray->pszDnsAdd, i+2
                );
            }
          }
          pIpInfoArray ++;
        }
      }
    }
    
    fprintf(f, _T("\npopd\n\n"));
    fclose(f);
    f = NULL;
  }
  
  if(PathFileExists(IPINFO_FILE_NAME))
  {
    CString strCmd;
    
    strCmd.Format(_T("netsh exec \"%s\""), IPINFO_FILE_NAME);
    ret = CProcessUtil_i::RunSilentProcess(strCmd, NULL , nTimeOut);
  }
  
  return ret;
}

