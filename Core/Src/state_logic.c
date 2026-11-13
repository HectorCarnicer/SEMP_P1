/*
 * state_logic.c
 *
 *  Created on: 22 oct 2026
 *      Author: hectorc
 */

#include <accelerometer.h>
#include "state_logic.h"
#include "main.h"
#include "main_config.h"
#include "RFID_state_logic.h"
#include "write_flash_rfid.h"
#include "stm32f411e_discovery_accelerometer.h"
#include "error_report.h"
#include <string.h>
#include <stddef.h>


// INCIALIZACIÓN DE VARIABLES
// -------------------
// Máquina de estados
// -------------------
static state_t state = CLOSE;

// -------------------
// Acelerómetro
// -------------------
// Muestra del vector window x,y,z
uint16_t sample_idx = 0;
// Estructura de componentes de aceleración
acc_sample window [N_SAMPLES] = {0};
//volatile acc_sample grav_window [N_SAMPLES] = {0};
volatile acc_sample grav_sample = {0};
acc_sample new_sample = {0};
// Vectores de apoyo
int16_t raw_acel[3] = {0};
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

// Contraseñas correctas hardcodeadas
// Leído de las tags físicas con la app NFC Tools del móvil
// No sé en qué sentido están los bytes, para testear pongo los dos
static const RFID_Uid ID_autorizadas[] = {
	// Tarjeta
    { 4, { 0xD7, 0x7D, 0x8E, 0x25 } },
	{ 4, { 0x25, 0x8E, 0x7D, 0xD7 } },
	// Tag
	{ 4, { 0x0A, 0xD4, 0x86, 0xE1 } },
	{ 4, { 0xE1, 0x86, 0xD4, 0x0A } },
};

// Comprueba si uid está en ID_autorizadas
bool UidAutorizado(const RFID_Uid *uid);

// -------------------
// LED Azul y botón usuario
// -------------------
volatile uint8_t TIM3_LED_flag = 0;
volatile uint8_t TIM3_LED_G_flag = 0;
volatile uint8_t PA0_BTUSR_flag = 0;

// Para contar 3 ciclos de TIM3 (3 * 1seg. = 3 seg.) y encender/apagar el LED verde
volatile int8_t open_counter = 0;

