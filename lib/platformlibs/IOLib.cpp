///////////////////////////////////////////////////////////////////////////////
//
// IOLib.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	02-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "IOLib.h"

///////////////////////////////////////////////////////////////////////////////
// void io_InitPins(void)


static const uint8_t g_InputPins[N_INPUT_BITS] =
{
	GPIO_NUM_36,	// LSB, bit 0
	GPIO_NUM_39,
	GPIO_NUM_34,
	GPIO_NUM_35,
	GPIO_NUM_32,
	GPIO_NUM_33,	// MSB, bit 5
};

static const uint8_t g_OutputPins[N_OUTPUT_BITS] =
{
	GPIO_NUM_25,	// LSB, bit 0
	GPIO_NUM_26,
	GPIO_NUM_27,
	GPIO_NUM_14,
	GPIO_NUM_12,
	GPIO_NUM_13,	// MSB, bit 5
};


///////////////////////////////////////////////////////////////////////////////
// void io_Init(void)

void io_Init(void)
{
	uint8_t pin = 0;

	for (pin = 0; pin < N_INPUT_BITS; pin++)
	{
		pinMode(g_InputPins[pin], INPUT_PULLDOWN); 
	}

	for (pin = 0; pin < N_OUTPUT_BITS; pin++)
	{
		pinMode(g_OutputPins[pin], OUTPUT); 
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t io_GetInput(void)

uint8_t io_GetInput(void)
{
	uint8_t value = 0;
	uint8_t bitNr = 0;

	for (bitNr = 0; bitNr < N_INPUT_BITS; bitNr++)
	{
		if (digitalRead(g_InputPins[bitNr]) == HIGH)
		{
			value = value | (0x01 << bitNr);
		}
	}

	return value;
}

///////////////////////////////////////////////////////////////////////////////
// bool io_IsValidBitNumber(uint8_t bitNumber)

bool io_IsValidBitNumber(uint8_t bitNumber)
{
	bool isValid = false;

	isValid = (bitNumber < N_INPUT_BITS);

	return isValid;
}

///////////////////////////////////////////////////////////////////////////////
// bool io_IsBitSet(uint8_t bitNumber)

bool io_IsBitSet(uint8_t bitNumber)
{
	bool isBitSet = false;

	if (io_IsValidBitNumber(bitNumber))
	{
		isBitSet = (digitalRead(bitNumber) == HIGH);
	}
	
	return isBitSet;
}

///////////////////////////////////////////////////////////////////////////////
// void io_SetOutput(uint8_t value)

void io_SetOutput(uint8_t value)
{
	uint8_t bitNr = 0;
	uint8_t bitOn = LOW;

	for(bitNr = 0; bitNr < N_OUTPUT_BITS; bitNr++)
	{
		if ((value & (0x01 << bitNr)) != 0)
		{
			bitOn = HIGH;
		}
		else
		{
			bitOn = LOW;
		}
		digitalWrite(g_OutputPins[bitNr], bitOn);
	}
}

///////////////////////////////////////////////////////////////////////////////
// oid io_SetBit(uint8_t bitNumber, bool bitOn)

void io_SetBit(uint8_t bitNumber, bool bitOn)
{
	uint8_t bitValue = bitOn ? HIGH : LOW;

	if (io_IsValidBitNumber(bitNumber))
	{
		digitalWrite(g_OutputPins[bitNumber], bitValue);
	}
}

///////////////////////////////////////////////////////////////////////////////
// int16_t io_GetGPIONumberInput(uint8_t inputBitNumber)

int16_t io_GetGPIONumberInput(uint8_t inputBitNumber)
{
	int16_t gpioNumber = -1;

	if (inputBitNumber < N_INPUT_BITS)
	{
		gpioNumber = g_InputPins[inputBitNumber];
	}

	return gpioNumber;
}
