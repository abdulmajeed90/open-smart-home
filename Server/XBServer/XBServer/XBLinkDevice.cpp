#include "StdAfx.h"
#include "XBLinkDevice.h"

void DumpPacket(void *ptrData);

XBLinkDevice::XBLinkDevice(void)
{
	hProcessingThread = NULL;
	hPacketEvent = NULL;
	dwThreadId = 0;
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection,0x00000400);
}

XBLinkDevice::~XBLinkDevice(void)
{
	RemoveAllGateDevices();
	for(int i=0;i<m_DataPackets.GetCount();i++)
		delete &m_DataPackets.GetAt(i);
	
	m_DataPackets.RemoveAll();
	DeleteCriticalSection(&m_CriticalSection);
	if(hProcessingThread)	CloseHandle(hProcessingThread);
	if(hPacketEvent)	CloseHandle(hPacketEvent);
}


//-------------  Child Device routine  ------------------------------------------

HRESULT XBLinkDevice::AddGateDevice(XBGateDevice *ptrDev)
{
	if(!ptrDev)	return E_FAIL;
	if(!m_GateDevices.Add(ptrDev))	return E_FAIL;
	return S_OK;
}

void XBLinkDevice::RemoveGateDevice(XBGateDevice *ptrDev)
{
	ASSERT(ptrDev);
	if(!ptrDev)	return;
	for(int i=0;i<m_GateDevices.GetCount();i++)
		if(m_GateDevices.GetAt(i)==ptrDev)	m_GateDevices.RemoveAt(i);
}

int XBLinkDevice::GetGateDeviceCount()
{
	return m_GateDevices.GetCount();
}

XBGateDevice* XBLinkDevice::GetGateDevice(int index)
{
	return m_GateDevices.GetAt(index);
}

void XBLinkDevice::RemoveAllGateDevices(void)
{
	int count = m_GateDevices.GetCount();
	for(int i=0;i<count;i++)
	{
		XBGateDevice* p = m_GateDevices[i];
		if(p)	delete p;
	}
	m_GateDevices.RemoveAll();
}

XBGateDevice *XBLinkDevice::FindGateDevice(XBAddress &Addr)
{
	for(int i=0;i<GetGateDeviceCount();i++)
	{
		XBGateDevice *p = GetGateDevice(i);
		if(p->GetAddress()==Addr)	return p;
//		if(memcmp(p->GetAddress(),addr,DEV_ADDRESS_SIZE_MAX)==0)		return p;
	}
	return NULL;
}


//-------------------------------------------------------------

HRESULT WINAPI XBLinkDevice::PacketProcessingThread(LPVOID lpParameter)
{
	ASSERT(lpParameter);
	XBLinkDevice *pThis = (XBLinkDevice*)lpParameter;
	
	while(1)
	{
		switch(WaitForSingleObject(pThis->hPacketEvent,INFINITE))		//	wait for new packet coming
		{
			case WAIT_TIMEOUT:	break;
			case WAIT_OBJECT_0:
				{
					EnterCriticalSection(&pThis->m_CriticalSection);
					for(int i=0;i<pThis->m_DataPackets.GetSize();i++)
					{
						XBDataPacket &pPacket = pThis->m_DataPackets.GetAt(i);

						//	check if there is some event handler for this packet 
						if(pPacket.IsEventPacket() && pThis->OnPacketRecieve(&pPacket)==S_OK)
							pThis->m_DataPackets.RemoveAt(i);

						//	check for old packets to remove from a gueue
						if(pPacket.IsTimeOut())	
							pThis->m_DataPackets.RemoveAt(i);
					}
					LeaveCriticalSection(&pThis->m_CriticalSection); 
					break;
				}
			default:	return E_FAIL;
		}
	}
	return S_OK;
}

