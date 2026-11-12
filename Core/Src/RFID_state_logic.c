/*
 * RFID_state_logic.c
 *
 *  Created on: 7 nov 2026
 *      Author: hectorc
 */



#include "RFID_state_logic.h"
#include "mfrc522.h"
#include <string.h>

#define MFRC522_REG_VERSION     0x37
#define MFRC522_REG_T_RELOAD_L  0x2D
#define RFID_SPI_TIMEOUT_MS     10U

static const RFID_Config  *cfg = NULL;
static MFRC522Driver       drv;
static const MFRC522Config drv_cfg;     /* vacía en la librería */

typedef enum {
    ST_RESET,
    ST_STARTUP,
    ST_READY,
    ST_ERROR
} State;

// ------------------------------------------------ //
// Estructurado como una FSM paralela a la global
// que se llama desde la global
// ------------------------------------------------ //


static State     state     = ST_RESET;	// Estado de la FSM
static uint32_t  t_ref     = 0;         // Base de tiempos obtenida de HAL_GetTick
static uint32_t  last_call = 0;         // Tiempo en ms cuando se llamó por última vez a StateMachine_Run
static bool      present   = false;     // había tarjeta en la última lectura
static uint8_t   misses    = 0;         // lecturas fallidas seguidas con tarjeta
static RFID_Uid  last_uid;              // UID de la tarjeta actual



// ------------------------------------------------------------------------ //
// LECTURA/ESCRITURA
// La librería que pone en el enunciado requiere definir estas funciones
// de lectura/escritura. Usamos SPI para la comm.
// ------------------------------------------------------------------------ //

// Mandamos dos bytes (MOSI) cada vez y leemos (MISO) simutáneamente:
// - En WriteRrgister: 	tx[0] -> addr0, 		tx[1] -> lo que sea, se escribe en addr0
//
// - En ReadRegister: 	tx[0] -> addr0, 		tx[1] -> 0x00, lo pone el datasheet
//						rx[0] -> basura, 		rx[1] -> data[addr0]
//

// Byte de dirección SPI del MFRC522 (datasheet, "SPI address byte"):
//
//     bit:   7      6   5   4   3   2   1      0
//           R/W  |   dirección (6 bits)   |   0



