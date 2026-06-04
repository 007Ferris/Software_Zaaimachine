//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsOLED.cpp
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

#include "TaskSleep.h"
#include "OLEDLibESP32.h"
#include "SystemTests.h"
#include "SystemTestsOLED.h"

///////////////////////////////////////////////////////////////////////////////
// void test_OLED_Repeat(void)

void test_OLED_Repeat(void)
{
	while (true)
	{
		test_OLED();

		test_ShowPass(1, 1);
		if (StopTest()) break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_OLED(void)

void test_OLED(void)
{
	uint8_t line = 0;
	uint16_t count = 0;
	char msg[80];

	oled_Clear();

	// 1: left align
	for (line = 0; line < OLED_NLINES; line++)
	{
		oled_WriteLine(line, "OLED left", ALIGN_LEFT);
	}

	taskSleep(1000);
	oled_Clear();

	// 2: right align
	for (line = 0; line < OLED_NLINES; line++)
	{
		oled_WriteLine(line, "OLED right", ALIGN_RIGHT);
	}

	taskSleep(1000);
	oled_Clear();

	// 3: center align
	for (line = 0; line < OLED_NLINES; line++)
	{
		oled_WriteLine(line, "OLED center", ALIGN_CENTER);
	}

	taskSleep(1000);
	oled_Clear();

	for (count = 0; count < 50; count++)
	{
		sprintf(msg, "count = %3d", count);
		for (line = 0; line < OLED_NLINES; line++)
		{
			oled_WriteLine(line, msg, ALIGN_CENTER);
		}
		oled_Clear();
		taskSleep(100);
	}
}
