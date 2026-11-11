/*
 * RFID_state_logic.h
 *
 *  Created on: 7 nov 2026
 *      Author: hectorc
 */

#ifndef INC_RFID_STATE_LOGIC_H_
#define INC_RFID_STATE_LOGIC_H_

// Usar la librería que nos dan
#define HAL_USE_MFRC522 1


#include "main_config.h"
#include "type_defs.h"
#include "stm32f4xx_it.h"
#include "type_defs.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "mfrc522.h"
#include <stdint.h>
#include <stdbool.h>



// ------------------------------------ //
// Parámetros ajustables, por si no se han definido en el main_config.h global
// ------------------------------------ //

#ifndef RFID_POLL_MS
#define RFID_POLL_MS            100U
#endif
#ifndef RFID_RST_LOW_MS
#define RFID_RST_LOW_MS         2U      /* duración del pulso de reset                */
#endif
#ifndef RFID_STARTUP_MS
#define RFID_STARTUP_MS         50U     /* arranque del oscilador del MFRC522         */
#endif
#ifndef RFID_RETRY_MS
#define RFID_RETRY_MS           1000U   /* reintento de inicialización tras un error  */
#endif
#ifndef RFID_REMOVE_MISSES
#define RFID_REMOVE_MISSES      2U      /* lecturas fallidas seguidas para "retirada" */
#endif
#ifndef RFID_RESUME_GAP_MS
#define RFID_RESUME_GAP_MS      (3U * RFID_POLL_MS)
#endif









/* Opcional: valor de TReloadRegL tras MFRC522Start() para acortar el timeout de
   la tarjeta (cada unidad = 0,5 ms con la configuración de la librería; ésta
   pone 30 = 15 ms). Si no se define, se deja el valor de la librería.         */
/* #define RFID_TIMER_RELOAD    6U */

// ------------------------------------ //
// Tipos específicos del RFID
// ------------------------------------ //

typedef enum {
    ST_RESET,
    ST_RESET_LOW,
    ST_STARTUP,
    ST_READY,
    ST_ERROR
} State;

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef      *cs_port;
    uint16_t           cs_pin;
    GPIO_TypeDef      *rst_port;
    uint16_t           rst_pin;
    // Puntero a función que devuelve los ms actuales. Lo suyo es pasarle HAL_GetTick() o alguna movida
    // Es para no usar HAL_Delay()s bloqueantes
    uint32_t         (*get_ms)(void);
} RFID_Config;

typedef struct {
    uint8_t size;                       /* 4, 7 o 10 bytes */
    uint8_t bytes[10];
} RFID_Uid;

typedef enum {
    RFID_EV_NONE = 0,       /* nada nuevo en esta llamada (inicializando, esperando
                               el periodo, o lectura sin cambios)                    */
    RFID_EV_CARD_ARRIVED,   /* tarjeta nueva (o distinta a la anterior); uid relleno */
    RFID_EV_CARD_REMOVED,   /* la tarjeta se ha retirado; uid = la que se fue        */
    RFID_EV_ERROR           /* el lector no responde por SPI (se reintenta solo)     */
} RFID_Event;

// ----------------------------------------- //
// Funciones para usar en state_logic global.
// ----------------------------------------- //

void        RFID_StateMachine_Init(const RFID_Config *cfg);
RFID_Event  RFID_StateMachine_Run(RFID_Uid *uid);
bool        RFID_IsReady(void);
bool        RFID_CardPresent(void);


#endif /* INC_RFID_STATE_LOGIC_H_ */
