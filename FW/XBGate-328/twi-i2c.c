/*
 * twi-i2c.c
 *
 *  Created on: 24.01.2012
 *      Author: Yuriy Gerasimov
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "twi-i2c.h"
#include "xb.h"

BYTE bI2CAddress = 0;		//	address of target I2C device

BYTE *pI2CBuffer = 0;		//	data to send
int iI2CBufferSize = 0;
int iI2CBufferIndex = 0;

BYTE *pAddrBuffer = 0;		//	addresses of control registers/pages
int iAddrBufferSize = 0;	//
int iAddrBufferIndex = 0;

enum I2CModes I2C_Mode = I2CModeNone;
enum I2CErrors I2C_Error = I2CErrorNone;

 void I2C_Init(void)
 {
 	I2C_DDR &=~(1<<I2C_SCL|1<<I2C_SDA);	//	Configure TWI pins
 	I2C_PORT |= 1<<I2C_SCL|1<<I2C_SDA;	// Set up TWI pins to 1

 	TWBR = 0x02;	// SCL freq = CPU clock * (16 + 2*TWBR)*Prescaler (bits 1..0 in TWSR)
 	TWSR = 0x00;	//	400 kHz for 8 MHz clock
 }

 void I2C_StartMaster(void)
 {
 	TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;
 }

 enum I2CErrors I2C_Write(BYTE bAddress, void *pSrcBuffer, int nSize)
 {
 	I2C_Error = I2CErrorNone;			//	clear Error flag
 	I2C_Mode = I2CModeMasterWrite;		//	set mode to write by master

 	pI2CBuffer = pSrcBuffer;			//	set source buffer pointer
 	iI2CBufferSize = nSize;				//	set bytes to send
 	iI2CBufferIndex = 0;

 	bI2CAddress = bAddress;				//	set reciever address

     I2C_StartMaster();

 //    while(I2C_Mode==I2CModeMasterWrite && I2C_Error==I2CErrorNone);	//	wait till error or complite
 	while(I2C_Mode!=I2CModeNone)
     	if(I2C_Error!=I2CErrorNone)	return I2C_Error;

    return I2C_Error;
 }

 enum I2CErrors I2C_Read(BYTE bAddress, void *pDestBuffer, int nSize)
 {
 	I2C_Error = I2CErrorNone;			//	clear Error flag
 	I2C_Mode = I2CModeMasterRead;		//	set mode to read by master

 	pI2CBuffer = pDestBuffer;			//	set source buffer pointer
 	iI2CBufferSize = nSize;				//	set bytes to send
 	iI2CBufferIndex = 0;

 	bI2CAddress = bAddress;				//	set reciever address

 	I2C_StartMaster();
 //    while(I2C_Mode!=I2CModeNone && I2C_Error==I2CErrorNone);	//	wait till error or complite
 	while(I2C_Mode!=I2CModeNone)
     	if(I2C_Error!=I2CErrorNone)	return I2C_Error;
     return I2C_Error;
 }

 enum I2CErrors I2C_WriteByAddress(BYTE i2c_Address, void *pAddr, int nAddrSize, void *pSrcBuffer, int nSize)
 {
 	I2C_Error = I2CErrorNone;			//	clear Error flag
 	I2C_Mode = I2CModeMasterAddrWrite;		//	set mode to write address by master

 	pAddrBuffer = pAddr;				//	set addresses of control registers/pages
 	iAddrBufferSize = nAddrSize;
 	iAddrBufferIndex = 0;

 	pI2CBuffer = pSrcBuffer;			//	set source buffer pointer
 	iI2CBufferSize = nSize;				//	set bytes to send
 	iI2CBufferIndex = 0;

 	bI2CAddress = i2c_Address;				//	set reciever address

 	I2C_StartMaster();

  //   while(I2C_Mode!=I2CModeNone && I2C_Error==I2CErrorNone);	//	wait till error or complite
 	while(I2C_Mode!=I2CModeNone)
     	if(I2C_Error!=I2CErrorNone)		break;

     return I2C_Error;
 }

 enum I2CErrors I2C_ReadByAddress(BYTE i2c_Address, void *pAddr, int nAddrSize, void *pDestBuffer, int nSize)
 {
 	I2C_Error = I2CErrorNone;			//	clear Error flag
 	I2C_Mode = I2CModeMasterAddrRead;		//	set mode to write address by master

 	pAddrBuffer = pAddr;				//	set addresses of control registers/pages
 	iAddrBufferSize = nAddrSize;
 	iAddrBufferIndex = 0;

 	pI2CBuffer = pDestBuffer;			//	set source buffer pointer
 	iI2CBufferSize = nSize;				//	set bytes to send
 	iI2CBufferIndex = 0;

 	bI2CAddress = i2c_Address;				//	set reciever address

 	I2C_StartMaster();
 //	while(I2C_Mode!=I2CModeNone && I2C_Error==I2CErrorNone);	//	wait till error or complite
 	while(I2C_Mode!=I2CModeNone)
     	if(I2C_Error!=I2CErrorNone)	return I2C_Error;
     return I2C_Error;
 }

 enum XBErrors ProcessI2CCmd(XBEndPointDevHeader *pDevHeader, BYTE* pAddr, BYTE* pExAddr, BYTE *pData)
{
	 enum XBErrors err = XBErrorCmdType;
	 switch(pDevHeader->CmdType)
	 {
	 	case XBCmdUnknown:		break;
	 	case XBCmdInit:			break;
	 	case XBCmdReset:		break;
	 	case XBCmdWrite:
	 		err = I2C_Write(pAddr[0],pData,pDevHeader->Size);
	 		break;
	 	case XBCmdWriteAddr:
	 		err = I2C_WriteByAddress(pAddr[0],pExAddr,pDevHeader->ExAddrSize,pData,pDevHeader->Size);
	 		break;
	 	case XBCmdRead:
	 		err = I2C_Read(pAddr[0],pData,pDevHeader->Size);
	 		break;
	 	case XBCmdReadAddr:
	 		err = I2C_ReadByAddress(pAddr[0],pExAddr,pDevHeader->ExAddrSize,pData,pDevHeader->Size);
	 		break;
	 	case XBCmdInterrupt:
	 	case XBCmdError:
	 	case XBCmdWriteDelay:
	 	case XBCmdReadDelay:
	 		err = XBErrorCmdType;
	 		break;
	 	default:
	 		err = XBErrorCmdType;
	 		break;
	 }
	 if(pDevHeader->CmdType!=XBCmdRead || pDevHeader->CmdType!=XBCmdReadAddr)	pDevHeader->Size = 0;
	return err;
}

 //-------------------	TWI Interrupt handler ---------------------------------
 //interrupt [TWI] void twi_isr(void)
 ISR(TWI_vect)
 {
 	switch(TWSR & 0xF8)	//	clear TWI prescaler bits
 	{
 		case 0x00:	//	bus error due to illegal START or STOP condition
 		{
 			I2C_Error = I2CErrorBus;
 			break;
 		}
 		case 0x08:	// START condition has been transmitted
 		{
         	switch(I2C_Mode)
 			{
 				case I2CModeMasterWrite:
 				case I2CModeMasterAddrWrite:
 				case I2CModeMasterAddrRead:			//	start addressed reading from writing address
 					bI2CAddress&=0xFE;	break;
 				case I2CModeMasterRead:
                 case I2CModeMasterAddrWR:
 					bI2CAddress|=0x01;	break;
 				default:
 				{
 					I2C_Error = I2CErrorMode;
 					return;
 				}
             }

             TWDR = bI2CAddress;
  			TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE;
 			break;
 		}
 		case 0x10:	// repeated START condition has been transmitted
 		{
 	       	switch(I2C_Mode)
 			{
 				case I2CModeMasterWrite:
 				case I2CModeMasterAddrWrite:
 					bI2CAddress&=0xFE;	break;
 				case I2CModeMasterAddrRead:			//	start addressed reading from writing address
 				case I2CModeMasterRead:
                 case I2CModeMasterAddrWR:
 					bI2CAddress|=0x01;	break;
 				default:
 					{
 						I2C_Error = I2CErrorMode;
 						return;
 					}
 			}
 			TWDR = bI2CAddress;
 			TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE;
 			break;
 		}
 		case 0x18:		//	SLA+W has been transmitted,	ACK has been received, start to send data
 		{
             switch(I2C_Mode)
 			{
 				case I2CModeMasterWrite:
 					TWDR = pI2CBuffer[iI2CBufferIndex++];
 					break;
 				case I2CModeMasterAddrWrite:
 				case I2CModeMasterAddrRead:
 					TWDR = pAddrBuffer[iAddrBufferIndex++];
 					break;
 				default:
 					{
 						I2C_Error = I2CErrorMode;     //	should never be here
                         TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE;	//	send STOP
 						return;
                 	}
 			}
             TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // Go!
 			break;

 		}
 		case 0x20:		//	 SLA+W has been transmitted, NOT ACK has been received - slave is busy or there is no slave on a bus
 		{
 			I2C_Error = I2CErrorSlaveAddrNack;
 			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // send STOP to TWI
 			break;
 		}
 		case 0x28:		//	 Data byte has been transmitted, ACK has been receive
 		{
 			switch(I2C_Mode)
 			{
 				case I2CModeMasterWrite:
 				{
 					if(iI2CBufferIndex==iI2CBufferSize)		//check for data buffer is empty
 					{
                     	I2C_Mode=I2CModeNone;	//	for returning from a loop
 						TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // send STOP
 					}
 					else
 					{
 						TWDR = pI2CBuffer[iI2CBufferIndex++];
 						TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // Go!
 					}
 					break;
 				}
 				case I2CModeMasterAddrWrite:				//	for addr + read data mode
 				{
 					if(iAddrBufferIndex==iAddrBufferSize)	//check for address buffer is empty
 					{
 						I2C_Mode = I2CModeMasterWrite;		//	switch mode to sending pure data
 						if(iI2CBufferIndex==iI2CBufferSize)	//	check for data buffer empty
 						{
 							TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // send STOP
 							I2C_Mode=I2CModeNone;			//	for returning from a loop
 						}
 						else
 						{
 							TWDR = pI2CBuffer[iI2CBufferIndex++];
 							TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // Go!
 						}
 					}
 					else
 					{
 						TWDR = pAddrBuffer[iAddrBufferIndex++];
 						TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // Go!
 					}
 					break;
 				}
 				case I2CModeMasterAddrRead:					//	for addr + read data mode
 				{
 					if(iAddrBufferIndex==iAddrBufferSize)	//check for address buffer is empty
 					{
                         I2C_Mode = I2CModeMasterAddrWR;
                         //I2C_Mode = I2CModeMasterAddrRead;
 						TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // send START
 					}
 					else
 					{
 						TWDR = pAddrBuffer[iAddrBufferIndex++];
 						TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // Go!
 					}
 					break;
 				}
 				default:
 				{
 					I2C_Error = I2CErrorMode;               								//	should never be here
                     TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // send STOP
 					break;
 				}
 			}
 			break;
 		}

 		case 0x30:	//	 Data byte has been transmitted, NOT ACK has been received
 		{
 			I2C_Error = I2CErrorSlaveWriteNack;
 			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // STOP
 			break;
 		}

         case 0x38:	//	 Arbitration lost in SLA+W or data bytes
 		{
 			iI2CBufferIndex = 0;			//	start again
 			iAddrBufferIndex = 0;
 	//		I2C_Error = I2CErrorPriority;		//	start again or return error ???
 			TWCR = 1<<TWSTA|0<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // try again after bus releasing
 			break;
 		}

 		case 0x40: //  SLA+R has been transmitted, ACK has been received
 		{
         		if(iI2CBufferIndex+1==iI2CBufferSize)	//	check for last byte will be received
 				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;	// GO + NACK
 			else
 				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;	// GO + ACK
 			break;
 		}

 		case 0x48:  //	SLA+R has been transmitted, NOT ACK has been received
 		{
 			I2C_Error = I2CErrorSlaveAddrNack;
 			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE; // STOP
 			break;
 		}

 		case 0x50: //  Data byte has been received, ACK has been returned
 		{
 			pI2CBuffer[iI2CBufferIndex++] = TWDR; // get data byte

 			if (iI2CBufferIndex+1 == iI2CBufferSize) // Если остался еще один байт из тех, что мы хотели считать
 				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|0<<TWEA|1<<TWEN|1<<TWIE;		// GO + NACK
 			else
 				TWCR = 0<<TWSTA|0<<TWSTO|1<<TWINT|1<<TWEA|1<<TWEN|1<<TWIE;		// GO + ACK
 			break;
 		}

 		case 0x58:  //	Data byte has been received, NOT ACK has been returned
 		{
 			pI2CBuffer[iI2CBufferIndex] = TWDR;									// get last data byte

 			TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE;	// STOP
 			I2C_Mode = I2CModeNone;
 			break;
 		}
 		default:
         {
         	TWCR = 0<<TWSTA|1<<TWSTO|1<<TWINT|I2C_IS_SLAVE<<TWEA|1<<TWEN|1<<TWIE;	// STOP
         	I2C_Mode = I2CModeNone;
 			I2C_Error = I2CErrorUnknown;
 			break;
         }
 	}
 }

