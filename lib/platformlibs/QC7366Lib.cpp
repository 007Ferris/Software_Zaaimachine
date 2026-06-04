///////////////////////////////////////////////////////////////////////////////
//
// QC7366Lib.cpp
//
// Author:	 	Roel Smeets
// Edit date: 	14-10-2022
//				25-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "SPILib.h"
#include "QC7366Lib.h"

///////////////////////////////////////////////////////////////////////////////
// SPI settings for quadrature LS7366R transactions

static SPISettings g_QCSPISettings(SPI_QC_SPEED, SPI_MSBFIRST, SPI_MODE0);

///////////////////////////////////////////////////////////////////////////////
// void qc_Init(void)

void qc_Init(void)
{
	uint8_t channel		 = 0;
	uint8_t defaultMode  = 0;
	mode_register_t modeRegister = QC_MODE_REGISTER_0;
	
	// mode depends on quadrature pulse definitions of the encoder used!!
	defaultMode = MODE_QC_1 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;
	
	spi_Init();
	spi_DeselectDevice();

	g_QCSPISettings._bitOrder = SPI_MSBFIRST;
	g_QCSPISettings._dataMode = SPI_MODE0;
	g_QCSPISettings._clock 	= SPI_QC_SPEED;

	spi_BeginTransaction(g_QCSPISettings);
	spi_EndTransaction();

	for (channel = 0; channel <= QC_MAX_CHANNEL; channel++)
	{
		qc_WriteModeRegister(channel, modeRegister, defaultMode);
		qc_DisableCounter(channel);
		qc_ClearCountRegister(channel);
	}

}

///////////////////////////////////////////////////////////////////////////////
// void qc_SelectSPIDevice(uint8_t qcChannel)

