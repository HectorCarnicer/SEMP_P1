/*
 * error_report.h
 *
 *  Created on: 12 nov 2026
 *      Author: hectorc
 */

#ifndef INC_ERROR_REPORT_H_
#define INC_ERROR_REPORT_H_

#include "main.h"
#include "main_config.h"

// ERROR_REPORT(...): enciende el LED de error y, sólo en Debug, hace printf()
// con los mismos argumentos. Se usa igual que printf():
// ERROR_REPORT("<Función donde ocurre el fallo>: fallo (HAL status %d)\n", <error de HAL>);
// Magia con macros
#ifdef DEBUG
#include <stdio.h>
#define ERROR_REPORT(...)                                                      \
    do {                                                                       \
        HAL_GPIO_WritePin(GPIO_LED_ERR_PORT, GPIO_LED_ERR_PIN, GPIO_PIN_SET);  \
        printf(__VA_ARGS__);                                                   \
    } while (0)
#define DEBUG_REPORT(...)  printf(__VA_ARGS__);
#else
#define ERROR_REPORT(...)                                                      \
    HAL_GPIO_WritePin(GPIO_LED_ERR_PORT, GPIO_LED_ERR_PIN, GPIO_PIN_SET)
#endif



#endif /* INC_ERROR_REPORT_H_ */
