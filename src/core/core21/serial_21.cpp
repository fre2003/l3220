#include "stdafx.h"
#include "serial_21.h"

CSerial_i::CSerial_i ()
	: m_lLastError(ERROR_SUCCESS)
	, m_hFile(0)
	, m_eEvent(EEventNone)
	, m_dwEventMask(0)
	, m_hevtOverlapped(0)
{
	m_lLastError = ERR_OK_I;
	m_hFile = NULL;
	m_eEvent = EEventNone;
	m_dwEventMask = 0;
	m_hevtOverlapped = 0;
	m_fStopping = FALSE;
	m_hThread = NULL;
	m_hOverlappedWorkerThreadEvent = NULL;
}

CSerial_i::~CSerial_i ()
{
	// If the device is already closed, then we don't need to do anything.
	if (m_hFile)
	{
		// Close implicitly
		Close();
	}
}

// dwInQueue dwOutQueue Both queues need to be at least 16 bytes large. 
LONG CSerial_i::Open (LPCTSTR lpszDevice,
                      DWORD dwInQueue, // Specifies the recommended size, in bytes, of the device's internal input buffer. 
                      DWORD dwOutQueue // Specifies the recommended size, in bytes, of the device's internal output buffer. 
                      ) 
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the port isn't already opened
	if (m_hFile)
	{
		m_lLastError = ERROR_ALREADY_INITIALIZED;
		return m_lLastError;
	}

	// Open the device
	m_hFile = ::CreateFile(lpszDevice,
						   GENERIC_READ|GENERIC_WRITE,
						   0,
						   0,
						   OPEN_EXISTING,
						   FILE_FLAG_OVERLAPPED,
						   0);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		// Reset file handle
		m_hFile = 0;

		// Display error
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}

	// We cannot have an event handle yet
	_ASSERTE(m_hevtOverlapped == 0);
  
  // Create the event handle for internal overlapped operations (manual reset)
  m_hevtOverlapped = ::CreateEvent(0,true,false,0);
  if (m_hevtOverlapped == 0)
  {
    // Obtain the error information
    m_lLastError = ::GetLastError();
    
    // Close the port
    ::CloseHandle(m_hFile);
    m_hFile = 0;
    
    // Return the error
    return m_lLastError;
  }

	// Setup the COM-port
	if (dwInQueue || dwOutQueue)
	{
		// Make sure the queue-sizes are reasonable sized. Win9X systems crash
		// if the input queue-size is zero. Both queues need to be at least 16 bytes large.
		_ASSERTE(dwInQueue >= 16);
		_ASSERTE(dwOutQueue >= 16);
    
		if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
		{
			// Display a warning
			long lLastError = ::GetLastError();

			// Close the port
			Close();

			// Save last error from SetupComm
			m_lLastError = lLastError;
			return m_lLastError;	
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
			TRACE("CSerial_i::Open - Unable to set default communication configuration.\n");
		}
	}
	else
	{
		// Display a warning
		TRACE("CSerial_i::Open - Unable to obtain default communication configuration.\n");
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial_i::Close (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile == 0)
	{
		// Display a warning
		TRACE("CSerial_i::Close - Method called when device is not open\n");
		return m_lLastError;
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

	// Return successful
	return m_lLastError;
}

LONG CSerial_i::Setup (DWORD dwBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::Setup - Device is not opened\n");
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	DCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::	GetLastError();

		// Display a warning
		TRACE("CSerial_i::Setup - Unable to obtain DCB information\n");
		return m_lLastError;
	}

	// Set the new data
	dcb.BaudRate = DWORD(dwBaudrate);
	dcb.ByteSize = BYTE(eDataBits);
	dcb.Parity   = BYTE(eParity);
	dcb.StopBits = BYTE(eStopBits);

	// Determine if parity is used
	dcb.fParity  = (eParity != EParNone);

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		TRACE("CSerial_i::Setup - Unable to set DCB information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial_i::SetMask (DWORD dwEventMask)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::SetMask - Device is not opened\n");
		return m_lLastError;
	}

	// Set the new mask. Note that this will generate an EEventNone
	// if there is an asynchronous WaitCommEvent pending.
	if (!::SetCommMask(m_hFile,dwEventMask))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		TRACE("CSerial_i::SetMask - Unable to set event mask\n");
		return m_lLastError;
	}

	// Save event mask and return successful
	m_dwEventMask = dwEventMask;
	return m_lLastError;
}

LONG CSerial_i::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::WaitEvent - Device is not opened\n");
		return m_lLastError;
	}

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		TRACE("CSerial_i::WaitEvent - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
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
			m_lLastError = lLastError;

			// Issue an error and quit
			TRACE("CSerial_i::WaitEvent - Unable to wait for COM event\n");
			return m_lLastError;
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
				::CancelIo(m_hFile);

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				TRACE("CSerial_i::WaitEvent - Unable to wait until COM event has arrived\n");
				return m_lLastError;
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
	return m_lLastError;
}


