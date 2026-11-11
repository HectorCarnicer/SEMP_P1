/*
 * write_flash_rfid.c
 *
 *  Created on: 11 nov 2026
 *      Author: hectorc
 */

#include "write_flash_rfid.h"

typedef struct {
	RFID_Uid uid;
	uint8_t  reserved[4]; // sin usar para qe al final den 16B
	uint8_t  status;  	// Flag por si hay escritura corrompida/borrada
} UidSlot; // 11 byttes en total

// Variable con el address actual sobre el que escribir
static const UidSlot *const slots = (const UidSlot *)UID_FLASH_ADDR;


// Función interna privada sencilla para programar un Uid en una dirección de flash
static HAL_StatusTypeDef ProgramUid(const UidSlot *slot, const RFID_Uid *uid)
{
	HAL_StatusTypeDef st;
	st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
			(uint32_t)&slot->uid.size, uid->size);
	if (st != HAL_OK) {
		return st;
	}

	for (uint8_t b = 0; b < uid->size; b++) {
		st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
				(uint32_t)&slot->uid.bytes[b], uid->bytes[b]);
		if (st != HAL_OK) {
			return st;
		}
	}

	return HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
			(uint32_t)&slot->status, SLOT_VALID);
}

// Función que programa el Uid en el primer hueco disponible desde 0x08060000 hasta el final
HAL_StatusTypeDef UidStore_Add(const RFID_Uid *uid)
{
    if (uid->size != 4 && uid->size != 7 && uid->size != 10) {
        return HAL_ERROR;
    }

    // Primer hueco libre. Se escriben en orden, así que los siguientes también lo están.
    uint32_t i = 0;
    while (i < UID_SLOT_COUNT && slots[i].uid.size != SLOT_FREE) {
        i++;
    }
    if (i == UID_SLOT_COUNT) {
        return HAL_ERROR;// lleno
    }

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_StatusTypeDef st = ProgramUid(&slots[i], uid);

    HAL_FLASH_Lock();
    return st;
}
