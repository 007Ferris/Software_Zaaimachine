//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsDAC.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "Config.h"
#include "DAC4922Lib.h"
#include "TaskSleep.h"
#include "SerialPrintf.h"
#include "SystemTests.h"
#include "SystemTestsDAC.h"

///////////////////////////////////////////////////////////////////////////////
// void test_DAC_Increment_Repeat(void)

void test_DAC_Increment_Repeat(void)
{
	uint8_t dacChannel = 0;
	uint16_t dacValue  = 0;

	while (true)
	{
		for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
		{
			for (dacValue = DAC_MIN_VALUE; dacValue <= DAC_MAX_VALUE; dacValue++)
			{
				dac_Write(dacChannel, dacValue);
			}
			dac_Write(dacChannel, 0);
		}

		test_ShowPass(1, 1);
		if (StopTest())
		{
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_DAC_SetOutputVoltage(void)

const float g_DACOutvoltageTable[] =
{
	-9.5,
	-5.0,
	-2.5,
	-0.5,
	 0.0,
	+0.5,
	+2.5,
	+5.0,
	+9.5
};

const uint8_t g_NrOfDACValues = sizeof(g_DACOutvoltageTable) / sizeof(float);

///////////////////////////////////////////////////////////////////////////////
// void test_DAC_SetOutputVoltage_Repeat(void)

void test_DAC_SetOutputVoltage_Repeat(void)
{
	uint8_t ix = 0;
	float dacOutputVoltage = 0.0;

	while (true)
	{
		for (ix = 0; ix < g_NrOfDACValues; ix++)
		{
			dacOutputVoltage = g_DACOutvoltageTable[ix];
			SerialPrintf("> setting all DAC channels to %.3f volt, press button B0 for next value...\n", dacOutputVoltage);
			dac_SetOutputVoltageAll(dacOutputVoltage);
			test_WaitForButton();
		}
		
		if (StopTest())	break;
	}
}
