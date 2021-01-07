/*
 * solar.h
 *
 *  Created on: Nov 18, 2020
 *      Author: Marcin Orszulak
 */

#ifndef INC_AZ_H_
#define INC_AZ_H_

#include "stm32f3xx_hal.h"
#include "stdbool.h"

// liczba krokow na pelen obrot silnika
#define STEPS_PER_REV	200
//liczba mikrokrokow -> 1 = brak mikrokrokow
#define MICROSTEP_NUM	4
// licznik odpowiedzialny za generowanie sygnalu PWM
#define	PULSE1_TIM		htim1
//kanal generowania sygnalu PWM
#define	PULSE1_TIM_CH	TIM_CHANNEL_4
// wykorzystany przetwornik ADC
#define FOTO_ADC		hadc1
//timer odpowiedzialny za zliczanie wykonanych krokow przez silnik
#define	STEPS1_TIM		htim2
//czestotliwosc wejsciowa timera PULSE_TIM w Hz
#define	XT_TIM_CLK		72000000
//prescaler timera PULSE_TIM
#define XT_TIM_PSC		3600-1
//port sygnału DIR
#define DIR_1_PORT		GPIOA
//pin sygnału DIR
#define DIR_1_PIN		GPIO_PIN_12
//port sygnału nRESET
#define RESET_1_PORT	GPIOB
//pin sygnału nRESET
#define RESET_1_PIN		GPIO_PIN_11
//port sygnału nSLEEP
#define SLEEP_1_PORT	GPIOB
//pin sygnału nSLEEP
#define SLEEP_1_PIN		GPIO_PIN_2
//stan pinu DIR podczas ruchu w dol
#define STEPPER_1_DOL		0
//stan pinu DIR podczas ruchu w gore
#define STEPPER_1_GORA		1
//przełożenie przekładni azacji
#define PRZELOZENIE_AZ	3.5
// przełożenie przekładni azacji
#define KAT_AZ_POCZ		100
//port sygnału krancowki gornej
#define KR_PRAWA				GPIOB
//pin sygnału krancowki prawej
#define KR_PRAWA_PIN			GPIO_PIN_4
//port sygnału krancowki lewej
#define KR_LEWA			GPIOB
//pin sygnału krancowki lewej
#define KR_LEWA_PIN		GPIO_PIN_5


// zdefiniowane funkcje

void az_init(void);
void az_wlacz(void);
void az_wylacz(void);
void az_lewo(void);
void az_prawo(void);
void az_stop(void);
void az_poz_pocz(void);
void az_predk(uint16_t rpm);
uint32_t * az_odczytaj_fotorez(void);
void az_steruj(void);
void az_kroki(uint16_t kroki);


#endif /* INC_AZ_H_ */
