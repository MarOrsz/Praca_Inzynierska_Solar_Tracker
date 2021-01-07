/*
 * solar.c
 *
 *  Created on: Nov 18, 2020
 *      Author: Marcin Orszulak
 */


#include "tim.h"
#include "gpio.h"
#include "stdbool.h"
#include "adc.h"
#include "dma.h"
#include "stdlib.h"
#include "elew.h"

//Inicjalizacja silnia elewacji
void elew_init(void)
{
//Uruchomienie momentu trzymającego na wale
	elew_wlacz();
	elew_predk(5);
//Sprawdzenie czy krancowka dolna jest osiagnieta
	while (HAL_GPIO_ReadPin(KR_DOL, KR_DOL_PIN))
	{

		elew_dol();
	}
//Zatrzymanie silnika po osiagnieciu dolnej krancowki
	elew_stop();
	HAL_Delay(1000);

//Urychomienie trybu przerwaniowego licznika zliczającego
//liczbę wykonanych kroków
	HAL_TIM_Base_Start_IT(&STEPS2_TIM);
//Ustawienie panelu w pozycji początkowej
	elew_poz_pocz();


//Zatrzymanie trybu przerwaniowego licznika zliczającego
//liczbe wykonanych kroków
	//HAL_TIM_Base_Stop_IT(&STEPS2_TIM);
}

//Uruchomienie trzymania na wale silnika śledzącego wysokość słońca
void elew_wlacz(void)
{
	HAL_GPIO_WritePin(ENA_2_PORT,ENA2_Pin, GPIO_PIN_RESET);
}

//Wyłączenie trzymania na wale silnika śledzącego wysokość słońca
void elew_wylacz(void)
{
	HAL_GPIO_WritePin(ENA_2_PORT,ENA2_Pin, GPIO_PIN_SET);
}

void elew_dol(void)
{
	if(HAL_TIM_PWM_Start(&PULSE2_TIM, PULSE2_TIM_CH) == 0 ||
	HAL_GPIO_ReadPin(DIR_2_PORT, DIR_2_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_WritePin(DIR_2_PORT, DIR_2_PIN, GPIO_PIN_RESET);
		HAL_TIM_PWM_Start(&PULSE2_TIM, PULSE2_TIM_CH);
	}
}

void elew_gora(void)
{
	if(HAL_TIM_PWM_Start(&PULSE2_TIM, PULSE2_TIM_CH) == 0 ||
	HAL_GPIO_ReadPin(DIR_2_PORT, DIR_2_PIN) == GPIO_PIN_RESET)
	{
	  HAL_GPIO_WritePin(DIR_2_PORT, DIR_2_PIN, GPIO_PIN_SET);
	  HAL_TIM_PWM_Start(&PULSE2_TIM, PULSE2_TIM_CH);
	}
}

void elew_stop(void)
{
	HAL_TIM_PWM_Stop(&PULSE2_TIM, PULSE2_TIM_CH);
}

void elew_poz_pocz(void)
{
	// pozycja poczatkowa elewacji panelu: 45st
	uint16_t kroki, licz;
	kroki = KAT_ELEW_POCZ*MICROSTEP_NUM*PRZELOZENIE_ELEW/1.8;

	elew_kroki(kroki);
	//elew_predk(100);

	elew_gora();




}

void elew_kroki(uint16_t kroki)
{
	//HAL_TIM_Base_Start_IT(&STEPS1_TIM);
	__HAL_TIM_SET_AUTORELOAD(&STEPS2_TIM, kroki);

}
void elew_predk(uint16_t rpm)
{
	if(rpm > 0)
	{
	  uint16_t arr_val = XT_TIM_CLK / (((XT_TIM_PSC+1) *
		  	rpm * STEPS_PER_REV * MICROSTEP_NUM) / 60) - 1;
	  uint16_t pulse_val = arr_val / 2;
	  __HAL_TIM_SET_AUTORELOAD(&PULSE2_TIM, arr_val);
	  __HAL_TIM_SET_COMPARE(&PULSE2_TIM, PULSE2_TIM_CH, pulse_val);

	}
	else
	{
		__HAL_TIM_SET_COMPARE(&PULSE2_TIM, PULSE2_TIM_CH, 0);
	}
}

uint32_t * elew_odczytaj_fotorez(void)
{
	static uint32_t foto[4];
	HAL_ADC_Start_DMA(&FOTO_ADC, foto, 4);
	HAL_Delay(10);
	HAL_ADC_Stop_DMA(&FOTO_ADC);

	return  foto;
}

void elew_steruj()
{
	uint32_t *foto;
	uint16_t LG, PG, LD, PD;
	const uint16_t cz_x = 100;
	int16_t x,p;
	foto = elew_odczytaj_fotorez();
	HAL_Delay(10);

	PD = foto[0];
	PG = foto[1];

	LG = foto[2];
	LD = foto[3];

	x = (LG+PG) - (LD+PD);

	p=HAL_GPIO_ReadPin(KR_GORA, KR_GORA_PIN);

	while ((x > cz_x) &&
	(HAL_GPIO_ReadPin(KR_GORA, KR_GORA_PIN) == 1))
	{
		while ((x > cz_x/2) &&
		(HAL_GPIO_ReadPin(KR_GORA, KR_GORA_PIN) == 1))
		{
			elew_gora();
			foto = elew_odczytaj_fotorez();
			PD = foto[0];
			PG = foto[1];
			LG = foto[2];


			LD = foto[3];

			x = (LG+PG) - (LD+PD);
		}

	}

	while ((x < -cz_x) &&
	(HAL_GPIO_ReadPin(KR_DOL, KR_DOL_PIN) == 1))
	{
		while ((x < -cz_x/2) &&
		(HAL_GPIO_ReadPin(KR_DOL, KR_DOL_PIN) == 1))
		{
			elew_dol();
			foto = elew_odczytaj_fotorez();
			PD = foto[0];
			PG = foto[1];
			LG = foto[2];
			LD = foto[3];

			x = (LG+PG) - (LD+PD);
		}

	}

	elew_stop();
}



