///////////////////////////////////////////////////////////////////////////////
//
// TaskADC.cpp
//
// Authors: 	Hans van der Linden
// Edit date: 	30-04-2026
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
#include "ADC3208Lib.h"

#include "TaskADC.h"

// Define a constant for the ADC capture interval (in milliseconds)
#define ADC_CAPTURE_INTERVAL_MS 200

// Define the ADC channels for the two potentiometers
#define ADC_POT_P0_CHANNEL 4
#define ADC_POT_P1_CHANNEL 5

// Local variables
static adc_values_t adcValues = {
	0, // P0
	0  // P1
};

static SemaphoreHandle_t adcMutex = NULL;

///////////////////////////////////////////////////////////////////////////////
// void task_ADC(void *pvParameters)

void task_ADC(void *pvParameters)
{
	// Create a mutex
	if (adcMutex == NULL) {
		adcMutex = xSemaphoreCreateMutex();
		if (adcMutex == NULL) {
			SerialPrintf("ERROR creating ADC mutex\n");
		} else {
			SerialPrintf("ADC mutex created\n");
		}
	}

	// Continuously read channels 4 and 5 from the ADC and store them
	while (true)
	{
		taskSleep(ADC_CAPTURE_INTERVAL_MS);
		// Enter mutual exclusion section to guarantee access to adcValues
		xSemaphoreTake(adcMutex, portMAX_DELAY);
        adcValues.potP0 = adc_ReadRaw(ADC_POT_P0_CHANNEL);
		// SerialPrintf("DEBUG: ADC chan 4 read: %u\n", adcValues.potP0);
		adcValues.potP1 = adc_ReadRaw(ADC_POT_P1_CHANNEL);
		// SerialPrintf("DEBUG: ADC chan 5 read: %u\n", adcValues.potP1);
		// Leave mutual exclusion section
		xSemaphoreGive(adcMutex);
		// For debugging
        // SerialPrintf("ADC Pot P0 = %u   Pot P1 = %u\n", adcValues.potP0, adcValues.potP1);
	}
	
	// Should never go here!
	vTaskDelete(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// adc_values_t adc_GetValues(void)

adc_values_t adc_GetValues(void)
{
	adc_values_t adcValuesCopy; // We need a local copy to return after we leave the mutex
	if (adcMutex == NULL) {
		// Mutex not yet initialized, so return dummy data
		adcValuesCopy.potP0 = 0;
		adcValuesCopy.potP1 = 0;
	} else {
		// SerialPrintf("DEBUG adc_GetValues trying to take semaphore\n");
		// Grab the mutex to enter the mutual exclusion section
		xSemaphoreTake(adcMutex, portMAX_DELAY);
		// SerialPrintf("DEBUG adc_GetValues success!\n");
		adcValuesCopy = adcValues;
		// SerialPrintf("DEBUG adc adcValues copied: %u %u\n", adcValuesCopy.potP0, adcValuesCopy.potP1);
		// Return the mutex to leave the mutual exclusion section
		xSemaphoreGive(adcMutex);
	}
	return adcValuesCopy;
}