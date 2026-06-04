//////////////////////////////////////////////////////////////////////////////
//
// DAC4922Lib.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"

///////////////////////////////////////////////////////////////////////////////
// application specific includes

#include "DAC4922Lib.h"
#include "SPILib.h"
#include "Config.h"
#include "fmap.h"
#include "SerialPrintf.h"

///////////////////////////////////////////////////////////////////////////////
// SPI settings for DAC transactions

static SPISettings g_DACSPISettings(SPI_DAC_SPEED, SPI_MSBFIRST, SPI_MODE0);

///////////////////////////////////////////////////////////////////////////////
// void dac_Init(void)

void dac_Init(void)
{
	// init the DAC chips the first time by writing any value - use zero volts
	float outputVoltage = 0.0;

	g_DACSPISettings._clock    = SPI_DAC_SPEED;
	g_DACSPISettings._bitOrder = MSBFIRST;
	g_DACSPISettings._dataMode = SPI_MODE0;
	
	for (uint8_t channel = 0; channel < N_DAC_CHANNELS; channel++)
	{
		dac_SetOutputVoltage(channel, outputVoltage);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void dac_SelectSPIDevice(uint8_t dacChannel)

void dac_SelectSPIDevice(uint8_t dacChannel)
{
	if (dacChannel < N_DAC_CHANNELS)
	{
		if ((dacChannel == 0) || (dacChannel == 1))
		{
			spi_SelectDevice(SPI_DEVICE_DAC01);
		}
		else if ((dacChannel == 2) || (dacChannel == 3))
		{
			spi_SelectDevice(SPI_DEVICE_DAC23);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// void dac_Write(uint8_t dacChannel, uint16_t dacValue)

void dac_Write(uint8_t dacChannel, uint16_t dacValue)
{
	uint16_t dacCommand = 0;

	if (dacChannel < N_DAC_CHANNELS)
	{
		dacCommand  = dacValue & 0xfff; // only 12 bits allowed for DAC value
		dacCommand |= DAC_VREF_BUFFERED | DAC_GAINSELECT_1 | DAC_POWER_ON;

		if ((dacChannel == 1) || (dacChannel == 3) ) // channnel 1 or 3 => B channel of MCP4922
		{
			dacCommand = dacCommand | DAC_SELECT_B;
		}

		spi_BeginTransaction(g_DACSPISettings);
		dac_SelectSPIDevice(dacChannel);

		spi_WriteWord(dacCommand);

		spi_DeselectDevice(); // DEselect DAC channel: cause CSDAC* to go high!!
		spi_EndTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dac_SetOutputVoltage(uint8_t dacChannel, float outputVoltage)
//
// Vout = -10 + 8*Vdac

void dac_SetOutputVoltage(uint8_t dacChannel, float outputVoltage)
{
	float dacValue = 0.0;
	
	outputVoltage = constrain(outputVoltage, DAC_MIN_VOLTAGE, DAC_MAX_VOLTAGE);
	
	dacValue = fmap(outputVoltage,	DAC_MIN_VOLTAGE, DAC_MAX_VOLTAGE, 
	 								DAC_MIN_VALUE, DAC_MAX_VALUE);	

	// SerialPrintf("DAC value channel %d = %d\n", dacChannel, (uint16_t)(dacValue));

	dac_Write(dacChannel, (uint16_t)dacValue);
}

///////////////////////////////////////////////////////////////////////////////
// void dac_SetOutputVoltageAll(float outputVoltage)

void dac_SetOutputVoltageAll(float outputVoltage)
{
	uint8_t channel = 0;

	for (channel = 0; channel <= DAC_MAX_CHANNEL; channel++)
	{
		dac_SetOutputVoltage(channel, outputVoltage);
	}
}
