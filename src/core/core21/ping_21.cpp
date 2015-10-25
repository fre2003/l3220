#include "StdAfx.h"
#include <afxsock.h>    // MFC socket extensions
#include <process.h>    /* _beginthread, _endthread */

// 
// Ping.h
// 

#pragma pack(1)

#define ICMP_ECHOREPLY	0
#define ICMP_ECHOREQ	8

class CPing
{
public:
  U32 Ping(LPCSTR pstrHost, UINT nRetries = 1, U32 ulTimeOut = 1);
  CString m_strRet;
  
public:
  int WaitForEchoReply(SOCKET s);
  
  // ICMP Echo Request/Reply functions
  int SendEchoRequest(SOCKET, LPSOCKADDR_IN);
  DWORD RecvEchoReply(SOCKET, LPSOCKADDR_IN, u_char*);
  u_short in_cksum(u_short* addr, int len);
  
protected:
  void WSAError(LPCSTR pstrFrom);
  U32 m_ulTimeOut;    // in sends
};

// IP Header -- RFC 791
typedef struct tagIPHDR
{
  u_char VIHL;    // Version and IHL
  u_char TOS; // Type Of Service
  short TotLen;   // Total Length
  short ID;   // Identification
  short FlagOff;  // Flags and Fragment Offset
  u_char TTL; // Time To Live
  u_char Protocol;    // Protocol
  u_short Checksum;   // Checksum
  struct in_addr iaSrc;   // Internet Address - Source
  struct in_addr iaDst;   // Internet Address - Destination
} IPHDR, *PIPHDR;

// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
  u_char Type;    // Type
  u_char Code;    // Code
  u_short Checksum;   // Checksum
  u_short ID; // Identification
  u_short Seq;    // Sequence
  char Data;  // Data
} ICMPHDR, *PICMPHDR;

#define REQ_DATASIZE 32 // Echo Request Data size

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
  ICMPHDR icmpHdr;
  DWORD dwTime;
  char cData[REQ_DATASIZE];
} ECHOREQUEST, *PECHOREQUEST;

// ICMP Echo Reply
typedef struct tagECHOREPLY
{
  IPHDR ipHdr;
  ECHOREQUEST echoRequest;
  char cFiller[256];
} ECHOREPLY, *PECHOREPLY;

#pragma pack()

U32 CPing::Ping(LPCSTR pstrHost, UINT nRetries, U32 ulTimeOut)
{
  U32 ret                   = 0;
  SOCKET rawSocket          = 0;
  LPHOSTENT lpHost          = NULL;
  UINT nLoop                = 0;
  int nRet                  = 0;
  struct sockaddr_in saDest = { 0, };
  struct sockaddr_in saSrc  = { 0, };
  DWORD dwTimeSent          = 0;
  DWORD dwElapsed           = 0;
  u_char cTTL               = 0;
  
  m_strRet = _T("");
  m_ulTimeOut = ulTimeOut;
  
  // Create a Raw socket
  rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (rawSocket == SOCKET_ERROR)
  {
    ret = WSAGetLastError();
    WSAError("socket()");
    return ret;
  }
  
  // Lookup host
  lpHost = gethostbyname(pstrHost);
  if (lpHost == NULL)
  {
    ret = WSAGetLastError();
    m_strRet.Format(_T("Host not found: %s"), pstrHost);
    return ret;
  }
  
  // Setup destination socket address
  saDest.sin_addr.s_addr =* ((u_long FAR*)(lpHost->h_addr));
  saDest.sin_family      = AF_INET;
  saDest.sin_port        = 0;
  
  // Tell the user what we're doing
  //m_strRet.Format("Pinging %s [%s] with %d bytes of data:", pstrHost, inet_ntoa(saDest.sin_addr), REQ_DATASIZE);
  m_strRet.Format(_T("Pinging %s with %d bytes of data:"), pstrHost, REQ_DATASIZE);
  
  // Ping multiple times
  for (nLoop = 0; nLoop < nRetries; nLoop++)
  {
    // Send ICMP echo request
    nRet = SendEchoRequest(rawSocket, &saDest);
    if (nRet != SOCKET_ERROR)
    {
      nRet = WaitForEchoReply(rawSocket);
      if (nRet == SOCKET_ERROR)
      {
        nRet = WSAGetLastError();
        WSAError("select()");
        break;
      }
      if (!nRet)
      {
        m_strRet.Format(_T("Request Timed Out"));
      }
      else
      {
        nRet = 0;
        
        // Receive reply
        dwTimeSent = RecvEchoReply(rawSocket, &saSrc, &cTTL);
        
        // Calculate elapsed time
        dwElapsed = GetTickCount() - dwTimeSent;
        m_strRet.Format(_T("Reply[%d] from: %s: bytes=%d time=%ldms TTL=%d"),
          nLoop + 1, inet_ntoa(saSrc.sin_addr), REQ_DATASIZE, dwElapsed, cTTL);
        
        if (nRetries == 1)
          break;
        
        Sleep(m_ulTimeOut*1000);
      }
    }
    else
    {
      nRet = WSAGetLastError();
      if (nRetries == 1)
        break;
      Sleep(m_ulTimeOut*1000);
    }
  }
  
  ret = closesocket(rawSocket);
  if (ret == SOCKET_ERROR)
  {
    WSAError("closesocket()");
  }
  
  return nRet;
}

