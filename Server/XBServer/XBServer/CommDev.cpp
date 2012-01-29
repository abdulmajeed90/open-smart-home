#include "StdAfx.h"
#include "XB.h"
#include "XBLinkDevice.h"
#include "utils.h"
#include "CommDev.h"


CommDev::CommDev(XBLinkDevice *ptrLinkDev)
{
	ASSERT(ptrLinkDev);
	m_pLinkDevice = ptrLinkDev;

	m_hComm = NULL;
	hReadThread = NULL;
	dwReadThreadId = 0;
	ZERO_MEM(&m_dcb,sizeof(m_dcb));

//	m_RxBuffer.SetSize(4096,256);		//	set size to 4K, grow by 1k step
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection,0x00000400);

	timeLastRx = 0;
}

CommDev::~CommDev(void)
{
	if(hReadThread)	CloseHandle(hReadThread);
	if(!m_hComm)	CloseHandle(m_hComm);
	DeleteCriticalSection(&m_CriticalSection);
}

//	garbage cleaning from a RX buffer
HRESULT WINAPI CommDev::CleaningThread(LPVOID lpParameter)
{
	/*
	CommDev* pThis = (CommDev *)lpParameter;
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	
	liDueTime.QuadPart = -100000000LL;
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (hTimer==NULL)	return E_FAIL;
	
	while(1)
	{
	//	Set timer for 10 sec...
		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))	return E_FAIL;
	//	wait for a timer
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)	return E_FAIL;
	//	if last RX was long time ago clean RxBuffer if it's not empty
		if(abs_diff(pThis->timeLastRx,GetTickCount())>XB_RXDATA_LIFETIME && pThis->m_RxBuffer.GetCount()>0)	
		{
				EnterCriticalSection(&pThis->m_CriticalSection); 
				pThis->m_RxBuffer.RemoveAll();
				LeaveCriticalSection(&pThis->m_CriticalSection);
		}
	}
	*/
	return S_OK;
}

HRESULT WINAPI CommDev::ReadingThread(LPVOID lpParameter)
{
	DWORD dwCommEvent;
	DWORD dwRead;
	BYTE  chRead;
	OVERLAPPED osStatus  = {0};

	CommDev* pThis = (CommDev *)lpParameter;
	if(!pThis->m_pLinkDevice)	return E_POINTER;
	if (!SetCommMask(pThis->m_hComm, EV_RXCHAR))	return E_FAIL;

	osStatus.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osStatus.hEvent == NULL)	return E_FAIL;
		
	for (;;) 
	{
	   if (WaitCommEvent(pThis->m_hComm, (LPDWORD)&dwCommEvent, NULL )) 
	   {
		  do {
			  chRead = 0;
			 if (ReadFile(pThis->m_hComm, &chRead, 1, (LPDWORD)&dwRead, &osStatus ))
			 {
				 if(dwRead==1)
				 {
					EnterCriticalSection(&pThis->m_CriticalSection); 
					_tprintf(_T("%02x\t"),chRead);
	//				pThis->m_RxBuffer.Add(chRead);
					pThis->timeLastRx = GetTickCount();
					LeaveCriticalSection(&pThis->m_CriticalSection);
	//				pThis->m_pLinkDevice->OnCharRecieve(&pThis->m_RxBuffer);
					pThis->m_pLinkDevice->OnCharRecieve(chRead);
				 }
			 }
			 else
				break;
		  } while (dwRead);
	   }
	   else
	   {
		  CloseHandle(osStatus.hEvent);
		  return E_FAIL;
		  break;
	   }
	}
	CloseHandle(osStatus .hEvent);
	return S_OK;
}

HRESULT CommDev::Open(int nPortIndex, int nBaudRate)
{
	TCHAR szPort[] = _T("\\\\.\\COM16");
	_stprintf_s(szPort,_T("\\\\.\\COM%i"),nPortIndex+1);
	m_hComm = CreateFile(szPort,  GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);
	if (m_hComm == INVALID_HANDLE_VALUE)	return E_FAIL;

	if (!GetCommState(m_hComm, &m_dcb))		return E_FAIL;
	m_dcb.BaudRate = nBaudRate;
	if (!SetCommState(m_hComm, &m_dcb))		return E_FAIL;
		
	hReadThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ReadingThread,this,0,(LPDWORD)&dwReadThreadId);
	if(!hReadThread)	return E_FAIL;
	return S_OK;
}

//------------	Read data  --------------------------------
/*
HRESULT CommDev::Read(void * pDestData, size_t nDataSize)
{
	ASSERT(pDestData);
	ASSERT(nDataSize);
	if(!nDataSize || !pDestData)	return E_INVALIDARG;

	if(m_RxBuffer.GetCount()<(int)nDataSize)	return E_FAIL;

	BYTE *pData = (BYTE *)pDestData;
	EnterCriticalSection(&m_CriticalSection);

	for(size_t i=0;i<nDataSize;i++)
		pData[i] = m_RxBuffer.GetAt(i);

	m_RxBuffer.RemoveAt(0,nDataSize);
	
	LeaveCriticalSection(&m_CriticalSection); 

	return S_OK;
}
*/
HRESULT CommDev::Write(void * pSrcData, size_t nDataSize)
{
	OVERLAPPED osWrite = {0};
	DWORD dwWritten;
	DWORD dwRes;
	HRESULT hr = E_FAIL;

	ASSERT(pSrcData);
	ASSERT(nDataSize);
	if(!nDataSize || !pSrcData)	return E_INVALIDARG;

	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)		return E_FAIL;

	if (!WriteFile(m_hComm, pSrcData, nDataSize, (LPDWORD)&dwWritten, &osWrite)) 
	{
		if (GetLastError() != ERROR_IO_PENDING)		dwRes = E_FAIL;
		else
		{
			if (!GetOverlappedResult(m_hComm, &osWrite, (LPDWORD)&dwWritten, TRUE))
				hr = E_FAIL;
			else
				hr = S_OK;
		}
   }
   else
      hr = S_OK;

   CloseHandle(osWrite.hEvent);
   return hr;
}
/*
size_t	CommDev::GetBufferDataSize(void)
{
	return (size_t)m_RxBuffer.GetCount();
}
size_t	CommDev::GetBufferSize(void)
{
	return (size_t)m_RxBuffer.GetSize();
}
*/