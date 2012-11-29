#include "HAL.h"
#include "RC_Core.h"

extern SYSTEM_STATUS_t System_Status;

static void CalculateChannelValue();
static void UpdateAxisReversals();


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


static void CalculateChannelValue()
{
  uint32_t TmpCalc;
  uint16_t AxisReversal;

  uint16_t cnt;

  for(cnt=0;cnt<8; cnt++)
  {
    TmpCalc=System_Status.Axis_Value[cnt];
    AxisReversal=System_Status.AxisReversal[cnt];

    if(AxisReversal != 0)
    {
    	TmpCalc = 1000 - TmpCalc;
    }

    // Convert from [0-1000] to [500-2000] (Servo pulse time)

    TmpCalc *= 17; //15;
    TmpCalc /= 10;

    // TmpCalc is now [0-1500]
    TmpCalc += 100; //500; // Add minimum pulse width

    // TmpCalc is now [500-2000]

    System_Status.PPM_ChTime[cnt]=TmpCalc;
  }
  return;
}

void AXISES_Init()
{
	uint16_t cnt;

	for(cnt=0;cnt<8;cnt++)
	{
		System_Status.AxisReversal[cnt]=0;
		System_Status.PPM_ChTime[cnt]=1250; // Middle position
		System_Status.ChannelMap[cnt]=cnt;
	}

	return;
}

void AXISES_Update()
{
  UpdateAxisReversals();
  CalculateChannelValue();
  return;
}

