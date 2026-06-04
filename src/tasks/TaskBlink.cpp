///////////////////////////////////////////////////////////////////////////////
//
// TaskBlink.cpp
//
// Authors: 	Hans van der Linden
// Edit date: 	03-05-2026
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
#include "IOLib.h"

#include "TaskADC.h"
#include "TaskDisplay.h"
#include "TaskBlink.h"

///////////////////////////////////////////////////////////////////////////////
// void task_Blink(void* pvParameters)

void task_Blink(void *pvParameters)
{
    SerialPrintf("DEBUG Blink task running...\n");
    blink_params_t blinkParams = *((blink_params_t *) pvParameters); // Make a local copy
    adc_values_t adcValues;
    double frequency;
    uint32_t intervalMs;
    double inputVal; // Scaled 0.0..1.0
    bool isSet = false;

	// Continuously blink our own LED with the frequency given by the indicated pot
	while (true)
	{
        io_SetBit(blinkParams.outputPinNumber, isSet);
        isSet = !isSet;
        adcValues = adc_GetValues();
        // SerialPrintf("DEBUG Blink adc values are %u %u\n", adcValues.potP0, adcValues.potP1);
        if (blinkParams.inputPotNumber == BLINK_INPUT_POT0)
        {
            inputVal = (double) adcValues.potP0 / (double) ADC_MAX_VALUE;
        }
        else if (blinkParams.inputPotNumber == BLINK_INPUT_POT1)
        {
            inputVal = (double) adcValues.potP1 / (double) ADC_MAX_VALUE;
        }
        else
        {
            inputVal = 0.5; // Some default value
            SerialPrintf("ERROR unknown input pot number selected\n");
        }
        frequency = blinkParams.minFrequencyHz + 
            (blinkParams.maxFrequencyHz - blinkParams.minFrequencyHz) * inputVal;
        intervalMs = (uint32_t) (500.0 / frequency); // toggle twice per period, so interval is half the period
        if (!display_SendFreqData(blinkParams.inputPotNumber, frequency)) {
            SerialPrintf("ERROR couldn't send freq data to display\n");
        }
        // SerialPrintf("DEBUG Blink pin %d freq is %f, interval is %u\n", blinkParams.outputPinNumber, frequency, intervalMs);
		taskSleep(intervalMs);
	}
	
	// Should never go here!
	vTaskDelete(NULL);
}
