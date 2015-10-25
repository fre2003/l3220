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
#include "stdafx.h"
#include "serial_21.h"


CSerialBase_i::CSerialBase_i (LPCTSTR pszRxThreadName) : 
  CThread_i(NULL, 0, 
            (pszRxThreadName==NULL)?_T("CSerialBase"):pszRxThreadName, 
            CThread_i::SELF_DRIVEN_MODE) 
	, m_hFile(0)
	, m_eEvent(EEventNone)
	, m_dwEventMask(0)
	, m_hevtOverlapped(0)
{
	m_dwLastError = ERR_OK_I;
	m_hFile = NULL;
	m_eEvent = EEventNone;
	m_dwEventMask = 0;
	m_hevtOverlapped = 0;
	m_bShutdown = FALSE;
 	m_hOverlappedWorkerThreadEvent = NULL;
  m_dwWaitEventTimeOut = INFINITE;
}

CSerialBase_i::~CSerialBase_i ()
{
	// If the device is already closed, then we don't need to do anything.
	if (m_hFile)
	{
		// Close implicitly
		Close();
    
    m_hFile = NULL;
	}
}


// dwInQueue dwOutQueue Both queues need to be at least 16 bytes large. 
u32 CSerialBase_i::Open (LPCTSTR lpszDevice,
                      bool bAutoRxEventNotify,
                      DWORD dwInQueue, // Specifies the recommended size, in bytes, of the device's internal input buffer. 
                      DWORD dwOutQueue // Specifies the recommended size, in bytes, of the device's internal output buffer. 
                      ) 
{
	// Reset error state
	u32 ret = ERR_OK_I;

  m_bShutdown  = FALSE;
  
	// Check if the port isn't already opened
	if (m_hFile)
	{
		ret = ERR_ALREADY_INITIALIZED_I;
		m_dwLastError = ret;
    
		return ret;
	}
  
	// Open the device
	m_hFile = ::CreateFile(lpszDevice,
						   GENERIC_READ|GENERIC_WRITE,
						   0,
						   0,
						   OPEN_EXISTING,
						   FILE_FLAG_OVERLAPPED,
						   0);
	if ((m_hFile == INVALID_HANDLE_VALUE) || (m_hFile == NULL))
	{
    CString strDevice = _T("\\\\.\\");
    strDevice += lpszDevice;
    
		// Reset file handle
		m_hFile = ::CreateFile(strDevice,
						   GENERIC_READ|GENERIC_WRITE,
						   0,
						   0,
						   OPEN_EXISTING,
						   FILE_FLAG_OVERLAPPED,
						   0);

    if ((m_hFile == INVALID_HANDLE_VALUE) || (m_hFile == NULL))
    {
      m_hFile = NULL;
		  ret = ERR_WIN32_ERROR_I;
		  m_dwLastError = ret;
		  return ret;
    }
	}
  
	// We cannot have an event handle yet
	_ASSERTE(m_hevtOverlapped == 0);
  
  // Create the event handle for internal overlapped operations (manual reset)
  m_hevtOverlapped = ::CreateEvent(0,true,false,0);
  if (m_hevtOverlapped == 0)
  {
    // Obtain the error information
    ret = ERR_WIN32_ERROR_I;
    m_dwLastError = ret;
    
    // Close the port
    ::CloseHandle(m_hFile);
    m_hFile = 0;
    
    // Return the error
    return ret;
  }

	// Setup the COM-port
	if (dwInQueue || dwOutQueue)
	{
		// Make sure the queue-sizes are reasonable sized. Win9X systems crash
		// if the input queue-size is zero. Both queues need to be at least 16 bytes large.
		_ASSERTE(dwInQueue >= 16);
		_ASSERTE(dwOutQueue >= 16);
    
    if(dwInQueue <16)
       dwInQueue = 16;
    if(dwOutQueue < 16)
      dwOutQueue = 16;
    
		if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
		{
			// Close the port
			Close();
      
			// Save last error from SetupComm
			ret = ERR_WIN32_ERROR_I;
			m_dwLastError = ret;
			return ret;	
		}
	}

	// Setup the default communication mask
	SetMask();

	// Non-blocking reads is default
	SetupReadTimeouts(EReadTimeoutNonblocking);
  
	// Setup the device for default settings
 	COMMCONFIG commConfig = {0};
	DWORD dwSize = sizeof(commConfig);
	commConfig.dwSize = dwSize;

	if (::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize))
	{
		// Set the default communication configuration
		if (!::SetCommConfig(m_hFile,&commConfig,dwSize))
		{
			// Display a warning
			TRACE("CSerialBase_i::Open - Unable to set default communication configuration.\n");
		}
	}
	else
	{
		// Display a warning
		TRACE("CSerialBase_i::Open - Unable to obtain default communication configuration.\n");
	}
  
  if(ret == ERR_OK_I && bAutoRxEventNotify)
  {
    ret = StartListener();
  }
  
	// Return successful
	return ret;
}

