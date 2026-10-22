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

static state_t state = IDLE;
static uint8_t g_sample_counter = 0;
static acc_sample acc [N_SAMPLES];
static uint32_t current_pwd [N_RFID_PWD];


error_t StateMachine_Init();

void StateMachine_Run();

error_t StateMachine_EnterState(state_t state);

error_t Process_Sample();



#endif /* INC_STATE_LOGIC_H_ */
