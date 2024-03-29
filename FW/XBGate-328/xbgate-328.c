#include <avr/io.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "xb.h"
#include "hw.h"
#include "uart.h"
#include "twi-i2c.h"
#include "1wire.h"
#include "2wire.h"
#include "dio.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define BUILD_NUMBER 1
#define MCU_VERSION "Atmega328P"
#define HELP_STR "use dev[index].cmd([addr],[ex-addr],data)"


BYTE bBuffer[AVR_BUFFER_SIZE];

XBEndPointDevHeader *pDevHeader;
BYTE *bData;
BYTE *bAddr;
BYTE *bExAddr;
BOOL bSendBack;

enum XBErrors ProcessCmd(void);


BYTE Crc8(BYTE *pcBlock, unsigned int len)
{
    BYTE crc = 0xFF;
    unsigned int i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;
}

BYTE Hex2Byte(char ch)
{
	BYTE data = 0xFF;
	if(ch>='0' && ch<='9')	data = ch-'0';
	else if(ch>='a' && ch<='f') data = ch - 'a'+ 10;
	return data;
}

BYTE HexStr2Bytes(char *strSrc, BYTE *dest)
{
	BYTE d1,d2;
	BYTE size=0;

	while(*strSrc)
	{
		d1 = Hex2Byte(*strSrc++);
		if(d1==0xFF)	return 0;
		*dest = d1;
		if(*strSrc)
		{
			d2 = Hex2Byte(*strSrc++);
			if(d1==0xFF)	return 0;
			*dest = d1<<4 | d2;
		}
		dest++;
		size++;
	}
	return size;
}

BYTE GetVarIndex(char *strDev)
{
	BYTE i = strlen(strDev)-1;
	BYTE index = 0;
	int pow = 1;
	while(i>0)
		{
			if(strDev[i]>='0' && strDev[i]<='9')
			{
				index = index + (strDev[i]-'0')*pow;
				pow*=10;
			}
			else break;
			i--;
		}
	return index;
}

enum XBDevTypes GetDeviceType(char *strDev)
{
	enum XBDevTypes dt = XBDevUnknown;

	if(!strncmp(strDev,"i2c",3))			dt = XBDevI2C;
	else if(!strncmp(strDev,"1w",2))		dt = XBDev1Wire;
	else if(!strncmp(strDev,"2w",2))		dt = XBDev2Wire;
	else if(!strncmp(strDev,"3w",2))		dt = XBDev3Wire;
	else if(!strncmp(strDev,"spi",3))		dt = XBDevSPI;
	else if(!strncmp(strDev,"pwm",3))		dt = XBDevPWM;
	else if(!strncmp(strDev,"adc",3))		dt = XBDevADC;
	else if(!strncmp(strDev,"eeprom",6))	dt = XBDevEEPROM;
	else if(!strncmp(strDev,"dio",3))		dt = XBDevDIO;
	else if(!strncmp(strDev,"ac",2))		dt = XBDevAC;
	else if(!strncmp(strDev,"pwr",3))		dt = XBDevPower;
	else if(!strncmp(strDev,"uart",4))		dt = XBDevUART;
	else if(!strncmp(strDev,"mcu",4))		dt = XBDevMCU;

	return dt;
}

enum XBCmdType GetCmdType(char *strCmd)
{
	enum XBCmdType cmd = XBCmdUnknown;
	if(strCmd)
	{
		if(!strcmp(strCmd,"i"))			cmd = XBCmdInit;			//	i
		else if(!strcmp(strCmd,"rst"))	cmd = XBCmdReset;			//	rst
		else if(!strcmp(strCmd,"w"))	cmd = XBCmdWrite;			//	w
		else if(!strcmp(strCmd,"wa"))	cmd = XBCmdWriteAddr;		//	wa
		else if(!strcmp(strCmd,"r"))	cmd = XBCmdRead;			//	r
		else if(!strcmp(strCmd,"ra"))	cmd = XBCmdReadAddr;		//	ra
		else if(!strcmp(strCmd,"int"))	cmd = XBCmdInterrupt;		//	int
		else if(!strcmp(strCmd,"err"))	cmd = XBCmdError;			//	err
		else if(!strcmp(strCmd,"wd"))	cmd = XBCmdWriteDelay;		//	wd
		else if(!strcmp(strCmd,"rd"))	cmd = XBCmdReadDelay;		//	rd
	}
	return cmd;
}

