/*
 * utilities.h
 *
 * Created: 29-2-2024 22:33:03
 *  Author: Roel Smeets
 */ 


#ifndef UTILITIES_H_
#define UTILITIES_H_

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// function prototypes

char *dtos(double value, uint8_t precision = 3);
uint16_t dtosbuf(double value, char *buf, uint8_t precision = 3);

#endif
