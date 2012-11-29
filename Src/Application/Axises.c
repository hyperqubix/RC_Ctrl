#include "HAL.h"
#include "Axises.h"


extern SYSTEM_STATUS_t System_Status;

static void CalculateChannelValue();




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

static void Init()
{

	return;
}

static void Update()
{
  CalculateChannelValue();
  return;
}

void AXISES_Constructor(AXISES_t *Axises_Obj)
{
	Axises_Obj->Init=&Init;
	Axises_Obj->Update=&Update;

	return;
}