U32 CSerialBase_i::Close (DWORD dwTimeout)
{
	// Reset error state
  U32 ret = ERR_OK_I;

	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile == 0)
	{
		// Display a warning
		TRACE("CSerialBase_i::Close - Method called when device is not open\n");
		return ret;
	}  
  
  ret = StopListener(dwTimeout);
  if(ret != ERR_OK_I)
  {
    return ret;
  }
  
	// Free event handle
	if (m_hevtOverlapped)
	{
		::CloseHandle(m_hevtOverlapped);
		m_hevtOverlapped = 0;
	}

	// Close COM port
	::CloseHandle(m_hFile);
	m_hFile = 0;
  
  m_dwLastError = ret;

	// Return successful
	return ret;
}

U32 CSerialBase_i::Setup (DWORD dwBaudrate,  DATABITS_E eDataBits, PARITY_E eParity, 
                          STOPBITS_E eStopBits,  HANDSHAKE_E eHandshake, DWORD dwWaitEventTimeOut)
{
  // Reset error state
  U32 ret = ERR_OK_I;
  
  if(dwWaitEventTimeOut != 0)
    m_dwWaitEventTimeOut = dwWaitEventTimeOut;
  
  // Check if the device is open
  if (m_hFile == 0)
  {
    // Set the internal error code
    ret = ERR_INVALID_HANDLE_I;
    m_dwLastError = ret;
    
    // Issue an error and quit
    TRACE("CSerialBase_i::Setup - Device is not opened\n");
    return ret;
  }
  
  // Obtain the DCB structure for the device
  DCB dcb;
  if (!::GetCommState(m_hFile,&dcb))
  {
    // Obtain the error code
    ret = ERR_WIN32_ERROR_I;
    m_dwLastError = ret;
    
    // Display a warning
    TRACE("CSerialBase_i::Setup - Unable to obtain DCB information\n");
    return ret;
  }
  
  // Set the new data
  dcb.BaudRate = DWORD(dwBaudrate);
  dcb.ByteSize = BYTE(eDataBits);
  dcb.Parity   = BYTE(eParity);
  dcb.StopBits = BYTE(eStopBits);
  
  // Determine if parity is used
  dcb.fParity  = (eParity != EParNone);
  
  // Set the handshaking flags
  switch (eHandshake)
  {
  case EHandshakeOff:
    dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
    dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
    dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
    dcb.fOutX = false;							// Disable XON/XOFF for transmission
    dcb.fInX = false;							// Disable XON/XOFF for receiving
    dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
    break;
    
  case EHandshakeHardware:
    dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
    dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
    dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
    dcb.fOutX = false;							// Disable XON/XOFF for transmission
    dcb.fInX = false;							// Disable XON/XOFF for receiving
    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
    break;
    
  case EHandshakeSoftware:
    dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
    dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
    dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
    dcb.fOutX = true;							// Enable XON/XOFF for transmission
    dcb.fInX = true;							// Enable XON/XOFF for receiving
    dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
    break;
    
  default:
    // This shouldn't be possible 
    ret = ERR_INVALID_PARAM_I;
    m_dwLastError = ret;
    return ret;
  }
  
  // Set the new DCB structure
  if (!::SetCommState(m_hFile,&dcb))
  {
    // Obtain the error code
    ret = ERR_WIN32_ERROR_I;
    m_dwLastError = ret;
    
    // Display a warning
    TRACE("CSerialBase_i::Setup - Unable to set DCB information\n");
    return ret;
  }
  
  // Return successful
  return ret;
}

