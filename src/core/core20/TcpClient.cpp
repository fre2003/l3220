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
// Sock.cpp: implementation of the CSock class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TcpClient.h"


// Get last error message
CString GetLastErrorMsg(DWORD dwLastErr)
{
  CString strRet(_T(""));
  if(dwLastErr == 0)
  {
    dwLastErr = ::GetLastError();
  }

  LPTSTR lpszMsgBuf;
  
  if ( !FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    dwLastErr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (TCHAR*)(&lpszMsgBuf),
    0,
    NULL ))
  {
    // Handle the error.
    return strRet; 
  }
  
  strRet = lpszMsgBuf;
  
  // Free the buffer.
  LocalFree(lpszMsgBuf); 

  return strRet;
}


CSocketClient::CSocketClient()
{
	WSADATA				wsd;
 	char				name[20];
  
  m_bReceiving = FALSE;
  m_bSending = FALSE;

  memset((VOID*)m_ip, 0, sizeof(m_ip));

	if (WSAStartup(0x202,&wsd) != 0)
	{
		return  ;
	}
	GetLocalHostName(name);
	GetIpAddress(name,&m_ip[0],&m_ip[1],&m_ip[2],&m_ip[3]);
}

CSocketClient::~CSocketClient()
{
  m_bReceiving = FALSE;
  m_bSending = FALSE;
}

//获得本地IP
int CSocketClient::GetIpAddress(char *sHostName, char *sIpAddress)
{
	struct hostent FAR * lpHostEnt=gethostbyname(sHostName);
	if(lpHostEnt==NULL)
	{
		//产生错误
		sIpAddress=NULL;
		return GetLastError();
	}
	//获取IP
	LPSTR lpAddr=lpHostEnt->h_addr_list[0];
	if(lpAddr)
	{
		struct in_addr inAddr;
		memmove(&inAddr,lpAddr,4);
		//转换为标准格式
		strcpy(sIpAddress,inet_ntoa(inAddr));
	}
	return 0;
}

//获得本地IP
int CSocketClient::GetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3)
{
	struct hostent FAR * lpHostEnt=gethostbyname(sHostName);
	if(lpHostEnt==NULL)
	{
		//产生错误
		f0=f1=f2=f3=0;
		return GetLastError();
	}
	//获取IP
	LPSTR lpAddr=lpHostEnt->h_addr_list[0];
	if(lpAddr)
	{
		struct in_addr inAddr;
		memmove(&inAddr,lpAddr,4);
		*f0=inAddr.S_un.S_un_b.s_b1;
		*f1=inAddr.S_un.S_un_b.s_b2;
		*f2=inAddr.S_un.S_un_b.s_b3;
		*f3=inAddr.S_un.S_un_b.s_b4;
	}
	return 0;
}

int CSocketClient::SendCmd(char *szSend)
{
  int		iSendLen = strlen(szSend);
  int		iSent = strlen(szSend);
  
  iSent = Send(m_ClientSocket, szSend);
  if(iSent != SOCKET_ERROR)
  {
    while(iSent < iSendLen)
    {
      iSendLen -= iSent;
      iSent = Send(m_ClientSocket, szSend+iSent);
    }
  }
  else 
  { 
  }
  
  return iSent;
}


int CSocketClient::Send(SOCKET Socket, char *szSend)
{
  int		iSent;

  m_bSending = TRUE;
  iSent = send(Socket,szSend,strlen(szSend),0);
  m_bSending = FALSE;

  return iSent;
}

int CSocketClient::Recv(SOCKET Socket, char *szRecv,int iRecvLen)
{
	int		iRecv;
	m_bReceiving = TRUE;
	iRecv = recv(Socket,szRecv,iRecvLen,0);
	if (iRecv == SOCKET_ERROR || iRecv == 0)
  {
    int err = WSAGetLastError();
    iRecv = 0;
  }

	szRecv[iRecv] = '\0';
	m_bReceiving = FALSE;

	return iRecv;
}

//获得本地计算机名称
int CSocketClient::GetLocalHostName(char *sHostName)
{
  char szHostName[256];
  int nRetCode;
  nRetCode = gethostname(szHostName,sizeof(szHostName));
  if(nRetCode!=0)
  { 
    strcpy(sHostName,"unknown");
    return -1;
  }
  else
  {
    strcpy(sHostName,szHostName);
  }
  
  return 0;
}

long CSocketClient::StartMonitor()
{
  CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)CSocketClient::LoadMonitorThreadFn,(LPVOID)this,0,&dwThreadId);
  
  return 0;
}

