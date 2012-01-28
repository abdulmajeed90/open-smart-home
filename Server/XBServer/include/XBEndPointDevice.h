#pragma once
#include "XB.h"
#include "XBDevice.h"

#include "XBDataPacket.h"
//#include "XBGateDevice.h"

class XBGateDevice;

class XBEndPointDevice : public XBDevice
{
public:
	XBEndPointDevice(XBGateDevice *pGateDevice);
	~XBEndPointDevice();
private:
	//	link to parent GateDevice
	XBGateDevice *m_ptrGateDevice;
	
public:
	
	virtual HRESULT Write(void *ptrData, size_t nSize);
	virtual HRESULT Write(void);

	virtual HRESULT Read(void *ptrData, size_t nSize);
	virtual HRESULT Read(void);

	virtual HRESULT OnPacketRecieve(XBDataPacket *pDataPacket);

protected:
	void *m_ptrDeviceData;		//	should point to Device data structure
	size_t m_nDeviceDataSize;	//	size of Device data structure

	XBEndPointDevHeader m_DevHeader;
};

