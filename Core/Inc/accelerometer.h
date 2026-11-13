/*
 * functions.h
 *
 *  Created on: 07 nov 2026
 *      Author: cesar
 */


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f411e_discovery_accelerometer.h"
#include "type_defs.h"
#include "main_config.h"


// Prototipos de funciones
acc_sample GetAcelData(int16_t *raw_data);
float CalcModuleDin(acc_sample sample, acc_sample grav);
void SaveWindow(acc_sample sample, acc_sample *window, uint16_t *index);
bool CheckMov(float a_din, float threshold);

#endif /* FUNCTIONS_H_ */
