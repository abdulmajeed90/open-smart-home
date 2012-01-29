#include "StdAfx.h"
#include "xb.h"
#include "XBDataPacket.h"
#include "utils.h"


XBDataPacket::XBDataPacket(void)
{
	m_Time = GetTickCount();
	ZERO_MEM(&m_EndPointHeader,sizeof(m_EndPointHeader));
}

XBDataPacket& XBDataPacket::operator=(const XBDataPacket &p)
{
	m_GateHeader.Address = p.m_GateHeader.Address;
	m_GateHeader.PANAddress = p.m_GateHeader.PANAddress;

	memcpy(&m_EndPointHeader,&p.m_EndPointHeader,sizeof(m_EndPointHeader));
	m_Address = p.m_Address;
	m_ExAddress = p.m_ExAddress;
	
	m_Data.SetSize(p.m_Data.GetCount());
	memcpy(m_Data.GetData(),p.m_Data.GetData(),p.m_Data.GetCount());

	m_Time = p.m_Time;
	return *this;
}

bool XBDataPacket::IsTimeOut(DWORD dwLifeTime)
{
	DWORD t = GetTickCount();
	if(abs_diff(t,m_Time)>XB_PACKET_LIFETIME)	return true;
	return false;
}

bool XBDataPacket::IsSameDevices(XBDataPacket *pDev)
{
	if(GetGateAddress()==pDev->GetGateAddress())
		if(GetAddress()==pDev->GetAddress())
			if(m_EndPointHeader.DevType == pDev->m_EndPointHeader.DevType)		return true;
	return false;
}

size_t XBDataPacket::GetSize(void)
{
	return sizeof(XBEndPointDevHeader) + GetValidDataSize() + m_EndPointHeader.AddrSize + m_EndPointHeader.ExAddrSize;
}

size_t XBDataPacket::GetValidDataSize(void)
{
	switch(m_EndPointHeader.CmdType)
	{
		case XBCmdRead:
		case XBCmdReadAddr:
		case XBCmdReadDelay:
			return 0;
		default:
			break;
	}
	return m_EndPointHeader.Size;
}
