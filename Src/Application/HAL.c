/*
 * BSP.c
 *
 *  Created on: 13. nov. 2012
 *      Author: eskelund
 */
#include <stdint.h>

#include "HAL.h"
#include "Default_Setup.h"
#include "RC_Core.h"


#define LOCKUP while(1){};

void NMI_Handler(void){LOCKUP}
void HardFault_Handler(void){LOCKUP}
void MemManage_Handler(void){LOCKUP}
void BusFault_Handler(void){LOCKUP}
void UsageFault_Handler(void){LOCKUP}
void DebugMon_Handler(void){LOCKUP}
void SVC_Handler(void){LOCKUP}
void PendSV_Handler(void){LOCKUP}
void SysTick_Handler(void){LOCKUP}

void ADC1_IRQHandler(void)
{
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}


static void Init_NVIC();
static void Init_CoreSys();
static void Init_DMA_ADC();
static void Init_GPIO();
static void Init_PPMTimer();
static void UpdateADCValues();
static void UpdateAxisReversals();

SYSTEM_STATUS_t System_Status;
static ADC_MEASUREMENT_t ADC_Measurement;

void IRQ_PPM(void)
{
  static uint16_t State=0;
  static uint16_t Channel=0;
  uint16_t Delay=0;
//  uint16_t timercnt=TIM_GetCounter(TIM4);


  if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1 );
    TIM_SetCounter(TIM4,0); // Reset counter

    if(State == 0) // Delay
    {
      State=1;
      GPIO_ResetBits(PPM_PORT, PPM_PIN);
      TIM_SetCompare1(TIM4,500); // Wait 0.4 ms
    }
    else
    {
      State=0;

      if(Channel < 8)
      {
        GPIO_SetBits(PPM_PORT, PPM_PIN);
        Delay=System_Status.PPM_ChTime[Channel];
        Channel++;
      }
      else
      {
        Delay = 12000; // 12 ms delay
        GPIO_SetBits(PPM_PORT, PPM_PIN);
        Channel = 0;
      }

      TIM_SetCompare1(TIM4,Delay);
    }
  }

}
static void Init_NVIC()
{
  NVIC_InitTypeDef NVIC_InitStructure;

  // Enable the TIM4 global Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/*  NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
*/
}

static void Init_CoreSys()
{
  // Setup RCC to use HSE, 8 MHz, with PLL * 3 = 24 MHz
  RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_3);
  RCC_HSEConfig(RCC_HSE_ON);
  RCC_WaitForHSEStartUp();
  RCC_PLLCmd(ENABLE);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  RCC_PCLK1Config(RCC_HCLK_Div1);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  return;
}

static void Init_GPIO()
{
  GPIO_InitTypeDef gpio_def;
  gpio_def.GPIO_Mode=GPIO_Mode_Out_PP;
  gpio_def.GPIO_Speed=GPIO_Speed_50MHz;

  gpio_def.GPIO_Pin=LED_GREEN_PIN;
  GPIO_Init(LED_GREEN_PORT, &gpio_def);

  // TIMER, PPM output
  gpio_def.GPIO_Mode=GPIO_Mode_Out_PP;
  gpio_def.GPIO_Pin=PPM_PIN;
  gpio_def.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(PPM_PORT, &gpio_def);

  // ADC
  gpio_def.GPIO_Mode=GPIO_Mode_AIN;
  gpio_def.GPIO_Speed=GPIO_Speed_50MHz;
  gpio_def.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;

  GPIO_Init(GPIOA, &gpio_def);
  GPIO_Init(GPIOC, &gpio_def);

  // Axis reversal switches
  gpio_def.GPIO_Speed=GPIO_Speed_50MHz;
  gpio_def.GPIO_Mode=GPIO_Mode_IPU; // Input - pull-up
  gpio_def.GPIO_Pin=SWITCH_REV_PIN0; GPIO_Init(SWITCH_REV_PORT0,&gpio_def);
  gpio_def.GPIO_Pin=SWITCH_REV_PIN1; GPIO_Init(SWITCH_REV_PORT1,&gpio_def);
  gpio_def.GPIO_Pin=SWITCH_REV_PIN2; GPIO_Init(SWITCH_REV_PORT2,&gpio_def);
  gpio_def.GPIO_Pin=SWITCH_REV_PIN3; GPIO_Init(SWITCH_REV_PORT3,&gpio_def);
  gpio_def.GPIO_Pin=SWITCH_REV_PIN4; GPIO_Init(SWITCH_REV_PORT4,&gpio_def);
  gpio_def.GPIO_Pin=SWITCH_REV_PIN5; GPIO_Init(SWITCH_REV_PORT5,&gpio_def);
  gpio_def.GPIO_Pin=SWITCH_REV_PIN6; GPIO_Init(SWITCH_REV_PORT6,&gpio_def);
  gpio_def.GPIO_Pin=SWITCH_REV_PIN7; GPIO_Init(SWITCH_REV_PORT7,&gpio_def);

  return;
}