void XBLinkDevice::OnCharRecieve(CByteArray *pArray)
{
	if(XBProtocol::IsPacket(pArray)==XBPacketUnknown)	return;
	if(m_XbProtocol.PutData(pArray))
	{
		XBDataPacket packet;
		if(m_XbProtocol.GetXBPacket(&packet))
		{
			//	synchronize with processing thread
			EnterCriticalSection(&m_CriticalSection);
			//	add packet into the queue
			m_DataPackets.Add(packet);
			LeaveCriticalSection(&m_CriticalSection); 
			SetEvent(hPacketEvent);
		}
	}

	/*
	BYTE *pPacket = pArray->GetData();
	size_t sz = pArray->GetCount();
	if(XBDataPacket::GetMinSize()>sz)	return;		

	if(IsPacketInBuffer(pPacket,sz))
	{
		XBDataPacket packet;
		XBEndPointDevHeader *pDevHeader = (XBEndPointDevHeader *)pPacket;
//		packet.m_GateHeader.DataSize = pDevHeader->Size + pDevHeader->AddrSize + pDevHeader->ExAddrSize;

	//	packet.SetGateAddress((BYTE *)"GateDev");

		if(pDevHeader->AddrSize)
		{
			packet.m_Address.SetSize(pDevHeader->AddrSize);
			memcpy(packet.m_Address.GetData(),pArray->GetData() + sizeof(XBEndPointDevHeader),pDevHeader->AddrSize);
		}
		if(pDevHeader->ExAddrSize)
		{
			packet.m_ExAddress.SetSize(pDevHeader->ExAddrSize);
			memcpy(packet.m_ExAddress.GetData(),pArray->GetData() + sizeof(XBEndPointDevHeader) + pDevHeader->AddrSize,pDevHeader->ExAddrSize);
		}

		if(pDevHeader->Size)
		{
			packet.m_Data.SetSize(pDevHeader->Size);
			memcpy(packet.m_Data.GetData(),pArray->GetData() + sizeof(XBEndPointDevHeader) + pDevHeader->AddrSize + pDevHeader->ExAddrSize,pDevHeader->Size);
		}
			
	//	synchronize with processing thread
		EnterCriticalSection(&m_CriticalSection);
	//	add packet into the queue
		m_DataPackets.Add(packet);
		LeaveCriticalSection(&m_CriticalSection); 
		SetEvent(hPacketEvent);
	}
	*/
}
/*
bool XBLinkDevice::IsPacketInBuffer(void *pBuffer, size_t nSize)
{
	ASSERT(pBuffer);
	if(!pBuffer)	return false;

	if(((BYTE *)pBuffer)[0]!=XBPacketAPI)	return false;				//	check for signature
	if(nSize<sizeof(XBEndPointDevHeader))	return false;				//	check for header size
	XBEndPointDevHeader *pHeader = (XBEndPointDevHeader *)pBuffer;
	if(nSize<sizeof(XBEndPointDevHeader) + pHeader->Size + pHeader->AddrSize + pHeader->ExAddrSize)	return false;	//	make sure size is correct

	return true;
}
*/
HRESULT XBLinkDevice::OnPacketRecieve(XBDataPacket *pDataPacket)
{
	ASSERT(pDataPacket);
	XBGateDevice * pGateDev = FindGateDevice(pDataPacket->m_GateHeader.Address);
	if(pGateDev)	return pGateDev->OnPacketRecieve(pDataPacket);
	return E_FAIL;
}

HRESULT XBLinkDevice::Open(void)
{
	//	create auto reset event object for packets ProcessingThread
	hPacketEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(!hPacketEvent)	return E_FAIL;

	hProcessingThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)PacketProcessingThread,this,0,(LPDWORD)&dwThreadId);
	if(!hProcessingThread)	return E_FAIL;

	return S_OK;	
}
















	//	_tprintf(_T("%c"),data);
	/*
	static int nDataCount = 0;
	BYTE *ptr = (BYTE *)&m_tempPacket.m_Packet;
	ptr[nDataCount++] = data;
	if(nDataCount>=XB_PACKET_MAX_DATA_SIZE)	nDataCount = 0;
	if(nDataCount>=sizeof(m_tempPacket.m_Packet))
	{
		int iSizeFullPacket = sizeof(m_tempPacket.m_Packet) + m_tempPacket.m_Packet.m_EndPointHeader.DataSize;
		if(nDataCount==iSizeFullPacket)
		{
			//	retrive address and attributes readed from Gate Device
			//	
			//	memcpy(m_tempPacket.m_GateHeader.Address,GetLastPacketGateDevAddress(),DEV_ADDRESS_SIZE_MAX);
			//	m_tempPacket.m_GateHeader.PanAddr = NULL;

			memcpy(m_tempPacket.m_GateHeader.Address,"GateDev1",DEV_ADDRESS_SIZE_MAX);

			m_DataPackets.Add(m_tempPacket);
			OnPacketRecieve(&m_DataPackets.GetAt(m_DataPackets.GetCount()-1));
			nDataCount = 0;
		}
	}
	*/

