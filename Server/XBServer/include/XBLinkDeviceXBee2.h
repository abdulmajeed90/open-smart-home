#pragma once
#include "xblinkdevice.h"
#include "commdev.h"

class XBLinkDeviceXBee2 : public XBLinkDevice
{
public:
	XBLinkDeviceXBee2(void);
	
	virtual HRESULT Write(XBDataPacket *pSrcPacket);
	virtual HRESULT Read(XBDataPacket *pDestPacket);
	HRESULT Config(int nPortNumber, int nBoudRate);
	virtual HRESULT Open(void);

private:
	int m_nPortIndex;
	int m_nBoudRate;
	CommDev m_Comm;
};

