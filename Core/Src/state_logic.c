/*
 * state_logic.c
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#include "state_logic.h"
#include "main.h"

// Estado del sistema
static state_t state = IDLE;

volatile uint8_t TIM3_LED_flag = 0;

void StateMachine_Run()
{
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim->Instance == TIM3)
	{
		TIM3_LED_flag = 1;
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
}



