/*
 * SysIntf.cpp
 *
 *  Created on: 8. juli 2012
 *      Author: eskelund
 */

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>

#include <BSP.h>
#include "SysIntf.h"

// Use a function pointer to point to the correct function for the interrupt
void SysIntf_IRQHandler()
{
	SYSINTF_t::Interrupt();
}
//void (*SysIntf_IRQHandler)(void)=SYSINTF_t::Interrupt;
static volatile char RxBuffer[SysIntf_RxBufferSize];
static volatile char TxBuffer[SysIntf_TxBufferSize];
static volatile uint16_t RxCntr;
static volatile uint16_t TxCntr;

SYSINTF_t::SYSINTF_t()
{
	RxCntr=0;
	TxCntr=0;
}

void SYSINTF_t::Interrupt()
{
	// First, check for reception:
	if(USART_GetITStatus(SysIntf_USART, USART_IT_RXNE) != RESET)
	{
		if(RxCntr < SysIntf_RxBufferSize)
		{
			RxBuffer[RxCntr++] = USART_ReceiveData(SysIntf_USART);
		}
		else
		{
			USART_ReceiveData(SysIntf_USART);
		}
	}

	// Check for transmission
	if(USART_GetITStatus(SysIntf_USART, USART_IT_TXE) != RESET)
	{
		if(TxCntr > 0)
		{
			USART_SendData(SysIntf_USART, TxBuffer[TxCntr--]);
		}
		else
		{
			USART_ClearFlag(SysIntf_USART, USART_IT_TC);
		}
	}

	return;
}

int SYSINTF_t::Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(SysIntf_RCC_Clks, ENABLE);

	// UART setup
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(SysIntf_USART, &USART_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SysIntf_USART_TX_Pin; // USART1 TX = PA 9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SysIntf_USART_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SysIntf_USART_RX_Pin;// USART1 RX = PA 10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SysIntf_USART_GPIO, &GPIO_InitStructure);

	// Enable RX and TX interrupts
	USART_ITConfig(SysIntf_USART, USART_IT_RXNE | USART_IT_TXE, ENABLE);

	// Enable the USART
	USART_Cmd(SysIntf_USART, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = SysIntf_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	return 0;
}

int SYSINTF_t::Send(char *str, uint16_t size)
{
	uint16_t cnt=0;

	for(cnt=0;cnt<size;cnt++)
	{
		TxBuffer[cnt]=str[cnt];
	}
	TxCntr=size;
	USART_SendData(SysIntf_USART,TxBuffer[TxCntr--]);

	return 0;
}