U32 CSerialBase_i::SetMask (DWORD dwEventMask)
{
  // Reset error state
  U32 ret = ERR_OK_I;
  
  // Check if the device is open
  if (m_hFile == 0)
  {
    // Set the internal error code
    ret = ERR_INVALID_HANDLE_I;
    m_dwLastError = ret;
    
    // Issue an error and quit
    TRACE("CSerialBase_i::SetMask - Device is not opened\n");
    return ret;
  }
  
  // Set the new mask. Note that this will generate an EEventNone
  // if there is an asynchronous WaitCommEvent pending.
  if (!::SetCommMask(m_hFile,dwEventMask))
  {
    // Obtain the error code
    ret = ERR_WIN32_ERROR_I;
    m_dwLastError = ret;
    
    // Display a warning
    TRACE("CSerialBase_i::SetMask - Unable to set event mask\n");
    return ret;
  }
  
  // Save event mask and return successful
  m_dwEventMask = dwEventMask;
  
  return ret;
}

U32 CSerialBase_i::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Reset error state
	U32 ret = ERR_OK_I;
  
	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		ret = ERR_INVALID_HANDLE_I;

		// Issue an error and quit
		TRACE("CSerialBase_i::WaitEvent - Device is not opened\n");
		return ret;
	}
  
	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		ret = ERR_INVALID_FUNCTION_I;

		// Issue an error and quit
		TRACE("CSerialBase_i::WaitEvent - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return ret;
	}
  
	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (!lpOverlapped && m_hevtOverlapped)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}
  
	// Make sure the overlapped structure isn't busy
	_ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));

	// Wait for the COM event
	if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();
    
		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			ret = ERR_WIN32_ERROR_I;
      
			// Issue an error and quit
			TRACE(_T("CSerialBase_i::WaitEvent - Unable to wait for COM event\n"));
			return ret;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				// ::CancelIo(m_hFile);
				// The operation timed out. Set the internal error code and quit
				ret = ERR_TIMEOUT_I;
				return ret;

			default:
				// Set the internal error code
				ret = ERR_WIN32_ERROR_I;
        
				// Issue an error and quit
				TRACE("CSerialBase_i::WaitEvent - Unable to wait until COM event has arrived\n");
				return ret;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		if (lpOverlapped)
			::SetEvent(lpOverlapped->hEvent);
	}

	// Return successfully
	return ret;
}


U32 CSerialBase_i::SetupReadTimeouts (READTIMEOUT_E eReadTimeout)
{
	// Reset error state
	U32 ret = ERR_OK_I;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		ret = ERR_INVALID_HANDLE_I;

		// Issue an error and quit
		TRACE("CSerialBase_i::SetupReadTimeouts - Device is not opened\n");
		return ret;
	}

	// Determine the time-outs
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		ret = ERR_WIN32_ERROR_I;

		// Display a warning
		TRACE("CSerialBase_i::SetupReadTimeouts - Unable to obtain timeout information\n");
		return ret;
	}
  
	// Set the new timeouts
	switch (eReadTimeout)
	{
	case EReadTimeoutBlocking:
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	case EReadTimeoutNonblocking:
		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	default:
		// This shouldn't be possible
		_ASSERTE(false);
		ret = ERR_INVALID_PARAM_I;
		m_dwLastError = ret;
		return ret;
	}
  
	// Set the new DCB structure
	if (!::SetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		ret = ERR_WIN32_ERROR_I;
    
		// Display a warning
		TRACE("CSerialBase_i::SetupReadTimeouts - Unable to set timeout information\n");
		return ret;
	}

	// Return successful
	return ret;
}

