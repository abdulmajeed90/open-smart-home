// LedDriverPCA9635.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "LedDriverPCA9635.h"
#include "XB.h"

//	dllmain.cpp was removed from this project after adding MFC support because of multiple DllMain symbol including

DLL_PUBLIC XBEndPointDevice* CreateInstance(XBGateDevice *pGateDevice)
{
	ASSERT(pGateDevice);
	ASSERT(sizeof(PCA9635Registers)==0x1C);	//	make sure that our registers take 28 bytes for precise mapping
	return new LedDriverPCA9635(pGateDevice);
}

DLL_PUBLIC void DeleteInstance(XBEndPointDevice* p)
{
	SAFE_DELETE((LedDriverPCA9635 *)p);
}

///////////////////////////////////////////////////////////////////////////////////////

LedDriverPCA9635::LedDriverPCA9635(XBGateDevice *pGateDevice) : XBLedDriver(pGateDevice)
{
	m_iLedCount = 16;
	m_iColorCount = 4;

	m_ptrDeviceData = &m_DevData;
	m_nDeviceDataSize = sizeof(m_DevData);
	
	//	fill DeviceHeader structure fields
	m_DevHeader.DevType = XBDevI2C;				//	this is a I2C led driver
	m_DevHeader.Size = m_nDeviceDataSize;		//	size of internal data
	m_DevHeader.DevIndex = 0;
	m_DevHeader.AddrSize = 1;					//	1byte address
	m_DevHeader.ExAddrSize = 1;					//	1 byte register address
	m_Address.SetSize(1);
	m_Address.SetAt(0,0xE0);					//	default address
	m_ExAddress.SetSize(1);
	m_ExAddress.SetAt(0,0x80);					//	default reset state of control register (by datasheet)

//	m_DevHeader.Address - set after object creation
//	m_DevHeader.CmdType - will be set by read/write/init/reset function
}

HRESULT LedDriverPCA9635::InitDefault(void)
{
	m_ExAddress[0] = 0x80;

	m_DevData.MODE1.Byte = 0x81;	// with sleep mode, use 0x81 for normal mode
	m_DevData.MODE2.Byte = 0x15;
	memset(m_DevData.PWM,0,16);	//	16 PWM values - set to 0
	m_DevData.GRPPWM = 0xFF;
	m_DevData.GRPFREQ = 0x00;
	m_DevData.SUBADDR[0]=0xE2;
	m_DevData.SUBADDR[1]=0xE4;
	m_DevData.SUBADDR[2]=0xE8;
	m_DevData.ALLCALLADDR = 0xE0;	
	m_DevData.LEDOUT[0].Byte = 0;
	m_DevData.LEDOUT[1].Byte = 0;
	m_DevData.LEDOUT[2].Byte = 0;
	m_DevData.LEDOUT[3].Byte = 0;
	return S_OK;
}

HRESULT LedDriverPCA9635::SetLedPWM(int index, WORD pwm)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;
	
	m_DevData.PWM[index] = (BYTE)(pwm>>8);
	return S_OK;
}

HRESULT LedDriverPCA9635::GetLedPWM(int index, WORD &pwm)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;
	pwm = (WORD)m_DevData.PWM[index]<<8;
	return S_OK;
}

HRESULT LedDriverPCA9635::SetLedState(int index, XBLedState state)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;
	int i = index/4;
	switch(index%4)
	{
		case 0:	m_DevData.LEDOUT[i].Bits.LDR0 = (BYTE)state;		break;
		case 1:	m_DevData.LEDOUT[i].Bits.LDR1 = (BYTE)state;		break;
		case 2:	m_DevData.LEDOUT[i].Bits.LDR2 = (BYTE)state;		break;
		case 3:	m_DevData.LEDOUT[i].Bits.LDR3 = (BYTE)state;		break;
		default:	return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT LedDriverPCA9635::GetLedState(int index, XBLedState &state)
{
	if(index>=m_iLedCount)	return E_INVALIDARG;
	BYTE res = 0;
	int i = index/4;
	switch(index%4)
	{
		case 0:	res = m_DevData.LEDOUT[i].Bits.LDR0;		break;
		case 1:	res = m_DevData.LEDOUT[i].Bits.LDR1;		break;
		case 2:	res = m_DevData.LEDOUT[i].Bits.LDR2;		break;
		case 3:	res = m_DevData.LEDOUT[i].Bits.LDR3;		break;
		default:	return E_INVALIDARG;
	}
	state = (XBLedState)res;
	return S_OK;
}

HRESULT LedDriverPCA9635::SetSleepMode(bool bSleep)
{
	m_DevData.MODE1.Bits.Sleep = (BYTE)bSleep;
	return S_OK;
}
HRESULT LedDriverPCA9635::GetSleepMode(bool &bSleep)
{
	bSleep = (bool)m_DevData.MODE1.Bits.Sleep;
	return S_OK;
}

HRESULT LedDriverPCA9635::GetLedErrorStatus(int index, XBLedError &error)
{
	return E_NOTIMPL;
}

HRESULT LedDriverPCA9635::SetCurrentControl(BYTE crnt)
{
	return E_NOTIMPL;
}
