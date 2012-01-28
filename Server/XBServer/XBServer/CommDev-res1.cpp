#include "StdAfx.h"
#include "XB.h"
#include "XBLinkDevice.h"
#include "CommDev.h"
#include "utils.h"

CommDev::CommDev(XBLinkDevice *ptrLinkDev)
{
	ASSERT(ptrLinkDev);
	m_pLinkDevice = ptrLinkDev;

	m_hComm = NULL;
//	m_hReadPacketEvent = NULL;
	hReadThread = NULL;
	dwReadThreadId = 0;
	ZERO_MEM(&m_dcb,sizeof(m_dcb));
}

CommDev::~CommDev(void)
{
	if(hReadThread)	CloseHandle(hReadThread);
	if(!m_hComm)	CloseHandle(m_hComm);
}

HRESULT WINAPI CommDev::ReadThread(LPVOID lpParameter)
{
	DWORD dwCommEvent = 0;
	DWORD dwRead = 0;
	BYTE  bRead = 0;
	HRESULT hr = E_FAIL;

	CommDev* pThis = (CommDev *)lpParameter;
	if(!pThis->m_pLinkDevice)	return E_POINTER;

	for (;;) 
	{
		if (WaitCommEvent(pThis->m_hComm, (LPDWORD)&dwCommEvent, NULL)) 
		{
		do {
			if (ReadFile(pThis->m_hComm, &bRead, 1, (LPDWORD)&dwRead, NULL))
				pThis->m_pLinkDevice->OnCharRecieve(bRead);
			else
                break;
			} while (dwRead);
		}
		else
            break;
	}
	return E_FAIL;
}

// open COM port by 0-based index (0 = COM1, 1 = COM2)
HRESULT CommDev::Open(int nPortIndex, int nBaudRate)
{
	DWORD dwMask = 0;
	TCHAR szPort[] = _T("\\\\.\\COM16");
	_stprintf_s(szPort,_T("\\\\.\\COM%i"),nPortIndex+1);
	m_hComm = CreateFile(szPort,  GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,0,0);
	if (m_hComm == INVALID_HANDLE_VALUE)	return E_FAIL;

	if (!GetCommState(m_hComm, &m_dcb))		return E_FAIL;
	m_dcb.BaudRate = nBaudRate;
	m_dcb.ByteSize = 8;
    m_dcb.Parity = NOPARITY;
    m_dcb.StopBits = ONESTOPBIT;
    m_dcb.fBinary = TRUE;
    m_dcb.fParity = TRUE;
	if (!SetCommState(m_hComm, &m_dcb))		return E_FAIL;
		
//	m_hReadPacketEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = MAXDWORD; 
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
 
	if (!SetCommTimeouts(m_hComm, &timeouts))	return E_FAIL;

//	GetCommMask(m_hComm,(LPDWORD)&dwMask);
	if(!SetCommMask(m_hComm,EV_RXCHAR))		return E_FAIL;

	hReadThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ReadThread,this,0,(LPDWORD)&dwReadThreadId);
	return S_OK;
}

//	write data to COM port
HRESULT CommDev::Write(void * pSrcData, size_t nDataSize)
{
//	DEBUG_FUNCTION_NAME;
	DWORD dwWritten = 0;
	HRESULT hr = S_OK;
	int nWriteCount = 0;

	//	try to write into COM port 3 times
	do
	{
	   if (!WriteFile(m_hComm, pSrcData, nDataSize, (LPDWORD)&dwWritten, NULL)) 
	   {
		  if (GetLastError() == ERROR_IO_PENDING) nWriteCount++;
		  hr = E_FAIL;
	   }
	   else
		  hr = S_OK;
	}
	while(nWriteCount<WRITE_MAX_TRY && hr!=S_OK);
 
   return hr;
}

HRESULT CommDev::Read(void * pDestData, size_t nDataSize)
{
//	DEBUG_FUNCTION_NAME;
	DWORD nReadCount = 0;
	if(ReadFile(m_hComm,pDestData,nDataSize,(LPDWORD)&nReadCount,NULL)!=0)	
		if(nReadCount==nDataSize)	return S_OK;

	return E_FAIL;
	/*
	HRESULT hr = S_OK;
	size_t nReadCount = 0;
	size_t i = 0;
	BYTE *p = (BYTE *)pDestData;
	BYTE data;
	do
	{
		if(ReadFile(m_hComm,&data,1,(LPDWORD)&nReadCount,NULL))
		{
			p[i++]=data;
		}
		else hr = E_FAIL;
	}
	while(i<nDataSize || hr!=S_OK);
	return hr;
	*/
}
/*
//	read data from COM port
HRESULT CommDev::Read(void * pDestData, size_t nDataSize)
{
	if(!m_hReadPacketEvent)	return E_HANDLE;
	switch(WaitForSingleObject(m_hReadPacketEvent,READ_FUNCTION_TIMEOUT))
	{
		case WAIT_OBJECT_0:		
			{
				memcpy(pDestData,m_ReadBuffer,m_nReadIndex);
				m_nReadIndex = 0;	// ???
				return S_OK;	break;
			}
		case WAIT_TIMEOUT:		return WAIT_TIMEOUT;	break;
		case WAIT_FAILED:		return E_FAIL;	break;
		case WAIT_ABANDONED:	return E_FAIL;	break;
		default:	break;
	}
	return E_FAIL;
}
*/

/*
HRESULT WINAPI CommDev::ReadThread(LPVOID lpParameter)
{
	DWORD dwCommEvent = 0;
	DWORD dwRead = 0;
	BYTE  bRead = 0;
	HRESULT hr = S_OK;

	CommDev* pThis = (CommDev *)lpParameter;
	pThis->m_nReadIndex = 0;

	ResetEvent(pThis->m_hReadPacketEvent);

	for (;;) 
	{
		if (WaitCommEvent(pThis->m_hComm, (LPDWORD)&dwCommEvent, NULL)) 
		{
		do {
			if (ReadFile(pThis->m_hComm, &bRead, 1, (LPDWORD)&dwRead, NULL))
			{
				pThis->m_ReadBuffer[pThis->m_nReadIndex++] = bRead;
				if(pThis->m_pLinkDevice)
				{
					if(pThis->m_pLinkDevice->IsPacketInBuffer(pThis->m_ReadBuffer,pThis->m_nReadIndex))
					{
						hr = pThis->m_pLinkDevice->OnRead(pThis->m_ReadBuffer,pThis->m_nReadIndex);
						if(hr == S_OK)	
						{
							SetEvent(pThis->m_hReadPacketEvent);
							pThis->m_nReadIndex = 0;
						}
					}
				}
				//	check for read buffer size and start from the begining if full
				if(pThis->m_nReadIndex>=READ_BUFFER_SIZE)	pThis->m_nReadIndex = 0;	
			}
			else
			{
				DWORD err = GetLastError();
//				if(err==ERROR_IO_PENDING)	
				pThis->m_nReadIndex = 0;
                break;
			}
			} while (dwRead);
		}
		else
            break;
	}
	return S_OK;
}
*/
