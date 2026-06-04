//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsInterrupt.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	26-07-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "Config.h"
#include "InterruptLib.h"
#include "IOLib.h"
#include "ButtonLib.h"
#include "TaskSleep.h"
#include "SerialPrintf.h"
#include "SystemTests.h"
#include "SystemTestsInterrupt.h"

///////////////////////////////////////////////////////////////////////////////
// Interrupt service routine (ISR), runs outside FreeRTOS scheduler! Keep as
// short as possible!

static uint8_t g_InterruptCount = 0;
bool g_InterruptFlag = false;

void IRAM_ATTR hardware_ISRTest(void)
{
	g_InterruptCount++;
}

///////////////////////////////////////////////////////////////////////////////
// void test_Interrupts_Repeat(void)

void test_Interrupts_Repeat(void)
{
	uint32_t errorCount = 0;
	uint32_t pass = 0;

	while (true)
	{
		errorCount += test_Interrupts();
		
		test_ShowPass(++pass, 1000);
		test_ErrorReport("interrupt test", pass, errorCount, 10000);

		if (StopTest()) break;
	}
}


///////////////////////////////////////////////////////////////////////////////
// uint16_t test_Interrupts(void)

uint16_t test_Interrupts(void)
{
	uint16_t errors = 0;
	int16_t  gpioNumber = -1;
	uint8_t  iobit = 0;

	g_InterruptCount = 0;

	io_SetOutput(0x00);

	// 1: attach all digital inputs to the same interrupt handler

	for (iobit = 0; iobit < N_INPUT_BITS; iobit++)
	{
		gpioNumber = io_GetGPIONumberInput(iobit);
		if (gpioNumber > 0)
		{
			interrupt_AttachHandler(hardware_ISRTest, gpioNumber, RISING);
			interrupt_Enable(gpioNumber);
		}
	}

	// 2: interrupt count should still be zero

	if (g_InterruptCount != 0)
	{
		SerialPrintf("> interrupt error [1]: interrupt count = %d. Expected: 0\n", g_InterruptCount);
		g_InterruptCount = 0;
		errors++;
	}

	// 3: generate an interrupt on each input bit

	for (iobit = 0; iobit < N_INPUT_BITS; iobit++)
	{
		io_SetBit(iobit, true);		// leading edge should generate an interrupt
		io_SetBit(iobit, false);
	}

	// 4: check if these interrupts are counted

	if (g_InterruptCount != N_INPUT_BITS)
	{
		SerialPrintf("> interrupt error [2]: interrupt count = %d. Expected: %d\n", g_InterruptCount, N_INPUT_BITS);
		errors++;
	}

	// 5: disable interrupts

	for (iobit = 0; iobit < N_INPUT_BITS; iobit++)
	{
		gpioNumber = io_GetGPIONumberInput(iobit);
		if (gpioNumber > 0)
		{
			interrupt_Disable(gpioNumber);
		}
	}

	// 6: generate an interrupt on each input bit, should be discarded now

	g_InterruptCount = 0;

	for (iobit = 0; iobit < N_INPUT_BITS; iobit++)
	{
		io_SetBit(iobit, true);		// should NOT generate an interrupt
		io_SetBit(iobit, false);
	}
	
	if (g_InterruptCount != 0)
	{
		SerialPrintf("> interrupt error [3]: interrupt count = %d. Expected: 0\n", g_InterruptCount);
		g_InterruptCount = 0;
		errors++;
	}

	// 7: detach all digital inputs from interrupt handler

	for (iobit = 0; iobit < N_INPUT_BITS; iobit++)
	{
		gpioNumber = io_GetGPIONumberInput(iobit);
		if (gpioNumber > 0)
		{
			interrupt_DetachHandler(gpioNumber);
		}
	}

	// 8: generate an interrupt on each input bit, should also be discarded now

	g_InterruptCount = 0;

	for (iobit = 0; iobit < N_INPUT_BITS; iobit++)
	{
		io_SetBit(iobit, true);		// should NOT generate an interrupt
		io_SetBit(iobit, false);
	}
	
	if (g_InterruptCount != 0)
	{
		SerialPrintf("> interrupt error [4]: interrupt count = %d. Expected: 0\n", g_InterruptCount);
		g_InterruptCount = 0;
		errors++;
	}

	io_SetOutput(0x00);

	// SerialPrintf("> interrupt errors: %d\n", errors);

	return errors;
}


///////////////////////////////////////////////////////////////////////////////
// Interrupt service routine (ISR) for button handler, runs outside FreeRTOS
// scheduler! Keep as short as possible!
// uses debounce code for button...

void IRAM_ATTR hardware_ButtonISRTest(void)
{
	// debounce part
	static int64_t lMillis = 0;
	
	 if((millis() - lMillis) < 500) 
	{
		SerialPrintf("*");
		return;
	}
	lMillis = millis(); 
	// end of debounce part

	g_InterruptCount++;
	g_InterruptFlag = true;
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t test_ButtonInterrupt_Repeat(void)

uint16_t test_ButtonInterrupt_Repeat(void)
{
	uint16_t errors = 0;
	int16_t gpioNumber = BUTTON_PIN;
	uint8_t iobit = 0;

	g_InterruptCount = 0;

	interrupt_AttachHandler(hardware_ButtonISRTest, gpioNumber, FALLING);
	interrupt_Enable(gpioNumber);

	SerialPrintf("> press button B0 to generate interrupt - might bounce...\n");

	if (g_InterruptCount != 0)
	{
		SerialPrintf("> button interrupt error: interrupt count = %d. Expected: 0\n", g_InterruptCount);
		g_InterruptCount = 0;
		errors++;
	}

	while (true)
	{
		while (g_InterruptFlag == false)
		{
			taskSleep(0);
		}

		SerialPrintf("> button interrupt count = %d\n", g_InterruptCount);

		// clear flag for next round
		g_InterruptFlag = false;
	}
}
