///////////////////////////////////////////////////////////////////////////////
//
// SPILib.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	02-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>
#include <spi.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SPILib.h"


///////////////////////////////////////////////////////////////////////////////
// file globals

static bool g_IsSPIInitialised = false;
SPIClass vspi = SPIClass(VSPI); 		    // Use VSPI bus
static SPISettings g_spiSettings(SPI_DEFAULT_SPEED, MSBFIRST, SPI_MODE0); // default values

///////////////////////////////////////////////////////////////////////////////
// pin table for MUX select bits

static const uint8_t g_SPISelectPins[SPI_N_SELECTBITS] =
{
	SPI_SEL_0,		// LSB, bit 0
	SPI_SEL_1,
	SPI_SEL_2,		// MSB, bit 2
};

///////////////////////////////////////////////////////////////////////////////
// void spi_Init(void)

void spi_Init(void)
{
	if (g_IsSPIInitialised == false)	// prevents multiple inits
	{
		pinMode(SPI_SEL_2, OUTPUT);
		pinMode(SPI_SEL_1, OUTPUT);
		pinMode(SPI_SEL_0, OUTPUT);

        // deselects all SPI devices:
        spi_DeselectDevice();

        // same as in constructor
        g_spiSettings._clock    = SPI_DEFAULT_SPEED;
        g_spiSettings._bitOrder = MSBFIRST;
        g_spiSettings._dataMode = SPI_MODE0;

		vspi.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
		vspi.setHwCs(false);  // false = disable VSPI_SS, Default = disabled!

		g_IsSPIInitialised = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void spi_BeginTransaction(SPISettings settings)

void spi_BeginTransaction(SPISettings settings)
{
    vspi.beginTransaction(settings);
}

///////////////////////////////////////////////////////////////////////////////
// void spi_BeginTransaction(void)

void spi_EndTransaction(void)
{
    vspi.endTransaction();
}

///////////////////////////////////////////////////////////////////////////////
// void spi_WriteByte(const uint8_t data)

void spi_WriteByte(const uint8_t data)
{
	vspi.write(data);
}

///////////////////////////////////////////////////////////////////////////////
// void spi_WriteWord(const uint16_t data)

void spi_WriteWord(const uint16_t data)
{
	vspi.write16(data);
}

///////////////////////////////////////////////////////////////////////////////
// void spi_ReadByte(uint8_t *byteData)

void spi_ReadByte(uint8_t *byteData)
{
	*byteData = vspi.transfer(0);
}

///////////////////////////////////////////////////////////////////////////////
// void spi_ReadWord(uint16_t *wordData)

void spi_ReadWord(uint8_t *wordData)
{
	*wordData = vspi.transfer16(0);
}


///////////////////////////////////////////////////////////////////////////////
// uint8_t spi_TransferByte(uint8_t byteToSend)

uint8_t spi_TransferByte(uint8_t byteToSend)
{
	uint8_t rcvByte = vspi.transfer(byteToSend);
 
	return rcvByte;
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t spi_TransferByte(uint8_t byteToSend)

uint16_t spi_TransferWord(uint16_t wordToSend)
{
	uint16_t rcvWord = vspi.transfer16(wordToSend);
 
	return rcvWord;
}

///////////////////////////////////////////////////////////////////////////////
// void spi_SelectDevice(uint8_t spiDeviceNumber)

void spi_SelectDevice(uint8_t spiDeviceNumber)
{
	uint8_t gpioPinNumber = 0;
	uint8_t bitValue = LOW;
	uint8_t bitNr = 0;

	if (spiDeviceNumber <= SPI_MAX_DEVICENUMBER)
	{
		for (bitNr = 0; bitNr < SPI_N_SELECTBITS; bitNr++)
		{
			gpioPinNumber = g_SPISelectPins[bitNr];
			bitValue = LOW;
			if ((spiDeviceNumber & (0x01 << bitNr)) != 0)
			{
				bitValue = HIGH;
			}
			digitalWrite(gpioPinNumber, bitValue);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// void spi_DeselectDevice(void)

void spi_DeselectDevice(void)
{
	spi_SelectDevice(SPI_DEVICE_UNUSED);
}
