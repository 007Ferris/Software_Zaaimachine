///////////////////////////////////////////////////////////////////////////////
//
// EepromLIB.h
//
// Created: 01-09-2024
//			14-08-2025
//
// Author:  Roel Smeets
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EEPROMLIB_H_
#define EEPROMLIB_H_

///////////////////////////////////////////////////////////////////////////////
// enums

enum eeprom_type_t  
{
    EEPROM_I2C,
    EEPROM_SPI,
};

//////////////////////////////////////////////////////////////////////////////
// function prototypes

bool eeprom_Init(eeprom_type_t type);

uint32_t eeprom_GetDeviceSize(eeprom_type_t type);
uint16_t eeprom_Verify(eeprom_type_t type, uint8_t value);
uint16_t eeprom_VerifyIncrement(eeprom_type_t type, uint8_t startValue);
uint16_t eeprom_FillIncrement(eeprom_type_t type, uint8_t startValue);
uint16_t eeprom_Fill(eeprom_type_t type, uint8_t value);

#endif	//	EEPROMLIB_H_
