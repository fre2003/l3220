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
#include "tchar.h"
#include "windows.h"
#include <afxtempl.h> 

#define ULONG_PTR    ULONG 
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

#include <SetupAPI.h>
#pragma comment(lib,"setupapi.lib")


static BOOL GetDeviceRegistryProperty( HDEVINFO hDevInfo, 
                                      PSP_DEVINFO_DATA pDeviceInfoData,
                                      DWORD dwProperty, CString& strProperty )
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
            // Don't exist Device desc
            bRet = FALSE;
            break;
        }
        else if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
        {
            // buffer size wrong
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


// Get PortName by DevInstanceId 
static INLINE CString GetPorName(LPCTSTR pszDevInstancedId)
{
  HKEY hKey;
  TCHAR szName[128] = {0, };
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


/*
Device ClassName='Ports'
 DescName='Communications Port'
 FriendName='Communications Port (COM2)'
 ManufacturerName='(Standard port types)'
 ServiceName='Serial'
 InstacneId='ACPI\PNP0501\4&2F94427B&0'
 PortName=COM2, GUID: 4D36E978-E325-11CE-BFC1-08002BE10318 
*/

u32 CDeviceMgr_i::GetDeviceInfo(LPCTSTR pszGuid,
                                LPCTSTR pszInstanceId, 
                                OUT CString& strClassName,
                                OUT CString& strDescName,
                                OUT CString& strFriendName,
                                OUT CString& strManufacturerName, 
                                OUT CString& strServiceName )
{
  u32 ret = ERR_OK_I;
  HDEVINFO DeviceInfoSet = NULL;
  SP_DEVINFO_DATA DeviceInfoData = {0, };
  u32 i;
  bool Found;
  TCHAR szInstanceId[512]  = {0, };
  
  strClassName.Empty();
  strDescName.Empty();
  strFriendName.Empty();
  strManufacturerName.Empty();
  strServiceName.Empty();

  if(pszGuid == NULL)
  {
    ret = ERR_INVALID_PARAM_I;

    return ret;
  }
  
  // 
  // Create a Device Information Set with all present devices.
  // 
  DeviceInfoSet = SetupDiGetClassDevs(NULL,     // All Classes
    0, 0, DIGCF_ALLCLASSES | DIGCF_PRESENT);    // All devices present on system
  if (DeviceInfoSet == INVALID_HANDLE_VALUE)
  {
     return ERR_WIN32_ERROR_I;
  }
   
  ret = ERR_WIN32_ERROR_I;

  // 
  //  Enumerate through all Devices.
  // 
  Found                 = FALSE;
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  
  for (i = 0; SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData); i++)
  {    
    CString strQuery; 
    CString strIn;

    if(pszInstanceId && (SetupDiGetDeviceInstanceId(DeviceInfoSet, &DeviceInfoData, (PTSTR)szInstanceId, sizeof(szInstanceId), NULL)))
    {
      strQuery = szInstanceId; 
      strIn = pszInstanceId;
      
      //TRACE_I("*** %s %s \n", szInstanceId, CStringUtl::GUID2String(DeviceInfoData.ClassGuid));
      
      strQuery.TrimLeft();
      strQuery.TrimRight();
      strIn.TrimLeft();
      strIn.TrimRight();
      strQuery.MakeUpper();
      strIn.MakeUpper();
      if(strIn != strQuery)
      {
        continue;
      }
    }
    
    strQuery = CStringUtl::GUID2String(DeviceInfoData.ClassGuid); 
    strIn  = pszGuid;
    strQuery.TrimLeft();
    strQuery.TrimRight();
    strIn.TrimLeft();
    strIn.TrimRight();
    strQuery.MakeUpper();
    strIn.MakeUpper();
    if(strIn != strQuery)
    {
      continue;
    }
    
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
    
#if 0 
    CString strDevObjName;
    if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME , strDevObjName ) )
    {
      // continue;
    }
#endif 
    
    ret = ERR_OK_I;
    break; 
  }
  
  if(DeviceInfoSet)
  {
    SetupDiDestroyDeviceInfoList(DeviceInfoSet);
  }
  
  return ret;
}


