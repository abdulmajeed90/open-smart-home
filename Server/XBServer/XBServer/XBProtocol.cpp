#include "StdAfx.h"
#include "XBProtocol.h"
#include "utils.h"


XBProtocol::XBProtocol(void)
{

}

size_t XBProtocol::PutXBPacket(XBDataPacket *p)
{
	size_t size = p->GetSize();
	if(!size)	return 0;

	m_bData.SetSize(size);
	memcpy(m_bData.GetData(),&p->m_EndPointHeader,sizeof(p->m_EndPointHeader));
	size = sizeof(p->m_EndPointHeader);
	if(p->m_EndPointHeader.AddrSize)
	{
		memcpy(m_bData.GetData()+size,p->m_Address.GetData(),p->m_EndPointHeader.AddrSize);
		size+=p->m_EndPointHeader.AddrSize;
	}
	if(p->m_EndPointHeader.ExAddrSize)
	{
		memcpy(m_bData.GetData()+size,p->m_ExAddress.GetData(),p->m_EndPointHeader.ExAddrSize);
		size+=p->m_EndPointHeader.ExAddrSize;
	}
	//	check if there is any read data to copy in current Command 
	if(p->GetValidDataSize())
	{
		memcpy(m_bData.GetData()+size,p->m_Data.GetData(),p->GetValidDataSize());
	}
	
	XBEndPointDevHeader *ep = (XBEndPointDevHeader *)m_bData.GetData();
	ep->CRC8 = Crc8(&ep->Size,m_bData.GetCount()-2);	

	return p->GetSize();
}

size_t XBProtocol::GetXBPacket(XBDataPacket *p)
{
	size_t nSize = 0;
	if(IsPacket(&m_bData)!=XBPacketAPI)	return 0;
	memcpy(&p->m_EndPointHeader,m_bData.GetData(),sizeof(p->m_EndPointHeader));
	nSize+=sizeof(p->m_EndPointHeader);
	if(p->m_EndPointHeader.AddrSize)
	{
		p->m_Address.SetSize(p->m_EndPointHeader.AddrSize);
		memcpy(p->m_Address.GetData(),m_bData.GetData()+nSize,p->m_EndPointHeader.AddrSize);
		nSize+=p->m_EndPointHeader.AddrSize;
	}
	if(p->m_EndPointHeader.ExAddrSize)
	{
		p->m_ExAddress.SetSize(p->m_EndPointHeader.ExAddrSize);
		memcpy(p->m_ExAddress.GetData(),m_bData.GetData()+nSize,p->m_EndPointHeader.ExAddrSize);
		nSize+=p->m_EndPointHeader.ExAddrSize;
	}
	if(p->m_EndPointHeader.Size)
	{
		p->m_Data.SetSize(p->m_EndPointHeader.Size);
		memcpy(p->m_Data.GetData(),m_bData.GetData()+nSize,p->m_EndPointHeader.Size);
		nSize+=p->m_EndPointHeader.Size;
	}
	return nSize;
}

//	check is there complete packet in data buffer and return its type
enum XBPacketSignatures XBProtocol::IsPacket(CByteArray *pData)
{
	ASSERT(pData);
	size_t size = (size_t)pData->GetCount();
	if(size>=sizeof(XBEndPointDevHeader))
	{
		XBEndPointDevHeader *p = (XBEndPointDevHeader *)pData->GetData();
		if(p->Signature==XBPacketAPI)
		{
			size_t nPacketSize = sizeof(XBEndPointDevHeader) + p->Size + p->AddrSize + p->ExAddrSize;
			if(size>=nPacketSize)
				if(Crc8(&p->Size,nPacketSize-2)==p->CRC8)	
					return XBPacketAPI;
		}
		else if(p->Signature==XBPacketCMD)
		{
			if(pData->GetAt(size-1)==XB_PACKET_CMD_EOL)	return XBPacketCMD;
		}
	}
	return XBPacketUnknown;
}

size_t XBProtocol::GetSize(void)
{
	return (size_t)m_bData.GetCount();
}

BYTE * XBProtocol::GetData(void)
{
	return m_bData.GetData();
}

int XBProtocol::GetTextData(char * strBuffer, size_t nBufferSize)
{
	return 0;
}

void XBProtocol::Clear(void)
{
	m_bData.RemoveAll();
}

size_t XBProtocol::PutData(CByteArray *pData)
{
	size_t nSize = pData->GetCount();
	if(nSize)
	{
		m_bData.SetSize(nSize);
		memcpy(m_bData.GetData(),pData->GetData(),nSize);
	}
	return nSize;
}