U32 CSerialBase_i::Write (const void* pData, size_t iLen, DWORD* pdwWritten, 
                          LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
  // Overlapped operation should specify the pdwWritten variable
  _ASSERTE(!lpOverlapped || pdwWritten);
  
  // Reset error state
  U32 ret = ERR_OK_I;
  
  // Use our own variable for read count
  DWORD dwWritten;
  if (pdwWritten == 0)
  {
    pdwWritten = &dwWritten;
  }
  
  // Reset the number of bytes written
  *pdwWritten = 0;
  
  // Check if the device is open
  if (m_hFile == 0)
  {
    // Set the internal error code
    ret = ERR_INVALID_HANDLE_I;
    
    // Issue an error and quit
    TRACE("CSerialBase_i::Write - Device is not opened, err= %d\n", ret);
    return ret;
  }
  
  // Check if an overlapped structure has been specified
  if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
  {
    // Set the internal error code
    ret = ERROR_INVALID_FUNCTION;
    
    // Issue an error and quit
    TRACE("CSerialBase_i::Write - Overlapped I/O is disabled, specified parameters are illegal, err= %d\n", ret);
    return ret;
  }
  
  // Wait for the event to happen
  OVERLAPPED ovInternal;
  if (!lpOverlapped && m_hevtOverlapped)
  {
    // Setup our own overlapped structure
    memset(&ovInternal,0,sizeof(ovInternal));
    ovInternal.hEvent = m_hevtOverlapped;
    
    // Use our internal overlapped structure
    lpOverlapped = &ovInternal;
  }
  
  // Make sure the overlapped structure isn't busy
  _ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));
  
  // Write the data
  if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,lpOverlapped))
  {
    // Set the internal error code
    long lLastError = ::GetLastError();
    
    // Overlapped operation in progress is not an actual error
    if (lLastError != ERROR_IO_PENDING)
    {
      // Save the error
      ret = ERR_WIN32_ERROR_I;
      
      // Issue an error and quit
      TRACE("CSerialBase_i::Write - Unable to write the data, err= %d\n", ret);
      return ret;
    }
    
    // We need to block if the client didn't specify an overlapped structure
    if (lpOverlapped == &ovInternal)
    {
      // Wait for the overlapped operation to complete
      switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
      {
      case WAIT_OBJECT_0:
        // The overlapped operation has completed
        if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
        {
          // Set the internal error code
          
          if (GetLastError() == ERROR_IO_INCOMPLETE)
          {
            ret = ERR_IO_INCOMPLETE_I;
          }
          else
          {
            ret = ERR_WIN32_ERROR_I;
          }
          
          TRACE("CSerialBase_i::Write - Overlapped completed without result(%u)\n", ret);
          return ret;
        }
        break;
        
      case WAIT_TIMEOUT:
        // Cancel the I/O operation
        // The operation timed out. Set the internal error code and quit
        ret = ERR_TIMEOUT_I;
        TRACE("CSerialBase_i::Write - Overlapped time out\n");
        return ret;
        
      default:
        // Set the internal error code
        ret = ERR_WIN32_ERROR_I;
        
        // Issue an error and quit
        TRACE("CSerialBase_i::Write - Unable to wait until data has been sent\n");
        return ret;
      }
    }
  }
  else
  {
    // The operation completed immediatly. Just to be sure
    // we'll set the overlapped structure's event handle.
    if (lpOverlapped)
      ::SetEvent(lpOverlapped->hEvent);
  }
  
  // Return successfully
  return ret;
}

U32 CSerialBase_i::Write (LPCSTR pString, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Determine the length of the string
	return Write(pString, strlen(pString),pdwWritten,lpOverlapped,dwTimeout);
}


