#include "StdAfx.h"
#include "XBEndPointDevice.h"
#include "XBLedDriver.h"

XBLedDriver::XBLedDriver(XBGateDevice *pGateDevice) : XBEndPointDevice(pGateDevice)
{
	m_iLedCount = 0;
	m_iColorCount = 0;
}