static void Init_PPMTimer()
{
  TIM_TimeBaseInitTypeDef tim_def;
  tim_def.TIM_Prescaler=PPM_US_PRESCALER; //0x0102;
  tim_def.TIM_Period=PPM_CH_PERIOD;
  tim_def.TIM_CounterMode=TIM_CounterMode_Up;
  tim_def.TIM_ClockDivision=TIM_CKD_DIV1;

  TIM_TimeBaseInit(PPM_OUTPUT_TIMER, &tim_def);

  TIM_OCInitTypeDef timoc_def;
  timoc_def.TIM_OCMode=TIM_OCMode_PWM1;
  timoc_def.TIM_OutputState=TIM_OutputState_Enable;
  timoc_def.TIM_OCPolarity=TIM_OCPolarity_High;
  timoc_def.TIM_Pulse=2000;

  TIM_OC1Init(PPM_OUTPUT_TIMER, &timoc_def);
  //TIM_OC1PreloadConfig(PPM_OUTPUT_TIMER, TIM_OCPreload_Enable);

  TIM_Cmd(PPM_OUTPUT_TIMER, ENABLE);

  TIM_ITConfig(PPM_OUTPUT_TIMER, TIM_IT_CC1, ENABLE);

  return;
}

static void Init_DMA_ADC()
{
  DMA_InitTypeDef DMA_def;
  DMA_DeInit(DMA1_Channel1);
  DMA_def.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_def.DMA_MemoryBaseAddr = (uint32_t)ADC_Measurement.Value;
  DMA_def.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_def.DMA_BufferSize = 8;
  DMA_def.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_def.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_def.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_def.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_def.DMA_Mode = DMA_Mode_Circular;
  DMA_def.DMA_Priority = DMA_Priority_High;
  DMA_def.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_def);

  // Enable DMA1 channel1
  DMA_Cmd(DMA1_Channel1, ENABLE);


  ADC_InitTypeDef adc_def;
  adc_def.ADC_Mode=ADC_Mode_Independent;
  adc_def.ADC_ScanConvMode=ENABLE;
  adc_def.ADC_ContinuousConvMode=ENABLE;
  adc_def.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
  adc_def.ADC_DataAlign=ADC_DataAlign_Right;
  adc_def.ADC_NbrOfChannel=8;

  ADC_Init(ADC1,&adc_def);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 5, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 6, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 7, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 8, ADC_SampleTime_55Cycles5);

  ADC_Cmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));

  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));

  ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);

  return;
}


static void Init_ADC()
{
	uint32_t cnt=0;

	// Init values to default
	for(cnt=0;cnt<8;cnt++)
	{
		ADC_Measurement.Value[cnt]=0;
		ADC_Measurement.MinValue[cnt]=1300000;
		ADC_Measurement.MaxValue[cnt]=2600000;
		ADC_Measurement.Fraction[cnt]=0;
	}

	// Default calibrated values
	  ADC_Measurement.MinValue[0]=ADC_Channel0_MinVal; ADC_Measurement.MaxValue[0]=ADC_Channel0_MaxVal;
	  ADC_Measurement.MinValue[1]=ADC_Channel1_MinVal; ADC_Measurement.MaxValue[1]=ADC_Channel1_MaxVal;
	  ADC_Measurement.MinValue[2]=ADC_Channel2_MinVal; ADC_Measurement.MaxValue[2]=ADC_Channel2_MaxVal;
	  ADC_Measurement.MinValue[3]=ADC_Channel3_MinVal; ADC_Measurement.MaxValue[3]=ADC_Channel3_MaxVal;
	  ADC_Measurement.MinValue[4]=ADC_Channel4_MinVal; ADC_Measurement.MaxValue[4]=ADC_Channel4_MaxVal;
	  ADC_Measurement.MinValue[5]=ADC_Channel5_MinVal; ADC_Measurement.MaxValue[5]=ADC_Channel5_MaxVal;
	  ADC_Measurement.MinValue[6]=ADC_Channel6_MinVal; ADC_Measurement.MaxValue[6]=ADC_Channel6_MaxVal;
	  ADC_Measurement.MinValue[7]=ADC_Channel7_MinVal; ADC_Measurement.MaxValue[7]=ADC_Channel7_MaxVal;

	  for(cnt=0;cnt<8;cnt++)
	  {
		  ADC_Measurement.Fraction[cnt]=ADC_Measurement.MaxValue[cnt]-ADC_Measurement.MinValue[cnt];
		  ADC_Measurement.Fraction[cnt]=1000000000 / ADC_Measurement.Fraction[cnt];
	  }

	return;
}


