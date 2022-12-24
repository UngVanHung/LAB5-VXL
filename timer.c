/*
 * timer.c
 *
 *  Created on: Dec 4, 2022
 *      Author: ASUS
 */

#include "timer.h"

//SOFT TIMER 0
int TIM2_timer0_flag = 0;
int TIM2_timer0_counter = 0;
int TIM2_TIMER0_CYCLE = 10;

void setTimer0(int duration){
	TIM2_timer0_counter = duration /TIM2_TIMER0_CYCLE;
	TIM2_timer0_flag = 0;
}
void timer_run0(){
	if(TIM2_timer0_counter > 0){
		TIM2_timer0_counter--;
		if(TIM2_timer0_counter == 0) TIM2_timer0_flag = 1;
	}
}

//SOFT TIMER 1
int TIM2_timer1_flag = 0;
int TIM2_timer1_counter = 0;
int TIM2_TIMER1_CYCLE = 10;

void setTimer1(int duration){
	TIM2_timer1_counter = duration /TIM2_TIMER1_CYCLE;
	TIM2_timer1_flag = 0;
}
void timer_run1(){
	if(TIM2_timer1_counter > 0){
		TIM2_timer1_counter--;
		if(TIM2_timer1_counter == 0) TIM2_timer1_flag = 1;
	}
}

unsigned char isTIM2_flag0()
{
	return TIM2_timer0_flag == 1;
}

unsigned char isTIM2_flag1()
{
	return TIM2_timer1_flag == 1;
}

void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2){
			timer_run0();
			timer_run1();
		}
}
