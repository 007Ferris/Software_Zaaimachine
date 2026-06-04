///////////////////////////////////////////////////////////////////////////////
//
// SPIeeprom.cpp
//
// Created: 01-09-2024
//			14-08-2025
// Author:  Roel Smeets
//
// class library for LS7366 Quadrature Counter using SPI bus
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
#include "QC7366.h"
#include "SPILib.h"

// TODO:
// static SPISettings g_spiSettings(SPI_CLOCK, MSBFIRST, SPI_MODE0); // default values

///////////////////////////////////////////////////////////////////////////////
// constructor

QC7366Counter::QC7366Counter(uint8_t spiDeviceNumber)
{
	_spiDeviceNumber = spiDeviceNumber;
}


///////////////////////////////////////////////////////////////////////////////
// functions

///////////////////////////////////////////////////////////////////////////////
// bool begin(void)

bool QC7366Counter::begin(void)
{
	bool result = true;
	uint8_t defaultMode  = 0;
	mode_register_t modeRegister = QC_MODE_REGISTER_0;
	
	// mode depends on quadrature pulse definitions of the encoder used!!
	defaultMode = MODE_QC_1 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	
	spi_Init();	// needed ??

	writeModeRegister(modeRegister, defaultMode);
	disableCounter();
	clearCountRegister();

	return result;
}


///////////////////////////////////////////////////////////////////////////////
// uint8_t readModeRegister(mode_register_t modeRegister)

uint8_t QC7366Counter::readModeRegister(mode_register_t modeRegister)
{
	uint8_t readMDRCommand = 0;
	uint8_t mdrValue = 0xff;
	
	if (modeRegister <= QC_MODE_REGISTER_1)
	{
		readMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? READ_MDR0 : READ_MDR1;
		
		spi_SelectDevice(_spiDeviceNumber);
		spi_WriteByte(readMDRCommand);
		spi_ReadByte(&mdrValue);
		spi_DeselectDevice();
	}
	
	return mdrValue;
}

///////////////////////////////////////////////////////////////////////////////
// void writeModeRegister(mode_register_t modeRegister, uint8_t valueMDR)

void QC7366Counter::writeModeRegister(mode_register_t modeRegister, uint8_t valueMDR)
{
	uint8_t writeMDRCommand = 0;
	
	if (modeRegister <= QC_MODE_REGISTER_1)
	{
		writeMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? WRITE_MDR0 : WRITE_MDR1;
		
		spi_SelectDevice(_spiDeviceNumber);
		spi_WriteByte(writeMDRCommand);
		spi_WriteByte(valueMDR);
		spi_DeselectDevice();
	}
}

///////////////////////////////////////////////////////////////////////////////
// 

void QC7366Counter::disableCounter(void)
{
	uint8_t mdrValue = 0;
	
	mdrValue  = readModeRegister(QC_MODE_REGISTER_1);
	mdrValue |= CNT_DISABLE;
	writeModeRegister(QC_MODE_REGISTER_1, mdrValue);

}

///////////////////////////////////////////////////////////////////////////////
// 

///////////////////////////////////////////////////////////////////////////////
// 

void QC7366Counter::clearCountRegister(void)
{

}
