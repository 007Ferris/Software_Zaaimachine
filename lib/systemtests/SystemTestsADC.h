//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsADC.h
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEMTESTS_ADC_H
#define SYSTEMTESTS_ADC_H

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <inttypes.h>

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void test_ADC_ReadSingleChannel(void);

void test_ADCReadSingle_Repeat(uint8_t channel);
void test_ADCReadAll_Repeat(void);
void test_ADCReadAllMultiple_Repeat(void);

uint16_t test_ADCDAC03_Loopback(bool manual, bool verbose);
void test_ADCDAC03_Loopback_Repeat(bool manual, bool verbose);

#endif  // SYSTEMTESTS_ADC_H
