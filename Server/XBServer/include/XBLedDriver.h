#pragma once
#include "xbendpointdevice.h"

enum XBLedState 
{
	LedStateOff = 0,		//	LED driver X is off
	LedStateOn = 1,			//	LED driver X if fully on (individual brightness and group dimming/blinking not controlled)
	LedStatePWM = 2,		//	LED driver X individual brightness can be controlled through its PWMx register
	LedStateGRPPWM = 3		//	LED driver X individual brightness and group dimming/blinking can be controlled through its PWMx register and the GRPPWM registers
};

enum XBLedError
{
	LedNoError = 0,
	LedOpenCircuit = 1,
	LedShortCut = 2
};

class XBLedDriver : public XBEndPointDevice
{
protected:
	int m_iLedCount;
	int m_iColorCount;

public:
	XBLedDriver(XBGateDevice *pGateDevice);

	//	LED driver related funcionality
	//	change for HRESULT return value ???

public:	
	int GetLedCount(void) { return m_iLedCount; }
	int GetColorCount(void)	{return m_iColorCount; }

	virtual HRESULT SetLedPWM(int index, WORD pwm) = 0;
	virtual HRESULT GetLedPWM(int index, WORD &pwm) = 0;

	virtual HRESULT SetLedState(int index, XBLedState state) = 0;
	virtual HRESULT GetLedState(int index, XBLedState &state) = 0;

	virtual HRESULT SetSleepMode(bool bSleep = true) = 0;
	virtual HRESULT GetSleepMode(bool &bSleep) = 0;

	virtual HRESULT GetLedErrorStatus(int index, XBLedError &error) = 0;
	virtual HRESULT SetCurrentControl(BYTE crnt) = 0;
};
