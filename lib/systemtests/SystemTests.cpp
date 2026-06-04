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
#include "ButtonLib.h"
#include "SerialPrintf.h"
#include "InterruptLib.h"
#include "LEDLib.h"

#include "SystemTests.h"
#include "SystemTestsDAC.h"
#include "SystemTestsADC.h"
#include "SystemTestsOLED.h"
#include "SystemTestsDigitalIO.h"
#include "SystemTestsQuadrature.h"
#include "SystemTestsInterrupt.h"
#include "SystemTestsUART.h"
#include "SystemTestsExtI2CSPI.h"
#include "SystemTestsButtons.h"

// #include "CommandParser.h"

///////////////////////////////////////////////////////////////////////////////
// uint32_t ReadNumber(void)

uint32_t ReadNumber(void)
{
	int32_t number = 0;
	String  inputString = "";
	char inChar = '\0';

	// wait for input
	while (Serial.available() == 0)
	{
	}

	// read the input, only digits allowed
	while (Serial.available() > 0)
	{
		inChar = Serial.read();
		if (isdigit(inChar))
		{
			inputString += inChar;
		}
	}

	number = inputString.toInt();

	return number;
}


///////////////////////////////////////////////////////////////////////////////
// uint8_t ReadChoice(uint8_t maxChoice)

uint8_t ReadChoice(uint8_t maxChoice)
{
	uint8_t choice = 0;

	do
	{
		SerialPrintf("enter choice > ");
		Serial.flush();
		choice = ReadNumber();
		SerialPrintf("%d\n", choice);
		if (choice <= maxChoice)
		{
			break;
		}
		SerialPrintf("choice must be <= %d\n", maxChoice);
	} while (true);

	return choice;
}

///////////////////////////////////////////////////////////////////////////////
// void FlushInput(void)

void FlushInput(void)
{
	while (Serial.available() != 0)
	{
		char inchar = Serial.read();
	}

}

///////////////////////////////////////////////////////////////////////////////
// bool StopTest(void)

bool StopTest(void)
{
	bool stop = false;
	char inchar = '\0';

	// just look for a 'q' or a 'Q':

	while (Serial.available() != 0)
	{
		inchar = Serial.read();
		if ((inchar == 'q') || (inchar == 'Q'))
		{
			stop = true;
			FlushInput();	// skip the rest!
		}
	}

	return stop;
}


///////////////////////////////////////////////////////////////////////////////
// void test_ShowPass(uint32_t pass, uint32_t moduloCount)

