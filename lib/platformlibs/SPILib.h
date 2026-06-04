///////////////////////////////////////////////////////////////////////////////
//
// SPILib.h
//
// Authors: 	Roel Smeets
// Edit date: 	25-06-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SPILIB_H_
#define SPILIB_H_

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>
#include <SPI.h>

///////////////////////////////////////////////////////////////////////////////
// #defines

#define SPI_MAX_DEVICENUMBER	7 	// max. 8 devices, do not use device 7 (!!)
#define SPI_N_SELECTBITS		3 	// means 3 bits required for selection

// select bits for 74HC138 MUX, select 1 of 8

#define SPI_SEL_2	GPIO_NUM_5
#define SPI_SEL_1	GPIO_NUM_17
#define SPI_SEL_0	GPIO_NUM_16

// device selection bits output of MUX

#define SPI_DEVICE_DAC01    0   // DAC channel 0 and 1
#define SPI_DEVICE_DAC23    1   // DAC channel 2 and 3
#define SPI_DEVICE_QC0      2   // Quadrature Counter 0
#define SPI_DEVICE_QC1      3   // Quadrature Counter 1
#define SPI_DEVICE_ADC      4   // 8 channel ADC
#define SPI_DEVICE_EXT_5    5   // external output for user device
#define SPI_DEVICE_EXT_6    6   // external output for user device
#define SPI_DEVICE_UNUSED   7   // used for deselecting all CS* signals!


// SPI pin definitions

#define VSPI_MISO	GPIO_NUM_19
#define VSPI_MOSI	GPIO_NUM_23
#define VSPI_SCLK	GPIO_NUM_18
#define VSPI_SS 	GPIO_NUM_5		// not used on board, done by MUX!

// SPI clock speed

#define SPI_DEFAULT_SPEED   4000000

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void spi_Init(void);
void spi_BeginTransaction(SPISettings settings);
void spi_EndTransaction(void);

void spi_WriteByte(const uint8_t data);
void spi_WriteWord(const uint16_t data);
void spi_ReadByte(uint8_t *byteData);
void spi_ReadWord(uint8_t *wordData);

uint8_t spi_TransferByte(uint8_t byteToSend);
uint16_t spi_TransferWord(uint16_t wordToSend);

void spi_SelectDevice(uint8_t spiDeviceNumber);
void spi_DeselectDevice(void);

#endif	// SPILIB_H_