/*

HRESULT XBLinkDevice::Write(void *pSrcData, size_t nSize)
{
	DumpPacket(pSrcData);

	return S_OK;
}

HRESULT XBLinkDevice::Read(void *pDestData, size_t nSize)
{
	DEBUG_FUNCTION_NAME;
	ASSERT(pDestData);
	ASSERT(nSize);
	
	_tprintf(_T("Simulating read device data, %i bytes\n"),nSize);

	//	simulate reading of header and data
	XBGateDevHeader gdh = {"GateDev",0x0047,0};
	gdh.DataSize = nSize - sizeof(gdh);
	
	XBEndPointDevHeader epdh = {"EPDev1",0x80,0x00,0x01,0x02,0};
	epdh.DataSize = nSize - sizeof(epdh) - sizeof(gdh);


	//	actual READ function should wait for data from PORT/BUFFER and copy to ptrData size of nSize
	//	check for size mismatch
	
	memcpy(pDestData,&gdh,sizeof(gdh));
	memcpy((BYTE*)pDestData + sizeof(gdh),&epdh,sizeof(epdh));

	BYTE *ptr = (BYTE*)pDestData + sizeof(gdh) + sizeof(epdh);
	int s = nSize - sizeof(gdh) - sizeof(epdh);
	if(s>0)
		memset(ptr,0xA0,s);
	DumpPacket(pDestData);

	return S_OK;
}

void DumpPacket(void *ptrData)
{
	ASSERT(ptrData);
	XBGateDevHeader *pHeader = (XBGateDevHeader *)ptrData;
	_tprintf(_T("Packet dump:\n"));
	if(pHeader->DataSize<sizeof(XBEndPointDevHeader))
	{
		_tprintf(_T("Wrong packet format.\n"));
		return;
	}
	
	_tprintf(_T("Gate Device header:\n"));
	_tprintf(_T("\t- address: "));
	for(unsigned int i=0;i<DEV_ADDRESS_SIZE_MAX;i++)
		_tprintf(_T("%02X "),pHeader->Address[i]);
	printf(" : %s\n",pHeader->Address);
	_tprintf(_T("\t- PAN: 0x%04X\n"),pHeader->PanAddr);
	_tprintf(_T("\t- DataSize: %i \n"),pHeader->DataSize);

	XBEndPointDevHeader *pEpHeader = (XBEndPointDevHeader *)((BYTE*)ptrData + sizeof(XBGateDevHeader));
	if(pEpHeader->DataSize>0x10000)
	{
		_tprintf(_T("Probably wrong data packet format.\n"));
		return;
	}

	_tprintf(_T("EndPoint Device header:\n"));
	_tprintf(_T("\t- address: "));
	for(unsigned int i=0;i<DEV_ADDRESS_SIZE_MAX;i++)
		_tprintf(_T("%02X "),pEpHeader->Address[i]);
	printf(" : %s\n",pEpHeader->Address);
	switch(pEpHeader->CmdType)
	{
		case XBCmdUnknown:	_tprintf(_T("\t- CmdType: Unknown\n"));	break;
		case XBCmdWrite:	_tprintf(_T("\t- CmdType: Write\n"));	break;
		case XBCmdRead:		_tprintf(_T("\t- CmdType: Read\n"));	break;
		case XBCmdReset:	_tprintf(_T("\t- CmdType: Reset\n"));	break;
		default:	_tprintf(_T("\t- CmdType: UNKNOWN\n"));	break;
	}
	_tprintf(_T("\t- RegAddress: 0x%02X \n"),pEpHeader->RegAddr);
	switch(pEpHeader->DevType)
	{
		case XBDeviceTypeUnknown :  _tprintf(_T("\t- DevType: Unknown\n"));	break;
		case XBDeviceTypeLedDriver :  _tprintf(_T("\t- DevType: LED driver\n"));	break;
		case XBDeviceTypeGPIO :  _tprintf(_T("\t- DevType: GPIO\n"));	break;
		case XBDeviceTypeRelay : _tprintf(_T("\t- DevType: Relay\n"));	break; 
		case XBDeviceTypeRTC : _tprintf(_T("\t- DevType: RTC\n"));	break;
		case XBDeviceTypePWM : _tprintf(_T("\t- DevType: PWM\n"));	break;
		default:	_tprintf(_T("\t- DevType: UNKNOWN\n"));	break;
	}
	switch(pEpHeader->BusType)
	{
		case XBBusTypeUnknown :  _tprintf(_T("\t- BusType: Unknown\n"));	break;
		case XBBusTypeLocalDev :  _tprintf(_T("\t- BusType: Local\n"));	break;
		case XBBusTypeI2C :  _tprintf(_T("\t- BusType: I2C\n"));	break;
		case XBBusType1Wire : _tprintf(_T("\t- BusType: 1-Wire\n"));	break; 
		case XBBusType3Wire : _tprintf(_T("\t- BusType: 3-Wire\n"));	break;
		default:	_tprintf(_T("\t- BusType: UNKNOWN\n"));	break;
	}

	_tprintf(_T("\t- DataSize: %i \n"),pEpHeader->DataSize);
	
	BYTE *ptr = (BYTE *)ptrData + sizeof(XBGateDevHeader) + sizeof(XBEndPointDevHeader);
	if(!pEpHeader->DataSize)	return;
	_tprintf(_T("Packet Data: (%i bytes)\n\t"),pEpHeader->DataSize);
	for(unsigned int i=0;i<pEpHeader->DataSize;i++)
	{
		_tprintf(_T("0x%02X "),*ptr++);
		if((i+1)%8==0)	_tprintf(_T("\n\t"));
	}
	_tprintf(_T("\n"));
}
*/