LONG CSerial_i::SetupHandshaking (EHandshake eHandshake)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::SetupHandshaking - Device is not opened\n");
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	DCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		TRACE("CSerial_i::SetupHandshaking - Unable to obtain DCB information\n");
		return m_lLastError;
	}

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
		_ASSERTE(false);
		m_lLastError = E_INVALIDARG;
		return m_lLastError;
	}

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		TRACE("CSerial_i::SetupHandshaking - Unable to set DCB information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial_i::SetupReadTimeouts (EReadTimeout eReadTimeout)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::SetupReadTimeouts - Device is not opened\n");
		return m_lLastError;
	}

	// Determine the time-outs
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		TRACE("CSerial_i::SetupReadTimeouts - Unable to obtain timeout information\n");
		return m_lLastError;
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
		m_lLastError = E_INVALIDARG;
		return m_lLastError;
	}

	// Set the new DCB structure
	if (!::SetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		TRACE("CSerial_i::SetupReadTimeouts - Unable to set timeout information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial_i::Write (const void* pData, size_t iLen, DWORD* pdwWritten, 
                       LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Overlapped operation should specify the pdwWritten variable
	_ASSERTE(!lpOverlapped || pdwWritten);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

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
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::Write - Device is not opened, err= %d\n", m_lLastError);
		return m_lLastError;
	}

	// Check if an overlapped structure has been specified
	if (!m_hevtOverlapped && (lpOverlapped || (dwTimeout != INFINITE)))
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		TRACE("CSerial_i::Write - Overlapped I/O is disabled, specified parameters are illegal, err= %d\n", m_lLastError);
		return m_lLastError;
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
			m_lLastError = lLastError;

			// Issue an error and quit
			TRACE("CSerial_i::Write - Unable to write the data, err= %d\n", m_lLastError);
			return m_lLastError;
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
					m_lLastError = ::GetLastError();

					TRACE("CSerial_i::Write - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				::CancelIo(m_hFile);

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				TRACE("CSerial_i::Write - Unable to wait until data has been sent\n");
				return m_lLastError;
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
	return m_lLastError;
}

LONG CSerial_i::Write (LPCSTR pString, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Determine the length of the string
	return Write(pString,strlen(pString),pdwWritten,lpOverlapped,dwTimeout);
}

LONG CSerial_i::Read (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Overlapped operation should specify the pdwRead variable
	_ASSERTE(!lpOverlapped || pdwRead);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

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
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::Read - Device is not opened\n");
		return m_lLastError;
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
		m_lLastError = ERROR_INVALID_FUNCTION;

		// Issue an error and quit
		TRACE("CSerial_i::Read - Overlapped I/O is disabled, specified parameters are illegal.\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
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
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			TRACE("CSerial_i::Read - Unable to read the data\n");
			return m_lLastError;
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
					m_lLastError = ::GetLastError();

					TRACE("CSerial_i::Read - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				::CancelIo(m_hFile);

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				TRACE("CSerial_i::Read - Unable to wait until data has been read\n");
				return m_lLastError;
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
	return m_lLastError;
}

CSerial_i::EError CSerial_i::GetError (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;
		return EErrorUnknown;
	}

	// Obtain COM status
	DWORD dwErrors = 0;
	if (!::ClearCommError(m_hFile,&dwErrors,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();
		return EErrorUnknown;
	}

	// Return the error
	return EError(dwErrors);
}

CSerial_i::EEvent CSerial_i::GetEventType (void)
{
	// Obtain the event (mask unwanted events out)
	EEvent eEvent = EEvent(m_eEvent & m_dwEventMask);

	// Reset internal event type
	m_eEvent = EEventNone;

	// Return the current cause
	return eEvent;
}

LONG CSerial_i::Purge()
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		TRACE("CSerial_i::Purge - Device is not opened\n");
		return m_lLastError;
	}

	if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();
		TRACE("CSerial_i::Purge - Overlapped completed without result\n");
	}
	
	// Return successfully
	return m_lLastError;
}


DWORD CSerial_i::GetModemStatus(COM_MODEMSTATUS& cfg)
{
  	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();
    
		return m_lLastError;
	}

	// Determine if CTS is on
	cfg.CTS = (dwModemStat & MS_CTS_ON) != 0;

  // Determine if DSR is on
	cfg.DSR = (dwModemStat & MS_DSR_ON) != 0;
  
  	// Determine if Ring is on
	cfg.Ring = (dwModemStat & MS_RING_ON) != 0;
  
  	// Determine if RLSD is on
	cfg.RLSD  = (dwModemStat & MS_RLSD_ON) != 0;

  return ERROR_SUCCESS;
}