// Read data from the serial port. Refer to the description of
// the 'SetupReadTimeouts' for an explanation about (non) blocking reads and how to use this.
U32 CSerialBase_i::Read (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
  // Overlapped operation should specify the pdwRead variable
  _ASSERTE(!lpOverlapped || pdwRead);
  
  // Reset error state
  U32 ret = ERROR_SUCCESS;
  
  // Use our own variable for read count
  DWORD dwRead;
  if (pdwRead == 0)
  {
    pdwRead = &dwRead;
  }
  
  // Reset the number of bytes read
  *pdwRead = 0;
  
  // Check if the device is open
  if (m_hFile == 0)
  {
    // Set the internal error code
    ret = ERR_INVALID_HANDLE_I;
    
    // Issue an error and quit
    TRACE("CSerialBase_i::Read - Device is not opened\n");
    return ret;
  }
  
#ifdef _DEBUG
  // The debug version fills the entire data structure with
  // 0xDC bytes, to catch buffer errors as soon as possible.
  memset(pData,0xDC,iLen);
#endif
  
  // Check if an overlapped structure has been specified
  if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
  {
    // Set the internal error code
    ret = ERR_INVALID_FUNCTION_I;
    
    // Issue an error and quit
    TRACE("CSerialBase_i::Read - Overlapped I/O is disabled, specified parameters are illegal.\n");
    return ret;
  }
  
  // Wait for the event to happen
  OVERLAPPED ovInternal = {0, };
  if (lpOverlapped == 0)
  {
    // Setup our own overlapped structure
    memset(&ovInternal,0,sizeof(ovInternal));
    ovInternal.hEvent = m_hevtOverlapped;
    
    // Use our internal overlapped structure
    lpOverlapped = &ovInternal;
  }
  
  // Make sure the overlapped structure isn't busy
  _ASSERTE(!m_hevtOverlapped || HasOverlappedIoCompleted(lpOverlapped));
  
  // Read the data
  if (!::ReadFile(m_hFile,pData,iLen,pdwRead,lpOverlapped))
  {
    // Set the internal error code
    LONG lLastError = ::GetLastError();
    
    // Overlapped operation in progress is not an actual error
    if (lLastError != ERROR_IO_PENDING)
    {
      // Save the error
      ret = ERR_WIN32_ERROR_I;
      
      // Issue an error and quit
      TRACE("CSerialBase_i::Read - Unable to read the data\n");
      return ret;
    }
    
    // We need to block if the client didn't specify an overlapped structure
    if (lpOverlapped == &ovInternal)
    {
      // Wait for the overlapped operation to complete
      switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
      {
      case WAIT_OBJECT_0:
        // The overlapped operation has completed
        if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
        {
          // Set the internal error code
          ret = ERR_WIN32_ERROR_I;
          
          TRACE("CSerialBase_i::Read - Overlapped completed without result\n");
          return ret;
        }
        break;
        
      case WAIT_TIMEOUT: 
        // The operation timed out. Set the internal error code and quit
        ret = ERR_TIMEOUT_I;
        TRACE("CSerialBase_i::Read - Overlapped completed time out\n");
        return ret;
        
      default:
        // Set the internal error code
        ret = ERR_WIN32_ERROR_I;
        
        // Issue an error and quit
        TRACE("CSerialBase_i::Read - Unable to wait until data has been read\n");
        return ret;
      }
    }
  }
  else
  {
    // The operation completed immediatly. Just to be sure
    // we'll set the overlapped structure's event handle.
    if (lpOverlapped)
    {
      ::SetEvent(lpOverlapped->hEvent);
    }
  }
  
  // Return successfully
  return ret;
}


CSerialBase_i::ERROR_E CSerialBase_i::GetError (void)
{
  // Reset error state
  m_dwLastError = ERR_OK_I;
  
  // Check if the device is open
  if (m_hFile == 0)
  {
    // Set the internal error code
    m_dwLastError = ERR_INVALID_HANDLE_I;
    return EErrorUnknown;
  }
  
  // Obtain COM status
  DWORD dwErrors = 0;
  if (!::ClearCommError(m_hFile,&dwErrors,0))
  {
    // Set the internal error code
    m_dwLastError = ERR_WIN32_ERROR_I;
    return EErrorUnknown;
  }
  
  // Return the error
  return ERROR_E(dwErrors);
}

CSerialBase_i::EVENT_E CSerialBase_i::GetEventType (void)
{
	// Obtain the event (mask unwanted events out)
	EVENT_E eEvent = EVENT_E(m_eEvent & m_dwEventMask);

	// Reset internal event type
	m_eEvent = EEventNone;
  
	// Return the current cause
	return eEvent;
}

U32 CSerialBase_i::Purge()
{
	// Reset error state
	U32 ret = ERR_OK_I;
  
	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		ret = ERR_INVALID_HANDLE_I;

		// Issue an error and quit
		TRACE("CSerialBase_i::Purge - Device is not opened\n");
		return ret;
	}

	if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		// Set the internal error code
		ret = ERR_WIN32_ERROR_I;
		TRACE("CSerialBase_i::Purge - Overlapped completed without result\n");
	}
	
	// Return successfully
	return ret;
}


