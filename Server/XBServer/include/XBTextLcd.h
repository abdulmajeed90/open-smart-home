#pragma once
#include "XBEndPointDevice.h"
class XBGateDevice;
class XBTextLcd : public XBEndPointDevice
{
public:
	XBTextLcd(XBGateDevice *pGateDevice);
	~XBTextLcd(void);
};