//	i2c.w(e0,8115)
//	i2c.wa(e0,80,811500FF000000FF000000FF000000FF0000)
//	pwm1.w(8A)
//	spi.w(10)
//	3w.r(10)
//	1w.w(80A1CF008067A912,40)
//	1w.r(80A1CF008067A912,2)
//	mcu.rst
//	dio1.w(1)
//	? - help
//	??? - switch mode API/CMD
//
/*
enum XBErrors ProcessCmd(void)
{
	enum XBErrors err = XBOK;
	switch(pDevHeader->DevType)
		{
			case XBDevI2C:		err = ProcessI2CCmd(pDevHeader,bAddr,bExAddr,bData);	break;
			case XBDev1Wire:
			case XBDev2Wire:
			case XBDev3Wire:
			case XBDevSPI:
			case XBDevPWM:
			case XBDevADC:
			case XBDevEEPROM:	err = XBErrorDevNoImpl;	break;
			case XBDevDIO:		err = ProcessDIO(pDevHeader,bAddr,bExAddr,bData);	break;
			case XBDevAC:
			case XBDevPower:
			case XBDevUART:
			case XBDevMCU:		err = XBErrorDevNoImpl;	break;
			default:			err = XBErrorDevType;	break;
		}
		return err;
}
*/
BYTE GetRealDataSize(void)
{
	switch(pDevHeader->CmdType & 0x7F)
		{
			case XBCmdRead:
			case XBCmdReadAddr:
			case XBCmdReadDelay:
				return 0;
				break;
			default:
				break;
		}
	return pDevHeader->Size;
}

BOOL IsResponse(void)
{
	if(pDevHeader->CmdType & XBCmdAskResponse)
	{
		pDevHeader->Size = 0;		//	cut off data
		return TRUE;
	}
	switch(pDevHeader->CmdType & 0x7F)
	{
		case XBCmdRead:
		case XBCmdReadAddr:
		case XBCmdReadDelay:
			return TRUE;
			break;
		default:
			break;
	}
	return FALSE;
}

enum XBErrors ProcessTextCmd(char *str)
{
	char s[16];
	if(strcmp(str,"ver")==0)	sprintf(s,"%s: %i.%i.%i",MCU_VERSION,MAJOR_VERSION,MINOR_VERSION,BUILD_NUMBER);
	else if(strcmp(str,"?")==0)	strcpy(s,HELP_STR);
	else return XBErrorCmdType;

	UARTWriteString(s);
	UARTWriteChar('\r');
	return XBOK;
}


//	convert string parameters into byte arrays
enum XBErrors ProcessParamStr(char *strParams)
{
	char *p[8]; // ????
	BYTE i=0;
	enum XBErrors err = XBOK;

	if(!strParams)	return XBOK;
	do
	{
		p[i] = strtok(strParams,",");
		strParams = NULL;
		if(!p[i])	break;
		i++;
	}
	while(1);

	switch(i)
	{
		case 0:	break;
		case 1:
			{
				pDevHeader->Size = HexStr2Bytes(p[0],bData);
				if(!pDevHeader->Size)	err = XBErrorParams;
				break;
			}
		case 2:
			{
				pDevHeader->AddrSize = HexStr2Bytes(p[0],bAddr);
				pDevHeader->Size = HexStr2Bytes(p[1],bData);
				if(!pDevHeader->Size || !pDevHeader->AddrSize)	err = XBErrorParams;
				break;
			}
		case 3:
			{
				pDevHeader->AddrSize = HexStr2Bytes(p[0],bAddr);
				pDevHeader->ExAddrSize = HexStr2Bytes(p[1],bExAddr);
				pDevHeader->Size = HexStr2Bytes(p[2],bData);
				if(!pDevHeader->Size || !pDevHeader->AddrSize || !pDevHeader->ExAddrSize)	err = XBErrorParams;
				break;
			}
		default:
			break;
	}
	return err;
}

enum XBErrors ProcessStringCmd(char *str)
{
	int i = 0;
	char *strDev = 0;
	char *strParam = 0;
	char strDeli[]=".:()";
	enum XBErrors err;