DWORD CSerialBase_i::GetModemStatus(COM_MODEMSTATUS& cfg)
{
  	// Reset error state
	U32 ret = ERR_OK_I;
  
	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		ret = ERR_WIN32_ERROR_I;
		m_dwLastError = ret;
    
		return ret;
	}

	// Determine if CTS is on
	cfg.CTS = (dwModemStat & MS_CTS_ON) != 0;

  // Determine if DSR is on
	cfg.DSR = (dwModemStat & MS_DSR_ON) != 0;
  
  	// Determine if Ring is on
	cfg.Ring = (dwModemStat & MS_RING_ON) != 0;
  
  	// Determine if RLSD is on
	cfg.RLSD  = (dwModemStat & MS_RLSD_ON) != 0;

  return ret;
}


DWORD CSerialBase_i::GetConfig(COM_CONFIG& cfg)
{
	// Reset error state
	U32 ret = ERROR_SUCCESS;

	m_dwLastError = ERROR_SUCCESS;
  
	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		ret = ERR_INVALID_HANDLE_I;
		m_dwLastError = ret;
		return ret;
	}

	// Obtain the DCB structure for the device
	DCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		ret = ERR_WIN32_ERROR_I;
		m_dwLastError = ret;
        
		return ret;
	}
   
  cfg.Baudrate    = dcb.BaudRate;
  cfg.DataBits    = DATABITS_E(dcb.ByteSize);
  
  cfg.Parity      = PARITY_E(dcb.Parity);
  cfg.StopBits    = STOPBITS_E(dcb.StopBits);
  
  // Check if hardware handshaking is being used
  if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
    cfg.Handshaking = EHandshakeHardware; 
  else if (dcb.fOutX && dcb.fInX) // Check if software handshaking is being used
    cfg.Handshaking = EHandshakeSoftware;
  else cfg.Handshaking = EHandshakeOff;
   
	return ret;
}

DWORD CSerialBase_i::GetEventMask (void)
{
  // Reset error state
  U32 ret = ERROR_SUCCESS;
  
  m_dwLastError = ERROR_SUCCESS;
  
  // Check if the device is open
  if (m_hFile == NULL)
  {
    // Set the internal error code
    ret = ERR_INVALID_HANDLE_I;
    m_dwLastError = ret;
    return ret;
  }
  
  // Return the event mask
  return m_dwEventMask;
}

//
// ERR_WIN32_ERROR_I
//
U32 CSerialBase_i::StartListener (void)
{
  U32 ret = ERR_OK_I;
  m_dwLastError = ret;
  
  if(m_hOverlappedWorkerThreadEvent)
  {
    return ret;
  }
  
  // Create an event that is used for the workerthread. The globally
  // used event is used by the main-thread and cannot be reused  for this thread.
  m_hOverlappedWorkerThreadEvent = ::CreateEvent(0,true,false,0);
  if (m_hOverlappedWorkerThreadEvent == 0)
  {
    // Obtain the error information
    ret = ERR_WIN32_ERROR_I;
    m_dwLastError = ret;
    
    // Return the error
    return ret;
  }
  
  // Check if the watcher thread was already running
  if (!m_bShutdown)
  {
    ret = Start();
    if(ret == ERR_OK_I || ret == ERR_THREAD_RESTART_I)
      ret = ERR_OK_I; 
  }

  m_dwLastError = ret;
  
  // Return the error
  return ret;
}

U32 CSerialBase_i::StopListener(DWORD dwTimeout)
{
  U32 ret = ERR_OK_I;
  
  if (!IsThreadAlive())
  {
    m_bShutdown   = TRUE;
    m_dwLastError = ret;
    if (m_hOverlappedWorkerThreadEvent)
    {
      ::CloseHandle(m_hOverlappedWorkerThreadEvent);
      m_hOverlappedWorkerThreadEvent = 0;
    }
    return ret;
  }
  
  m_bShutdown   = TRUE;
  m_dwLastError = ret;
  
  // Cancel the pending WaitEvent, but we won't do this using
  // CancelIo. This would break Win95 compatibility and some
  // USB serial dongles cannot handle CancelIo correctly. By
  // setting the event mask again, the call will also be
  // completed before the thread exits.
  SetMask(GetEventMask());
  
  if (Shutdown(dwTimeout))
  {
    if (m_hOverlappedWorkerThreadEvent)
    {
      ::CloseHandle(m_hOverlappedWorkerThreadEvent);
      m_hOverlappedWorkerThreadEvent = 0;
    }
  }
  else
  {
    ret = ERR_TIMEOUT_I;
  }
  
  m_dwLastError = ret;
  
  return ret;
}

