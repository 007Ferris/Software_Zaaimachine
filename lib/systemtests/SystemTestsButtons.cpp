//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsButtons.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	13-09-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "Config.h"
#include "ADC3208Lib.h"
#include "ButtonLib.h"
#include "TaskSleep.h"
#include "SerialPrintf.h"
#include "SystemTests.h"
#include "SystemTestsButtons.h"

///////////////////////////////////////////////////////////////////////////////
// void test_Buttons_Repeat(void)

void test_Buttons_Repeat(void)
{
	uint8_t button = 0;

	SerialPrintf("> press one or more buttons B2..B0\n");
	
	while (true)
	{
		for (button = 0; button < N_BUTTONS; button++)
		{
			if (button_IsPressed(button))
			{
				SerialPrintf("> button pressed: B%d\n", button);
				taskSleep(500);
			}
		}
	}
}
