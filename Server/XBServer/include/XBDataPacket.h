#pragma once
#include "xb.h"
#include "XBAddress.h"
#define GetEndPointDevHeaderSize GetMinSize

//#pragma pack(1)
struct XBGateDevHeader	//	data header between server and Link device (local side)
{
	XBAddress Address;
	XBAddress PANAddress;
	// not needed ???
//	DWORD	DataSize;						//	size of data
};

class XBDataPacket
{
public:
	XBGateDevHeader m_GateHeader;
	XBEndPointDevHeader m_EndPointHeader;
	XBAddress m_Address;
	XBAddress m_ExAddress;
	CByteArray m_Data;

private:
	DWORD m_Time;

public:
	XBDataPacket(void);
	~XBDataPacket(void);
	bool IsTimeOut(DWORD dwLifeTime = XB_PACKET_LIFETIME);
	
	size_t GetSize(void)
	{
		return sizeof(XBEndPointDevHeader) + m_EndPointHeader.Size + m_EndPointHeader.AddrSize + m_EndPointHeader.ExAddrSize;
	}
	
	static size_t GetGateDevHeaderSize(void)
	{
		return sizeof(XBGateDevHeader);
	}
	
	static size_t GetMinSize(void)
	{
		return sizeof(XBEndPointDevHeader);
	}

	bool IsEventPacket(void)
	{
		if(m_EndPointHeader.CmdType & XBCmdInterrupt)
			return true;
		return false;
	}

	bool IsSameDevices(XBDataPacket *pDev);
	/*
	size_t GetAddressSize(void)
	{
		return (size_t)m_Address.GetCount();
	}
	*/
	XBAddress &GetGateAddress(void)	
	{
		return m_GateHeader.Address;
	}

	XBAddress &GetAddress(void)	
	{
		return m_Address;
	}

	XBDataPacket& operator=(const XBDataPacket &p);

};

//	add timestamp for auto clearance