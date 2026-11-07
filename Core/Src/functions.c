/*
 * functions.c
 *
 *  Created on: 07 nov 2026
 *      Author: cesar
 */

#include "functions.h"


acc_sample GetAcelData(int16_t *raw_data){

	acc_sample dato = {0};

	// Cojo los valores del acelerómetro
	BSP_ACCELERO_GetXYZ(raw_data);

	// Los asigno a mi struct y los devuelvo
	dato.x_axis = raw_data[0];
	dato.y_axis = raw_data[1];
	dato.z_axis = raw_data[2];

	return dato;
}

float CalcModuleDin(acc_sample sample, acc_sample grav){
	float ax_din = (float)(sample.x_axis - grav.x_axis);
    float ay_din = (float)(sample.y_axis - grav.y_axis);
    float az_din = (float)(sample.z_axis - grav.z_axis);

    return sqrtf(ax_din * ax_din + ay_din * ay_din + az_din * az_din);
}

void SaveWindow(acc_sample sample, acc_sample *window, uint16_t *index){
	window[*index] = sample;
	*index = (*index + 1) % N_SAMPLES;
}

bool CheckMov(float a_din, float threshold){
	return (a_din > threshold);
}



