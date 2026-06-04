//////////////////////////////////////////////////////////////////////////////
//
// ADC3208Lib.h
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC3208_H
#define ADC3208_H

///////////////////////////////////////////////////////////////////////////////
// bit defines for ADC MCP3208

#define ADC_STR     BIT_10
#define ADC_SINGLE  BIT_9

///////////////////////////////////////////////////////////////////////////////
// SPI settings for ADC MCP3208

#define SPI_ADC_SPEED	2000000

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void adc_Init(void);

uint16_t adc_ReadRaw(uint8_t channel, uint8_t averageCount = 1);
void adc_ReadRawMultiple(uint8_t channelList[], uint8_t numChannels, uint16_t rawValues[]);
void adc_ReadVoltageMultiple(uint8_t channelList[], uint8_t numChannels, double voltages[]);

double adc_ReadVoltage(uint8_t channel, uint8_t averageCount = 1);
double adc_RawToVoltage(uint16_t adcRaw, uint8_t channel);
bool   adc_IsButtonPressed(uint8_t analogButton);

#endif  // ADC3208_H
