#include "StdAfx.h"
#include "XBGateDevice.h"
#include "XBLinkDevice.h"
#include "utils.h"

XBGateDevice::XBGateDevice(XBLinkDevice *ptrLinkDevice)
{
	ASSERT(ptrLinkDevice);
	m_ptrLinkDevice = ptrLinkDevice;
//	ZERO_MEM(&m_DevHeader,sizeof(XBGateDevHeader));
//	m_ptrLinkDevice->AddGateDevice(this);
}
/*
XBGateDevice::~XBGateDevice()
{
//	m_ptrLinkDevice->RemoveGateDevice(this);
}
*/
HRESULT XBGateDevice::AddEndPointDevice(XBEndPointDevice *ptrDev)
{
	ASSERT(ptrDev);
	if(!ptrDev)	return E_POINTER;
	m_EndPointDevices.Add(ptrDev);
	return S_OK;
}

void XBGateDevice::RemoveEndPointDevice(XBEndPointDevice *ptrDev)
{
	ASSERT(ptrDev);
	if(!ptrDev)	return;
	for(int i=0;i<m_EndPointDevices.GetCount();i++)
		if(m_EndPointDevices.GetAt(i)==ptrDev)	m_EndPointDevices.RemoveAt(i);
}


int XBGateDevice::GetEndPointDeviceCount()
{
	return m_EndPointDevices.GetCount();
}

XBEndPointDevice *XBGateDevice::GetEndPointDevice(int index)
{
	return m_EndPointDevices.GetAt(index);
}

void XBGateDevice::RemoveAllEndPointDevices(void)
{
	int count = m_EndPointDevices.GetCount();
	for(int i=0;i<count;i++)
	{
		XBEndPointDevice* p = m_EndPointDevices[i];
		if(p)	delete p;
	}
	m_EndPointDevices.RemoveAll();
}


XBEndPointDevice *XBGateDevice::FindEndPointDevice(XBAddress &addr)
{
	for(int i=0;i<m_EndPointDevices.GetCount();i++)
	{
		XBEndPointDevice *p = m_EndPointDevices.GetAt(i);
		if(p->GetAddress()==addr)	return p;
//		if(memcmp(p->GetAddress(),addr,nSize)==0)		return p;
	}
	return NULL;
}

HRESULT XBGateDevice::Write(XBDataPacket *pPacket)
{
	ASSERT(m_ptrLinkDevice);
	ASSERT(pPacket);
	//	+ addr_size + ex_addr_size ???
//	m_DevHeader.DataSize = pPacket->m_Packet.m_EndPointHeader.Size + sizeof(m_DevHeader);
//	memcpy(&pPacket->m_GateHeader,&m_DevHeader,sizeof(m_DevHeader));

	//	replace by real values
	pPacket->m_GateHeader.Address.SetSize(0);
	pPacket->m_GateHeader.PANAddress.SetSize(0);

	return m_ptrLinkDevice->Write(pPacket);
}
HRESULT XBGateDevice::Read(XBDataPacket *pPacket)
{
	ASSERT(m_ptrLinkDevice);
	ASSERT(pPacket);

	//	replace by real values
	pPacket->m_GateHeader.Address.SetSize(0);
	pPacket->m_GateHeader.PANAddress.SetSize(0);

	//	+ addr_size + ex_addr_size ???
//	m_DevHeader.DataSize = pPacket->m_Packet.m_EndPointHeader.Size + sizeof(m_DevHeader);
//	memcpy(&pPacket->m_GateHeader,&m_DevHeader,sizeof(m_DevHeader));
	return m_ptrLinkDevice->Read(pPacket);
}

/*
HRESULT XBGateDevice::Write(void *ptrSrcData, size_t nSize)
{
	DEBUG_FUNCTION_NAME;
	ASSERT(m_ptrLinkDevice);

	HRESULT hr = S_OK;
	BYTE *ptrBuffer = new BYTE[sizeof(m_DevHeader) + nSize];
	if(!ptrBuffer)	return E_OUTOFMEMORY;

	m_DevHeader.DataSize = nSize;
	memcpy(ptrBuffer, &m_DevHeader, sizeof(m_DevHeader));
	if(nSize)
		memcpy(ptrBuffer + sizeof(m_DevHeader), ptrSrcData, nSize);

	hr = m_ptrLinkDevice->Write(ptrBuffer,sizeof(m_DevHeader) + nSize);
	delete ptrBuffer;
	return hr;
}

HRESULT XBGateDevice::Read(void *ptrDestData, size_t nSize)
{
	DEBUG_FUNCTION_NAME;
	ASSERT(ptrDestData);
	ASSERT(m_ptrLinkDevice);
	
	HRESULT hr = S_OK;
	if(!nSize || !ptrDestData)	return E_POINTER;

	BYTE *ptrBuffer = new BYTE[sizeof(m_DevHeader) + nSize];
	if(!ptrBuffer)	return E_OUTOFMEMORY;

	hr = m_ptrLinkDevice->Read(ptrBuffer,nSize+sizeof(m_DevHeader));
	if(hr==S_OK)	
	{
		XBEndPointDevHeader *p = (XBEndPointDevHeader *)ptrBuffer;
		if(p->Signature==XB_PACKET_SIGNATURE &&	p->CRC == Crc8(&p->Address[0],nSize))		
			memcpy(ptrDestData, ptrBuffer + sizeof(m_DevHeader), nSize);
	}
	delete ptrBuffer;
	return hr;
}
*/
HRESULT XBGateDevice::OnPacketRecieve(XBDataPacket *pDataPacket)
{
	XBEndPointDevice *pEndDev = FindEndPointDevice(pDataPacket->m_Address);
	if(pEndDev)	return pEndDev->OnPacketRecieve(pDataPacket);
	return E_FAIL;
}