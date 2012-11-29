
#include <stm32f10x_adc.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <misc.h>

#include "stm32f10x_it.h"
#include "main.h"
#include "RC_Core.h"
#include "HAL.h"
#include "Axises.h"



void Init()
{
  HAL_Init();
  AXISES_Init();
  return;
}

void Update()
{
	HAL_Update();
	AXISES_Update();

	return;
}

int main()
{
  Init();

  while(1)
  {
    Update();
  }
}
