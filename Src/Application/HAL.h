#include <stm32f10x_gpio.h>


#define TIM1_CH1_Output PA7
#define TIM1_CH3n_Output PB15
#define ADC_IN0 PA10
#define ADC_IN1 PA11
#define ADC_IN2 PA12
#define ADC_IN3 PA13
#define ADC_IN4 PA0
#define ADC_IN5 PA1
#define ADC_IN6 PA2
#define ADC_IN7 PA3

#define ADC1_DR_Address     0x4001244C

#define PPM_OUTPUT_TIMER TIM4
#define PPM_US_PRESCALER 24000000/1000000
#define PPM_CH_PERIOD 22000 // 2,2 ms (*8 = 17.6)
#define PPM_TOTAL_PERIOD 22500 // 22.5 ms

#define LED_BLUE_PORT PORTC
#define LED_BLUE_PIN GPIO_Pin_8

#define PPM_PORT GPIOC
#define PPM_PIN GPIO_Pin_13

#define LED_GREEN_PORT GPIOC
#define LED_GREEN_PIN GPIO_Pin_9

// Reverse axis ports and pins
#define SWITCH_REV_PORT0 GPIOA
#define SWITCH_REV_PIN0 GPIO_Pin_4

#define SWITCH_REV_PORT1 GPIOA
#define SWITCH_REV_PIN1 GPIO_Pin_5

#define SWITCH_REV_PORT2 GPIOA
#define SWITCH_REV_PIN2 GPIO_Pin_6

#define SWITCH_REV_PORT3 GPIOA
#define SWITCH_REV_PIN3 GPIO_Pin_7

#define SWITCH_REV_PORT4 GPIOC
#define SWITCH_REV_PIN4 GPIO_Pin_4

#define SWITCH_REV_PORT5 GPIOC
#define SWITCH_REV_PIN5 GPIO_Pin_5

#define SWITCH_REV_PORT6 GPIOB
#define SWITCH_REV_PIN6 GPIO_Pin_0

#define SWITCH_REV_PORT7 GPIOB
#define SWITCH_REV_PIN7 GPIO_Pin_1

typedef struct ADC_Measurement
{
  uint32_t Value[8];
  uint32_t MinValue[8];
  uint32_t MaxValue[8];
  uint32_t Fraction[8];
} ADC_MEASUREMENT_t;

#define FLAG_AxisReversal 1

typedef struct System_Status
{
  uint32_t Axis_Value[8];
  uint32_t PPM_ChTime[8];
  uint8_t AxisReversal[8];
  uint8_t ChannelMap[8];
} SYSTEM_STATUS_t;

void HAL_Init();
void HAL_PPM_IRQ();
void HAL_Update();
