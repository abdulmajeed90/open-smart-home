#pragma once
#include "XB.h"
#include "XBDevice.h"
#include "XBDataPacket.h"
#include "XBEndPointDevice.h"
//#include "XBLinkDevice.h"

class XBLinkDevice;
//class XBEndPointDevice;

class XBGateDevice : public XBDevice
{
public:
	XBGateDevice(XBLinkDevice *pLinkDevice);
	~XBGateDevice();

private:
	XBLinkDevice *m_ptrLinkDevice;
public:
	CArray <XBEndPointDevice*> m_EndPointDevices;

public:
	HRESULT AddEndPointDevice(XBEndPointDevice *ptrDev);
	void RemoveEndPointDevice(XBEndPointDevice *ptrDev);

	int GetEndPointDeviceCount();
	XBEndPointDevice *GetEndPointDevice(int index);
	void RemoveAllEndPointDevices(void);
	XBEndPointDevice *FindEndPointDevice(XBAddress &addr);
	

	virtual HRESULT Write(XBDataPacket *pPacket);
	virtual HRESULT Read(XBDataPacket *pPacket);

//	virtual HRESULT Write(void *ptrData, size_t nSize);
//	virtual HRESULT Read(void *ptrData, size_t nSize);

	HRESULT OnPacketRecieve(XBDataPacket *pDataPacket);

protected:
	XBGateDevHeader m_DevHeader;
};