int CPing::SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr)
{
  static ECHOREQUEST echoReq;
  static u_short nId  = 1;
  static u_short nSeq = 1;
  CHAR nRet;
  
  // Fill in echo request
  echoReq.icmpHdr.Type     = ICMP_ECHOREQ;
  echoReq.icmpHdr.Code     = 0;
  echoReq.icmpHdr.Checksum = 0;
  echoReq.icmpHdr.ID       = nId++;
  echoReq.icmpHdr.Seq      = nSeq++;
  
  // Fill in some data to send
  for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
    echoReq.cData[nRet] = ' ' + nRet;
  
  // Save tick count when sent
  echoReq.dwTime = GetTickCount();
  
  // Put data in packet and compute checksum
  echoReq.icmpHdr.Checksum = in_cksum((u_short*)&echoReq, sizeof(ECHOREQUEST));
  
  // Send the echo request                                  
  nRet = sendto(s,    /* socket */
    (LPSTR) & echoReq,    /* buffer */
    sizeof(ECHOREQUEST), 0,   /* flags */
    (LPSOCKADDR) lpstToAddr,  /* destination */
    sizeof(SOCKADDR_IN)); /* address length */
  
  if (nRet == SOCKET_ERROR)
  {
    // WSAError("sendto()");
  }
  
  return (nRet);
}

DWORD CPing::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char* pTTL)
{
  ECHOREPLY echoReply;
  int nRet;
  int nAddrLen = sizeof(struct sockaddr_in);
  
  // Receive the echo reply   
  nRet = recvfrom(s,  // socket
    (LPSTR) & echoReply,    // buffer
    sizeof(ECHOREPLY),  // size of buffer
    0,  // flags
    (LPSOCKADDR) lpsaFrom,  // From address
    &nAddrLen); // pointer to address len
  
  // Check return value
  if (nRet == SOCKET_ERROR)
    WSAError("recvfrom()");
  
  // return time sent and IP TTL
  *pTTL = echoReply.ipHdr.TTL;
  
  return (echoReply.echoRequest.dwTime);
}

int CPing::WaitForEchoReply(SOCKET s)
{
  struct timeval Timeout;
  fd_set readfds;
  
  readfds.fd_count    = 1;
  readfds.fd_array[0] = s;
  Timeout.tv_sec      = m_ulTimeOut;
  Timeout.tv_usec     = 0;
  
  return (select(1, &readfds, NULL, NULL, &Timeout));
}

void CPing::WSAError(LPCSTR lpMsg)
{
  CString strMsg;
  
  strMsg.Format(_T("%s - WSAError: %ld"), lpMsg, WSAGetLastError());
}

// 
// Mike Muuss' in_cksum() function
// and his comments from the original
// ping program
// 
// * Author -
// *    Mike Muuss
// *    U. S. Army Ballistic Research Laboratory
// *    December, 1983

/*
*			I N _ C K S U M
*
* Checksum routine for Internet Protocol family headers (C Version)
*
*/
u_short CPing::in_cksum(u_short* addr, int len)
{
  register int nleft  = len;
  register u_short* w = addr;
  register u_short answer;
  register int sum = 0;
  
  /* 
  * Our algorithm is simple, using a 32 bit accumulator (sum),
  *  we add sequential 16 bit words to it, and at the end, fold
  *  back all the carry bits from the top 16 bits into the lower
  *  16 bits.
  */
  while (nleft > 1)
  {
    sum +=* w++;
    nleft -= 2;
  }
  
  /* mop up an odd byte, if necessary */
  if (nleft == 1)
  {
    u_short u = 0;
    
    *(u_char*)(&u) = *(u_char*)w;
    sum += u;
  }
  
  /* 
  *add back carry outs from top 16 bits to low 16 bits
  */
  sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
  sum += (sum >> 16); /* add carry */
  answer = (u16)~sum;  /* truncate to 16 bits */
  return (answer);
}

#if 0
class CPingThread
{
public:
  CPingThread();
  virtual~ CPingThread();
  
  UINT m_dwID;
  HANDLE m_hThread;
  HANDLE m_hKillEvent;
  HANDLE m_hSignalEvent;
  
  static UINT __stdcall ThreadProc(void* pThis);
  
  void StartPing(UINT nRetries, CString strHost, HWND hWnd);
  
  CPing m_ping;
  CString m_strHost;
  HWND m_hWnd;
  UINT m_nRetries;
  U32 m_ulTimeOut;    // in seconds
};