	//	make all char low case
	while(str[i])
		{
			str[i] = tolower(str[i]);
			i++;
		}

	ZERO_MEM(pDevHeader,sizeof(XBEndPointDevHeader));

	//	get device type & index
	strDev = strtok(str,strDeli);
	pDevHeader->DevType = GetDeviceType(strDev);
	pDevHeader->DevIndex = GetVarIndex(strDev);

	if(pDevHeader->DevType==XBDevUnknown)	return ProcessTextCmd(str);

	//	get command type
	pDevHeader->CmdType = GetCmdType(strtok(NULL,strDeli));
	if(pDevHeader->CmdType==XBCmdUnknown)	return XBErrorCmdType;

	//	get parameters
	strParam = strtok(NULL,strDeli);
	err = ProcessParamStr(strParam);
	if(err!=XBOK) return err;

	return ProcessCmd();
}

enum XBErrors ProcessTXTPacket(void)
{
	BYTE ch;
	BYTE i = 0;

	do
	{
		ch = UARTReadChar();
		bData[i++] = ch;
		if(i>=256) return XBErrorCmdLength;
	} while(ch!=0x0D);
	bData[i-1]=0;

	return ProcessStringCmd((char *)bData);
}

//---------  API packets ----------------------------------

enum XBErrors ProcessCmd(void)
{
	enum XBErrors err = XBOK;
	LED_PORT = 1 << LED1;
	switch(pDevHeader->DevType)
	{
		case XBDevI2C:		err = ProcessI2CCmd(pDevHeader,bAddr,bExAddr,bData);	break;
		case XBDev1Wire:
		case XBDev2Wire:
		case XBDev3Wire:
		case XBDevSPI:
		case XBDevPWM:
		case XBDevADC:
		case XBDevEEPROM:	err = XBErrorDevNoImpl;	break;
		case XBDevDIO:		err = ProcessDIO(pDevHeader,bAddr,bExAddr,bData);	break;
		case XBDevAC:
		case XBDevPower:
		case XBDevUART:
		case XBDevMCU:		err = XBErrorDevNoImpl;	break;
		default:			err = XBErrorDevType;	break;
	}
	LED_PORT = 0 << LED1;
	return err;
}

enum XBErrors ProcessAPICmd(void)
{
	BYTE i;
	enum XBErrors err = XBOK;
	switch(pDevHeader->CmdType & 0x7F)
	{
		case XBCmdWrite:
		case XBCmdWriteAddr:
		case XBCmdWriteDelay:
		{
			for(i=0;i<pDevHeader->Size;i++)		bData[i] = UARTReadChar();
			if(Crc8(&pDevHeader->Size,sizeof(XBEndPointDevHeader) + pDevHeader->AddrSize + pDevHeader->ExAddrSize + pDevHeader->Size - 2)==pDevHeader->CRC8)
				err = ProcessCmd();
			else err = XBErrorCrc;
			pDevHeader->Size = 0;		//	clear size of data send back
			break;
		}
		case XBCmdRead:
		case XBCmdReadAddr:
		case XBCmdReadDelay:
		{
			if(Crc8(&pDevHeader->Size,sizeof(XBEndPointDevHeader) + pDevHeader->AddrSize + pDevHeader->ExAddrSize - 2)==pDevHeader->CRC8)
				err = ProcessCmd();
			else
				err = XBErrorCrc;
			bSendBack = TRUE;		//	send data back in any case
			break;
		}
		case XBCmdInit:
		case XBCmdReset:
		case XBCmdInterrupt:
		case XBCmdError:
		case XBCmdAskResponse:
		default:
		{
			pDevHeader->Size = 0;
			err = XBErrorCmdType;
			break;
		}
	}
	if(pDevHeader->CmdType&XBCmdAskResponse)	bSendBack = TRUE;
	return err;
}

enum XBErrors ProcessAPIPacket(void)
{
	BYTE i;
	BYTE n=1;
	enum XBErrors err = XBOK;
	BYTE *p = (BYTE *)pDevHeader;

//	LED_PORT = 1 << LED1;
	bSendBack = FALSE;

