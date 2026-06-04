///////////////////////////////////////////////////////////////////////////////
//
// SPIeeprom.h
//
// Created: 13-08-2025
// Author:  Roel Smeets
//
// class library for 25LC640 EEPROM using SPI bus
//
// see also:
// https://github.com/wollewald/EEPROM_SPI_WE
// https://github.com/dndubins/EEPROMsimple
///////////////////////////////////////////////////////////////////////////////

#ifndef SPIEEPROM_H_
#define SPIEEPROM_H_

///////////////////////////////////////////////////////////////////////////////
// #includes

#include <inttypes.h>
#include <SPI.h>

///////////////////////////////////////////////////////////////////////////////
// enums

// redefine the number of bits on the chip to number of bytes

enum eeprom_size_t 
{    
    EEPROM_KBITS_2 		= 256,
    EEPROM_KBITS_4 		= 512,
    EEPROM_KBITS_8 		= 1024,
    EEPROM_KBITS_16 	= 2028,
    EEPROM_KBITS_32 	= 4096,
    EEPROM_KBITS_64		= 8192,
    EEPROM_KBITS_128	= 16384,
    EEPROM_KBITS_256	= 32768,
    EEPROM_KBITS_512	= 65536,
    EEPROM_KBITS_1024	= 131072,
    EEPROM_KBITS_2048	= 262144,
    EEPROM_KBITS_4096	= 524288,
};

// pagesize in bytes

enum eeprom_pagesize_t  
{
    EEPROM_PAGE_SIZE_16  = 16, 
	EEPROM_PAGE_SIZE_32  = 32, 
	EEPROM_PAGE_SIZE_64  = 64, 
	EEPROM_PAGE_SIZE_128 = 128, 
	EEPROM_PAGE_SIZE_256 = 256,
};


//////////////////////////////////////////////////////////////////////////////
// class definition of class SPIeeprom for 25LC640 EEPROM type using SPI bus

class SPI_eeprom
{
	// public section, constants. Opcodes
public:
	static constexpr uint8_t EEP_READ  = 0x03;  // read data
	static constexpr uint8_t EEP_WRITE = 0x02;  // write data
	static constexpr uint8_t EEP_WREN  = 0x06;  // set write enable latch, enables write
	static constexpr uint8_t EEP_WRDI  = 0x04;  // reset write disable latch, diables write
	static constexpr uint8_t EEP_RDSR  = 0x05;  // read status register
	static constexpr uint8_t EEP_WRSR  = 0x01;  // write status register

	// public section, functions
public:
	SPI_eeprom(uint8_t spiDeviceNumber, eeprom_size_t deviceSize = EEPROM_KBITS_64);
	SPI_eeprom(uint8_t spiDeviceNumber, eeprom_size_t deviceSize, eeprom_pagesize_t pageSize = EEPROM_PAGE_SIZE_32);

	bool begin(void);
	void setDeviceSize(eeprom_size_t memorySize);
	uint32_t getDeviceSize(void);
	void setPageSize(eeprom_pagesize_t pageSize);
	uint32_t getPageSize(void);
	int16_t writeByte(uint32_t address, uint8_t value);
	uint8_t readByte(uint32_t address);
	uint16_t setBlock(const uint16_t addr, const uint8_t data, const uint16_t length);

	// private section, constants
private:
	static constexpr uint8_t  SR_WIP = _BV(0); // Write In Progress bit (bit 0, read-only)
	static constexpr uint8_t  SR_WEL = _BV(1); // Write Enable Latch (bit 1, read-only)
	static constexpr uint32_t SPI_EEPROM_SPEED = 3000000;	// SPI clock frequency in Hz
		
	// private section, functions
private:
	void writeBuffer(uint32_t addr, const uint8_t *buf, uint16_t sizeOfBuf, bool noIncrement = false);
	void readBuffer(uint32_t addr, uint8_t *buf, uint16_t sizeOfBuf);
	bool isBusy(void);
	uint8_t getStatus(void);
	void waitForWriteCompletion(void);

	// private section, variables
private:
	uint32_t _deviceSize = 0;
	uint16_t _pageSize   = 0;
	uint8_t  _spiDeviceNumber = 0; // must be between 0..6 device 7 = deselect
	SPISettings _spisettings;		// SPI settings, speed selection
};

#endif	// SPIEEPROM_H_
