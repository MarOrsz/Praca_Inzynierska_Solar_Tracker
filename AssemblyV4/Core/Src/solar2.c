/*
 * solar.c
 *
 *  Created on: Nov 18, 2020
 *      Author: Marcin Orszulak
 */

#include "solar2.h"
#include "tim.h"
#include "gpio.h"
#include "stdbool.h"
#include "adc.h"
#include "dma.h"
#include "stdlib.h"


//Inicjalizacja silnia elewacji
void solar_init(void)
{
	solar_wlacz_elew();

	while (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))				//Ustawienie pozycji 45deg
	HAL_TIM_PWM_Start(&PULSE1_TIM, PULSE1_TIM_CH);
	HAL_TIM_PWM_Stop(&PULSE1_TIM, PULSE1_TIM_CH);

}

//Uruchomienie trzymania na wale silnika śledzącego wysokość słońca
void solar_wlacz_s_wys(void)
{
	HAL_GPIO_WritePin(RESET_1_PORT, RESET_1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SLEEP_1_PORT, SLEEP_1_PIN, GPIO_PIN_SET);
}

//Wyłączenie trzymania na wale silnika śledzącego wysokość słońca
void solar_wylacz_s_wys(void)
{
	HAL_GPIO_WritePin(RESET_1_PORT, RESET_1_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SLEEP_1_PORT, SLEEP_1_PIN, GPIO_PIN_RESET);
}

void solar_sprawdz_wys(bool* kierunek1, bool* ruch1)
{
	uint32_t foto[4];
	uint16_t LG, PG, LD, PD;
	HAL_ADC_Start_DMA(&FOTO_ADC, foto, 4);
HAL_Delay(10);

	//HAL_ADC_PollForConversion(&FOTO_ADC, 10);

	LG = foto[3];
	PG = foto[0];
	LD = foto[1];
	PD = foto[2];



		if ((foto[3]+foto[0]) - (foto[1]+foto[2])>200)
		{
			*ruch1 = 1;
			int16_t por;
			por = (foto[3]+foto[0]) - (foto[1]+foto[2]);
			//if (((foto[0] + foto[1]) - (foto[2] + foto[3]))>0)
			if (por>0)
			{
				*kierunek1 = 1;
			}
			else
			{
				*kierunek1 = 0;
			}
		}
		else
		{
			*ruch1 = 0;
		}

		HAL_ADC_Stop_DMA(&FOTO_ADC);

}


void solar_steruj_s_wys(void)
{
	bool kierunek1, ruch1;

	solar_sprawdz_wys(&kierunek1, &ruch1);
	bool wlaczony = 0;


		while(ruch1 == 1)
		{

				if(kierunek1 == 1)
				{
					HAL_GPIO_WritePin(DIR_1_PORT, DIR_1_PIN, GPIO_PIN_SET);
				}
				else
				{
					HAL_GPIO_WritePin(DIR_1_PORT, DIR_1_PIN, GPIO_PIN_RESET);
				}

				if (wlaczony == 0)
				{
					HAL_TIM_PWM_Start(&PULSE1_TIM, PULSE1_TIM_CH);
					wlaczony = 1;
				}
				solar_sprawdz_wys(&kierunek1, &ruch1);


		}


		HAL_TIM_PWM_Stop(&PULSE1_TIM, PULSE1_TIM_CH);


}

