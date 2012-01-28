#include "StdAfx.h"
#include "XBLinkDeviceXBee2.h"

XBLinkDeviceXBee2::XBLinkDeviceXBee2(void) : m_Comm(this)
{
	m_nBoudRate = CBR_57600;
	m_nPortIndex = 0;
}

HRESULT XBLinkDeviceXBee2::Write(XBDataPacket *pSrcPacket)
{
	HRESULT hr = E_FAIL;
	ASSERT(pSrcPacket);

	if(m_XbProtocol.PutXBPacket(pSrcPacket))
		hr = m_Comm.Write(m_XbProtocol.GetData(),m_XbProtocol.GetSize());
	/*
	hr = m_Comm.Write(&pSrcPacket->m_EndPointHeader,sizeof(pSrcPacket->m_EndPointHeader));
	if(pSrcPacket->m_EndPointHeader.AddrSize && hr == S_OK)
		hr = m_Comm.Write(pSrcPacket->m_Address.GetData(),pSrcPacket->m_Address.GetCount());
	if(pSrcPacket->m_EndPointHeader.ExAddrSize && hr == S_OK)
		hr = m_Comm.Write(pSrcPacket->m_ExAddress.GetData(),pSrcPacket->m_ExAddress.GetCount());
	if(pSrcPacket->m_EndPointHeader.Size && hr == S_OK)
		hr = m_Comm.Write(pSrcPacket->m_Data.GetData(),pSrcPacket->m_Data.GetCount());
	*/
	return hr;
//	return m_Comm.Write(&pSrcPacket->m_Packet,pSrcPacket->GetPacketSize());
}

HRESULT XBLinkDeviceXBee2::Read(XBDataPacket *pDestPacket)
{
	HRESULT hr = E_FAIL;
	ASSERT(pDestPacket);
	if(m_XbProtocol.PutXBPacket(pDestPacket))
		hr = m_Comm.Write(m_XbProtocol.GetData(),m_XbProtocol.GetSize());
	if(hr!=S_OK)	return hr;

	/*


	if(m_Comm.Write(&pDestPacket->m_EndPointHeader,sizeof(pDestPacket->m_EndPointHeader))!=S_OK)	return E_FAIL;
	if(pDestPacket->m_EndPointHeader.AddrSize)
		m_Comm.Write(pDestPacket->m_Address.GetData(),pDestPacket->m_Address.GetCount());
	if(pDestPacket->m_EndPointHeader.ExAddrSize)
		m_Comm.Write(pDestPacket->m_ExAddress.GetData(),pDestPacket->m_ExAddress.GetCount());
	*/
//	if(m_Comm.Write(&pDestPacket->m_Packet,pDestPacket->GetEndPointDevHeaderSize())!=S_OK)	return E_FAIL;

	int count = 0;
	for(int n=0;n<100;n++)
	{
		Sleep(100);
		count = m_DataPackets.GetCount();
		for(int i=0;i<count;i++)
		{
			XBDataPacket &packet = m_DataPackets.GetAt(i);
			if(packet.IsSameDevices(pDestPacket))
			{
				*pDestPacket = packet;		//	copy data packet to destination

				EnterCriticalSection(&m_CriticalSection);
				m_DataPackets.RemoveAt(i);
				LeaveCriticalSection(&m_CriticalSection);
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT XBLinkDeviceXBee2::Config(int nPortNumber, int nBoudRate)
{
	m_nPortIndex = nPortNumber;
	m_nBoudRate = nBoudRate;
	return S_OK;
}

HRESULT XBLinkDeviceXBee2::Open(void)
{
	HRESULT hr = S_OK;
	hr = XBLinkDevice::Open();
	if(hr==S_OK)
		hr = m_Comm.Open(m_nPortIndex,m_nBoudRate);
	return hr;
}
