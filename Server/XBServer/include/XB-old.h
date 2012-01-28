#ifndef _XB_H_
#define _XB_H_

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

//-------  Basic types definition -------------------------------------------------

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

// For BINARY constants defining ***********************************************************************************

#define HEX__(n) 0x##n##LU

#define B8__(x) ((x&0x0000000FLU)?1:0)\
				+((x&0x000000F0LU)?2:0)\
				+((x&0x00000F00LU)?4:0)\
				+((x&0x0000F000LU)?8:0)\
				+((x&0x000F0000LU)?16:0)\
				+((x&0x00F00000LU)?32:0)\
				+((x&0x0F000000LU)?64:0)\
				+((x&0xF0000000LU)?128:0)

#define B8(d) ((BYTE)B8__(HEX__(d)))
#define B16(dmsb,dlsb) (((WORD)B8(dmsb)<<8) + B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) (((DWORD)B8(dmsb)<<24) + ((DWORD)B8(db2)<<16) + ((DWORD)B8(db3)<<8) + B8(dlsb))

/*
using:
- B8(011100111)
- B16(10011011,10011011)
- B32(10011011,10011011,10011011,10011011)
*/
//---------------------------------------------------------------------
//	Bitwise operations

#define SETBITS(mem, bits)      (mem) |= (bits)
#define CLEARBITS(mem, bits)    (mem) &= ~(bits)

//---------------------------------------------------------------------

#define SAFE_DELETE(x) if(x) delete(x)
#define ZERO_MEM(ptr,size)	memset(ptr,0,size)

// debug and messages

#ifdef _DEBUG

#ifndef ASSERT 
	#define ASSERT(x)	if(!x)	DebugBreak()
#endif
	#define DBGOUT(x)	OutputDebugString(x)
	#define DEBUG_FUNCTION_NAME _tprintf(TEXT(__FUNCTION__)); _tprintf(_T("\n"))
	#define LOG(x)	_tprintf(x); _tprintf(_T("\n"))
	#include "tchar.h"
#endif

enum XBLinkDeviceTypes 
{
	XBLinkDeviceTypeUnknown = 0, 
	XBLinkDeviceTypeXBee2, 
	XBLinkDeviceTypeXBee2Pro, 
	XBLinkDeviceTypeCAN, 
	XBLinkDeviceTypeRS285
};

enum XBGateDeviceTypes 
{
	XBGateDeviceTypeUnknown = 0, 
	XBGateDeviceTypeAtmega8, 
	XBGateDeviceTypeAtmega328, 
	XBGateDeviceTypeSTM8, 
	XBGateDeviceTypeSTM32, 
	XBGateDeviceTypeXBee2
};


//----------		Type of DEVICEs -----------------------------------
enum XBDevTypes
{
	XBDevUnknown,
	XBDevI2C,
	XBDev1Wire,
	XBDev2Wire,
	XBDev3Wire,
	XBDevSPI,
	XBDevPWM,
	XBDevADC,
	XBDevEEPROM,
	XBDevDIO,
	XBDevAC,
	XBDevPower,
	XBDevUART
};

enum	XBCmdMode
{
	XBCmdModeAPI,
	XBCmdModeTXT
};

enum XBErrors
{
	XBOK = 0,
	XBErrorDevType,		//	wrong device type
	XBErrorDevIndex,	//	wrong device index
	XBErrorDevNoImpl,	//	device support is not implemented
	XBErrorCmdType,		//	wrong command type
};

enum XBBusTypes 
{
	XBBusTypeUnknown = 0, 
	XBBusTypeLocal,
	XBBusTypeI2C, 
	XBBusType1Wire, 
	XBBusType3Wire,
	XBBusTypeSPI
};
enum XBDeviceFamily
{
	XBDeviceTypeUnknown = 0, 
	XBDeviceTypeLedDriver, 
	XBDeviceTypeGPIO, 
	XBDeviceTypeRelay, 
	XBDeviceTypeRTC, 
	XBDeviceTypePWM
};
enum XBCmdType
{
	XBCmdUnknown = 0,
	XBCmdInit,
	XBCmdReset,
	XBCmdWrite,
	XBCmdWriteAddr,
	XBCmdRead,
	XBCmdReadAddr,
	XBCmdInterrupt,
	XBCmdError,
	XBCmdWriteDelay,
	XBCmdReadDelay,
	XBCmdWaitComfirm = 0x40,	//	wait for answer from EndPoint device
	XBCmdEvent = 0x80			//	generate event on server side
};
/*
#pragma pack(1)
struct XBPacketHeader			//	������ ���������� ��������� ���� ��������� ���� ��������� ��� �������� ������
{
	BYTE	Address[DEV_ADDRESS_SIZE_MAX];	//	����� ���������� �� ���� ��� ���������� ����������
	WORD	DevType;						//	��� �������
	BYTE	CmdType;						//	��� ������� - Write / Read / AskRead
	BYTE	Register;						// 	����� �������� ��� ���. �����
	WORD	DataSize;						//	������ ������, ��������� �� ���� ���������
};
*/


#define DEV_ADDRESS_SIZE_MAX 8
#define DEV_NAME_SIZE_MAX 32
#define DEV_REG_ADDR_MAX 2

#define XB_PACKET_SIGNATURE_API 0xA5			//	signature for API mode
#define XB_PACKET_SIGNATURE_TXT 0xAA			//	signature for TXT mode
#define XB_READ_TIMEOUT	1000					//	DeviceRead function timeout
#define XB_RXDATA_LIFETIME 60000				//	lifetime of data in RX buffer, msec
#define XB_PACKET_LIFETIME 60000				//	lifetime of data packets, msec
#define XB_PACKET_MAX_DATA_SIZE 0x100

#pragma pack(1)
struct XBEndPointDevHeader	//	data header between MCU gate and Link device (remote side)
{
	BYTE	Signature;						//	��������� ������ (0xAA)
	BYTE	CRC;							//	����������� ����� ������ ����� ����� ����
	BYTE	Address[DEV_ADDRESS_SIZE_MAX];	//	����� ���������� + ������ ���������
	BYTE	RegAddr[DEV_REG_ADDR_MAX];		// 	����� ��������, ����� EEPROM,  � �.�.
	BYTE	RegAddrSize:2;					//	size of RegAddr field
	BYTE	BusType:6;						//	��� ���� - I2C / 1-Wire / 3-Wire / Local
	BYTE	CmdType;						//	��� ������� - Write / Read / AskRead
	BYTE	DevType;						//	��� ������� - GPIO, PWM, LedDriver, ADC (��� Local bus)
	BYTE	Error;							//	���� � ����������� �� ������
	WORD	DataSize;						//	������ ������, ��������� �� ���� ���������
};

#pragma pack(1)
struct XBGateDevHeader	//	data header between server and Link device (local side)
{
	BYTE	Address[DEV_ADDRESS_SIZE_MAX];	//	����� GATE ���������� (destination XBee serial number)
	WORD	PanAddr;						// 	����� ����
	DWORD	DataSize;						//	������ ������, ��������� �� ���� ���������
};

typedef struct
{
	BYTE Sgn;
	BYTE CmdIt;
	BYTE Crc8;
	BYTE DataSize;
} XBPacketHeader;

#ifndef S_OK
#define S_OK 0
#endif


#endif
