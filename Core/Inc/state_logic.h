/*
 * state_logic.h
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#ifndef INC_STATE_LOGIC_H_
#define INC_STATE_LOGIC_H_

#include "type_defs.h"
#include "main_config.h"
#include "mfrc522.h"
#include "main.h"

// DECLARACIÓN DE VARIABLES
// Ventana de muestreo, índice y flag para almacenar 200 primeras muestras como media para calcular
extern volatile uint16_t sample_idx;

extern volatile acc_sample window [N_SAMPLES];
extern volatile acc_sample grav_window [N_SAMPLES];
extern volatile acc_sample grav_sample;
extern volatile int16_t raw_acel[3];
extern volatile acc_sample new_sample;

extern volatile uint8_t Cal_OK_flag;

extern volatile uint32_t current_pwd [N_RFID_PWD];

extern volatile uint8_t TIM3_LED_flag;
extern volatile uint8_t TIM2_ACL_flag;
extern volatile uint8_t PA0_BTUSR_flag;

error_t StateMachine_Init();

void StateMachine_Run();

error_t StateMachine_EnterState(state_t state);

error_t Process_Sample();



#endif /* INC_STATE_LOGIC_H_ */
