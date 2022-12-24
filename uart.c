/*
 * uart.c
 *
 *  Created on: Dec 4, 2022
 *      Author: ASUS
 */
#include "timer.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "main.h"

#define MAX_BUFFER_SIZE 30
unsigned char temp = 0;
uint8_t buffer[MAX_BUFFER_SIZE] = "!ADC=";
uint8_t index_buffer = 0;
uint8_t flagSendData = 0;

#define INIT_SYSTEM			0
#define WAIT_HEADER			1
#define RECEIVE_DATA		2
#define DATA_RST_2  		3
#define DATA_RST_3  		4
#define END_DATA_RST 		5
#define DATA_OK_2 			6
#define END_DATA_OK 		7
uint8_t statusReceive = WAIT_HEADER;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		HAL_UART_Receive_IT(&huart2, &temp, 1);
		HAL_UART_Transmit(&huart2, &temp, 1, 200);
		fsm_command();
	}
}

uint16_t ADC_value = 0;

char str[4] = "XXXX";

enum comState {
	WAIT_COMMAND, SEND_DATA, RESEND_DATA
};
enum comState statusOfCom = WAIT_COMMAND;
void uart_fsm(void) {
	switch (statusOfCom) {
	case WAIT_COMMAND:
		if (flagSendData) {
			statusOfCom = SEND_DATA;
			//Reading ADC
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 100);
			ADC_value = HAL_ADC_GetValue(&hadc1);
			// Convert to string and print
			sprintf(str, "%hu", ADC_value);
		}
		break;
	case SEND_DATA:
		HAL_UART_Transmit(&huart2, buffer, 5, 100);
		HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
		HAL_UART_Transmit(&huart2, (uint8_t*) "#\r\n", 3, 100);
		if (flagSendData) {
			statusOfCom = RESEND_DATA;
			setTimer1(1000);
		}
		break;
	case RESEND_DATA:
		if (isTIM2_flag1()) {
			HAL_UART_Transmit(&huart2, buffer, 5, 100);
			HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
			HAL_UART_Transmit(&huart2, (uint8_t*) "#\r\n", 3, 100);
			setTimer1(1000);
		}
		if (!flagSendData) {
			statusOfCom = WAIT_COMMAND;
			setTimer1(100);
		}
		break;
	default:
		statusOfCom = WAIT_COMMAND;
	}
}

void fsm_command(void) {
	switch (statusReceive) {
	case WAIT_HEADER:
 // first text is !
		if (temp == '!') {
			statusReceive = RECEIVE_DATA;
		}
		break;
	case RECEIVE_DATA:
// if second text is R in (!RST#) move to DATA_RST_2
		if (temp == 'R')
			statusReceive = DATA_RST_2;
		// if second text is O in (!OK#) move to DATA_RST_2
		else if (temp == 'O')
			statusReceive = DATA_OK_2;
		else
			// if second is not R or O comeback first text
			statusReceive = WAIT_HEADER;
		break;
	case DATA_RST_2:
		// if third text is S in (!RST#) move to DATA_RST_3
		if (temp == 'S')
			statusReceive = DATA_RST_3;
		// else move to first text
		else
			statusReceive = WAIT_HEADER;
		break;
	case DATA_RST_3:
		// if fouth text is T in (!RST#) move to END_DATA_RST
		if (temp == 'T')
			statusReceive = END_DATA_RST;
		else
			// else move to first text
			statusReceive = WAIT_HEADER;
		break;
	case DATA_OK_2:
		// if third text is K in (!OK#) move to END_DATA_OK
		if (temp == 'K')
			statusReceive = END_DATA_OK;
		// else move to first text
		else
			statusReceive = WAIT_HEADER;
		break;
	case END_DATA_RST:
		// complete string !RST#
		if (temp == '#') {
			statusReceive = WAIT_HEADER;
			flagSendData = 1;
		} else
			statusReceive = WAIT_HEADER;
		break;
	case END_DATA_OK:
		//complete string !OK#
		if (temp == '#') {
			statusReceive = WAIT_HEADER;
			flagSendData = 0;
		} else
			statusReceive = WAIT_HEADER;
		break;
	default:
		statusReceive = INIT_SYSTEM;
		break;
	}
}

