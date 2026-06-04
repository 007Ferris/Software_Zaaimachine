///////////////////////////////////////////////////////////////////////////////
//
// HartbeatTask.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	02-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "Config.h"
#include "LEDLib.h"


///////////////////////////////////////////////////////////////////////////////
// void task_Heartbeat(void *pvParameters)

void task_Heartbeat(void *pvParameters)
{
	// SerialPrintf("> Hartbeat should be running, flashing onboard LED...\n");
	// char *taskName = pcTaskGetName(NULL);
	// TaskHandle_t handle = xTaskGetHandle(taskName);
	// SerialPrintf("> task name: [%s], handle = 0x%08x\n", taskName, handle);

	while (true)
	{
		taskSleep(500);
		led_Set(LED_BLUE, false);
		taskSleep(500);
		led_Set(LED_BLUE, true);
	}
	
	// Should never go here!
	vTaskDelete(NULL);
}
