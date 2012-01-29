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

#ifndef __AVR_ARCH__
typedef unsigned long long  QWORD;
#endif
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
// typedef unsigned char		BOOL;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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


//#define DEV_ADDRESS_SIZE_MAX 8
#define DEV_NAME_SIZE_MAX 32

#define XB_READ_TIMEOUT	1000					//	DeviceRead function timeout
#define XB_RXDATA_LIFETIME 60000				//	lifetime of data in RX buffer, msec
#define XB_PACKET_LIFETIME 60000				//	lifetime of data packets, msec
#define XB_PACKET_MAX_DATA_SIZE 0x100
#define XB_PACKET_CMD_EOL 0x0D


//---	Enumirations ---------

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
	XBDevUART,
	XBDevMCU
};

enum XBCmdMode
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
	XBErrorCmdLength,	//	packet size error
	XBErrorParams,		//	invalid parameters
	XBErrorCrc			//	packet CRC error
};

enum XBCmdType
{
	XBCmdUnknown = 0,
	XBCmdInit,				//	i
	XBCmdReset,				//	rst
	XBCmdWrite,				//	w
	XBCmdWriteAddr,			//	wa
	XBCmdRead,				//	r
	XBCmdReadAddr,			//	ra
	XBCmdInterrupt,			//	int
	XBCmdError,				//	err
	XBCmdWriteDelay,		//	wd
	XBCmdReadDelay,			//	rd
	XBCmdAskResponse=0x80,	//	rd		//	MSB means returning a data at any command
};

enum XBPacketSignatures
{
	XBPacketUnknown = 0,
	XBPacketAPI = 0xA5,
	XBPacketCMD = '?'
};

#ifndef __AVR_ARCH__
#pragma pack(1)
#endif

//	Basic header structure of communication protocol
typedef struct
{
	BYTE Signature;		//	signature of packet (0xA5 for data packer)
	BYTE CRC8;			//	crc of data, followed by this field
	BYTE Size;			//	size of data followed by this structure after address fields
	BYTE Error;			//	error code
	BYTE DevType;		//	device type:	see XBDevTypes union
	BYTE DevIndex;		//	device index (0..255)
	BYTE CmdType;		//	command type - XBCmdType
	BYTE AddrSize:4;	//	size of the address of device on a bus
	BYTE ExAddrSize:4;	//	size of extra address fields (or registers)

	//	BYTE Addr[AddrSize]
	//	BYTE ExAddr[ExAddrSize]
	//	BYTE Data [Size]
} XBEndPointDevHeader;


#define XB_DATA_SIZE_MAX 256		//	maximum data size on API packet
#define XB_ARRDESS_SIZE_MAX 8		//	maximum address size in API packet

#define AVR_BUFFER_SIZE sizeof(XBEndPointDevHeader) + XB_DATA_SIZE_MAX + XB_ARRDESS_SIZE_MAX*2		//	buffer size for packet processing

#ifndef S_OK
#define S_OK XBOK
#endif

#define LED1 0
#define LED_PORT PORTB
#define LED_DDR DDRB

#endif
