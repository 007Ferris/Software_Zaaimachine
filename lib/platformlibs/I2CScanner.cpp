/*
 *  I2CScanner.cpp
 *
 *  Created: 23-5-2023 18:58:48
 *  Author: Roel Smeets
 */ 


///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"
#include <Wire.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "I2CScanner.h"

///////////////////////////////////////////////////////////////////////////////
// uint8_t i2cScanBus(void)
// returns nuber of I2C devices found

uint8_t i2c_ScanBus(void)
{
	uint8_t error = 0;
	uint8_t address = 0;
	uint8_t deviceCount = 0;
	
	SerialPrintf("> Scanning I2C bus...\n");
	
	for(address = 1; address <= I2C_MAX_ADDRESS; address++)
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == I2C_SUCCESS)
		{
			SerialPrintf("> I2C device found at address 0x%02x\n", address);
			deviceCount++;
		}
		else if (error == I2C_ERROR)
		{
			SerialPrintf("> Unknown error at address 0x%02x\n", address);
		}
	}
	
	return deviceCount;
}
