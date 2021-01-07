/*
 * solar.h
 *
 *  Created on: Nov 18, 2020
 *      Author: Marcin Orszulak
 */

#ifndef INC_SOLAR_H_
#define INC_SOLAR_H_


#include "stm32f3xx_hal.h"
#include "stdbool.h"


// liczba krokow na pelen obrot silnika
#define STEPS_PER_REV	200
//liczba mikrokrokow -> 1 = brak mikrokrokow
#define MICROSTEP_NUM	16
// licznik odpowiedzialny za generowanie sygnalu PWM
#define	PULSE2_TIM		htim8
//kanal generowania sygnalu PWM
#define	PULSE2_TIM_CH	TIM_CHANNEL_4
// wykorzystany przetwornik ADC
#define FOTO_ADC		hadc1
//timer odpowiedzialny za zliczanie wykonanych krokow przez silnik
#define	STEPS2_TIM		htim4
//czestotliwosc wejsciowa timera PULSE_TIM w Hz
#define	XT_TIM_CLK		72000000
//prescaler timera PULSE_TIM
#define XT_TIM_PSC		360-1
//port sygnału DIR
#define DIR_2_PORT		GPIOB
//pin sygnału DIR
#define DIR_2_PIN		GPIO_PIN_8

//port sygnału nSLEEP
#define ENA_2_PORT	    GPIOA
//pin sygnału nSLEEP
#define ENA_2_PIN		GPIO_PIN_6
//stan pinu DIR podczas ruchu w dol
#define STEPPER_2_DOL		0
//stan pinu DIR podczas ruchu w gore
#define STEPPER_2_GORA		1
//przełożenie przekładni elewacji
#define PRZELOZENIE_ELEW	4.6

// przełożenie przekładni elewacji
#define KAT_ELEW_POCZ		50
//port sygnału krancowki prawej
#define KR_GORA		   GPIOB
//pin sygnału krancowki prawej
#define KR_GORA_PIN	 GPIO_PIN_10
//port sygnału krancowki lewej
#define KR_DOL			GPIOA
//pin sygnału krancowki lewej
#define KR_DOL_PIN		GPIO_PIN_8

// zdefiniowane funkcje

void elew_init(void);
void elew_wlacz(void);
void elew_wylacz(void);
void elew_dol(void);
void elew_gora(void);
void elew_stop(void);
void elew_poz_pocz(void);
void elew_predk(uint16_t rpm);
uint32_t * elew_odczytaj_fotorez(void);
void elew_steruj();
void elew_kroki(uint16_t kroki);


#endif /* INC_SOLAR_H_ */
