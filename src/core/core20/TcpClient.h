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
// client.h: interface for the CSocketClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCK_H__DDEB84A3_241A_42AD_BBD7_5A62FB3DC627__INCLUDED_)
#define AFX_SOCK_H__DDEB84A3_241A_42AD_BBD7_5A62FB3DC627__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afx.h"
#include <winsock2.h>

class CSocketClient  
{
public:  
	CSocketClient();
	virtual ~CSocketClient();

public:
  LONG CreateSocker(char*);
  LONG Connect(WORD wPort);
	long StartMonitor();
  LONG Exit();
  LONG Reset();
  int SendCmd(char *szSend);
  
private:
	DWORD dwThreadId;
  static unsigned __stdcall LoadMonitorThreadFn(void*pvParam);
  DWORD RunMonitorThead();
  
private:
  int Send(SOCKET Socket,char *szSend);
  int Recv(SOCKET Socket,char *szRecv,int iRecvLen);
  DWORD Receive(SOCKET Socket,char *pData,DWORD iLength);

private:
	int GetLocalHostName(char* sHostName);
  int GetIpAddress(char *sHostName, char *sIpAddress);
  int GetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3);

private:
  BYTE m_ServerIp[255];
  UINT m_ServerPort;
  struct	sockaddr_in	m_ServerAddr;
  HANDLE m_hMutex;
  
private:
  BOOL m_bSending;
  BOOL m_bReceiving;
  BOOL m_bConnected;
  BOOL m_bSent;
  SOCKET m_ClientSocket;
  BYTE m_ip[4];
  char m_szMessage[81];

private:
#define RETURN(err, str) case err: return (str);
  CONST char* GetErrString(long error)
  {
    switch(error)
    {
      RETURN(WSASYSNOTREADY, "Indicates that the underlying network subsystem is not ready for network communication.");
      RETURN(WSAVERNOTSUPPORTED, "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.");
      RETURN(WSAEPROCLIM, "Limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
      RETURN(WSAEFAULT, "The lpWSAData of WSAStartup() is not a valid pointer .");
      RETURN(WSANOTINITIALISED, "A successful WSAStartup call must occur before using this function.");
      RETURN(WSAENETDOWN, "The network subsystem has failed.");
      RETURN(WSAEAFNOSUPPORT, "The specified address family is not supported.");
      RETURN(WSAEINPROGRESS, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
      RETURN(WSAEMFILE, "No more socket descriptors are available.");
      RETURN(WSAENOBUFS, "No buffer space is available. The socket cannot be created.");
      RETURN(WSAEPROTONOSUPPORT, "The specified protocol is not supported.");
      RETURN(WSAEPROTOTYPE, "The specified protocol is the wrong type for this socket.");
      RETURN(WSAESOCKTNOSUPPORT, "The specified socket type is not supported in this address family.");
      RETURN(WSAENOTCONN, "The socket is not connected.");
      RETURN(WSAENOTSOCK, "The descriptor is not a socket.");
      RETURN(WSAESHUTDOWN, "The socket has been shut down; ");
      RETURN(WSAEMSGSIZE, "The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.");
      RETURN(WSAEINVAL, "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.");
      RETURN(WSAEHOSTUNREACH, "The remote host cannot be reached from this host at this time.");
      RETURN(WSAECONNABORTED, "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.");
      RETURN(WSAETIMEDOUT, "The connection has been dropped, because of a network failure or because the system on the other end went down without notice.");
      RETURN(WSA_NOT_ENOUGH_MEMORY, "Not enough free memory available to create the event object.");
      RETURN(WSAECONNRESET, "The virtual circuit was reset by the remote side.");
      RETURN(WSAEDISCON, "Socket s is message oriented and the virtual circuit was gracefully closed by the remote side.");
      RETURN(WSA_IO_PENDING, "An overlapped operation was successfully initiated and completion will be indicated at a later time.");
      RETURN(WSA_OPERATION_ABORTED, "The overlapped operation has been canceled due to the closure of the socket.");
      RETURN(WSAEINTR, "The (blocking) call was canceled through WSACancelBlockingCall.");
      RETURN(WSAENETRESET, "The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.");
                            
    }
    return "Unknown error";
  };
};

#endif 
