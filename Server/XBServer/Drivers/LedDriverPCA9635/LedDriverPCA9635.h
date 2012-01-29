#include "xbleddriver.h"

extern "C"
{
	DLL_PUBLIC XBEndPointDevice* CreateInstance(XBGateDevice *pGateDevice);
	DLL_PUBLIC void DeleteInstance(XBEndPointDevice* p);
}

// This class is exported from the LedDriverPCA9635.dll
//	PCA9635PW - 16-bit Fm+ I2C-bus LED drive
//	Datasheet: http://www.nxp.com/documents/data_sheet/PCA9635.pdf

//	2Do:
//	- ввести проверку адреса при его установке


//	align each structure memder to byte border
//	not needed in this case

#pragma pack(1)
struct PCA9635Registers
{
	union
	{
		BYTE Byte;
		struct
		{
			BYTE AllCall:1;	
			BYTE Sub3:1;	
			BYTE Sub2:1;
			BYTE Sub1:1;
			BYTE Sleep:1;
			BYTE AI0:1;		//	read only
			BYTE AI1:1;		//	read only
			BYTE AI2:1;		//	read only
		}Bits;
	} MODE1;

	union
	{
		BYTE Byte;
		struct
		{
			BYTE Res2:1;
			BYTE Res1:1;
			BYTE DmBlink:1;
			BYTE Invrt:1;
			BYTE OCh:1;
			BYTE OutDrv:1;
			BYTE OutNE:2;
		} Bits;
	}MODE2;

	BYTE PWM[16];
	BYTE GRPPWM;
	BYTE GRPFREQ;
	union
	{
		BYTE Byte;
		struct
		{
			BYTE LDR0:2;
			BYTE LDR1:2;
			BYTE LDR2:2;
			BYTE LDR3:2;
		} Bits;
	} LEDOUT[4];
	BYTE SUBADDR[3];
	BYTE ALLCALLADDR;
};


//-----------------------------------------------------------------------------------------
//	Sample data packet for turing on R+G+B+W leds
//	a5 73 1c 00 01 00 04 11 e0 
//	80 81 15 00 40 00 00 00 40 00 00 00 40 00 00 00 40 00 00 ff 00 aa aa aa aa e2 e4 e8 e0
//-----------------------------------------------------------------------------------------


class LedDriverPCA9635 : public XBLedDriver
{
public:
	LedDriverPCA9635(XBGateDevice *pGateDevice);
public:
	PCA9635Registers m_DevData;

	//	overrided functions
public:
	virtual HRESULT InitDefault(void);
	virtual HRESULT SetLedPWM(int index, WORD pwm);
	virtual HRESULT GetLedPWM(int index, WORD &pwm);

	virtual HRESULT SetLedState(int index, XBLedState state);
	virtual HRESULT GetLedState(int index, XBLedState &state);

	virtual HRESULT SetSleepMode(bool bSleep = true);
	virtual HRESULT GetSleepMode(bool &bSleep);

	virtual HRESULT GetLedErrorStatus(int index, XBLedError &error);
	virtual HRESULT SetCurrentControl(BYTE crnt);
};
