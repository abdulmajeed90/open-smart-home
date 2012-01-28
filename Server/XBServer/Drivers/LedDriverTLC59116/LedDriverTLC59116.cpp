// LedDriverTLC59116.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "LedDriverTLC59116.h"
#include "XB.h"

DLL_PUBLIC XBEndPointDevice* CreateInstance(XBGateDevice *pGateDevice)
{
	ASSERT(pGateDevice);
	ASSERT(sizeof(TLC59116Registers)==0x1E);	//	make sure that our registers take 30 bytes for precise mapping
	return new LedDriverTLC59116(pGateDevice);
}

DLL_PUBLIC void DeleteInstance(XBEndPointDevice* p)
{
	SAFE_DELETE((LedDriverTLC59116 *)p);
}

///////////////////////////////////////////////////////////////////////////////////////

LedDriverTLC59116::LedDriverTLC59116(XBGateDevice *pGateDevice) : XBLedDriver(pGateDevice)
{
	m_iLedCount = 16;
	m_iColorCount = 4;

	m_ptrDeviceData = &m_DevData;
	m_nDeviceDataSize = sizeof(m_DevData);

	//	fill DeviceHeader structure fields
	m_DevHeader.DevType = XBDevI2C;					//	this is a I2C led driver
	m_DevHeader.Size = m_nDeviceDataSize;			//	size of internal data
	m_DevHeader.AddrSize = 1;						//	size of address ( 1 byte on I2C)
	m_DevHeader.ExAddrSize = 1;						//	size of ext. address
	m_ExAddress[0] = 0x80;					//	default reset state of control register (by datasheet)
	
//	m_DevHeader.Address - set after object creation
//	m_DevHeader.CmdType - will be set by read/write/init/reset function
}

HRESULT LedDriverTLC59116::InitDefault(void)
{
//	m_DevData.MODE1 = 0x91;	// with sleep mode, use 0x81 for normal mode
//	m_DevData.MODE2 = 0x05;
	m_DevData.GRPPWM = 0xFF;
	m_DevData.GRPFREQ = 0x00;
	memset(m_DevData.PWM,0,16);	//	16 PWM values
	//	è ò.ä.
	return S_OK;
}

HRESULT LedDriverTLC59116::SetLedPWM(int index, WORD pwm)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;

	m_DevData.PWM[index] = (BYTE)(pwm>>8);
	_tprintf(_T("LedDriverPCA9635::SetLedPWM (%i, %i)\n"),index,pwm);
	return S_OK;
}

HRESULT LedDriverTLC59116::GetLedPWM(int index, WORD &pwm)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;
	pwm = (WORD)m_DevData.PWM[index]<<8;
	return S_OK;
}

HRESULT LedDriverTLC59116::SetLedState(int index, XBLedState state)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;
	
	int i = index/4;
	switch(index%4)
	{
		case 0:	m_DevData.LEDOUT[i].LDR0 = (BYTE)state;		break;
		case 1:	m_DevData.LEDOUT[i].LDR1 = (BYTE)state;		break;
		case 2:	m_DevData.LEDOUT[i].LDR2 = (BYTE)state;		break;
		case 3:	m_DevData.LEDOUT[i].LDR3 = (BYTE)state;		break;
		default:	return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT LedDriverTLC59116::GetLedState(int index, XBLedState &state)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;

	BYTE res = 0;
	int i = index/4;
	switch(index%4)
	{
		case 0:	res = m_DevData.LEDOUT[i].LDR0;		break;
		case 1:	res = m_DevData.LEDOUT[i].LDR1;		break;
		case 2:	res = m_DevData.LEDOUT[i].LDR2;		break;
		case 3:	res = m_DevData.LEDOUT[i].LDR3;		break;
		default:	return E_INVALIDARG;
	}
	state = (XBLedState)res;
	return S_OK;
}

HRESULT LedDriverTLC59116::SetSleepMode(bool bSleep)
{
	m_DevData.MODE1.Osc = (BYTE)bSleep;
	return S_OK;
}
HRESULT LedDriverTLC59116::GetSleepMode(bool &bSleep)
{
	bSleep = (bool)m_DevData.MODE1.Osc;
	return S_OK;
}

HRESULT LedDriverTLC59116::GetLedErrorStatus(int index, XBLedError &error)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;
	BYTE err = 0;
	
	switch(index/8)
	{
		case 0:	err = m_DevData.EFLAG1;	break;
		case 1:	err = m_DevData.EFLAG2;	break;
		default:	return E_INVALIDARG;
	}

	error = (XBLedError)((err>>index%8)&1);		//	shift right to INDEX bits and clear all bits except first
	return S_OK;
}

HRESULT LedDriverTLC59116::SetCurrentControl(BYTE crnt)
{
	m_DevData.IREF.CC = crnt;			//	add extra work with other fields of this register
	return S_OK;
}
