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

#define STEPS_PER_REV	200				/* liczba krokow na pelen obrot silnika */
#define MICROSTEP_NUM	1			/* liczba mikrokrokow -> 1 = brak mikrokrokow */

#define	PULSE1_TIM		htim1			/* timer odpowiedzialny za generowanie sygnalu PWM */
#define	PULSE1_TIM_CH	TIM_CHANNEL_4	/* kanal generowania sygnalu PWM */

#define FOTO_ADC		hadc1

#define	STEPS_TIM		htim2			/* timer odpowiedzialny za zliczanie wykonanych krokow przez silnik */

#define	XT_TIM_CLK		72000000		/* czestotliwosc wejsciowa timera PULSE_TIM w Hz */
#define XT_TIM_PSC		31			/* prescaler timera PULSE_TIM */

#define DIR_1_PORT		GPIOA				/* port sygnału DIR */
#define DIR_1_PIN		GPIO_PIN_12			/* pin sygnału DIR */
#define RESET_1_PORT	GPIOB			/* port sygnału nRESET */
#define RESET_1_PIN		GPIO_PIN_11			/* pin sygnału nRESET */
#define SLEEP_1_PORT	GPIOB				/* port sygnału nSLEEP */
#define SLEEP_1_PIN		GPIO_PIN_2			/* pin sygnału nSLEEP */

#define STEPPER_1_CW		0				/* stan pinu DIR podczas ruchu CW */
#define STEPPER_1_CCW		1				/* stan pinu DIR podczas ruchu CCW */


void solar_init(void);
void solar_wlacz_s_wys(void);
void solar_sprawdz_wys(bool* kierunek1, bool* ruch1);
void solar_steruj_s_wys(void);




#endif /* INC_SOLAR_H_ */
