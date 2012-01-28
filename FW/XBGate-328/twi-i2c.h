/*
 * twi-i2c.h
 *
 *  Created on: 24.01.2012
 *      Author: ����
 */

#ifndef TWI_I2C_H_
#define TWI_I2C_H_

#include "xb.h"

#define I2C_PORT PORTC	// TWI is on PORTC
#define I2C_DDR DDRC
#define I2C_SCL 5	// SCL is on PORTC.5
#define I2C_SDA 4	// SDA - PORTC.4
#define I2C_IS_SLAVE 0		//	set to 1 if we have slave mode

enum I2CModes
{
	I2CModeNone = 0,
	I2CModeMasterWrite,
	I2CModeMasterRead,
	I2CModeMasterAddrWrite,
	I2CModeMasterAddrRead,
    I2CModeMasterAddrWR,
	I2CModeUnknown
};

enum  I2CErrors
{
	I2CErrorNone = 0,
	I2CErrorMode,				//	wrong mode specified
	I2CErrorBus,				//	I2C bus if busy
	I2CErrorPriority,			//	lost of bus arbitration
	I2CErrorSlaveAddrNack,		//	NACK after sending ADDR - nobody home ???
	I2CErrorSlaveWriteNack,		//	NACK after writing a byte to slave - error or transmission interrupted
	I2CErrorUnknown
};


void I2C_Init(void);
enum XBErrors ProcessI2CCmd(XBEndPointDevHeader *pDevHeader, BYTE* pAddr, BYTE* pExAddr, BYTE *pData);

void I2C_StartMaster(void);
BYTE I2C_Read(BYTE bAddress, void *pDestBuffer, int nSize);
BYTE I2C_Write(BYTE bAddress, void *pSrcBuffer, int nSize);
BYTE I2C_WriteByAddress(BYTE i2c_Address, void *pAddr, int nAddrSize, void *pSrcBuffer, int nSize);
BYTE I2C_ReadByAddress(BYTE i2c_Address, void *pAddr, int nAddrSize, void *pDestBuffer, int nSize);
//interrupt [TWI] void twi_isr(void);

extern BYTE I2C_Mode;
extern BYTE I2C_Error;

#endif /* TWI_I2C_H_ */
