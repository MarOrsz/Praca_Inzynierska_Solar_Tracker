/*
 * solar.c
 *
 *  Created on: Nov 18, 2020
 *      Author: Marcin Orszulak
 */

#include <az.h>
#include "tim.h"
#include "gpio.h"
#include "stdbool.h"
#include "adc.h"
#include "dma.h"
#include "stdlib.h"


//Inicjalizacja silnia azymutu
void az_init(void)
{
//Uruchomienie momentu trzymającego na wale
	az_wlacz();
	az_predk(5);
//Sprawdzenie czy krancowka dolna jest osiagnieta
	while (HAL_GPIO_ReadPin(KR_LEWA, KR_LEWA_PIN))
	{
		az_lewo();
	}
//Zatrzymanie silnika po osiagnieciu dolnej krancowki
	az_stop();
//Stabilizacja panelu
	HAL_Delay(1000);
/* Urychomienie trybu przerwaniowego licznika zliczającego
liczbę wykonanych kroków */
	HAL_TIM_Base_Start_IT(&STEPS1_TIM);
//Ustawienie panelu w pozycji początkowej
	az_poz_pocz();

/* Urychomienie trybu przerwaniowego licznika zliczającego
liczbę wykonanych kroków */

}

//Uruchomienie trzymania na wale silnika śledzącego wysokość słońca
void az_wlacz(void)
{
	HAL_GPIO_WritePin(RESET_1_PORT, RESET_1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SLEEP_1_PORT, SLEEP_1_PIN, GPIO_PIN_SET);
}

//Wyłączenie trzymania na wale silnika śledzącego wysokość słońca
void az_wylacz(void)
{
	HAL_GPIO_WritePin(RESET_1_PORT, RESET_1_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SLEEP_1_PORT, SLEEP_1_PIN, GPIO_PIN_RESET);
}

void az_lewo(void)
{
	if((HAL_TIM_PWM_Start(&PULSE1_TIM, PULSE1_TIM_CH) == 0 ||
	HAL_GPIO_ReadPin(DIR_1_PORT, DIR_1_PIN) == GPIO_PIN_RESET))
	{
		HAL_GPIO_WritePin(DIR_1_PORT, DIR_1_PIN, GPIO_PIN_SET);
		HAL_TIM_PWM_Start(&PULSE1_TIM, PULSE1_TIM_CH);
	}
}

void az_prawo(void)
{
	if(HAL_TIM_PWM_Start(&PULSE1_TIM, PULSE1_TIM_CH) == 0 ||
	HAL_GPIO_ReadPin(DIR_1_PORT, DIR_1_PIN) == GPIO_PIN_SET)
	{
	  HAL_GPIO_WritePin(DIR_1_PORT, DIR_1_PIN, GPIO_PIN_RESET);
	  HAL_TIM_PWM_Start(&PULSE1_TIM, PULSE1_TIM_CH);
	}
}

void az_stop(void)
{
	HAL_TIM_PWM_Stop(&PULSE1_TIM, PULSE1_TIM_CH);
}

void az_poz_pocz(void)
{
	// pozycja poczatkowa azacji panelu: 45st
	uint16_t kroki, licz;
	kroki = KAT_AZ_POCZ*PRZELOZENIE_AZ*MICROSTEP_NUM/1.8;
	az_kroki(kroki);
	//az_predk(10);
	az_prawo();
	//licz = __HAL_TIM_GET_COUNTER(&STEPS1_TIM);

}

void az_kroki(uint16_t kroki)
{
	__HAL_TIM_SET_AUTORELOAD(&STEPS1_TIM, kroki);
}
void az_predk(uint16_t rpm)
{
	if(rpm > 0)
	{
	  uint16_t arr_val = XT_TIM_CLK / (((XT_TIM_PSC+1) *
		  	rpm * STEPS_PER_REV * MICROSTEP_NUM) / 60) - 1;
	  uint16_t pulse_val = arr_val / 2;
	  __HAL_TIM_SET_AUTORELOAD(&PULSE1_TIM, arr_val);
	  __HAL_TIM_SET_COMPARE(&PULSE1_TIM, PULSE1_TIM_CH, pulse_val);

	}
	else
	{
		__HAL_TIM_SET_COMPARE(&PULSE1_TIM, PULSE1_TIM_CH, 0);
	}
}

uint32_t * az_odczytaj_fotorez(void)
{
	static uint32_t foto[4];
	HAL_ADC_Start_DMA(&FOTO_ADC, foto, 4);
	HAL_Delay(10);
	HAL_ADC_Stop_DMA(&FOTO_ADC);

	return  foto;
}

void az_steruj(void)
{
	uint32_t *foto;
	uint16_t LG, PG, LD, PD;
	const uint16_t cz_x = 100;
	int16_t x,p;
	foto = az_odczytaj_fotorez();
	HAL_Delay(10);


	PD = foto[0];
	PG = foto[1];
	LG = foto[2];
	LD = foto[3];

	x = (LG+LD) - (PG+PD);

	p=HAL_GPIO_ReadPin(KR_PRAWA, KR_PRAWA_PIN);

	while ((x < -cz_x) &&
	(HAL_GPIO_ReadPin(KR_PRAWA, KR_PRAWA_PIN) == 1))
	{
		while ((x < -cz_x/2) &&
		(HAL_GPIO_ReadPin(KR_PRAWA, KR_PRAWA_PIN) == 1))
		{
			az_prawo();
			foto = az_odczytaj_fotorez();
			PD = foto[0];
			PG = foto[1];
			LG = foto[2];
			LD = foto[3];
			x = (LG+LD) - (PG+PD);
		}

	}
	while ((x > cz_x) &&
	(HAL_GPIO_ReadPin(KR_LEWA, KR_LEWA_PIN) == 1))
	{
		while ((x > cz_x/2) &&
		(HAL_GPIO_ReadPin(KR_LEWA, KR_LEWA_PIN) == 1))
		{
			az_lewo();
			foto = az_odczytaj_fotorez();
			PD = foto[0];
			PG = foto[1];
			LG = foto[2];
			LD = foto[3];

			x = (LG+LD) - (PG+PD);
		}

	}
	az_stop();
}



