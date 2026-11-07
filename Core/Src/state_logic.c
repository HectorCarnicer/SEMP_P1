/*
 * state_logic.c
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#include "state_logic.h"
#include "main.h"

// INCIALIZACIÓN DE VARIABLES
// Estado
static state_t state = IDLE;

// -------------------
// Acelerómetro
// -------------------
// Muestra del vector window x,y,z
volatile uint8_t sample_idx = 0;
// Vector de componentes de aceleración
volatile acc_sample window [N_SAMPLES] = {0};
// Nueva muestra
volatile acc_sample nueva_muestra = {0};
// Tomo 200 primeras muestras para tener la media con la que comparar si la cerradura se mueve o no en las siguientes muestras
volatile uint8_t Cal_OK_flag = 0;
// Flag de interrupción de timer para tomar muetras de acelerómetro
volatile uint8_t TIM2_ACL_flag = 0;

// RFID
volatile uint32_t current_pwd [N_RFID_PWD] = {0};

// LED Azul y botón usuario
volatile uint8_t TIM3_LED_flag = 0;
volatile uint8_t PA0_BTUSR_flag = 0;


void StateMachine_Run()
{
	// LED Azul
	if (TIM3_LED_flag == 1)
	{
		TIM3_LED_flag = 0;
		HAL_GPIO_TogglePin(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
	}

	// Máquina de estados
	switch (state) {
		// Estado incial espera de pulsación botón usuario PA0, pasa a active
		case IDLE:
			if (PA0_BTUSR_flag == 1){
				PA0_BTUSR_flag = 0;
				state = ACTIVE;
			}
		break;

		// Muestreo de acelerómetro y etiqueta válida
		case ACTIVE:

		break;

		// Si a,dim>TH_MAX, LED ROJO ON y espera botón usuario PA0
		case ALARM:

		break;

		// Si etiqueta valida, escribe clave en FLASH y LED VERDE ON 3s, después ACTIVE
		case OPENCLOSE:

		break;

		default:

		break;
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	// LED azul
	if (htim->Instance == TIM3)
	{
		TIM3_LED_flag = 1;
	}

	// Acelerómetro
	if (htim->Instance == TIM2)
	{

	}
}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{

}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{


}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{

}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_USER_BUTTON_PIN){
		PA0_BTUSR_flag = 1;
	}

}



