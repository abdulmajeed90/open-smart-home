#include "StdAfx.h"
#include "XBEndPointDevice.h"
#include "XBGateDevice.h"
#include "utils.h"

XBEndPointDevice::XBEndPointDevice(XBGateDevice *pGateDevice)
{
	m_ptrGateDevice = pGateDevice;
	m_DevHeader.DevType = XBDevUnknown;
	m_DevHeader.CmdType = XBCmdUnknown;
	m_DevHeader.Size = 0;
	m_DevHeader.Signature = XBPacketAPI;
	m_DevHeader.CRC8 = 0xFF;
	m_DevHeader.Error = 0;
	m_ptrDeviceData = NULL;
	m_nDeviceDataSize = 0;
//	m_ptrGateDevice->AddEndPointDevice(this);
}

XBEndPointDevice::~XBEndPointDevice()
{
//	m_ptrGateDevice->RemoveEndPointDevice(this);
}

HRESULT XBEndPointDevice::Write(void *ptrSrcData, size_t nSize)
{
	XBDataPacket packet;
	ASSERT(ptrSrcData);
		
	packet.m_Address = m_Address;
	packet.m_ExAddress = m_ExAddress;
	if(m_ExAddress.GetCount())
		m_DevHeader.CmdType = XBCmdWriteAddr;
	else
		m_DevHeader.CmdType = XBCmdWrite;

	m_DevHeader.Size = nSize;
	memcpy(&packet.m_EndPointHeader, &m_DevHeader, sizeof(m_DevHeader));

	packet.m_Data.SetSize(nSize);
	memcpy(packet.m_Data.GetData(),ptrSrcData,nSize);
	return m_ptrGateDevice->Write(&packet);
}
HRESULT XBEndPointDevice::Read(void *ptrSrcData, size_t nSize)
{
	ASSERT(ptrSrcData);

	HRESULT hr = S_OK;
	XBDataPacket packet;
	
	packet.m_Address = m_Address;
	packet.m_ExAddress = m_ExAddress;

	if(m_ExAddress.GetCount())
		m_DevHeader.CmdType = XBCmdReadAddr;
	else
		m_DevHeader.CmdType = XBCmdRead;
	m_DevHeader.Size = nSize;
		
	memcpy(&packet.m_EndPointHeader, &m_DevHeader, sizeof(m_DevHeader));
	
	hr = m_ptrGateDevice->Read(&packet);
	if(hr!=S_OK)	return hr;
	
	//	check for correspinding nSize and packet.m_Data.GetCount()
	packet.m_Data.SetSize(nSize);
	memcpy(ptrSrcData,packet.m_Data.GetData(),nSize);
	return hr;
}

HRESULT XBEndPointDevice::Write(void)
{
	ASSERT(m_ptrDeviceData);
	ASSERT(m_nDeviceDataSize);
	if(!m_ptrDeviceData || !m_nDeviceDataSize)	return E_POINTER;
	return Write(m_ptrDeviceData, m_nDeviceDataSize);
}

HRESULT XBEndPointDevice::Read(void)
{
	ASSERT(m_ptrDeviceData);
	ASSERT(m_nDeviceDataSize);
	if(!m_ptrDeviceData || !m_nDeviceDataSize)	return E_POINTER;
	return Read(m_ptrDeviceData, m_nDeviceDataSize);
}

HRESULT XBEndPointDevice::OnPacketRecieve(XBDataPacket *pDataPacket)
{
	return E_NOTIMPL;
}

#if 0
HRESULT XBEndPointDevice::Write(void *ptrSrcData, size_t nSize, XBCmdType cmdType)
{
	ASSERT(m_ptrGateDevice);

	HRESULT hr = S_OK;
	BYTE *ptrBuffer = new BYTE[sizeof(m_DevHeader) + nSize];
	if(!ptrBuffer)	return E_OUTOFMEMORY;

	m_DevHeader.CmdType = cmdType;
	m_DevHeader.DataSize = nSize;
	memcpy(ptrBuffer, &m_DevHeader, sizeof(m_DevHeader));
	if(nSize)
		memcpy(ptrBuffer + sizeof(m_DevHeader), ptrSrcData, nSize);

	hr = m_ptrGateDevice->Write(ptrBuffer,sizeof(m_DevHeader) + nSize);
	delete ptrBuffer;
	return hr;
}




// recieve data from parent GateDevice into ptrDestData buffer
HRESULT XBEndPointDevice::Read(void *ptrDestData, size_t nSize)
{
	ASSERT(ptrDestData);
	ASSERT(m_ptrGateDevice);

	XBDataPacket packet;
	HRESULT hr = S_OK;

	memcpy(&packet.m_Packet.m_EndPointHeader,&m_DevHeader,sizeof(m_DevHeader));
	hr = m_ptrGateDevice->Read(&packet);
	if(hr==S_OK)
	{
		int size = packet.m_Packet.m_EndPointHeader.DataSize;
		if(nSize<size)	size = nSize;
		if(size)
			memcpy(ptrDestData, packet.m_Packet.m_Data, size);
	}
	return hr;
	/*
	DEBUG_FUNCTION_NAME;
	ASSERT(ptrDestData);
	ASSERT(m_ptrGateDevice);
	
	HRESULT hr = S_OK;
	BYTE *ptrBuffer = new BYTE[sizeof(m_DevHeader) + nSize];
	if(!ptrBuffer)	return E_OUTOFMEMORY;

	hr = m_ptrGateDevice->Read(ptrBuffer,nSize+sizeof(m_DevHeader));
	if(hr==S_OK)	
	{
		if(nSize && ptrDestData)
			memcpy(ptrDestData, ptrBuffer + sizeof(m_DevHeader), nSize);
	}
	delete ptrBuffer;
	return hr;
	*/
}

//	читать из связанного LinkDevice и записать в буффер EndPointDevice

#endif