unsigned __stdcall CSocketClient::LoadMonitorThreadFn(LPVOID lpParam) 
{
  CSocketClient* pClientSocket = (CSocketClient*)lpParam;
  pClientSocket->RunMonitorThead();
  
  return 0;
}

//LoadWinsock用来装载和初始化Winsock，绑定本地地址，创建监听socket，等候客户端连接
DWORD CSocketClient::RunMonitorThead() 
{
  while (TRUE)
  { 
#if 0 
    INT iUpdates;
    char szMessage[81];
		strcpy(szMessage, "REFRESH");
		INT iSent = Send(m_ClientSocket,szMessage);
		memset(szMessage,'\0',sizeof(szMessage));
		INT iRecv = Recv(m_ClientSocket,szMessage,80);
		if (iRecv == SOCKET_ERROR || iRecv == 0)
    {
      PrintErrLog(0, "Recv Failed, NO DATA or SOCKET ERROR. ");
      break;
    }
		iUpdates = atoi(szMessage);

    strcpy(szMessage,"OK\0");
    iSent = Send(m_ClientSocket,szMessage);
#endif 

    char pData[1024];
    
    while(TRUE)
    {
      pData[0] = '\0';
      
      DWORD dwRet = Receive(m_ClientSocket, pData, 1);
      
      if(dwRet == SOCKET_ERROR)
      {
        //break;
        continue;
      }
      
      pData[dwRet] = '\0';
      //iSent = Send(m_ClientSocket, "Receive 2\n");
    }
  }

	return 0;
}

LONG CSocketClient::Exit()
{
  // 发送结束消息到服务器
  memset(m_szMessage,'\0',sizeof(m_szMessage));
  strcpy(m_szMessage, "DISCONNECT");
  
  int iSent = Send(m_ClientSocket,m_szMessage);
  
  // 关闭socket
  closesocket(m_ClientSocket);
  // 清除socket
  WSACleanup();
  
  return 0;
}

LONG CSocketClient::Reset()
{
  // 发送结束消息到服务器
  memset(m_szMessage,'\0',sizeof(m_szMessage));
  strcpy(m_szMessage, "DISCONNECT");
  
  int iSent = Send(m_ClientSocket,m_szMessage);
  
  // 关闭socket
  closesocket(m_ClientSocket);
  
  return 0;
}

