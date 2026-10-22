/*
 * type_defs.h
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#ifndef INC_TYPE_DEFS_H_
#define INC_TYPE_DEFS_H_

typedef enum {
	IDLE,
	ACTIVE,
	OPEN,
	ALARM
} state_t;

typedef enum
{
	TIMEOUT,
	ERROR_1,
	ERROR_2
}error_t;

typedef struct
{
	uint32_t x_axis;
	uint32_t y_axis;
	uint32_t z_axis;
}acc_sample;




#endif /* INC_TYPE_DEFS_H_ */
