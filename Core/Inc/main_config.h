/*
 * main_config.h
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#ifndef INC_MAIN_CONFIG_H_
#define INC_MAIN_CONFIG_H_

#define TH_MAX 0.1f
#define GRAV_ACC 9.81f
#define N_SAMPLES 200
#define N_RFID_PWD 6


#define GPIO_LED_BLUE_PORT GPIOD
#define GPIO_LED_BLUE_PIN GPIO_PIN_15

#define GPIO_LED_RED_PORT GPIOD
#define GPIO_LED_RED_PIN GPIO_PIN_14

#define GPIO_LED_GREEN_PORT GPIOD
#define GPIO_LED_GREEN_PIN GPIO_PIN_12

#define GPIO_USER_BUTTON_PORT GPIOA
#define GPIO_USER_BUTTON_PIN GPIO_PIN_0

#endif /* INC_MAIN_CONFIG_H_ */
