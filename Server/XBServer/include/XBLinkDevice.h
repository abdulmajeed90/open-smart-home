#pragma once
#include "XB.h"
#include "XBDevice.h"
#include "XBDataPacket.h"
#include "XBGateDevice.h"
#include "XBEndPointDevice.h"
#include "XBProtocol.h"

// class XBGateDevice;
//class XBEndPointDevice;
//class XBDataPacket;

class XBLinkDevice : public XBDevice
{
protected:
	CArray <XBGateDevice*> m_GateDevices;		//	list of gate devices connected to this LINK device
	CArray <XBDataPacket> m_DataPackets;		//	array of data packets, 
	CRITICAL_SECTION m_CriticalSection; 
private:
	HANDLE hProcessingThread;
	HANDLE hPacketEvent;
	DWORD dwThreadId;

public:
	XBLinkDevice(void);
	~XBLinkDevice(void);

	XBProtocol m_XbProtocol;

	//	child devices support
	HRESULT AddGateDevice(XBGateDevice *ptrDev);
	void RemoveGateDevice(XBGateDevice *ptrDev);

	int GetGateDeviceCount();
	XBGateDevice *GetGateDevice(int index);
	void RemoveAllGateDevices(void);
	XBGateDevice *FindGateDevice(XBAddress &Addr);

	virtual HRESULT Open(void);
	virtual HRESULT Write(XBDataPacket *pSrcPacket) = 0;
	virtual HRESULT Read(XBDataPacket *pDestPacket) = 0;

//	bool IsPacketInBuffer(void *pBuffer, size_t nSize);	//	check if data packet in buffer (XBEndPointDevHeader + Data)

	virtual HRESULT OnPacketRecieve(XBDataPacket *pDataPacket);
	virtual void OnCharRecieve(CByteArray *pArray);

private:
	static HRESULT WINAPI PacketProcessingThread(LPVOID lpParameter);
};

