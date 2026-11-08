/*
 * RFID_state_logic.h
 *
 *  Created on: 7 nov 2026
 *      Author: hectorc
 */

#ifndef INC_RFID_STATE_LOGIC_H_
#define INC_RFID_STATE_LOGIC_H_


#include "main_config.h"
#include "type_defs.h"
#include "stm32f4xx_it.h"
#include "type_defs.h"
#include "main.h"


typedef enum
{
	RFID_RESET = 0,
	RFID_STARTUP = 1,
	RFID_NO_CARD = 2,
	RFID_CARD = 3,
	ERROR = 4,
}RFID_state_t;



// Acceder al estado de la FSM de RFID desde la FSM global
extern volatile RFID_state_t state;

void RFID_FSM_Start();
void RFID_FSM_Run();
void RFID_FSM_EnterMode();




#endif /* INC_RFID_STATE_LOGIC_H_ */