u32 CDeviceMgr_i::GetDeviceInfo(LPCTSTR pszGuid,
                                LPCTSTR pszInstanceId, 
                                DWORD dwProperty, // SPDRP_CLASS, SPDRP_DEVICEDESC, SPDRP_XXXXX 
                                OUT CString& strProperty)
{
  u32 ret = ERR_OK_I;
  HDEVINFO DeviceInfoSet = NULL;
  SP_DEVINFO_DATA DeviceInfoData = {0, };
  u32 i;
  bool Found;
  TCHAR szInstanceId[512]  = {0, };
  
  if((pszGuid == NULL) || (pszInstanceId == NULL ))
  {
    ret = ERR_INVALID_PARAM_I;

    return ret;
  }
  
  // 
  // Create a Device Information Set with all present devices.
  // 
  DeviceInfoSet = SetupDiGetClassDevs(NULL,     // All Classes
    0, 0, DIGCF_ALLCLASSES | DIGCF_PRESENT);    // All devices present on system
  if (DeviceInfoSet == INVALID_HANDLE_VALUE)
  {
     return ERR_WIN32_ERROR_I;
  }
   
  ret = ERR_WIN32_ERROR_I;

  // 
  //  Enumerate through all Devices.
  // 
  Found                 = FALSE;
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  
  for (i = 0; SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData); i++)
  {
    if (SetupDiGetDeviceInstanceId(DeviceInfoSet, &DeviceInfoData, 
      (PTSTR)szInstanceId, sizeof(szInstanceId), NULL)) 
    {
      CString strQuery = szInstanceId; 
      CString strIn = pszInstanceId;

      //TRACE_I("*** %s %s \n", szInstanceId, CStringUtl::GUID2String(DeviceInfoData.ClassGuid));

      strQuery.TrimLeft();
      strQuery.TrimRight();
      strIn.TrimLeft();
      strIn.TrimRight();
      strQuery.MakeUpper();
      strIn.MakeUpper();
      if(strIn != strQuery)
      {
        continue;
      }
      
      strQuery = CStringUtl::GUID2String(DeviceInfoData.ClassGuid); 
      strIn  = pszGuid;
      strQuery.TrimLeft();
      strQuery.TrimRight();
      strIn.TrimLeft();
      strIn.TrimRight();
      strQuery.MakeUpper();
      strIn.MakeUpper();
      
      if(strIn != strQuery)
      {
        continue;
      }
      
      if ( !GetDeviceRegistryProperty( DeviceInfoSet, &DeviceInfoData, dwProperty, strProperty ) )
      {
        // continue;
      }
      
      ret = ERR_OK_I;
      break;
    }
  }
  
  if(DeviceInfoSet)
  {
    SetupDiDestroyDeviceInfoList(DeviceInfoSet);
  }
  
  return ret;
}


