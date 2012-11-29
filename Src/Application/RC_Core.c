#include "RC_Core.h"
#include "Axises.h"
#include "HAL.h"

extern AXISES_t Axises;
extern HAL_t Hal;

static void Update()
{
  return;
}

static void Init()
{
  return;
}

static void MainLoop()
{
	while(1)
	{
		Hal.Update();
		Axises.Update();
	}
	return;
}

void RC_CORE_Constructor(RC_CORE_t *RC_Core_Obj)
{
	RC_Core_Obj->Init = &Init;
	RC_Core_Obj->Update = &Update;
	RC_Core_Obj->MainLoop = &MainLoop;
}


