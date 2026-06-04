///////////////////////////////////////////////////////////////////////////////
//
// InfoRTOS.cpp
//
// Authors: 	Roel Smeets (Avans)
// Edit date: 	25-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>
#include "esp_timer.h"
#include "WiFi.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "InfoRTOS.h"


///////////////////////////////////////////////////////////////////////////////
// file globals

static uint8_t g_nTasksRegistered = 0;
static char g_TaskNameList[MAX_TASKS][configMAX_TASK_NAME_LEN];

///////////////////////////////////////////////////////////////////////////////
// bool info_RegisterTaskByName(const char *taskName)

bool info_RegisterTaskByName(const char *taskName)
{
	bool resultOK = false;

	if (g_nTasksRegistered < MAX_TASKS)
	{
		strncpy(g_TaskNameList[g_nTasksRegistered], taskName, configMAX_TASK_NAME_LEN - 1);
		g_nTasksRegistered++;
		resultOK = true;
	}

	return resultOK;
}


///////////////////////////////////////////////////////////////////////////////
// void info_Tasks(void)

void info_Tasks(void)
{
	uint8_t count = 0;
	eTaskState state = eInvalid;
	char *taskName = NULL;
	TaskHandle_t handle = NULL;
	UBaseType_t highWaterMark = 0;

	uint8_t nTasks = uxTaskGetNumberOfTasks();

	SerialPrintf("--- %d tasks registered, %d tasks executing ---\n", g_nTasksRegistered, nTasks);
	SerialPrintf("------------------------------------------\n");
	SerialPrintf("%2s %s       %s     %s   %s\n", "id", "task name", "handle", "state", "HWM");
	SerialPrintf("------------------------------------------\n");

	for (count = 0; count < g_nTasksRegistered; count++)
	{
		taskName = g_TaskNameList[count];
		state = eInvalid;
		highWaterMark = 0;

		handle = xTaskGetHandle(taskName);
		if (handle != NULL)
		{
			state = eTaskGetState(handle);
			highWaterMark = uxTaskGetStackHighWaterMark(handle);
		}

		SerialPrintf("%2d %-15s 0x%08x %-7s %4d\n", count + 1, taskName, handle, GetTaskState(state), highWaterMark);
	}
}

///////////////////////////////////////////////////////////////////////////////
// char *GetTaskState(eTaskState state)

char *GetTaskState(eTaskState state)
{
	char *name = NULL;
	static const char *s[] = {"RUN", "READY", "BLOCK", "SUSP", "DEL", "INV"};

	name = (char *) (s[state]);

	return name;
}

void info_CPU(void)
{
	// setCpuFrequencyMhz(80);
	uint32_t cpuFreq = getCpuFrequencyMhz();
	uint8_t  nCores = ESP.getChipCores();
	BaseType_t coreId = xPortGetCoreID();
	uint64_t usSinceBoot = esp_timer_get_time();

	uint32_t freeheap = ESP.getFreeHeap();
	uint32_t cyclecount = ESP.getCycleCount();
	uint32_t freesketchspace = ESP.getFreeSketchSpace();
	uint32_t sketchsize = ESP.getSketchSize();
	uint8_t revision =  ESP.getChipRevision();

	const char *model = ESP.getChipModel();
	
	SerialPrintf("------------------------------------------\n");
	SerialPrintf("> chip model:   	   %s\n", model);
	SerialPrintf("> chip revision: 	   %d\n", revision);
	SerialPrintf("> number of cores:   %d\n", nCores);
	SerialPrintf("> CPU frequency:     %lu MHz\n", cpuFreq);
	SerialPrintf("> ESP32 core id:     %d\n", coreId);
	SerialPrintf("> time since boot:   %llu us\n", usSinceBoot);
	SerialPrintf("> free heap:   	   %lu bytes\n", freeheap);
	SerialPrintf("> sketch size:	   %lu\n", sketchsize);
	SerialPrintf("> free sketch space: %lu\n", freesketchspace);
	SerialPrintf("------------------------------------------\n");
}

///////////////////////////////////////////////////////////////////////////////
// void info_Version(void)

void info_Version(void)
{
	const char *version = ESP.getSdkVersion();

	SerialPrintf("------------------------------------------\n");
	SerialPrintf("> Build:     %s\n", __TIMESTAMP__);
	SerialPrintf("> ESP32 SDK: %s\n", version);
	SerialPrintf("> FreeRTOS:  %s\n", tskKERNEL_VERSION_NUMBER);
	SerialPrintf("------------------------------------------\n");
}
