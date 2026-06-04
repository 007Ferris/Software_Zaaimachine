///////////////////////////////////////////////////////////////////////////////
//
// SPIeeprom.cpp
//
// Created: 01-09-2024
//			14-08-2025
// Author:  Roel Smeets
//
// class library for 25LC640 EEPROM using SPI bus
//
// see also:
// https://github.com/wollewald/EEPROM_SPI_WE
// https://github.com/dndubins/EEPROMsimple
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"
#include <SPI.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "Config.h"
#include "SerialPrintf.h"
#include "SPIeeprom.h"
#include "SPILib.h"

///////////////////////////////////////////////////////////////////////////////
// constructors

SPI_eeprom::SPI_eeprom(uint8_t spiDeviceNumber, eeprom_size_t deviceSize)
{
	_spiDeviceNumber = spiDeviceNumber;
	_deviceSize 	 = deviceSize;
	_pageSize 		 = EEPROM_PAGE_SIZE_32;

	_spisettings._clock = SPI_EEPROM_SPEED;
}

SPI_eeprom::SPI_eeprom(uint8_t spiDeviceNumber, eeprom_size_t deviceSize, eeprom_pagesize_t pageSize)
{
	_spiDeviceNumber = spiDeviceNumber;
	_deviceSize 	 = deviceSize;
	_pageSize 		 = pageSize;

	_spisettings._clock = SPI_EEPROM_SPEED;
}

///////////////////////////////////////////////////////////////////////////////
// functions


///////////////////////////////////////////////////////////////////////////////
// void setDeviceSize(eeprom_size_t memorySize)

void SPI_eeprom::setDeviceSize(eeprom_size_t memorySize)
{
	_deviceSize = memorySize;
}

///////////////////////////////////////////////////////////////////////////////
// uint32_t getDeviceSize(void)

uint32_t SPI_eeprom::getDeviceSize(void)
{
	return _deviceSize;
}

///////////////////////////////////////////////////////////////////////////////
// void setPageSize(eeprom_size_t size)

void SPI_eeprom::setPageSize(eeprom_pagesize_t pageSize)
{
	_pageSize = pageSize;
}

///////////////////////////////////////////////////////////////////////////////
// uint32_t getPageSize(void)

uint32_t SPI_eeprom::getPageSize(void)
{
	return _pageSize;
}

///////////////////////////////////////////////////////////////////////////////
// bool begin(void)

bool SPI_eeprom::begin(void)
{
	bool result = true;

	spi_Init();

	spi_SelectDevice(_spiDeviceNumber);
	spi_DeselectDevice();

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t readByte(uint32_t address)

uint8_t SPI_eeprom::readByte(uint32_t address)
{
	uint8_t value = 0;

	readBuffer(address, &value, 1);

	return value;
}


///////////////////////////////////////////////////////////////////////////////
// bool writeByte(uint32_t address, uint8_t value)

int16_t SPI_eeprom::writeByte(uint32_t address, uint8_t value)
{
	uint8_t valRead = 0;

	valRead = readByte(address);

    if (valRead != value) // write needed
	{
        writeBuffer(address, &value, 1);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// void readBuffer(uint32_t addr, uint8_t *buf, uint16_t sizeOfBuf)

void SPI_eeprom::readBuffer(uint32_t addr, uint8_t *buf, uint16_t sizeOfBuf)
{    
	spi_SelectDevice(_spiDeviceNumber);
	spi_BeginTransaction(_spisettings);
	spi_TransferByte(EEP_READ);
	spi_TransferWord(addr);

    for(uint16_t i = 0; i < sizeOfBuf; i++)
	{
        buf[i] = spi_TransferByte(0x00);
	}

	spi_EndTransaction();
	spi_DeselectDevice();
}


///////////////////////////////////////////////////////////////////////////////
// void writeBuffer(uint32_t addr, const uint8_t *buf, uint16_t sizeOfBuf, 
//					bool noIncrement)

void SPI_eeprom::writeBuffer(uint32_t addr, const uint8_t *buf, uint16_t sizeOfBuf, bool noIncrement)
{
	uint16_t noOfBytesStillToWrite = sizeOfBuf;
    uint16_t arrayIndex = 0;
    
    while (noOfBytesStillToWrite != 0)
	{
        uint16_t chunk = noOfBytesStillToWrite;
        uint16_t positionInPage = addr % _pageSize;
        uint16_t spaceLeftInPage = _pageSize - positionInPage;
        
        if(spaceLeftInPage < noOfBytesStillToWrite)
		{
            chunk = spaceLeftInPage;
        }

		// prepare for write operation of a chunk
		spi_SelectDevice(_spiDeviceNumber);
		spi_TransferByte(EEP_WREN);
		spi_DeselectDevice();

		// start the write operation
		spi_SelectDevice(_spiDeviceNumber);
		spi_BeginTransaction(_spisettings);
		spi_TransferByte(EEP_WRITE);
		spi_TransferWord(addr);

		// write data
        for(uint16_t i = arrayIndex; i < (arrayIndex + chunk); i++)
		{
			if (noIncrement)	// in case of filling a block with constant data
			{
    	        spi_TransferByte(buf[0]);
			}
			else
			{
	            spi_TransferByte(buf[i]);
			}
        }

		spi_DeselectDevice();
		spi_EndTransaction();

        addr += chunk;
        arrayIndex += chunk;
        noOfBytesStillToWrite -= chunk;

		waitForWriteCompletion();
    }
}

///////////////////////////////////////////////////////////////////////////////
// void setBlock(const uint16_t addr, const uint8_t data, const uint16_t length)
//
// fill EEPROM memory block with constant data

uint16_t SPI_eeprom::setBlock(const uint16_t addr, const uint8_t data, const uint16_t length)
{
	bool noIncrement = true;

	writeBuffer(addr, &data, length, noIncrement);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// void aitForWriteDone(void)

void SPI_eeprom::waitForWriteCompletion(void)
{
	while (isBusy())
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
// bool isBusy(void)

bool SPI_eeprom::isBusy(void)
{
	bool busy = false;

    uint8_t statusReg = getStatus();

	busy = ((statusReg & SR_WIP) != 0);

    return busy;
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t getStatus(void)

uint8_t SPI_eeprom::getStatus(void)
{
    uint8_t statusReg = 0;
    
	spi_SelectDevice(_spiDeviceNumber);
	spi_BeginTransaction(_spisettings);
	spi_TransferByte(EEP_RDSR);

	statusReg = spi_TransferByte(0x00);

	spi_EndTransaction();
	spi_DeselectDevice();

    return statusReg;
}