void test_ShowPass(uint32_t pass, uint32_t moduloCount)
{
	if ((pass % moduloCount) == 0)
	{
		SerialPrintf(".");
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_ErrorReport(uint32_t pass, uint32_t errors, uint32_t moduloCount)

void test_ErrorReport(const char *nameOfTest, uint32_t pass, uint32_t nErrors, uint32_t modulo)
{
	if ((pass % modulo) == 0)
	{
		SerialPrintf("\n--- %s ---\n", nameOfTest);
		SerialPrintf("   pass   : %ld\n", pass);
		SerialPrintf("   errors : %ld\n", nErrors);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void RunSystemTests(void)

extern xTaskHandle handle_CLITask;

void RunSystemTestsMenu(void)
{
	uint8_t choice = 0;
	bool manual = true;
	bool verbose = true;
	bool internal = true;

	// take over the keyboard handler for menu selections!
	vTaskSuspend(handle_CLITask);

	while (true)
	{
		SerialPrintf("* INSERT LOOPBACK CONNECTORS !! *\n");
		SerialPrintf("* make a selection:\n");
		SerialPrintf("--- LED & buttons tests ---\n");
		SerialPrintf("1.     LED IO15 test\n");
		SerialPrintf("2.     LED IO15 test & buttons test\n");
		SerialPrintf("3.     IO LED test\n");
		SerialPrintf("4.     IO LED & buttons test\n");
		SerialPrintf("5.     IO loopback test\n");
		SerialPrintf("--- OLED test ---\n");
		SerialPrintf("6.     OLED test\n");
		SerialPrintf("--- DAC tests ---\n");
		SerialPrintf("7.     DAC increment test, all channels\n");
		SerialPrintf("8.     DAC output voltage test (manual)\n");
		SerialPrintf("--- ADC tests ---\n");
		SerialPrintf("9.     ADC read single channel\n");
		SerialPrintf("10     ADC read all channels\n");
		SerialPrintf("11.    ADC/DAC loopback test (manual)\n");
		SerialPrintf("--- Quadrature tests ---\n");
		SerialPrintf("12.    register R/W tests\n");
		SerialPrintf("13.    count & index test\n");
		SerialPrintf("--- UART tests ---\n");
		SerialPrintf("14.    register R/W test\n");
		SerialPrintf("15.    internal loopback test\n");
		SerialPrintf("16.    external loopback test\n");
		SerialPrintf("--- interrupt tests ---\n");
		SerialPrintf("17.    interrupt test\n");
		SerialPrintf("--- external SPI/I2C tests ---\n");
		SerialPrintf("18.    I2C memory test\n");
		SerialPrintf("19.    SPI memory test\n");
		SerialPrintf("--- batch test ---\n");
		SerialPrintf("20.    batch test\n");

		choice = ReadChoice(20);

		switch (choice)
		{
		case 1:
			test_io_LEDIO15_Repeat();
			break;
		case 2:
			test_io_LEDIO15_Button_Repeat();
			break;
		case 3:
			test_io_LEDS_Repeat();
			break;
		case 4:
			test_io_Outputs_Button_Repeat();
			break;
		case 5:
			test_io_Loopback_Repeat();
			break;
		case 6:
			test_OLED_Repeat();
			break;
		case 7:
			test_DAC_Increment_Repeat();
			break;
		case 8:
			test_DAC_SetOutputVoltage_Repeat();
			break;
		case 9:
			test_ADC_ReadSingleChannel();
			break;
		case 10:
			test_ADCReadAll_Repeat();
			break;
		case 11:
			test_ADCDAC03_Loopback_Repeat(manual, verbose);
			break;
		case 12:
			test_Quadrature_Registers_RW();
			break;
		case 13:
			test_Quadrature_CountIndex();
			break;
		case 14:
			test_uart_RW_Repeat();
			break;
		case 15:
			internal = true;
			test_uart_LoopbackTest_Repeat(internal);
			break;
		case 16:
			internal = false;
			test_uart_LoopbackTest_Repeat(internal);
			break;
		case 17:
			test_Interrupts_Repeat();
			break;
		case 18:
			test_EEPROM_Repeat(EEPROM_I2C);
			break;
		case 19:
			test_EEPROM_Repeat(EEPROM_SPI);
			break;
		case 20:
			test_BatchTest();
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// void RunSystemTests(void)

void RunSystemTests(void)
{
	bool verbose = false;
	bool internalLoopback = false;
	bool manual = true;
	uint8_t channel = 0;

	while (true)
	{
		// // digital IO tests
		// test_io_LEDIO15_Repeat();				// OK
		// test_io_LEDIO15_Button_Repeat();			// OK
		// test_io_LEDS();							// OK
		// test_io_Outputs_Button_Repeat();			// OK
		// test_io_Loopback_Repeat();				// OK

		// // OLED tests
		// test_OLED_Repeat();						// OK

		// // DAC tests
		// test_DAC_Increment_Repeat();				// OK
		// test_DAC_SetOutputVoltage_Repeat();		// OK

		// //ADC tests
		// test_ADCReadSingle_Repeat(channel = 5);	// OK
		// test_ADCDAC03_Loopback(manual = true, verbose = true);				// OK
		// test_ADCReadAll_Repeat();				// OK
		// test_ADCReadAllMultiple_Repeat();		// OK

		// test_Buttons_Repeat();					// OK

		// // quadrature tests
		// test_qc_RW(channel = 0);					// OK
		// test_qc_RW(channel = 1);					// OK
		// test_qc_CountExternal_Repeat();			// OK
		// test_qc_IndexExternal_Repeat();
		// test_qc_CountSimulated(verbose = true);				// OK
		// test_qc_IndexSimulated(channel = 0, verbose = true);	// OK
		// test_qc_IndexSimulated(channel = 1, verbose = true);	// OK

		// // UART tests
		// test_uart_RWTest();									// OK
		// test_uart_LoopbackTest(internalLoopback = true);		// OK
		// test_uart_LoopbackTest(internalLoopback = false);	// OK
		// test_uart_StreamData_Repeat();						// OK
		// test_uart_Echo_Repeat();								// OK

		// external I2C/SPI tests with EEPROM's
		// test_EEPROM(EEPROM_I2C);						// OK
		// test_EEPROM(EEPROM_SPI);						// OK

		// interrupt test on digital inputs
		test_ButtonInterrupt_Repeat(); // OK, might bounce
									   // test_Interrupts();							// OK

		// test_BatchTest();
	}
}



///////////////////////////////////////////////////////////////////////////////
// void test_WaitForButton(void)

void test_WaitForButton(void)
{
	while (button_IsPressed(0) == false)
	{
		// wait until button pressed
	}
	while (button_IsPressed(0) == true)
	{
		// now wait until button released
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_BatchTest(void)

void test_BatchTest(void)
{
	uint32_t errors_io = 0;
	uint32_t pass = 0;
	uint32_t errors = 0;
	uint8_t channel = 0;
	bool internalLoopback = false;
	bool verbose = false;
	bool manual = false;

	while (true)
	{
		SerialPrintf("> LED test\n");
		test_io_LEDS();

		SerialPrintf("> I/O loopback test\n");
		errors += test_io_Loopback();

		SerialPrintf("> ADC/DAC loopback test\n");
		errors += test_ADCDAC03_Loopback(manual = false, verbose = false);

		SerialPrintf("> interrupt test\n");
		errors += test_Interrupts();

		SerialPrintf("> quadrature test\n");
		errors += test_qc_RW(channel = 0);
		errors += test_qc_RW(channel = 1);
		errors += test_qc_IndexSimulated(channel = 0, verbose = false);
		errors += test_qc_IndexSimulated(channel = 1, verbose = false);
		errors += test_qc_CountSimulated(verbose = false);

		SerialPrintf("> UART test\n");
		errors += test_uart_RWTest();
		errors += test_uart_LoopbackTest(internalLoopback = true);
		errors += test_uart_LoopbackTest(internalLoopback = false);

		SerialPrintf("> I2C test\n");
		errors += test_EEPROM(EEPROM_I2C);

		SerialPrintf("> SPI test\n");
		errors += test_EEPROM(EEPROM_SPI);

		pass++;
		test_ErrorReport("batch test", pass, errors, 1);

		if (StopTest()) break;
	}
}
