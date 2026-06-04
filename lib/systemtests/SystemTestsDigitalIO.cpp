//////////////////////////////////////////////////////////////////////////////
//
// SystemTests.cpp
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
#include "LEDLib.h"
#include "ButtonLib.h"
#include "IOLib.h"
#include "SerialPrintf.h"
#include "Config.h"
#include "SystemTests.h"
#include "SystemTestsDigitalIO.h"


///////////////////////////////////////////////////////////////////////////////
// void test_LEDIO15(void)

void test_io_LEDIO15(void)
{
	led_Set(LED_IO15, true);
	taskSleep(200);
	led_Set(LED_IO15, false);
	taskSleep(200);
}

///////////////////////////////////////////////////////////////////////////////
// void test_io_LEDIO15_Repeat(void)

void test_io_LEDIO15_Repeat(void)
{
	while (true)
	{
		test_io_LEDIO15();
		test_ShowPass(1, 1);

		if (StopTest()) break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_io_LEDS_Repeat(void)

void test_io_LEDS_Repeat(void)
{
	uint32_t pass = 0;

	while (true)
	{
		test_io_LEDS();

		test_ShowPass(1, 1);
		if (StopTest()) break;
	}
}


///////////////////////////////////////////////////////////////////////////////
// void test_io_LEDS(void)
// also show values on the 6 LED's + LED IO15

void test_io_LEDS(void)
{
	uint8_t value = 0;

	// 1: all on
	io_SetOutput(0x3f);
	taskSleep(100);

	// 2: all off
	io_SetOutput(0x00);
	taskSleep(100);

	// 3: increment values from 0x00..0x3f
	for (value = 0x00; value <= 0x3f; value++)
	{
		io_SetOutput(value);
		taskSleep(50);
	}

	io_SetOutput(0x00);

	// 4: walking 1 to left
	test_io_WalkLeft();

	// 5: walking 1 to right
	test_io_WalkRight();

	test_io_LEDIO15();
}


///////////////////////////////////////////////////////////////////////////////
// void test_io_WalkLeft(void)

void test_io_WalkLeft(void)
{
	uint8_t ix = 0;
	uint8_t value = 0x01;

	for (ix = 0; ix < N_OUTPUT_BITS; ix++)
	{
		io_SetOutput(value);
		taskSleep(50);
		value = value << 1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_io_WalkRight(void)

void test_io_WalkRight(void)
{
	uint8_t ix = 0;
	uint8_t value = 0x01 << (N_OUTPUT_BITS - 1);

	for (ix = 0; ix < N_OUTPUT_BITS; ix++)
	{
		io_SetOutput(value);
		taskSleep(50);
		value = value >> 1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_io_LEDIO15_Button_Repeat(void)

void test_io_LEDIO15_Button_Repeat(void)
{
	SerialPrintf("> press button 0 to test\n");

	while (true)
	{
		if (button_IsPressed(0))
		{
			led_Set(LED_IO15, true);
		}
		else
		{
			test_io_LEDIO15();
			test_ShowPass(1, 1);
		}

		if (StopTest()) break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_io_Outputs_Button_Repeat(void)

void test_io_Outputs_Button_Repeat(void)
{
	SerialPrintf("> press button 0 to test\n");

	while (true)
	{
		if (button_IsPressed(0))
		{
			test_io_WalkLeft();
			test_io_WalkRight();
		}
		else
		{
			io_SetOutput(0xaa);
			taskSleep(250);
			io_SetOutput(0x55);
			taskSleep(250);
		}

		test_ShowPass(1, 1);
		if (StopTest()) break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t test_io_RW(uint8_t write)

uint8_t test_io_RW(uint8_t write)
{
	uint8_t read  = 0;
	uint8_t error = 0;

	write = write & 0x3f;	// maximum 6 IO bits!

	io_SetOutput(write);
	read = io_GetInput();
	if (read != write)
	{
		SerialPrintf("> R/W error: write = 0x%02x, read = 0x%02x\n", write, read);
		error++;
	}
	
	return error;
}

///////////////////////////////////////////////////////////////////////////////
// uint32_t test_io_Loopback(void)

uint32_t test_io_Loopback(void)
{
	uint8_t write = 0;
	uint8_t maxValue = 0x3f;
	uint16_t errorCount = 0;
	uint8_t ix = 0;

	// 1: increment
	for (write = 0; write <= maxValue; write++)
	{
		errorCount += test_io_RW(write);
	}

	// 2: walking 1
	for (ix = 0 ; ix < N_OUTPUT_BITS; ix++)
	{
		write = 0x01 << ix;
		errorCount += test_io_RW(write);
	}

	// 3: walking 0
	for (ix = 0 ; ix < N_OUTPUT_BITS; ix++)
	{
		write = ~(0x01 << ix);
		errorCount += test_io_RW(write);
	}

	// 4: flipping bits
	errorCount += test_io_RW(0x00);
	errorCount += test_io_RW(0xff);
	errorCount += test_io_RW(0xaa);
	errorCount += test_io_RW(0x55);

	return errorCount;
}


///////////////////////////////////////////////////////////////////////////////
// void test_io_Loopback_Repeat(void)

void test_io_Loopback_Repeat(void)
{
	uint32_t errorCount = 0;
	uint32_t pass = 0;

	while (true)
	{
		errorCount += test_io_Loopback();

		test_ShowPass(++pass, 100);
		test_ErrorReport("IO loopback test", pass, errorCount, 1000);

		if (StopTest()) break;
	}
}