DWORD CSerialBase_i::ThreadExit(DWORD dwExitCode)
{
  return CThread_i::ThreadExit(dwExitCode); 
}

DWORD CSerialBase_i::ThreadHandler(UINT msg, WPARAM wParam, LPARAM lParam)
{
  ULONG ret = 0;
  
	// Keep looping
	do
	{
    if(m_bShutdown)
    {
      return ERR_THREAD_SHUTDOWN_I;
    }
    
		// Reset the event handle
		::ResetEvent(m_hOverlappedWorkerThreadEvent);

		// Initialize the overlapped structure
		OVERLAPPED ovInternal = {0};
		ovInternal.hEvent = m_hOverlappedWorkerThreadEvent;
    
		// Start the WaitEvent (use our own overlapped structure)
    U32 evtRet = WaitEvent(&ovInternal);
    if(evtRet == ERR_TIMEOUT_I)
    {
      OnEvent(CSerialBase_i::EEventTimeOut, CSerialBase_i::EErrorUnknown); 
    }
    else if(evtRet == ERR_INVALID_HANDLE_I)
    {
      TRACE("CSerialBase_i::WaitEvent - Fail, exit Monitor Thread\n");
      return ERR_THREAD_SHUTDOWN_I;
    }
    else if(evtRet)
    {
      //return ERR_THREAD_SHUTDOWN_I;
    }
   
		// Wait for the overlapped operation to complete
		DWORD dwRet = ::WaitForSingleObject(m_hOverlappedWorkerThreadEvent, m_dwWaitEventTimeOut);
    if(dwRet == WAIT_ABANDONED)
    {
			// Set the internal error code
			m_dwLastError = ERR_WIN32_ERROR_I;
      ret = OnEvent(CSerialBase_i::EEventAbandoned, CSerialBase_i::EErrorUnknown); 
      
			// Issue an error and quit
			TRACE("CSerialBase_i::ThreadHandler - WAIT_ABANDONED, Unable to wait until COM event has arrived\n");
			return ERR_THREAD_SHUTDOWN_I;
    }
    else if(dwRet == WAIT_OBJECT_0)
    {
      ; // right 
    }
		else if(dwRet == WAIT_TIMEOUT) // The time-out interval elapsed, and the object's state is nonsignaled. 
    { 
      ret = OnEvent(CSerialBase_i::EEventTimeOut, CSerialBase_i::EErrorUnknown); 
      continue;
    }
    else 
    {
      TRACE("CSerialBase_i::WaitForSingleObject - Fail, exit Monitor Thread\n");
      return ERR_THREAD_SHUTDOWN_I;
    }
    
		// Wait until one of the events happens
		if (!m_bShutdown)
		{
			// Determine the event
			EVENT_E eEvent = GetEventType();
      
			// Obtain the error status during this event
			DWORD dwErrors = 0;
			if (!::ClearCommError(m_hFile,&dwErrors,0))
			{
				// Set the internal error code
				m_dwLastError = ::GetLastError();

				// Issue an error and quit
				TRACE( "CSerialBase_i::ThreadProc - Unable to obtain COM status\n");
			}
      
			// Convert the error
			ERROR_E eError = ERROR_E(dwErrors);

			// There was a COMM event, which needs handling. We'll call the
			// event handler. We can receive a "zero" event, when the
			// mask or event character has been set. We won't pass this
			// down to the window.
			if (eEvent)
      {
        ret = OnEvent(eEvent,eError);
      }
		}
    
    if(ret == ERR_THREAD_SHUTDOWN_I)
    {
      return ret;
    }
    
  } while (!m_bShutdown);
  
	return ret;
}