	//	read header after Signature byte (it'a already read)
	for(i=0;i<sizeof(XBEndPointDevHeader)-1;i++)
		p[n++] = UARTReadChar();
	bAddr =  bBuffer + sizeof(XBEndPointDevHeader);
	bExAddr = bAddr + pDevHeader->AddrSize;
	bData = bExAddr + pDevHeader->ExAddrSize;
	for(i=0;i<pDevHeader->AddrSize;i++)			p[n++] = UARTReadChar();
	for(i=0;i<pDevHeader->ExAddrSize;i++)		p[n++] = UARTReadChar();

	err = ProcessAPICmd();
	if(bSendBack==TRUE)
	{
		p = (BYTE *)pDevHeader;
		pDevHeader->Error = err;
		if(err!=XBOK)	pDevHeader->Size = 0;		//	don't send data back in a case of error
		pDevHeader->CRC8 = Crc8(&pDevHeader->Size,sizeof(XBEndPointDevHeader) + pDevHeader->AddrSize + pDevHeader->ExAddrSize + pDevHeader->Size - 2);
		for(i=0;i<sizeof(XBEndPointDevHeader);i++)	UARTWriteChar(*p++);
		for(i=0;i<pDevHeader->AddrSize;i++)			UARTWriteChar(*p++);
		for(i=0;i<pDevHeader->ExAddrSize;i++)		UARTWriteChar(*p++);
		for(i=0;i<pDevHeader->Size;i++)				UARTWriteChar(*p++);
	}
//	LED_PORT = 0 << LED1;
	return err;
}
/*
enum XBErrors ProcessAPIPacket(void)
{
	enum XBErrors err;
	BYTE size = 0;
	BYTE i = 0;
	BYTE n = 1;
	BYTE *p = (BYTE *)pDevHeader;

	//	read header after Signature byte (it'a already read)
	for(i=0;i<sizeof(XBEndPointDevHeader)-1;i++)
		p[n++] = UARTReadChar();

	bAddr =  bBuffer + sizeof(XBEndPointDevHeader);
	bExAddr = bAddr + pDevHeader->AddrSize;
	bData = bExAddr + pDevHeader->ExAddrSize;

	//	read address
	for(i=0;i<pDevHeader->AddrSize;i++)
		p[n++] = UARTReadChar();
	//	read extended address
	for(i=0;i<pDevHeader->ExAddrSize;i++)
		p[n++] = UARTReadChar();
	//	read data if needed
	size = GetRealDataSize();
	for(i=0;i<size;i++)
		p[n++] = UARTReadChar();

	if(Crc8(&pDevHeader->Size,n-2)==pDevHeader->CRC8)
	{
		LED_PORT = 1 << LED1;
		err = ProcessCmd();
		LED_PORT = 0 << LED1;
		if(IsResponse()==TRUE)
		{
			pDevHeader->Error = err;
			p = (BYTE *)pDevHeader;
			pDevHeader->CRC8 = Crc8(&pDevHeader->Size,sizeof(XBEndPointDevHeader)+pDevHeader->AddrSize+pDevHeader->ExAddrSize+pDevHeader->Size-2);
			for(i=0;i<sizeof(XBEndPointDevHeader);i++)
				UARTWriteChar(p[i]);
			for(i=0;i<pDevHeader->AddrSize;i++)
				UARTWriteChar(bAddr[i]);
			for(i=0;i<pDevHeader->ExAddrSize;i++)
				UARTWriteChar(bExAddr[i]);
			for(i=0;i<pDevHeader->Size;i++)
				UARTWriteChar(bData[i]);
		}
	}
	else err = XBErrorCrc;
	return err;
}
*/
int main(void)
{
	BYTE ch;
	enum XBErrors err = S_OK;

	pDevHeader = (XBEndPointDevHeader *)bBuffer;
	HW_Init();

	LED_DDR = 1;
	LED_PORT = 0 << LED1;
	while (1)
	{
		ch = UARTReadChar();
		if(ch==XBPacketAPI)
		{
			pDevHeader->Signature = ch;
			err = ProcessAPIPacket();
		}
		/*else if(ch==XBPacketCMD)
		{
			err = ProcessTXTPacket();
			if(err!=XBOK)
			{
				char s[4];
				sprintf(s,"%02X\r",err);
				UARTWriteString(s);
			}
		}
		*/
	}
	return 0;
}
