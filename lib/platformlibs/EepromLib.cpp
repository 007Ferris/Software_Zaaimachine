///////////////////////////////////////////////////////////////////////////////
//
// EepromLib.cpp
//
// Created: 13-08-2025
// Author:  Roel Smeets
//
// see also:
// https://github.com/wollewald/EEPROM_SPI_WE
// https://github.com/dndubins/EEPROMsimple
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "Arduino.h"
#include "Config.h"
#include "SerialPrintf.h"
#include "SPILib.h"
#include "I2C_eeprom.h"
#include "SPIeeprom.h"
#include "EepromLib.h"


///////////////////////////////////////////////////////////////////////////////
//	global EEPROM objects for I2C version and SPI version

I2C_eeprom i2c_eeprom(I2C_ADDRESS_EEPROM, I2C_DEVICESIZE_24LC64);
SPI_eeprom spi_eeprom(SPI_DEVICE_EXT_5, EEPROM_KBITS_64, EEPROM_PAGE_SIZE_32);

///////////////////////////////////////////////////////////////////////////////
// bool eeprom_Init(eeprom_type_t type)

bool eeprom_Init(eeprom_type_t type)
{
	bool success = false;
	
	if (type == EEPROM_I2C)
	{
		success = i2c_eeprom.begin();
	}
	else
	{
		success = spi_eeprom.begin();
	}

	SerialPrintf("> EEPROM init %s\n", success ? "OK" : "FAILED");

	return success;
}

///////////////////////////////////////////////////////////////////////////////
// uint32_t eeprom_GetDeviceSize(eeprom_type_t type)

uint32_t eeprom_GetDeviceSize(eeprom_type_t type)
{
	uint32_t deviceSize = 0;
	
	if (type == EEPROM_I2C)
	{
		deviceSize = i2c_eeprom.getDeviceSize();
	}
	else
	{
		deviceSize = spi_eeprom.getDeviceSize();
	}	

	return deviceSize;
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t eeprom_Fill(eeprom_type_t type, uint8_t value)

uint16_t eeprom_Fill(eeprom_type_t type, uint8_t value)
{
	uint16_t startAddress = 0;
	uint16_t deviceSize   = 0;
	int16_t  result	= 0;

	deviceSize = eeprom_GetDeviceSize(type);

	if (type == EEPROM_I2C)
	{
		result = i2c_eeprom.setBlock(startAddress, value, deviceSize);
	}
	else
	{
		result = spi_eeprom.setBlock(startAddress, value, deviceSize);
	}	

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t eeprom_Verify(eeprom_type_t type, uint8_t value)

uint16_t eeprom_Verify(eeprom_type_t type, uint8_t value)
{
	uint16_t errorCount	= 0;
	uint16_t address	= 0;
	uint8_t  byteRead	= 0;
	uint16_t deviceSize = 0;

	deviceSize = eeprom_GetDeviceSize(type);

	for (address = 0; address < deviceSize; address++)
	{
		if (type == EEPROM_I2C)
		{
			byteRead = i2c_eeprom.readByte(address);
		}
		else
		{
			byteRead = spi_eeprom.readByte(address);
		}

		if (byteRead != value)
		{
			errorCount++;
			SerialPrintf("> EEPROM verify failed at address 0x%0x\n", address);
			SerialPrintf("> 	data = 0x%02x, expected: 0x%02x\n", byteRead, value);
		}
	}

	return errorCount;
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t eeprom_VerifyIncrement(eeprom_type_t type, uint8_t startValue)

uint16_t eeprom_VerifyIncrement(eeprom_type_t type, uint8_t startValue)
{
	uint16_t errorCount = 0;
	uint16_t address	= 0;
	uint8_t  byteRead	= 0;
	uint8_t  expect		= 0;
	uint16_t deviceSize = 0;

	deviceSize = eeprom_GetDeviceSize(type);

	for (address = 0; address < deviceSize; address++)
	{
		if (type == EEPROM_I2C)
		{
			byteRead = i2c_eeprom.readByte(address);
		}
		else
		{
			byteRead = spi_eeprom.readByte(address);
		}

		expect = startValue + address;
		if (byteRead != expect)
		{
			errorCount++;
			SerialPrintf("> EEPROM increment verify failed at address 0x%04x\n", address);
			SerialPrintf("> 	data = 0x%02x, expected: 0x%02x\n", byteRead, expect);
		}
	}

	return errorCount;
}


///////////////////////////////////////////////////////////////////////////////
// uint16_t eeprom_FillIncrement(eeprom_type_t type, uint8_t startValue)
//
// little bit slow, not very efficient, perhaps make a faster version 
// with a 32-byte buffer to write as a single EEPROM page?

uint16_t eeprom_FillIncrement(eeprom_type_t type, uint8_t startValue)
{
	uint16_t address = 0;
	int16_t result = 0;
	uint16_t deviceSize = 0;

	deviceSize = eeprom_GetDeviceSize(type);

	for (address = 0; address < deviceSize; address++)
	{
		if (type == EEPROM_I2C)
		{
			result += i2c_eeprom.writeByte(address, startValue + address);
		}
		else
		{
			result += spi_eeprom.writeByte(address, startValue + address);
		}

		if ((address % 512) == 0)
		{
			SerialPrintf(".");
		}
	}

	return result;
}
