//////////////////////////////////////////////////////////////////////////////
//
// ADC3208Lib.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	23-07-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"

///////////////////////////////////////////////////////////////////////////////
// application specific includes

#include "ADC3208Lib.h"
#include "SPILib.h"
#include "Config.h"
#include "fmap.h"
#include "bits.h"

///////////////////////////////////////////////////////////////////////////////
// SPI settings for ADC transactions

static SPISettings g_ADCSPISettings(SPI_ADC_SPEED, SPI_MSBFIRST, SPI_MODE0);

///////////////////////////////////////////////////////////////////////////////
// void adc_Init(void)

void adc_Init(void)
{
	g_ADCSPISettings._clock    = SPI_ADC_SPEED;
	g_ADCSPISettings._bitOrder = MSBFIRST;
	g_ADCSPISettings._dataMode = SPI_MODE0;

    spi_SelectDevice(SPI_DEVICE_ADC);   // select
    spi_DeselectDevice();   			// and deselect again
}

///////////////////////////////////////////////////////////////////////////////
//
// Build the following command pattern that will be sent as 3 bytes 
// in separate transactions:
//
// channel is coded in D2 D1 D0:
//      byte 1: <0 0 0 0 0 startbit sgl/diff D2>
//      byte 2: <D1 D0 0 0 0 0 0 0>          
//      byte 3: <0 0 0 0 0 0 0 0>
// 
// And we expect to read from the ADC:
//      byte 1: <x x x x x x x x> 
//      byte 2: <x x x 0 B11 B10 B9 B8> 
//      byte 3: <B7 B6 B5 B4 B3 B2 B1 B0>
//
// refer to datasheet of MCP3208 and https://github.com/Rom3oDelta7/MCP320X


///////////////////////////////////////////////////////////////////////////////
// uint16_t adc_ReadRaw(uint8_t channel, uint8_t averageCount)

uint16_t adc_ReadRaw(uint8_t channel, uint8_t averageCount)
{
    uint16_t adcCommand = 0;
    uint16_t adcValue = 0;
	uint16_t ix = 0;
	uint32_t raw = 0;

    if (channel < N_ADC_CHANNELS)
    {
        adcCommand = ADC_STR | ADC_SINGLE | (channel << 6);
        
        spi_BeginTransaction(g_ADCSPISettings);

		raw = 0;

		for (ix = 0; ix < averageCount; ix++)
		{
	        spi_SelectDevice(SPI_DEVICE_ADC);

			uint8_t b1  = spi_TransferByte(highByte(adcCommand));
			uint8_t msb = spi_TransferByte(lowByte(adcCommand));
			uint8_t lsb = spi_TransferByte(0);

			adcValue = ((msb & 0x0f) << 8) | lsb;
			raw += adcValue;

			spi_DeselectDevice();
		}

		raw /= averageCount;

		spi_EndTransaction();
    }

    return raw;
}

///////////////////////////////////////////////////////////////////////////////
// void adc_ReadRawMultiple(uint8_t channelList[], uint8_t numChannels, 
//                          uint16_t rawValues[])

void adc_ReadRawMultiple(uint8_t channelList[], uint8_t numChannels, uint16_t rawValues[])
{
    uint16_t adcCommand = 0;
    uint16_t adcValue   = 0;
    uint8_t channel		= 0;
    uint8_t ix = 0;

    numChannels = constrain(numChannels, 0, N_ADC_CHANNELS);

    spi_BeginTransaction(g_ADCSPISettings);

    for (ix = 0; ix < numChannels; ix++)
    {
        channel = channelList[ix];

        if (channel < N_ADC_CHANNELS)
        {
            spi_SelectDevice(SPI_DEVICE_ADC);

            adcCommand = ADC_STR | ADC_SINGLE | (channel << 6);

            uint8_t b1  = spi_TransferByte(highByte(adcCommand));
            uint8_t msb = spi_TransferByte(lowByte (adcCommand));
            uint8_t lsb = spi_TransferByte(0);

            adcValue = ((msb & 0x0f) << 8) | lsb;
            rawValues[ix] = adcValue;

            spi_DeselectDevice();
        }
    }

    spi_EndTransaction();
}

///////////////////////////////////////////////////////////////////////////////
// void adc_ReadVoltageMultiple(uint8_t channelList[], uint8_t numChannels, 
//                              double voltages[])

void adc_ReadVoltageMultiple(uint8_t channelList[], uint8_t numChannels, double voltages[])
{
    uint16_t rawValues[N_ADC_CHANNELS];
    uint8_t ix = 0;

    numChannels = constrain(numChannels, 0, N_ADC_CHANNELS);

    adc_ReadRawMultiple(channelList, numChannels, rawValues);

    // raw to voltage conversion is dependent on the channel range:
    // channel 0..3: -10 volt .. +10 volt
    // channel 4..7: 0 volt .. +2.5 volt
    
    for (ix = 0; ix < numChannels; ix++)
    {
        voltages[ix] = adc_RawToVoltage(rawValues[ix], channelList[ix]);
    }
}

///////////////////////////////////////////////////////////////////////////////
// double adc_ReadVoltage(uint8_t channel, uint8_t averageCount)

double adc_ReadVoltage(uint8_t channel, uint8_t averageCount)
{
    uint16_t adcRaw = 0;
    double  voltage = 0.0;

	adcRaw = adc_ReadRaw(channel, averageCount);
   	voltage = adc_RawToVoltage(adcRaw, channel);
     
    return voltage;
}

///////////////////////////////////////////////////////////////////////////////
// double adc_RawToVoltage(uint16_t adcRaw, uint8_t channel)

double adc_RawToVoltage(uint16_t adcRaw, uint8_t channel)
{
    double voltage = 0.0;
    
    if ((channel >= 0) && (channel <= 3))   // channels 0..3: -10 .. +10 volt
    {
        voltage = fmap(adcRaw, ADC_MIN_VALUE, ADC_MAX_VALUE, ADC03_MIN_VOLTAGE, ADC03_MAX_VOLTAGE);
    }
    else if ((channel >= 4) && (channel <= 7)) // channels 4..7: 0.. +2.5 volt
    {
        voltage = fmap(adcRaw, ADC_MIN_VALUE, ADC_MAX_VALUE, ADC47_MIN_VOLTAGE, ADC47_MAX_VOLTAGE);
    }
  
    return voltage;
}

///////////////////////////////////////////////////////////////////////////////
// bool adc_IsButtonPressed(uint8_t analogButton)
//
// button 1 mapped to ADC channel 6 
// button 2 mapped to ADC channel 7 
//
// button pressed = LOW voltage!

bool adc_IsButtonPressed(uint8_t buttonNumber)
{
    bool isPressed = false;
    uint16_t adcValue = 0;
	uint8_t  adcChannel = 0;

    if ((buttonNumber == 1) || (buttonNumber == 2))
    {
		adcChannel = buttonNumber + 5;	// ADC channel 6/7
        adcValue = adc_ReadRaw(adcChannel);
        isPressed = (adcValue < (ADC_MAX_VALUE/2));
    }

    return isPressed;
}