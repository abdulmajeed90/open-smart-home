#include "xbleddriver.h"

extern "C"
{
	DLL_PUBLIC XBEndPointDevice* CreateInstance(XBGateDevice *pGateDevice);
	DLL_PUBLIC void DeleteInstance(XBEndPointDevice* p);
}

// This class is exported from the LedDriverTLC59116.dll
//	TLC59116 - 16-CHANNEL Fm+ I2C-BUS CONSTANT-CURRENT LED SINK DRIVE
//	Datasheet: http://www.ti.com/lit/ds/symlink/tlc59116.pdf

//	2Do:
//	- ввести проверку адреса при его установке


//	align each structure memder to byte border
//	not needed in this case 

#pragma pack(1)
struct TLC59116Registers
{
	struct ModeRegister1
	{
		BYTE AllCall:1;	
		BYTE Sub3:1;	
		BYTE Sub2:1;
		BYTE Sub1:1;
		BYTE Osc:1;
		BYTE AI0:1;		//	read only
		BYTE AI1:1;		//	read only
		BYTE AI2:1;		//	read only

	}MODE1;

	struct ModeRegister2
	{
		BYTE Res1:1;
		BYTE Res2:1;
		BYTE OutCh:1;
		BYTE Res3:1;
		BYTE DBlink:1;
		BYTE Res4:1;
		BYTE ErrFlg:1;

	}MODE2;

	BYTE PWM[16];
	BYTE GRPPWM;
	BYTE GRPFREQ;
	struct LedOutRegister
	{
		BYTE LDR0:2;
		BYTE LDR1:2;
		BYTE LDR2:2;
		BYTE LDR3:2;
	} LEDOUT[4];
	BYTE SUBADDR[3];
	BYTE ALLCALLADDR;
	struct OutputGainRegister
	{
		BYTE CC:6;
		BYTE HC:1;
		BYTE CM:1;
	} IREF;
	BYTE EFLAG1;		//	read only
	BYTE EFLAG2;		//	read only
};


class LedDriverTLC59116 : public XBLedDriver
{
public:
	TLC59116Registers m_DevData;

public:
	LedDriverTLC59116(XBGateDevice *pGateDevice);
	
	virtual HRESULT SetLedPWM(int index, WORD pwm);
	virtual HRESULT GetLedPWM(int index, WORD &pwm);

	virtual HRESULT SetLedState(int index, XBLedState state);
	virtual HRESULT GetLedState(int index, XBLedState &state);

	virtual HRESULT SetSleepMode(bool bSleep = true);
	virtual HRESULT GetSleepMode(bool &bSleep);

	virtual HRESULT InitDefault(void);

	virtual HRESULT GetLedErrorStatus(int index, XBLedError &error);
	virtual HRESULT SetCurrentControl(BYTE crnt);
};