static bool FindPorts(IN LPCTSTR pszGuidArray,
                      OUT CStringArray& strPortNameArray, 
                      OUT CStringArray& strGuidArray, 
                      OUT CStringArray& strInstanceIdArray)
{
  HDEVINFO DeviceInfoSet = NULL;
  SP_DEVINFO_DATA DeviceInfoData = {0, };
  DWORD i = 0;
  bool Found = FALSE;
  CString strPortName;
  TCHAR szInstanceId[512]  = {0, };
  
  CStringToken_i oGuiStringToken(pszGuidArray, _T(','));
  
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
  
  // 
  //  Enumerate through all Devices.
  // 
  Found                 = FALSE;
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  
  for (i = 0; SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData); i++)
  {
    if (SetupDiGetDeviceInstanceId(DeviceInfoSet, &DeviceInfoData, 
      (PTSTR)szInstanceId, sizeof(szInstanceId), NULL)) 
    {
      strPortName = GetPorName(szInstanceId);
      if(strPortName.IsEmpty())
        continue;

      CString strGuid = CStringUtl::GUID2String(DeviceInfoData.ClassGuid);
      
      if(pszGuidArray && pszGuidArray[0])
      {
        for (s32 m = 0; m< oGuiStringToken.GetCount(); m++)
        {
          CString str = oGuiStringToken.GetAt(m);
          
          TRIM_CSTRING_UPPER_I(str);
          TRIM_CSTRING_UPPER_I(strGuid);
          
          if(strGuid == str)
          {
            strPortNameArray.Add(strPortName);
            strGuidArray.Add(strGuid);
            strInstanceIdArray.Add(szInstanceId);
          }
        }
      }
      else 
      {
        strPortNameArray.Add(strPortName);
        strGuidArray.Add(strGuid);
        strInstanceIdArray.Add(szInstanceId);
      }
    }
  }
  
  if(DeviceInfoSet)
  {
    SetupDiDestroyDeviceInfoList(DeviceInfoSet);
  }
  
  return 0;
}