void MFRC522WriteRegister(MFRC522Driver *mfrc522p, uint8_t addr, uint8_t val)
{
    (void)mfrc522p;

    const uint8_t addr_mask = (uint8_t) ~((1U << 7) + 1U);	// = 0x7E = 1000 0001

    uint8_t tx[2] = { (uint8_t)((addr << 1) & addr_mask), val }; /* bit7 = 0: escritura */

    HAL_GPIO_WritePin(cfg->cs_port, cfg->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(cfg->hspi, tx, 2, RFID_SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(cfg->cs_port, cfg->cs_pin, GPIO_PIN_SET);
}

uint8_t MFRC522ReadRegister(MFRC522Driver *mfrc522p, uint8_t addr)
{
    (void)mfrc522p;

    const uint8_t addr_mask = (uint8_t) ~((1U << 7) + 1U); 	// = 0x7E = 1000 0001
    const uint8_t read_bit  = (uint8_t)(1U << 7);			// = 0x80 = 1000 0000


    uint8_t tx[2] = { (uint8_t)(((addr << 1) & addr_mask) | read_bit), 0x00 }; /* bit7 = 1: lectura */
    uint8_t rx[2] = { 0 };

    HAL_GPIO_WritePin(cfg->cs_port, cfg->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(cfg->hspi, tx, rx, 2, RFID_SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(cfg->cs_port, cfg->cs_pin, GPIO_PIN_SET);
    return rx[1];
}

// ------------------------------------ //
// Funciones internas
// ------------------------------------ //
static void ForgetCard(void)
{
    present       = false;
    misses        = 0;
    last_uid.size = 0;
}

// Una muestra de lectura, lee las variables estáticas para comparar la actual con las anteriores
// Devuelve un evento que se devuelve a la función global
static RFID_Event Poll(RFID_Uid *uid)
{
    struct MifareUID id;
    memset(&id, 0, sizeof id);

    if (MifareCheck(&drv, &id) == MIFARE_OK && id.size > 0 && id.size <= sizeof id.bytes) {
    	// Lectura correcta
        misses = 0;

        // Misma tarjeta
        bool same_card = 	present &&
                    		id.size == last_uid.size &&
							memcmp(id.bytes, last_uid.bytes, id.size) == 0;
        if (same_card)
        {
            return RFID_EV_NONE;
        }

        // Tarjeta nueva
        last_uid.size = id.size;
        memcpy(last_uid.bytes, id.bytes, id.size);
        present = true;
        if (uid) *uid = last_uid;
        return RFID_EV_CARD_ARRIVED;
    }

    // Sin tarjeta o lectura fallida: por si acaso se chequea RFID_REMOVE_MISSES veces antes
    // de determnar
    if (!present)
    {
        return RFID_EV_NONE;
    }
    misses++;
    if (misses < RFID_REMOVE_MISSES)
    {
        return RFID_EV_NONE;
    }

    // No hay tarjeta 100%
    if (uid) *uid = last_uid; //antes de ForgetCard(), que la borra
    ForgetCard();
    return RFID_EV_CARD_REMOVED;
}

// Función estándar de transición de estado -> Máuina de Mealy y esas cosas
static void RFID_EnterState(State next, uint32_t now)
{
    switch (next)
    {
    case ST_RESET:
        HAL_GPIO_WritePin(cfg->rst_port, cfg->rst_pin, GPIO_PIN_RESET);
        break;

    case ST_STARTUP:
        HAL_GPIO_WritePin(cfg->rst_port, cfg->rst_pin, GPIO_PIN_SET);
        break;

    case ST_READY:
        MFRC522ObjectInit(&drv);
        MFRC522Start(&drv, &drv_cfg);
        MFRC522WriteRegister(&drv, MFRC522_REG_T_RELOAD_L, RFID_TIMER_RELOAD); // Bajar el tiempo bloqueado
        ForgetCard();
        last_call = now;
        break;

    case ST_ERROR:
    default:
        break;
    }

    t_ref = now;
    state = next;
}


// ------------------------------------ //
// API para usar en state_logic.c
// ------------------------------------ //


void RFID_StateMachine_Init(const RFID_Config *config)
{
    cfg       = config;
    RFID_EnterState(ST_RESET, cfg->get_ms());
    last_call = cfg->get_ms();
    ForgetCard();
    HAL_GPIO_WritePin(cfg->cs_port, cfg->cs_pin, GPIO_PIN_SET);
}

RFID_Event RFID_StateMachine_Run(RFID_Uid *uid)
{
    if (cfg == NULL) {
        return RFID_EV_NONE;                        /* RFID_Init() no llamado */
    }

    uint32_t   now = cfg->get_ms();
    RFID_Event ev  = RFID_EV_NONE;

    switch (state)
    {
    case ST_RESET:
        if (now - t_ref >= RFID_RST_LOW_MS) {
            RFID_EnterState(ST_STARTUP, now);
        }
        break;

    case ST_STARTUP:
        if (now - t_ref >= RFID_STARTUP_MS) {
            // Si la versión son t0do unos o ceros hay error
            uint8_t version = MFRC522ReadRegister(&drv, MFRC522_REG_VERSION);
            if (version == 0x00 || version == 0xFF) {
                RFID_EnterState(ST_ERROR, now);
                ev = RFID_EV_ERROR;
            } else {
                RFID_EnterState(ST_READY, now);
            }
        }
        break;

    case ST_READY:

    	// ------------------------------------ //
        // Reanudación: si hace tiempo que nadie llamaba (la FSM global estaba en otro
    	// estado), se olvida la tarjeta anterior y se lee en esta misma llamada.
    	// Así una tarjeta que sigue en el lector vuelve a generar CARD_ARRIVED y
    	// nunca se emite un REMOVED de algo que ocurrió mientras no se miraba.
    	// ------------------------------------ //

        bool resumed = (now - last_call) > RFID_RESUME_GAP_MS;
        last_call = now;

        if (resumed)
        {
            ForgetCard();
        }

        // Lectura normal cada RFID_POLL_MS
        if (now - t_ref >= RFID_POLL_MS)
        {
            t_ref = now;
            ev = Poll(uid);
        }
        break;

    case ST_ERROR:
        if (now - t_ref >= RFID_RETRY_MS)
        {
            RFID_EnterState(ST_RESET, now);
        }
        break;

    default:
        RFID_EnterState(ST_RESET, now);
        break;
    }

    return ev;
}

bool RFID_IsReady(void)
{
    return state == ST_READY;
}

bool RFID_CardPresent(void)
{
    return present;
}
