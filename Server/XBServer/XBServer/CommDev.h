#pragma once

#define RX_BUFFER_SIZE 32768
#define READ_FUNCTION_TIMEOUT 1000
#define READ_TIMEOUT 1000      // milliseconds
#define WRITE_MAX_TRY 3

class CommDev
{
public:
	CommDev(XBLinkDevice *ptrLinkDev);
	~CommDev(void);

	// open COM port by 0-based index (0 = COM1, 1 = COM2)
	HRESULT Open(int nPortIndex, int nBaudRate = CBR_57600);
	HRESULT Write(void * pSrcData, size_t nDataSize);
	
	//	read all inside ReadThread
	//	HRESULT Read(void * pDestData, size_t nDataSize);
	
//	size_t	GetBufferDataSize(void);
//	size_t	GetBufferSize(void);

	static HRESULT WINAPI ReadingThread(LPVOID lpParameter);
	static HRESULT WINAPI CleaningThread(LPVOID lpParameter);

private:
	XBLinkDevice *m_pLinkDevice;
//	size_t m_iRdIndex, m_iRwIndex, m_iRxCounter;

//	CByteArray m_RxBuffer;
	DWORD timeLastRx;

	CRITICAL_SECTION m_CriticalSection; 
//	BYTE *m_pRxBuffer;
	
	HANDLE m_hComm;
	DCB m_dcb;
	DWORD dwReadThreadId;
	HANDLE hReadThread;
};

