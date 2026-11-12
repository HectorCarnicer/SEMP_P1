/*
 * write_flash_rfid.h
 *
 *  Created on: 11 nov 2026
 *      Author: hectorc
 */

#ifndef INC_WRITE_FLASH_RFID_H_
#define INC_WRITE_FLASH_RFID_H_

#include <RFID_state_logic.h>
#include <stm32f4xx_hal_def.h>
#include <stdint.h>

#ifndef UID_FLASH_ADDR
#define UID_FLASH_ADDR    0x08060000U  // Inicio del último sector de flash según datasheet
#endif
#ifndef UID_FLASH_SIZE
#define UID_FLASH_SIZE    (128U * 1024U)   // Tamaño del sector (=128KB)
#endif
#ifndef UID_SLOT_COUNT
#define UID_SLOT_COUNT    (UID_FLASH_SIZE / sizeof(UidSlot))
#endif

// ----------------------------- //
// Para escribir en flash las claves y no sobreescribirlas hay que escribir en la propia flash si el hueco está libre o no
// Lo hacemos poniendo SLOT_FREE (=0xFF) en el campo size
// Estas constantes son los códigos quue leer para saber dónde escribir
// ----------------------------- //


#ifndef SLOT_FREE
#define SLOT_FREE         0xFFU                 /* uid.size de un hueco sin usar (flash borrada) */
#endif
#ifndef SLOT_VALID
#define SLOT_VALID        0x55U                 /* status: clave válida                         */
#endif
#ifndef SLOT_DELETED
#define SLOT_DELETED      0x00U                 /* status: clave borrada                        */
#endif

// Realmente no vamos a leer la flash así que los flags del final sirven entre poco y nada
// No se pierde nada en ponerlos

// ----------------------------- //
// API: Única función para escribir en la flahs, se llama desde state_logic.c
// ----------------------------- //

HAL_StatusTypeDef UidStore_Add(const RFID_Uid *uid);


#endif /* INC_WRITE_FLASH_RFID_H_ */