DWORD CSerial_i::GetConfig(COM_CONFIG& cfg)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	DCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		return m_lLastError;
	}
   
  cfg.Baudrate    = dcb.BaudRate;
  cfg.DataBits    = EDataBits(dcb.ByteSize);
  
  cfg.Parity      = EParity(dcb.Parity);
  cfg.StopBits    = EStopBits(dcb.StopBits);
  
  // Check if hardware handshaking is being used
  if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
    cfg.Handshaking = EHandshakeHardware; 
  else if (dcb.fOutX && dcb.fInX) // Check if software handshaking is being used
    cfg.Handshaking = EHandshakeSoftware;
  else cfg.Handshaking = EHandshakeOff;
   
	return m_lLastError;
}

DWORD CSerial_i::GetEventMask (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;
		return 0;
	}

	// Return the event mask
	return m_dwEventMask;
}

LONG CSerial_i::StartListener (void)
{
  // Create an event that is used for the workerthread. The globally
  // used event is used by the main-thread and cannot be reused  for this thread.
  m_hOverlappedWorkerThreadEvent = ::CreateEvent(0,true,false,0);
  if (m_hOverlappedWorkerThreadEvent == 0)
  {
    // Obtain the error information
    m_lLastError = ::GetLastError();
    
    // Return the error
    return m_lLastError;
  }
  
	// Check if the watcher thread was already running
	if (m_hThread == 0)
	{
		// Make sure the thread has stopped
		_ASSERTE(!m_fStopping);

		// Start the watcher thread
		DWORD dwThreadId = 0;
		m_hThread = ::CreateThread(0,0,ThreadProc,LPVOID(this),0,&dwThreadId);
		if (m_hThread == 0)
		{
			// Display a warning
			TRACE( "CSerial_i::StartListener - Unable to start COMM watcher thread\n");

			// Set the error code and exit
			m_lLastError = ::GetLastError();
			return m_lLastError;
		}
	}

	// Return the error
	m_lLastError = ERROR_SUCCESS;
	return m_lLastError;
}

LONG CSerial_i::StopListener (DWORD dwTimeout)
{
	// Check if the thread is running
	if (m_hThread)
	{
		// Set the flag that the thread must be stopped
		m_fStopping = true;

		// Cancel the pending WaitEvent, but we won't do this using
		// CancelIo. This would break Win95 compatibility and some
		// USB serial dongles cannot handle CancelIo correctly. By
		// setting the event mask again, the call will also be
		// completed before the thread exits.
		SetMask(GetEventMask());
		
		// Wait until the watcher thread has stopped
		::WaitForSingleObject(m_hThread,dwTimeout);

		// The thread has stopped
		m_fStopping = false;

		// Close handle to the thread
		::CloseHandle(m_hThread);
		m_hThread = 0;
	}

  if (m_hOverlappedWorkerThreadEvent)
	{
		::CloseHandle(m_hOverlappedWorkerThreadEvent);
		m_hOverlappedWorkerThreadEvent = 0;
	}

	// Return the error
	m_lLastError = ERROR_SUCCESS;
	return m_lLastError;
}

DWORD WINAPI CSerial_i::ThreadProc (LPVOID lpArg)
{
	// Route the method to the actual object
	CSerial_i* pThis = reinterpret_cast<CSerial_i*>(lpArg);

  if(pThis)
  {
    return pThis->ThreadProc();
  }
  else 
  {
    return ERROR_INVALID_HANDLE;
  }
}

DWORD CSerial_i::ThreadProc (void)
{
	// Keep looping
	do
	{
		// Reset the event handle
		::ResetEvent(m_hOverlappedWorkerThreadEvent);

		// Initialize the overlapped structure
		OVERLAPPED ovInternal = {0};
		ovInternal.hEvent = m_hOverlappedWorkerThreadEvent;

		// Start the WaitEvent (use our own overlapped structure)
		if (WaitEvent(&ovInternal) != ERROR_SUCCESS)
			return m_lLastError;

		// Wait for the overlapped operation to complete
		if (::WaitForSingleObject(m_hOverlappedWorkerThreadEvent,INFINITE) != WAIT_OBJECT_0)
		{
			// Set the internal error code
			m_lLastError = ::GetLastError();

			// Issue an error and quit
			TRACE("CSerial_i::ThreadProc - Unable to wait until COM event has arrived\n");
			return m_lLastError;
		}

		// Wait until one of the events happens
		if (!m_fStopping)
		{
			// Determine the event
			EEvent eEvent = GetEventType();

			// Obtain the error status during this event
			DWORD dwErrors = 0;
			if (!::ClearCommError(m_hFile,&dwErrors,0))
			{
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				TRACE( "CSerial_i::ThreadProc - Unable to obtain COM status\n");
			}

			// Convert the error
			EError eError = EError(dwErrors);

			// There was a COMM event, which needs handling. We'll call the
			// event handler. We can receive a "zero" event, when the
			// mask or event character has been set. We won't pass this
			// down to the window.
			if (eEvent)
				OnEvent(eEvent,eError);
		}
	}
	while (!m_fStopping);

	// Bye bye
	return 0;
}
