/*
 * state_logic.c
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#include "state_logic.h"
#include "main.h"
#include "main_config.h"
#include "functions.h"
#include "stm32f411e_discovery_accelerometer.h"


// INCIALIZACIÓN DE VARIABLES
// -------------------
// Máquina de estados
// -------------------
static state_t state = ACTIVE;

// -------------------
// Acelerómetro
// -------------------
// Muestra del vector window x,y,z
volatile uint16_t sample_idx = 0;
// Estructura de componentes de aceleración
volatile acc_sample window [N_SAMPLES] = {0};
//volatile acc_sample grav_window [N_SAMPLES] = {0};
volatile acc_sample grav_sample = {0};
volatile acc_sample new_sample = {0};
// Vectores de apoyo
volatile int16_t raw_acel[3] = {0};
static float sum_x = 0;
static float sum_y = 0;
static float sum_z = 0;
// Tomo 200 primeras muestras para tener la media con la que comparar si la cerradura se mueve o no en las siguientes muestras
volatile uint8_t Cal_OK_flag = 0;
// Flag de interrupción de timer para tomar muetras de acelerómetro
volatile uint8_t TIM2_ACL_flag = 0;

// -------------------
// RFID
// -------------------
volatile uint32_t current_pwd [N_RFID_PWD] = {0};

// -------------------
// LED Azul y botón usuario
// -------------------
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
		/*case IDLE:
			if (PA0_BTUSR_flag == 1){

				PA0_BTUSR_flag = 0;
				Cal_OK_flag = 0;
				TIM2_ACL_flag = 0;

				state = ACTIVE;
			}
		break;*/

		// Muestreo de acelerómetro y etiqueta válida
		case ACTIVE:
			// Muestreo acelerómetro
			if (TIM2_ACL_flag == 1){
				// reinicio flag
				TIM2_ACL_flag = 0;

				// Obtengo datos de acelerómetro
				new_sample = GetAcelData(raw_acel);

				// Acumular las primeras 200 muestras para gravedad de referencia
				if (Cal_OK_flag == 0) {
					// Acumular la suma de cada eje
					sum_x += (float)new_sample.x_axis;
					sum_y += (float)new_sample.y_axis;
					sum_z += (float)new_sample.z_axis;
					sample_idx++;

					// Al completar las 200 muestras (1 segundo a 200 Hz)
					if (sample_idx >= N_SAMPLES) {
						grav_sample.x_axis = (int16_t)(sum_x / N_SAMPLES);
						grav_sample.y_axis = (int16_t)(sum_y / N_SAMPLES);
						grav_sample.z_axis = (int16_t)(sum_z / N_SAMPLES);

						sum_x = 0;
						sum_y = 0;
						sum_z = 0;

						Cal_OK_flag = 1;
						sample_idx = 0; // Reiniciar índice para el buffer de ventana
					}
				}

				// Cálculo continuo de aceleración dinamica comparando con gravedad de referencia
				else {
					// Guardar en la ventana histórica (SaveWindow incrementa sample_idx internamente)
					SaveWindow(new_sample, window, &sample_idx);

					// Calcular aceleración dinámica restando la gravedad de referencia
					float a_din = CalcModuleDin(new_sample, grav_sample);

					// Verificar condición de alarma
					if (CheckMov(a_din, TH_MAX)) {

						//PA0_BTUSR_flag = 0;
						Cal_OK_flag = 0;
						TIM2_ACL_flag = 0;

						sample_idx = 0;

						state = ALARM;
					}
				}

			}
			// Etiqueta válida
			else
			{

			}

			// Se pone fuera para evitar que pulses antes de moverlo y que no entre a ALARM
			PA0_BTUSR_flag = 0;


		break;

		// Si a,dim>TH_MAX, LED ROJO ON y espera botón usuario PA0
		case ALARM:

			if (PA0_BTUSR_flag == 1){
				PA0_BTUSR_flag = 0;
				HAL_GPIO_WritePin(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN, 0);
				state = ACTIVE;
			}
			else {
				HAL_GPIO_WritePin(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN, 1);
			}


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
		TIM2_ACL_flag = 1;
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