// Limit the ADC values to 0-1000
static void UpdateADCValues()
{
	uint32_t TmpCalc;
	uint32_t MaxValue;
	uint32_t MinValue;
	uint32_t Fraction;

	uint16_t cnt=0;

	for(cnt=0;cnt<8;cnt++)
	{
		TmpCalc=ADC_Measurement.Value[cnt]*1000;
		MaxValue=ADC_Measurement.MaxValue[cnt];
		MinValue=ADC_Measurement.MinValue[cnt];
		Fraction=ADC_Measurement.Fraction[cnt];

    // Cap value
    if(TmpCalc > MaxValue)
    {
      TmpCalc=MaxValue;
    }
    else if(TmpCalc > MinValue)
    {
      TmpCalc -= MinValue;
    }
    else
    {
      TmpCalc = 0;
    }

    TmpCalc *= Fraction;
    TmpCalc /= 1000000;

    if(TmpCalc > 1000)
    	TmpCalc = 1000;

    // We now have a value between 0-1000
    System_Status.Axis_Value[cnt]=TmpCalc;

	}

	return;
}

static void UpdateAxisReversals()
{
	System_Status.AxisReversal[0] = GPIO_ReadInputDataBit(SWITCH_REV_PORT0, SWITCH_REV_PIN0);
	System_Status.AxisReversal[1] = GPIO_ReadInputDataBit(SWITCH_REV_PORT1, SWITCH_REV_PIN1);
	System_Status.AxisReversal[2] = GPIO_ReadInputDataBit(SWITCH_REV_PORT2, SWITCH_REV_PIN2);
	System_Status.AxisReversal[3] = GPIO_ReadInputDataBit(SWITCH_REV_PORT3, SWITCH_REV_PIN3);
	System_Status.AxisReversal[4] = GPIO_ReadInputDataBit(SWITCH_REV_PORT4, SWITCH_REV_PIN4);
	System_Status.AxisReversal[5] = GPIO_ReadInputDataBit(SWITCH_REV_PORT5, SWITCH_REV_PIN5);
	System_Status.AxisReversal[6] = GPIO_ReadInputDataBit(SWITCH_REV_PORT6, SWITCH_REV_PIN6);
	System_Status.AxisReversal[7] = GPIO_ReadInputDataBit(SWITCH_REV_PORT7, SWITCH_REV_PIN7);

  return;
}

static void Update()
{
	UpdateADCValues();
	UpdateAxisReversals();

	return;
}


static void Init()
{
	uint16_t cnt;

	for(cnt=0;cnt<8;cnt++)
	{
		System_Status.Axis_Value[cnt]=0;
		System_Status.AxisReversal[cnt]=0;
		System_Status.PPM_ChTime[cnt]=1250; // Middle position
		System_Status.ChannelMap[cnt]=cnt;
	}

	Init_CoreSys();
	Init_NVIC();
	Init_DMA_ADC();
	Init_ADC();
	Init_PPMTimer();

  // All is set up. Liven the board up :-)
  Init_GPIO();

  return;
}

void HAL_Constructor(HAL_t *Hal_Obj)
{

	Hal_Obj->IRQ_PPM=&IRQ_PPM;
	Hal_Obj->Init=&Init;
	Hal_Obj->Update=&Update;

	return;
}