bool CDeviceMgr_i::EnumComPort(COM_PORT_INFO ArrayCom[32], u32& ulArrayComCnt, bool bCheckComState)
{
  HKEY hKey = NULL;
  
  CStringArray strArrayCom;
  
  m_ulLastError = ERR_OK_I;  
  ulArrayComCnt = 0;

  RegOpenKey(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);
  if (hKey == NULL)
  {
    m_ulLastError = ERR_SYSTEM_NOT_REGISTERED_I;
    return FALSE;
  }
  
  U32 ulIdx              = 0;
  TCHAR pValueName[512] = { 0, };
  U32 ncbValueNameSize   = 512 - 1;
  U32 ulType             = REG_SZ;
  TCHAR pData[512]      = { 0, };
  U32 ncbDataSize        = 512 - 1;
  
  while (RegEnumValue(hKey, ulIdx, pValueName, &ncbValueNameSize, NULL, &ulType, (LPBYTE) pData, &ncbDataSize)
    != ERROR_NO_MORE_ITEMS)
  {
    ulIdx++;
    ncbValueNameSize = 512 - 1;
    ulType           = REG_SZ;
    ncbDataSize      = 512 - 1;
    strArrayCom.Add(pData);
  }
  
  RegCloseKey(hKey);
  
  for (S32 i = 0; i< strArrayCom.GetSize(); i++)
  {
    BOOL bFound = TRUE;
    U32 err = 0;
    U32 ulState = COMPORT_UNKNOWN;
    HANDLE     hCom = NULL;    
    
    if(bCheckComState)
    {
      hCom = CreateFile(strArrayCom[i],0,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      if(hCom == INVALID_HANDLE_VALUE)
      {
        hCom = NULL;
        err = ::GetLastError();
        if(err == ERROR_FILE_NOT_FOUND)
        {
          TRACE_I(_T("The system cannot find the port specified(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        else if(err == ERROR_ACCESS_DENIED )
        {
          TRACE_I(_T("Access is denied.(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        else if(err == ERROR_SHARING_VIOLATION )
        {
          TRACE_I(_T("The process cannot access the file because it is being used by another process.(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        hCom = CreateFile(_T("\\\\.\\") + strArrayCom[i],0,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      }
      
      if(INVALID_HANDLE_VALUE == hCom)
      {
        hCom = NULL;
        err = ::GetLastError();
        if(err == ERROR_FILE_NOT_FOUND)
        {
          TRACE_I(_T(" The system cannot find the port specified(err=%d). Port=%s \n"), err, strArrayCom[i]);
          bFound = FALSE;
          ulState = COMPORT_ACCESS_DENIED;
        }
        else if(err == ERROR_ACCESS_DENIED )
        {
          bFound = TRUE;
          ulState = COMPORT_BUSY;
          TRACE_I(_T(" Access is denied.(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        else if(err == ERROR_SHARING_VIOLATION )
        {
          TRACE_I(_T("The process cannot access the file because it is being used by another process.(err=%d). Port=%s \n"), err, strArrayCom[i]);
          bFound = TRUE;
          ulState = COMPORT_BUSY;
        }
        else 
        {
          bFound = TRUE;
          ulState = COMPORT_UNKNOWN;
        }
      }
      
      if(hCom)
      {
        bFound = TRUE;
        ulState = COMPORT_READY;
      }
    }
    else 
    {
      bFound = TRUE;
      ulState = COMPORT_UNKNOWN;
    }
    
    if(bFound)
    {
      ArrayCom[ulArrayComCnt].ulState  = (COMPORT_STATE_E)ulState;
        
      if(strArrayCom.GetSize() > i)
      {
        StrCpyN(ArrayCom[ulArrayComCnt].szPortName, strArrayCom[i], sizeof(ArrayCom[ulArrayComCnt].szPortName));
      }
      else 
      {
        ArrayCom[ulArrayComCnt].szPortName[0] = 0;
      }
      
      ArrayCom[ulArrayComCnt].szGuid[0] = 0; 
      
      ulArrayComCnt++;
      
      if(ulArrayComCnt>=32)
        break;
    }
    
    if(hCom)
    {
      CloseHandle(hCom);
      hCom = NULL;
    }
  }
  
  return TRUE;
}


//
// ERR_SYSTEM_NOT_REGISTERED_I : Don't register the Serial COM device to system.
//
u32 CDeviceMgr_i::QueryComPort(OUT COMPORT_ARRAY_T& rComPortArray, bool bCheckComState, LPCTSTR pszGuidArray)
{
  HKEY hKey = NULL;
  U32 ret = ERR_OK_I;
  CStringArray strArrayCom;
  
  rComPortArray.RemoveAll();
  
  RegOpenKey(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);
  if (hKey == NULL)
  {
    return ERR_SYSTEM_NOT_REGISTERED_I;
  }
  
  U32 ulIdx              = 0;
  TCHAR pValueName[512] = { 0, };
  U32 ncbValueNameSize   = 512 - 1;
  U32 ulType             = REG_SZ;
  TCHAR pData[512]      = { 0, };
  U32 ncbDataSize        = 512 - 1;
  
  while (RegEnumValue(hKey, ulIdx, pValueName, &ncbValueNameSize, NULL, &ulType, (LPBYTE) pData, &ncbDataSize)
    != ERROR_NO_MORE_ITEMS)
  {
    ulIdx++;
    ncbValueNameSize = 512 - 1;
    ulType           = REG_SZ;
    ncbDataSize      = 512 - 1;
    strArrayCom.Add(pData);
  }
  
  RegCloseKey(hKey);
     
  CStringArray strPortNameArray;
  CStringArray strInstanceIdArray;
  CStringArray strGuidArray;
  
  FindPorts(pszGuidArray,
    strPortNameArray, 
    strGuidArray, 
    strInstanceIdArray);
  
  CStringArray strPortNameChecked;
  CStringArray strGuidArrayChecked;
  CStringArray strInstanceIdChecked;
  
  for (S32 i = 0; i< strArrayCom.GetSize(); i++)
  {
    CString strPort = strArrayCom.GetAt(i);
    
    TRIM_CSTRING_UPPER_I(strPort); 
    
    for (S32 k = 0 ; k< strPortNameArray.GetSize(); k++)
    {
      CString str = strPortNameArray.GetAt(k);

      TRIM_CSTRING_UPPER_I(str); 
      if(strPort == str)
      {
        // find it.
        strPortNameChecked.Add(str);
        strGuidArrayChecked.Add(strGuidArray.GetAt(k));
        strInstanceIdChecked.Add(strInstanceIdArray.GetAt(k));
      }
    }
  } 
  
  if(bCheckComState)
  {
    HANDLE     hCom = NULL;
    for (S32 i = 0; i< strPortNameChecked.GetSize(); i++)
    { 
      BOOL bFound = TRUE;
      U32 err = 0;
      U32 ulState = COMPORT_UNKNOWN;
      hCom = NULL;
      
      hCom = ::CreateFile(strArrayCom[i],0,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      if(hCom == INVALID_HANDLE_VALUE)
      {
        hCom = NULL;
        err = ::GetLastError();
        if(err == ERROR_FILE_NOT_FOUND)
        {
          TRACE_I(_T("The system cannot find the port specified(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        else if(err == ERROR_ACCESS_DENIED )
        {
          TRACE_I(_T("Access is denied.(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        else if(err == ERROR_SHARING_VIOLATION )
        {
          TRACE_I(_T("The process cannot access the file because it is being used by another process.(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        hCom = CreateFile(_T("\\\\.\\") + strArrayCom[i],0,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      }
      
      if(INVALID_HANDLE_VALUE == hCom)
      {
        hCom = NULL;
        err = ::GetLastError();
        if(err == ERROR_FILE_NOT_FOUND)
        {
          TRACE_I(_T(" The system cannot find the port specified(err=%d). Port=%s \n"), err, strArrayCom[i]);
          bFound = FALSE;
          ulState = COMPORT_ACCESS_DENIED;
        }
        else if(err == ERROR_ACCESS_DENIED )
        {
          bFound = TRUE;
          ulState = COMPORT_BUSY;
          TRACE_I(_T(" Access is denied.(err=%d). Port=%s \n"), err, strArrayCom[i]);
        }
        else if(err == ERROR_SHARING_VIOLATION )
        {
          TRACE_I(_T("The process cannot access the file because it is being used by another process.(err=%d). Port=%s \n"), err, strArrayCom[i]);
          bFound = TRUE;
          ulState = COMPORT_BUSY;
        }
        else 
        {
          bFound = TRUE;
          ulState = COMPORT_UNKNOWN;
        }
      }
      
      if(hCom)
      {
        bFound = TRUE;
        ulState = COMPORT_READY;
      }
      
      if(bFound)
      {
        COM_PORT_INFO comport;

        comport.ulState = (COMPORT_STATE_E)ulState; 
        if(strPortNameChecked.GetSize() > i)
        {
          StrCpyN(comport.szPortName, strPortNameChecked[i], sizeof(comport.szPortName));
        }
        else 
        {
          comport.szPortName[0] = 0;
        }
        
        if(strGuidArrayChecked.GetSize() > i)
        {
          StrCpyN(comport.szGuid, strGuidArrayChecked[i], sizeof(comport.szGuid));
        }
        else 
        {
          comport.szGuid[0] = 0;
        }
        
        if(strInstanceIdChecked.GetSize() > i)
        {
          StrCpyN(comport.szInstanceId, strInstanceIdChecked[i],  sizeof(comport.szInstanceId));
        }
        else 
        {
          comport.szInstanceId[0] = 0;
        }
        rComPortArray.Add(comport);
      }
      if(hCom)
      {
        CloseHandle(hCom);
        hCom = NULL;
      }
    }
  }
  else 
  {
    COM_PORT_INFO comport;
    
    for (int j = 0; j < strPortNameChecked.GetSize(); j++)
    {
      comport.ulState = COMPORT_UNKNOWN;
      
      StrCpyN(comport.szPortName,    strPortNameChecked[j],     sizeof(comport.szPortName));      
      StrCpyN(comport.szGuid,        strGuidArrayChecked[j],    sizeof(comport.szGuid));
      StrCpyN(comport.szInstanceId,  strInstanceIdChecked[j],   sizeof(comport.szInstanceId));
      
      rComPortArray.Add(comport);
    } 
  }
  
  return ret;
}


#if 0 
BOOL ITS_EnumRegLPT(U32 ulArrayLpt[10])
{
  HKEY hKey;
  int PortId = 0;
  DWORD dinx = 0;
  TCHAR valuename[70]; // 分配数值名称缓冲区
  DWORD nsize = 69;   // 数值名称缓冲区大小
  DWORD k     = REG_SZ;   // 指明数据类型
  TCHAR vari[70]; // 分配数值缓冲区
  TCHAR strcom[70];
  DWORD ncbvari = 69; // 数值缓冲区大小
  int numPort   = 0;
  
  hKey = RegOpenKey(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\PARALLEL PORTS"), (PHKEY)(PVOID)&hKey);
  if (hKey == NULL)
  {
    return FALSE;
  }
  
  U32 num = 0;
  while (RegEnumValue(HKEY_LOCAL_MACHINE, dinx, valuename, &nsize, NULL, &k, (LPBYTE)vari, &ncbvari) != ERROR_NO_MORE_ITEMS)
  {
    dinx++; // 索引+1,准备取下一个值
    nsize   = 69; // 恢复原来大小
    ncbvari = 69;
    
    memset(strcom, 0, sizeof(strcom));
    memcpy(strcom, vari, sizeof(vari));
    
    if ((_stscanf(strcom, _T("LPT%d"), &PortId)) || _stscanf(strcom, _T("lpt%d"), &PortId))
    {
      ulArrayLpt[num++] = PortId;
    }
  }
  
  RegCloseKey(hKey);
  
  return TRUE;
}
#endif 

#if 0 
evice ClassName='Ports'
 DescName='Communications Port'
 FriendName='Communications Port (COM2)'
 ManufacturerName='(Standard port types)'
 ServiceName='Serial'
 InstacneId='ACPI\PNP0501\4&2F94427B&0'
 PortName=COM2, GUID: 4D36E978-E325-11CE-BFC1-08002BE10318 


Device ClassName='CNCPorts'
 DescName='com0com - serial port emulator'
 FriendName=''
 ManufacturerName='Vyacheslav Frolov'
 ServiceName='com0com'
 InstacneId='COM0COM\PORT\CNCA0'
 PortName=CNCA0, GUID: DF799E12-3C56-421B-B298-B6D3642BC878 


Device ClassName='CNCPorts'
 DescName='com0com - serial port emulator'
 FriendName=''
 ManufacturerName='Vyacheslav Frolov'
 ServiceName='com0com'
 InstacneId='COM0COM\PORT\CNCA1'
 PortName=COM10, GUID: DF799E12-3C56-421B-B298-B6D3642BC878 


Device ClassName='CNCPorts'
 DescName='com0com - serial port emulator'
 FriendName=''
 ManufacturerName='Vyacheslav Frolov'
 ServiceName='com0com'
 InstacneId='COM0COM\PORT\CNCB0'
 PortName=CNCB0, GUID: DF799E12-3C56-421B-B298-B6D3642BC878 


Device ClassName='CNCPorts'
 DescName='com0com - serial port emulator'
 FriendName=''
 ManufacturerName='Vyacheslav Frolov'
 ServiceName='com0com'
 InstacneId='COM0COM\PORT\CNCB1'
 PortName=COM11, GUID: DF799E12-3C56-421B-B298-B6D3642BC878 


Device ClassName='Modem'
 DescName='Conexant HDA D330 MDC V.92 Modem'
 FriendName='Conexant HDA D330 MDC V.92 Modem'
 ManufacturerName='Conexant'
 ServiceName='Modem'
 InstacneId='HDAUDIO\FUNC_02&VEN_14F1&DEV_2C06&SUBSYS_14F1000F&REV_1000\4&8F2C18F&0&0102'
 PortName=COM3, GUID: 4D36E96D-E325-11CE-BFC1-08002BE10318 
#endif 
