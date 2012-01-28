/*
 * 1wire.h
 *
 *  Created on: 24.01.2012
 *      Author: ёрий
 */

#ifndef _1WIRE_H_
#define _1WIRE_H_

#include "xb.h"

enum XBErrors Process1W(BYTE dev_index, BYTE cmd_type, BYTE *params, BYTE *sizes);

#endif /* 1WIRE_H_ */