CPingThread::CPingThread()
{
  m_dwID         = 0;
  m_hThread      = NULL;
  m_hKillEvent   = CreateEvent(NULL, TRUE, FALSE, NULL);
  m_hSignalEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  
  m_hThread = (HANDLE)_beginthreadex(NULL,    // Security
    0,   // Stack size - use default
    ThreadProc,  // Thread fn entry point
    (void*)this, 0,  // Init flag
    &m_dwID);    // Thread address
  
}

CPingThread::~CPingThread()
{
  SetEvent(m_hKillEvent);
  WaitForSingleObject(m_hThread, INFINITE);
  
}

UINT CPingThread::ThreadProc(void* lpParam)
{
  CPingThread* pThis = reinterpret_cast < CPingThread * >(lpParam);
  
  while (1)
  {
    HANDLE hObjects[2];
    
    hObjects[0] = pThis->m_hKillEvent;
    hObjects[1] = pThis->m_hSignalEvent;
    
    DWORD dwWait = WaitForMultipleObjects(2, hObjects, FALSE, INFINITE);
    
    if (dwWait == WAIT_OBJECT_0)
      break;
    
    if (dwWait == WAIT_OBJECT_0 + 1)
      pThis->m_ping.Ping(pThis->m_strHost, pThis->m_nRetries, pThis->m_ulTimeOut);
  }
  
  return 0;
}

void CPingThread::StartPing(UINT nRetries, CString strHost, HWND hWnd)
{
  m_nRetries = nRetries;
  m_strHost  = strHost;
  m_hWnd     = hWnd;
  SetEvent(m_hSignalEvent);
}

class CWinPing
{
public:
  CWinPing();
  virtual~ CWinPing();
  
  U32 StartPing(LPCTSTR pszHost, U32 m_ulTimeOut = 1, UINT nRetries = 1, HWND hWnd = NULL);
  U32 StopPing();
protected:
  VOID* pObject;
};

CWinPing::CWinPing()
{
  
}

CWinPing::~CWinPing()
{
  
}

U32 CWinPing::StartPing(LPCTSTR pszHost, U32 m_ulTimeOut /*=1*/ , UINT nRetries /*=1*/ , HWND hWnd /*=NULL*/ )
{
  CPingThread* p = new CPingThread;
  pObject = (VOID*)(p);
  p->}

U32 CWinPing::StopPing()
{
  ;
}
#endif

// Call WSAStartup or ITS_InitSocket to initiate use of Ws2_32.dll by a process
// Call WSACleanup or ITS_ExitSocke to terminate use of the Ws2_32.dll.
// EX: Ping_2(_T("127.0.0.1"));

U32 InitSocket()
{
  U32 ret = 0;
  
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  
  wVersionRequested = MAKEWORD(2, 2);
  err               = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0)
  {
    ret = WSAGetLastError();
    return ret;
  }
  
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
  {
    ret = WSAGetLastError();
    
    WSACleanup();
    return ret;
  }
  
  return ret;
}

U32 ExitSocket()
{
  U32 ret = WSACleanup();
  
  return ret;
}

CPing_i::CPing_i()
{
  InitSocket();
}

CPing_i::~CPing_i()
{
  ExitSocket();
}

U32 CPing_i::Ping(LPCSTR pstrHost, UINT nRetries, U32 ulTimeOut)
{
  U32 ret = 0;

  CPing ping;
  
  ret = ping.Ping(pstrHost, nRetries, ulTimeOut);
    
  return ret;
}

U32 CPing_i::Ping(LPCSTR pstrHost, CString& strErr, CString& strPingResult, UINT nRetries, U32 ulTimeOut)
{
  U32 ret = 0;
    
  strErr = _T("");

  CPing ping;
  
  ret = ping.Ping(pstrHost, nRetries, ulTimeOut);
  if (ret == WSAEADDRNOTAVAIL)
  {
    strErr.Format(_T("The remote address '%s' is not a valid address"), pstrHost);
  }
  else if(ret == WSANOTINITIALISED)
  {
    strErr.Format(_T("A successful WSAStartup call must occur before using this function"));
  }
  else if(ret == WSAENETDOWN)
  {
    strErr.Format(_T("The network subsystem has failed"));
  }  
  else if(ret == WSAEHOSTUNREACH)
  {
    strErr.Format(_T("The remote host cannot be reached from this host at this time"));
  } 
  else if(ret == WSAENETUNREACH)
  {
    strErr.Format(_T("The network cannot be reached from this host at this time"));
  }  
  else if(ret == WSAEDESTADDRREQ)
  {
    strErr.Format(_T("A destination address is required"));
  }   
  else if(ret == WSAETIMEDOUT)
  {
    strErr.Format(_T("The connection has been dropped"));
  }
  strPingResult = ping.m_strRet;
  
  return ret;
}

