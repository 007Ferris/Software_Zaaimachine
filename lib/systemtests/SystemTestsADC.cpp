//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsADC.cpp
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

#include "Config.h"
#include "ADC3208Lib.h"
#include "DAC4922Lib.h"
#include "TaskSleep.h"
#include "SerialPrintf.h"
#include "SystemTests.h"
#include "SystemTestsADC.h"

///////////////////////////////////////////////////////////////////////////////
// uint8_t GetADCChannel(void)

uint8_t GetADCChannel(void)
{
	uint8_t channel = 0;

	do 
	{
		channel = ReadNumber();
		if (channel >= 8)
		{
			SerialPrintf("> ADC channel must be in range 0..7\n");
		}
	} while (channel >= 8);

	return channel;
}

///////////////////////////////////////////////////////////////////////////////
// void test_ADC_ReadSingleChannel(void)

void test_ADC_ReadSingleChannel(void)
{
	uint8_t choice = 0;
	uint8_t channel = 0;

	while (true)
	{
		SerialPrintf("* make a selection:\n");
		SerialPrintf("1.    select ADC channel, current = %d\n", channel);
		SerialPrintf("2.    read ADC channel\n");

		choice = ReadChoice(2);

		switch (choice)
		{
		case 0: // up one level in menu
			return;
		case 1:
			SerialPrintf("> enter channel number (0..7) > ");
			channel = GetADCChannel();
			break;
		case 2:
			test_ADCReadSingle_Repeat(channel);
			break;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// void test_ADCReadSingle_Repeat(uint8_t channel)

void test_ADCReadSingle_Repeat(uint8_t channel)
{
	uint16_t raw = 0;
	double voltage = 0.0;

	while (true)
	{
		raw = adc_ReadRaw(channel);
		voltage = adc_RawToVoltage(raw, channel);
		SerialPrintf("> ADC channel %d: raw = %4d, voltage = %4.1f mV\n", channel, raw, voltage * VOLT_TO_MV);

		taskSleep(100);

		if (StopTest())	break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_ADCReadAll_Repeat(void)

void test_ADCReadAll_Repeat(void)
{
	uint8_t  channel = 0;
	uint32_t raw = 0;
	uint16_t averageCount = 5;
	uint16_t ix = 0;
	double 	 voltage = 0.0;
	double   dacOutputVoltage = 5.0;
	int16_t  delta = 0;
	uint16_t min[N_ADC_CHANNELS];
	uint16_t max[N_ADC_CHANNELS];
	
	for (channel = 0; channel < N_ADC_CHANNELS; channel++)
	{
		min[channel] = 60000;
		max[channel] = 0;
	}

	// set DAC output voltage, so we can use the 4 DAC channels as a voltage source 
	// for the ADC on channels 0..3

	dac_SetOutputVoltageAll(dacOutputVoltage);

	while (true)
	{
		SerialPrintf("***** ADC read all channels, DAC = %.2f mV, average = %d *****\n", 
					dacOutputVoltage * VOLT_TO_MV, averageCount);
		for (channel = 0; channel < N_ADC_CHANNELS; channel++)
		{
			raw = adc_ReadRaw(channel, averageCount);
	
			if (raw < min[channel])
			{
				min[channel] = raw;
			}
			if (raw > max[channel])
			{
				max[channel] = raw;
			}
			delta = abs(max[channel] - min[channel]);

			voltage = adc_RawToVoltage(raw, channel);
			SerialPrintf("> ADC channel %d: raw = %4d, voltage = %6.1f mV. min = %4d, max = %4d (delta = %4d)\n", 
				channel, raw, voltage * VOLT_TO_MV, min[channel], max[channel], delta);

		}

		if (StopTest()) break;
		taskSleep(1000);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_ADCReadAllMultiple_Repeat(void)

void test_ADCReadAllMultiple_Repeat(void)
{
	uint8_t  channelList[N_ADC_CHANNELS] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t  channel = 0;
	uint16_t raw[N_ADC_CHANNELS];
	double voltage = 0.0;
	double dacOutputVoltage = 2.0;

	dac_SetOutputVoltageAll(dacOutputVoltage);

	while (true)
	{
		SerialPrintf("***** ADC read all channels multiple *****\n");
		adc_ReadRawMultiple(channelList, N_ADC_CHANNELS, raw);
		for (channel = 0; channel < N_ADC_CHANNELS; channel++)
		{
			voltage = adc_RawToVoltage(raw[channel], channel);
			SerialPrintf("> ADC channel %d: raw = %4d, voltage = %6.1f mV\n", channel, raw[channel], voltage * VOLT_TO_MV);
		}
		taskSleep(1000);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void test_ADCDAC03_Loopback_Repeat(bool manual, bool verbose)

void test_ADCDAC03_Loopback_Repeat(bool manual, bool verbose)
{
	while (true)
	{
		test_ADCDAC03_Loopback(manual, verbose);
		if (StopTest()) break;
	}	
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t test_ADCDAC03_Loopback(void)
// loopback on channels 0..3

extern float g_DACOutvoltageTable[];
extern uint8_t g_NrOfDACValues;

uint16_t test_ADCDAC03_Loopback(bool manual, bool verbose)
{
	double dacOutputVoltage = 0.0;
	double voltage = 0.0;
	double delta   = 0.0;
	double maxDelta    = 200 * MV_TO_VOLT;	// also includes ADC & DAC offsets!!
	uint8_t adcChannel = 0;
	uint16_t errors    = 0;
	uint8_t ix = 0;
	uint8_t averagecount = 10;

	for (ix = 0; ix < g_NrOfDACValues; ix++)
	{
		dacOutputVoltage = g_DACOutvoltageTable[ix];
		if (verbose)
		{
			SerialPrintf("> setting all DAC channels to %.3f volt\n", dacOutputVoltage);
		}
		if (manual)
		{
			SerialPrintf("> press button B0 for next value...\n");
		}
		dac_SetOutputVoltageAll(dacOutputVoltage);
		taskSleep(10);

		for (adcChannel = 0; adcChannel <= 3; adcChannel++)	// only channels 0..3 in loopback
		{
			voltage = adc_ReadVoltage(adcChannel, averagecount);
			delta   = voltage - dacOutputVoltage;
			if (verbose)
			{
				SerialPrintf("> ADC channel %d: %.3f volt, delta = %2.1f mV\n", adcChannel, voltage, delta * VOLT_TO_MV);
			}
			if (fabs(delta) > maxDelta)
			{
				SerialPrintf("> ADC range error channel %d: delta = %2.1f mV\n", adcChannel, delta * VOLT_TO_MV);
				errors++;
			}
		}

		if (manual)
		{
			test_WaitForButton();
		}
	}

	return errors;
}
