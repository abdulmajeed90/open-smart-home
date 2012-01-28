/*
 * dio.h
 *
 *  Created on: 24.01.2012
 *      Author: ёрий
 */

#ifndef DIO_H_
#define DIO_H_

#include "xb.h"

enum XBErrors ProcessDIO(XBEndPointDevHeader *pDevHeader, BYTE* pAddr, BYTE* pExAddr, BYTE *pData);

#endif /* DIO_H_ */
