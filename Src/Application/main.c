#include "main.h"
#include "RC_Core.h"
#include "HAL.h"
#include "Axises.h"


AXISES_t Axises;
HAL_t Hal;
RC_CORE_t RC_Core;


// Mappings of interrupts
void TIM4_IRQHandler(void)
{
	Hal.IRQ_PPM();
}


void Init()
{
  // IMPORTANT! IRQ's must NOT be enabled before the constructors are called
  AXISES_Constructor(&Axises);
  HAL_Constructor(&Hal);
  RC_CORE_Constructor(&RC_Core);

  Hal.Init();
  Axises.Init();
  return;
}

void Update()
{

	return;
}

int main()
{
  Init();

  RC_Core.MainLoop();

  return 0;
}
