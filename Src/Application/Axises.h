/*
 * Axises.h
 *
 *  Created on: 13. nov. 2012
 *      Author: eskelund
 */

#ifndef AXISES_H_
#define AXISES_H_

#include <stdint.h>
#include "BSP.h"
#include "RC_Core.h"

// This struct identifies the "public" functions
typedef struct AXISES_t
{
	void (*Init)(void);
	void (*Update)(void);
}AXISES_t;

void AXISES_Constructor(AXISES_t *Axises_Obj);

#endif /* AXISES_H_ */