void StateMachine_Run()
{
	// LED Azul
	if (TIM3_LED_flag == 1)
	{
		TIM3_LED_flag = 0;
		HAL_GPIO_TogglePin(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN);
	}
	RFID_Uid uid;
	RFID_Event ev;

	// Máquina de estados
	switch (state) {
	// @TODO:No hace falta el estado IDLE
	// Estado incial espera de pulsación botón usuario PA0, pasa a CLOSE
/*	case IDLE:
		if (PA0_BTUSR_flag == 1)
		{

			PA0_BTUSR_flag = 0;
			Cal_OK_flag = 0;
			TIM2_ACL_flag = 0;

			state = CLOSE;
		}
		break;*/

		// Muestreo de acelerómetro y etiqueta válida
	case CLOSE:
		// Muestreo acelerómetro
		// @TODO: Comprobar errores del acelerómetro y lanzar ERROR_REPORT()
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
					DEBUG_REPORT("[DEBUG]: StateMachine_Run() -> CLOSE: CheckMov()\n");
					//PA0_BTUSR_flag = 0;
					Cal_OK_flag = 0;
					TIM2_ACL_flag = 0;

					sample_idx = 0;

					DEBUG_REPORT("[DEBUG]: StateMachine_Run -> CLOSE: Pasamos a estado de ALARM");
					state = ALARM;
				}
			}

		}
		else
		{

		}

		// Evaluar FSM del RFID
		ev = RFID_StateMachine_Run(&uid);
		if (ev == RFID_EV_ERROR)
		{
		    ERROR_REPORT("[ERR]: RFID: el MFRC522 no responde por SPI\n");
		}
		if (ev == RFID_EV_CARD_ARRIVED)
		{
			DEBUG_REPORT("[DEBUG]: StateMachine_Run() -> CLOSE: ev == RFID_EV_CARD_ARRIVED\n");
			if (UidAutorizado(&uid))
			{
				DEBUG_REPORT("[DEBUG]: StateMachine_Run() -> CLOSE: UidAutorizado\n");
				open_counter = 0;
				HAL_StatusTypeDef st = UidStore_Add(&uid);
				if (st != HAL_OK)// Escritura en la flash -> mirar write_flash_rfid.h
				{
					ERROR_REPORT("[ERR]: StateMachine_Run() -> CLOSE:UidStore_Add(&uid) -> Error HAL %d\n", st);
				}
				TIM3_LED_G_flag = 0;
				state = OPEN;
			} else {
				// tarjeta desconocida -> ignorar
			}
		}

		// Se pone fuera para evitar que pulses antes de moverlo y que no entre a ALARM
		PA0_BTUSR_flag = 0;


		break;

		// Si a,dim>TH_MAX, LED ROJO ON y espera botón usuario PA0
	case ALARM:
		//DEBUG_REPORT("[DEBUG]: StateMachine_Run() -> ALARM\n");
		if (PA0_BTUSR_flag == 1){
			PA0_BTUSR_flag = 0;
			HAL_GPIO_WritePin(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN, GPIO_PIN_RESET);
			state = CLOSE;
		}
		else {
			HAL_GPIO_WritePin(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN, GPIO_PIN_SET);
		}


		break;

		// Si etiqueta valida, escribe clave en FLASH y LED VERDE ON 3s, después si etiqueta válida CLOSE
	case OPEN:
		// Contar ciclos de TIM3 (1 seg.) para encender el LED. El contador se pone negativo para que solo lo haga una vez
		if (TIM3_LED_G_flag == 1)
		{
			if (open_counter >= 0) open_counter ++;
			TIM3_LED_G_flag = 0;
		}
		if (open_counter >= 0 && open_counter <= 2)
		{
			HAL_GPIO_WritePin(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN, GPIO_PIN_SET);
		}
		else if (open_counter == 3)
		{
			HAL_GPIO_WritePin(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN, GPIO_PIN_RESET);
			open_counter = -1;
		}

		// Evaluar FSM del RFID
		ev = RFID_StateMachine_Run(&uid);
		if (ev == RFID_EV_ERROR)
		{
		    ERROR_REPORT("[ERR]: RFID: el MFRC522 no responde por SPI\n");
		}
		if (ev == RFID_EV_CARD_ARRIVED)
		{
			DEBUG_REPORT("[DEBUG]: StateMachine_Run() -> OPEN: ev == RFID_EV_CARD_ARRIVED\n");
			if (UidAutorizado(&uid))
			{
				DEBUG_REPORT("[DEBUG]: StateMachine_Run() -> OPEN: UidAutorizado\n");
				open_counter = 0;
				HAL_StatusTypeDef st = UidStore_Add(&uid);// Escritura en la flash -> mirar write_flash_rfid.h
				if (st != HAL_OK)
				{
					ERROR_REPORT("[ERR]: StateMachine_Run() -> OPEN:UidStore_Add(&uid) -> Error HAL %d\n", st);
				}
				HAL_GPIO_WritePin(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN, GPIO_PIN_RESET);
				state = CLOSE;
			} else {
				// tarjeta desconocida -> ignorar
			}
		}

		break;

	default:
		ERROR_REPORT("[ERR]: StateMachine_Run() -> DEFAULT -> estado incorrecto\n");
		state = CLOSE;
		Cal_OK_flag = 0;
		break;
		}

	}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	// LED azul y LED verde
	if (htim->Instance == TIM3)
	{
		TIM3_LED_flag = 1;
		TIM3_LED_G_flag = 1;
	}

	// Acelerómetro
	if (htim->Instance == TIM2)
	{
		TIM2_ACL_flag = 1;
	}
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

bool UidAutorizado(const RFID_Uid *uid)
{
    for (size_t i = 0; i < sizeof ID_autorizadas / sizeof ID_autorizadas[0]; i++)
    {
        if (ID_autorizadas[i].size == uid->size &&
            memcmp(ID_autorizadas[i].bytes, uid->bytes, uid->size) == 0)
        {
            return true;
        }
    }
    return false;
}




