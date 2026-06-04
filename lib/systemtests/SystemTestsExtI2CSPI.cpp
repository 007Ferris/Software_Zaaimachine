//////////////////////////////////////////////////////////////////////////////
//
// SystemTestExtI2CSPI.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	09-08-2025
//				17-08-2025
//
// The external I2C and SPI connections are tested with serial EEPROMS
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "Config.h"
#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "I2C_eeprom.h"
#include "EepromLib.h"
#include "SystemTestsExtI2CSPI.h"
#include "SystemTests.h"

///////////////////////////////////////////////////////////////////////////////
// void test_EEPROM_Repeat(eeprom_type_t type)

void test_EEPROM_Repeat(eeprom_type_t type)
{
	uint32_t errorCount = 0;
	uint32_t pass = 0;
	char msg[40];

	sprintf(msg, "%s EEPROM memory test", type == EEPROM_I2C ? "I2C" : "SPI");

	while (true)
	{
		errorCount += test_EEPROM(type);
		test_ShowPass(++pass, 1);
		test_ErrorReport(msg, pass, errorCount, 1);

		if (StopTest()) break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint32_t test_EEPROM(eeprom_type_t type)

uint32_t test_EEPROM(eeprom_type_t type)
{
	uint32_t errorCount = 0;
	uint32_t eepromSize = 0;
	uint8_t data = 0;
	const char *id = (type == EEPROM_I2C) ? "I2C" : "SPI";
	const uint8_t testdata[] = {0x00, 0xff, 0xaa, 0x55, 0x0f, 0xf0};
	const uint8_t size = sizeof(testdata) / sizeof(uint8_t);

	eepromSize = eeprom_GetDeviceSize(type);
	SerialPrintf("> Testing EEPROM [%s], size: %d bytes\n", id, eepromSize);

	for (uint8_t ix = 0; ix < size; ix++)
	{
		data = testdata[ix];
		SerialPrintf("> EEPROM test data [%s]: 0x%02x\n", id, data);
		errorCount += eeprom_Fill(type, data);
		errorCount += eeprom_Verify(type, data);
	}

	#if (EEPROM_INCREMENT_TEST == 1)
	data = 0;
	SerialPrintf("> EEPROM test increment [%s], writing", id);
	errorCount += eeprom_FillIncrement(type, data);
	SerialPrintf("verifying...");
	errorCount += eeprom_VerifyIncrement(type, data);
	SerialPrintf("\n");
	#endif
	
	// SerialPrintf("> EEPROM errors [%s]: %d\n", id, errorCount);

	return errorCount;
}
