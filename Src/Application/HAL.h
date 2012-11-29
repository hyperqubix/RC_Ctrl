#ifndef HAL_H_
#define HAL_H_

#include "BSP.h"
#include "RC_Core.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM4_IRQHandler(void);

typedef struct ADC_Measurement
{
  uint32_t Value[8];
  uint32_t MinValue[8];
  uint32_t MaxValue[8];
  uint32_t Fraction[8];
} ADC_MEASUREMENT_t;

typedef struct System_Status
{
  uint32_t Axis_Value[8];
  uint32_t PPM_ChTime[8];
  uint8_t AxisReversal[8];
  uint8_t ChannelMap[8];
} SYSTEM_STATUS_t;

#define FLAG_AxisReversal 1


// This struct identifies the "public" functions and variables
typedef struct HAL_t
{
	SYSTEM_STATUS_t *System_Status;

	void (*IRQ_PPM)(void);
	void (*Init)(void);
	void (*Update)(void);
}HAL_t;

void HAL_Constructor(HAL_t *Hal_Obj);

#endif

