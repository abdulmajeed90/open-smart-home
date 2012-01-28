#include "StdAfx.h"
#include "xb.h"
#include "XBDataPacket.h"
#include "utils.h"


XBDataPacket::XBDataPacket(void)
{
	m_Time = GetTickCount();
	ZERO_MEM(&m_EndPointHeader,sizeof(m_EndPointHeader));
}

XBDataPacket::~XBDataPacket(void)
{

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

/*
void XBDataPacket::operator =(const XBDataPacket &p)
{
	memcpy(&m_GateHeader,&p.m_GateHeader,sizeof(m_GateHeader));
	memcpy(&m_Packet,&p.m_Packet,sizeof(m_Packet));
	m_Time = GetTickCount();
}
*/
bool XBDataPacket::IsTimeOut(DWORD dwLifeTime)
{
	DWORD t = GetTickCount();
	if(abs_diff(t,m_Time)>XB_PACKET_LIFETIME)	return true;
	return false;
}
/*
void XBDataPacket::SetGateAddress(BYTE *pAddr)
{
	m_GateHeader.Address
	memcpy(m_Address,pAddr,DEV_ADDRESS_SIZE_MAX);
}
*/
bool XBDataPacket::IsSameDevices(XBDataPacket *pDev)
{
	if(GetGateAddress()==pDev->GetGateAddress())
		if(GetAddress()==pDev->GetAddress())
			if(m_EndPointHeader.DevType == pDev->m_EndPointHeader.DevType)		return true;
	return false;
}