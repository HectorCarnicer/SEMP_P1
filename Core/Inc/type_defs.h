/*
 * type_defs.h
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#ifndef INC_TYPE_DEFS_H_
#define INC_TYPE_DEFS_H_

#include <stdint.h>

typedef enum {
	IDLE,
	ACTIVE,
	OPENCLOSE,
	ALARM
} state_t;

typedef enum
{
	TIMEOUT,
	ERROR_SPI,
	ERROR_TIMER
}error_t;



typedef struct
{
	int16_t x_axis;
	int16_t y_axis;
	int16_t z_axis;
}acc_sample;


#endif /* INC_TYPE_DEFS_H_ */