// void memblast(void* dest,void* src,DWORD count)
// {
// 	DWORD	iCount;
// 
// 	__asm
// 	{
// 		MOV		ECX,count
// 		SHR		ECX,2
// 		SHL		ECX,2
// 		MOV		iCount,ECX
// 		MOV		ESI,src	
// 		MOV		EDI,dest
// 		MOV		ECX,iCount
// 		SHR		ECX,2	
// 		REP		MOVSD	
// 
// 		MOV		ECX,count
// 		MOV		EAX,iCount
// 		SUB		ECX,EAX
// 
// 		JZ		Exit
// 		MOV		ESI,src	
// 		ADD		ESI,EAX	
// 		MOV		EDI,dest
// 		ADD		EDI,EAX
// 		REP		MOVSB	
// 		Exit:
// 	}
// }
// 
// 
// int SelectProtocols(DWORD dwSetFlags,DWORD dwNotSetFlags,LPWSAPROTOCOL_INFO lpProtocolBuffer,LPDWORD lpdwBufferLength,WSAPROTOCOL_INFO *pProtocol)
// {
// 	LPBYTE				pBuf;
// 	LPWSAPROTOCOL_INFO	pInfo;
// 	DWORD				dwNeededLen;
// 	LPWSAPROTOCOL_INFO	pRetInfo;
// 	DWORD				dwRetLen;
// 	int					nCount;
// 	int					nMatchCount;
// 	int					nRet;
// 
// 	dwNeededLen = 0;
// 	nRet = WSAEnumProtocols(NULL, NULL, &dwNeededLen);
// 	if (nRet == SOCKET_ERROR)
// 	{
// 		if (WSAGetLastError() != WSAENOBUFS)
// 			return SOCKET_ERROR;
// 	}
// 
// 	pBuf = (LPBYTE)malloc(dwNeededLen);
// 	if (pBuf == NULL)
// 	{
// 		WSASetLastError(WSAENOBUFS);
// 		return SOCKET_ERROR;
// 	}
// 
// 	nRet = WSAEnumProtocols(NULL,(LPWSAPROTOCOL_INFO)pBuf,&dwNeededLen);
// 	if (nRet == SOCKET_ERROR)
// 	{
// 		free(pBuf);
// 		return SOCKET_ERROR;
// 	}
// 	#define REJECTSET(f) \
// 	    ((dwSetFlags & f) && !(pInfo->dwServiceFlags1 & f))
// 	#define REJECTNOTSET(f) \
// 	    ((dwNotSetFlags &f) && (pInfo->dwServiceFlags1 & f))
// 	#define REJECTEDBY(f) (REJECTSET(f) || REJECTNOTSET(f))
// 
// 	pInfo = (LPWSAPROTOCOL_INFO)pBuf;	
// 	pRetInfo = lpProtocolBuffer;
// 	dwRetLen = 0;
// 	nMatchCount = 0;
// 	for(nCount = 0; nCount < nRet; nCount++)
// 	{
// 		while(1)
// 		{
// 			if (REJECTEDBY(XP1_CONNECTIONLESS))
// 				break;
// 			if (REJECTEDBY(XP1_GUARANTEED_DELIVERY))
// 				break;
// 			if (REJECTEDBY(XP1_GUARANTEED_ORDER))
// 				break;
// 			if (REJECTEDBY(XP1_MESSAGE_ORIENTED))
// 				break;
// 			if (REJECTEDBY(XP1_PSEUDO_STREAM))
// 				break;
// 			if (REJECTEDBY(XP1_GRACEFUL_CLOSE))
// 				break;
// 			if (REJECTEDBY(XP1_EXPEDITED_DATA))
// 				break;
// 			if (REJECTEDBY(XP1_CONNECT_DATA))
// 				break;
// 			if (REJECTEDBY(XP1_DISCONNECT_DATA))
// 				break;
// 			if (REJECTEDBY(XP1_SUPPORT_BROADCAST)) 
// 				break;
// 			if (REJECTEDBY(XP1_SUPPORT_MULTIPOINT))
// 				break;
// 			if (REJECTEDBY(XP1_MULTIPOINT_DATA_PLANE))
// 				break;
// 			if (REJECTEDBY(XP1_QOS_SUPPORTED))
// 				break;
// 			if (REJECTEDBY(XP1_UNI_SEND))
// 				break;
// 			if (REJECTEDBY(XP1_UNI_RECV))
// 				break;
// 			if (REJECTEDBY(XP1_IFS_HANDLES))
// 				break;
// 			if (REJECTEDBY(XP1_PARTIAL_MESSAGE))
// 				break;
// 
// 			dwRetLen += sizeof(WSAPROTOCOL_INFO);
// 			if (dwRetLen > *lpdwBufferLength)
// 			{
// 				WSASetLastError(WSAENOBUFS);
// 				*lpdwBufferLength = dwNeededLen;
// 				free(pBuf);
// 				return SOCKET_ERROR;
// 			}
// 			nMatchCount++;
// 			memblast(pRetInfo,pInfo,sizeof(WSAPROTOCOL_INFO));
// 			if (strcmp(pInfo->szProtocol,"MSAFD Tcpip [TCP/IP]") == 0)
// 				memblast(pProtocol,pInfo, sizeof(WSAPROTOCOL_INFO));
// 
// 			pRetInfo++;
// 			break;
// 		}
// 		pInfo++;
// 	}
// 	free(pBuf);
// 	*lpdwBufferLength = dwRetLen;
// 	return(nMatchCount);
// }

#define SETFLAGS			XP1_GUARANTEED_DELIVERY|XP1_GUARANTEED_ORDER
#define NOTSETFLAGS			XP1_CONNECTIONLESS
#define	LPBMIH				LPBITMAPINFOHEADER

LONG CSocketClient::CreateSocker(char* pServerIp)
{
  // 初始化IP地址
  if(pServerIp == NULL)
  {
    sprintf((char*)m_ServerIp, "%d.%d.%d.%d",m_ip[0],m_ip[1],m_ip[2],m_ip[3]);
  }
  else 
  {
    sprintf((char*)m_ServerIp, pServerIp);
  }

  //sprintf((char*)m_ServerIp, " 192.168.1.101");
  
  WSADATA	 wsd;
  DWORD		 dwLen;
  int		 nRet,nZero;
  //LPBYTE	 pBuf;
  //WSAPROTOCOL_INFO	Protocol;
  
  if (WSAStartup(0x202,&wsd) != 0)
  {
    return 1;
  }
  
  dwLen = 0;
  nRet = WSAEnumProtocols(NULL,NULL,&dwLen);
  if (nRet == SOCKET_ERROR)
  {
    int err = WSAGetLastError();
    if (err != WSAENOBUFS)
    {
      return err;
    }
  }
  
//   pBuf = (LPBYTE)malloc(dwLen);
//   
//   nRet = SelectProtocols(SETFLAGS,NOTSETFLAGS,(LPWSAPROTOCOL_INFO)pBuf,&dwLen,&Protocol);
//   
//   free(pBuf);
  
  m_ClientSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_IP,NULL,0,SOCK_STREAM);
  if (m_ClientSocket == INVALID_SOCKET)
  {
    return 1;
  }
  
  nZero = 0;
  setsockopt(m_ClientSocket,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));

  return ERROR_SUCCESS;
}