void qc_SelectSPIDevice(uint8_t qcChannel)
{
	uint8_t spiDevice = 0;
	
	if (qcChannel <= QC_MAX_CHANNEL)
	{
		if (qcChannel == 0)
		{
			spiDevice = SPI_DEVICE_QC0;
		}
		else if (qcChannel == 1)
		{
			spiDevice = SPI_DEVICE_QC1;
		}
		spi_SelectDevice(spiDevice);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void qc_ClearStatusRegister(uint8_t channel)

void qc_ClearStatusRegister(uint8_t channel)
{
	qc_SendCommand(channel, CLR_STR);
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t qc_ReadStatusRegister(uint8_t channel)

uint8_t qc_ReadStatusRegister(uint8_t channel)
{
	uint8_t statusValue = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{

		spi_BeginTransaction(g_QCSPISettings);
		qc_SelectSPIDevice(channel);
	
		spi_WriteByte(READ_STR);
		spi_ReadByte(&statusValue);

		spi_DeselectDevice();
		spi_EndTransaction();
	}
	
	return statusValue;
}


///////////////////////////////////////////////////////////////////////////////
// bool qc_IsIndexSet(uint8_t channel)

bool qc_IsIndexSet(uint8_t channel)
{
	bool indexSet = false;
	uint8_t statusValue = 0;
	
	statusValue = qc_ReadStatusRegister(channel);
	
	if ((statusValue & IDX_BIT) != 0)
	{
		indexSet = true;
	}
	
	return indexSet;
}

///////////////////////////////////////////////////////////////////////////////
// void qc_WriteModeRegister(uint8_t channel, mode_register_t modeRegister, 
//							 uint8_t valueMDR)

void qc_WriteModeRegister(uint8_t channel, mode_register_t modeRegister, uint8_t valueMDR)
{
	uint8_t writeMDRCommand = 0;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		writeMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? WRITE_MDR0 : WRITE_MDR1;
		
		spi_BeginTransaction(g_QCSPISettings);
		qc_SelectSPIDevice(channel);

		spi_WriteByte(writeMDRCommand);
		spi_WriteByte(valueMDR);

		spi_DeselectDevice();
		spi_EndTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t qc_ReadModeRegister(uint8_t channel, mode_register_t modeRegister)

uint8_t qc_ReadModeRegister(uint8_t channel, mode_register_t modeRegister)
{
	uint8_t readMDRCommand = 0;
	uint8_t mdrValue = 0xff;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		readMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? READ_MDR0 : READ_MDR1;
	
		spi_BeginTransaction(g_QCSPISettings);
		qc_SelectSPIDevice(channel);

		spi_WriteByte(readMDRCommand);
		spi_ReadByte(&mdrValue);

		spi_DeselectDevice();
		spi_EndTransaction();
	}
	
	return mdrValue;
}

///////////////////////////////////////////////////////////////////////////////
// void qc_ClearModeRegister(uint8_t channel, mode_register_t modeRegister)

void qc_ClearModeRegister(uint8_t channel, mode_register_t modeRegister)
{
	uint8_t readMDRCommand = 0;
	
	if ((channel <= QC_MAX_CHANNEL) && (modeRegister <= QC_MODE_REGISTER_1))
	{
		readMDRCommand = (modeRegister == QC_MODE_REGISTER_0) ? CLR_MDR0 : CLR_MDR1;
		qc_SendCommand(channel, readMDRCommand);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void qc_ClearCountRegister(uint8_t channel)

void qc_ClearCountRegister(uint8_t channel)
{
	qc_SendCommand(channel, CLR_CNTR);
}

///////////////////////////////////////////////////////////////////////////////
// int32_t qc_ReadCountRegister(uint8_t channel)

int32_t  qc_ReadCountRegister(uint8_t channel)
{
	int32_t count = 0;
	uint8_t ix	  = 0;
	uint8_t val	  = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{

		spi_BeginTransaction(g_QCSPISettings);
		qc_SelectSPIDevice(channel);

		spi_WriteByte(READ_CNTR);
		for (ix = 0; ix < 4; ix++)
		{
			spi_ReadByte(&val);
			count = (count << 8) | val;
		}

		spi_DeselectDevice();
		spi_EndTransaction();
	}
	
	return count;
}

///////////////////////////////////////////////////////////////////////////////
// void qc_TransferDataRegisterToCountRegister(uint8_t channel)

void qc_TransferDataRegisterToCountRegister(uint8_t channel)
{
	qc_SendCommand(channel, LOAD_CNTR);
}

///////////////////////////////////////////////////////////////////////////////
// void qc_WriteDataRegister(uint8_t channel, int32_t dtrValue)

void qc_WriteDataRegister(uint8_t channel, int32_t dtrValue)
{
	uint8_t ix = 0;
	uint8_t spiData = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_BeginTransaction(g_QCSPISettings);
		qc_SelectSPIDevice(channel);

		spi_WriteByte(WRITE_DTR);
		for (ix = 0; ix < 4; ix++) // Most Significant byte first!
		{
			spiData = (uint8_t)(dtrValue >> 8*(3 - ix));	// shift right 24, 16, 8, 0
			spi_WriteByte(spiData);
		}		

		spi_DeselectDevice();
		spi_EndTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// void qc_TranferCountRegisterToOutputRegister(uint8_t channel)

void qc_TranferCountRegisterToOutputRegister(uint8_t channel)
{
	qc_SendCommand(channel, LOAD_OTR);
}

///////////////////////////////////////////////////////////////////////////////
// int32_t qc_ReadOutputRegister(uint8_t channel)

int32_t qc_ReadOutputRegister(uint8_t channel)
{
	int32_t count = 0;
	uint8_t ix = 0;
	uint8_t val = 0;
	
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_BeginTransaction(g_QCSPISettings);
		qc_SelectSPIDevice(channel);

		spi_WriteByte(READ_OTR);
		for (ix = 0; ix < 4; ix++)
		{
			spi_ReadByte(&val);
			count = (count << 8) | val;
		}

		spi_DeselectDevice();
		spi_EndTransaction();
	}
	
	return count;
}

///////////////////////////////////////////////////////////////////////////////
// void qc_DisableCounter(uint8_t channel)

void qc_DisableCounter(uint8_t channel)
{
	uint8_t mdrValue = 0;
	
	mdrValue  = qc_ReadModeRegister(channel, QC_MODE_REGISTER_1);
	mdrValue |= CNT_DISABLE;
	qc_WriteModeRegister(channel, QC_MODE_REGISTER_1, mdrValue);
}

///////////////////////////////////////////////////////////////////////////////
// void qc_DisableCounter(uint8_t channel)

void qc_EnableCounter(uint8_t channel)
{
	uint8_t mdrValue = 0;
	
	mdrValue  = qc_ReadModeRegister(channel, QC_MODE_REGISTER_1);
	mdrValue &= ~CNT_DISABLE;
	qc_WriteModeRegister(channel, QC_MODE_REGISTER_1, mdrValue);
}

///////////////////////////////////////////////////////////////////////////////
// void qc_SendCommand(uint8_t channel)

void qc_SendCommand(uint8_t channel, uint8_t commandByte)
{
	if (channel <= QC_MAX_CHANNEL)
	{
		spi_BeginTransaction(g_QCSPISettings);
		qc_SelectSPIDevice(channel);

		spi_WriteByte(commandByte);

		spi_DeselectDevice();
		spi_EndTransaction();
	}
}
