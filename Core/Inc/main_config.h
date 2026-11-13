    /*
 * main_config.h
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#ifndef INC_MAIN_CONFIG_H_
#define INC_MAIN_CONFIG_H_

#define TH_MAX 10000.0f
#define GRAV_ACC 9.81f
#define N_SAMPLES 200
#define N_RFID_PWD 6

// LEDS
#define GPIO_LED_GREEN_PORT     GPIOD      // LD4: cerradura abierta
#define GPIO_LED_GREEN_PIN      GPIO_PIN_12

#define GPIO_LED_ERR_PORT       GPIOD      // LD3 naranja: error
#define GPIO_LED_ERR_PIN        GPIO_PIN_13

#define GPIO_LED_RED_PORT       GPIOD      // LD5: alarma
#define GPIO_LED_RED_PIN        GPIO_PIN_14

#define GPIO_LED_BLUE_PORT      GPIOD      // LD6: latido (TIM3)
#define GPIO_LED_BLUE_PIN       GPIO_PIN_15

// Botón de usuario
#define GPIO_USER_BUTTON_PORT GPIOA
#define GPIO_USER_BUTTON_PIN GPIO_PIN_0


#define GPIO_SPI_CS_PORT GPIOC
#define GPIO_SPI_CS_PIN GPIO_PIN_4

#define GPIO_SPI_RST_PORT GPIOC
#define GPIO_SPI_RST_PIN GPIO_PIN_5

// Según datasheet
#define UID_FLASH_ADDR 0x08060000U

// Pines del SPI para el RFID -> Solo se usa la def. de CS y RST, resto CubeMX
#define GPIO_RFID_SCK_PORT      GPIOA
#define GPIO_RFID_SCK_PIN       GPIO_PIN_5

#define GPIO_RFID_MISO_PORT     GPIOA
#define GPIO_RFID_MISO_PIN      GPIO_PIN_6

#define GPIO_RFID_MOSI_PORT     GPIOA
#define GPIO_RFID_MOSI_PIN      GPIO_PIN_7

#define GPIO_RFID_CS_PORT       GPIOC
#define GPIO_RFID_CS_PIN        GPIO_PIN_4

#define GPIO_RFID_RST_PORT      GPIOC
#define GPIO_RFID_RST_PIN       GPIO_PIN_5

#define GPIO_SPI_CS_PORT 		GPIOC // Conectar al SDA
#define GPIO_SPI_CS_PIN 		GPIO_PIN_4

#define GPIO_SPI_RST_PORT 		GPIOC
#define GPIO_SPI_RST_PIN 		GPIO_PIN_5

// Giroscopio
#define GPIO_GYRO_CS_PORT       GPIOE
#define GPIO_GYRO_CS_PIN        GPIO_PIN_3




#endif /* INC_MAIN_CONFIG_H_ */