LONG CSocketClient::Connect(WORD wPort)
{ 
  m_ServerPort = wPort;  
  m_ServerAddr.sin_family = AF_INET;
  m_ServerAddr.sin_port = htons(wPort);
  m_ServerAddr.sin_addr.s_addr = inet_addr((char*)m_ServerIp);
  
  printf("Connect Port = %d \n", wPort);
  
  if (connect(m_ClientSocket,(struct sockaddr *)&m_ServerAddr,sizeof(m_ServerAddr)) != 0)
  {
    DWORD ret = WSAGetLastError();
    switch(ret)
    {
    case WSAECONNREFUSED :
    case WSAENETUNREACH:
    case WSAETIMEDOUT:
      break;
    }
    return 1;
  }
  
#if 0 
  int		iSend,iRecv, iWidth;
  char	szRecv[81];
  
  memset(szMessage,'\0',sizeof(szMessage));
  sprintf(szMessage,"OK\0");
  iSend = Send(m_ClientSocket,szMessage);
  iRecv = Recv(m_ClientSocket,szRecv,80);
  iWidth = atoi(szRecv);
  
  if (m_bConnected)
  {
    
  }
#endif 

  return ERROR_SUCCESS;
}


DWORD CSocketClient::Receive(SOCKET Socket,char *pData,DWORD iLength)
{
	WSAOVERLAPPED	olRecv;

	WSABUF		buffRecv;
	DWORD		dwRet,dwNumBytes,dwFlags;
	int			nWSAError;
	DWORD		iNumBytes = 0;

	WSAEVENT	hEventOlSock = 0;
  WSAEVENT	eventArray[2] = {0, };

	hEventOlSock = WSACreateEvent();
  if(hEventOlSock == WSA_INVALID_EVENT)
  {
     return 0;
  }
  
	eventArray[0] = hEventOlSock;
  
	ZeroMemory(&olRecv,sizeof(WSAOVERLAPPED));
	olRecv.hEvent= hEventOlSock;
	buffRecv.len = iLength;
	buffRecv.buf = pData;
  
	while (TRUE)
	{
		dwFlags    = 0;
		dwNumBytes = 0;
    
    // receives data from a connected socket.
		if ((dwRet = WSARecv(Socket,&buffRecv,1,&dwNumBytes,&dwFlags,&olRecv,NULL)) == SOCKET_ERROR) 
		{
			nWSAError = WSAGetLastError();
      if(nWSAError == WSAENOTCONN)
      {
          goto end; // The socket is not connected.
      }
			else if (nWSAError != ERROR_IO_PENDING)
			{
        // Re-connect sock.
        if(m_ClientSocket)
        {
          closesocket(m_ClientSocket);
          m_ClientSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_IP,NULL,0,SOCK_STREAM);
          if (m_ClientSocket == INVALID_SOCKET)
          {
            nWSAError = WSAGetLastError();
            return 1;
          }
          
          int nZero = 0;
          setsockopt(m_ClientSocket,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));
        }
        
        while(Connect(m_ServerPort) != ERROR_SUCCESS)
        {
          Sleep(10);
        }
        
        goto end;
			}
		}
    
		if (WSAWaitForMultipleEvents(1, eventArray,FALSE,WSA_INFINITE,FALSE) == WSA_WAIT_FAILED) 
		{
		}
    
		WSAResetEvent(eventArray[0]);
		if (WSAGetOverlappedResult(Socket,&olRecv,&dwNumBytes,FALSE,&dwFlags) == TRUE)
		{
			if (dwNumBytes == 0)
				break;
		} 
		else 
    {   
      goto end;
		}
    
		buffRecv.len -= dwNumBytes;
		iNumBytes = iNumBytes + dwNumBytes;
		if (buffRecv.len == 0) 
			break;
		else
			buffRecv.buf += dwNumBytes;
  } // while .
  
  if(hEventOlSock)
    WSACloseEvent(hEventOlSock);

  return iNumBytes;
  
end:
	return SOCKET_ERROR;
}