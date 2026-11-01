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


extern volatile uint8_t g_sample_counter;
extern volatile acc_sample acc [N_SAMPLES];
extern volatile uint32_t current_pwd [N_RFID_PWD];

extern volatile uint8_t TIM3_LED_flag;


error_t StateMachine_Init();

void StateMachine_Run();

error_t StateMachine_EnterState(state_t state);

error_t Process_Sample();



#endif /* INC_STATE_LOGIC_H_ */
