/*
 * Utilities.cpp
 *
 * Created: 6-3-2024 17:56:46
 *  Author: Roel Smeets
 */ 


///////////////////////////////////////////////////////////////////////////////
// #includes

#include "Arduino.h"

// #if (TARGET == NANO33_IOT)
// #include "api/deprecated-avr-comp/avr/dtostrf.c.impl"
// #endif

///////////////////////////////////////////////////////////////////////////////
// char *dtos(double value, uint8_t precision)
// NOT re-entrant!! because of static string - just ONE instance!

char *dtos(double value, uint8_t precision)
{
	static char doubleString[20];
	uint8_t minWidth  = 0;

	dtostrf(value, minWidth, precision, doubleString);
	
	return doubleString;
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t dtosbuf(double value, char *buf, uint8_t precision)
// re-entrant version, needs a user-supplied destination buffer (buf)

uint16_t dtosbuf(double value, char *buf, uint8_t precision)
{
	uint8_t  minWidth  = 0;
	uint16_t len = 0;

	dtostrf(value, minWidth, precision, buf);
	len = strlen(buf);
	
	return len;
}
