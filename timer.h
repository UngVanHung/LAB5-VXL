/*
 * timer.h
 *
 *  Created on: Dec 4, 2022
 *      Author: ASUS
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_


#include "main.h"

void setTimer0(int duration);
void timer_run0();
void setTimer1(int duration);
void timer_run1();
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim);
unsigned char isTIM2_flag0();
unsigned char isTIM2_flag1();
#endif /* INC_TIMER_H_ */
