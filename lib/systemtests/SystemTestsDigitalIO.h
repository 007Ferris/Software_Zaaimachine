//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsDigitalIO.h
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEMTESTS_IO_H
#define SYSTEMTESTS_IO_H

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <inttypes.h>

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void test_io_WalkLeft(void);
void test_io_WalkRight(void);
uint8_t test_io_RW(uint8_t write);
uint32_t test_io_Loopback(void);
void test_io_LEDS_Repeat(void);
void test_io_LEDS(void);

void test_io_Loopback_Repeat(void);
void test_io_Outputs_Button_Repeat(void);
void test_io_LEDIO15(void);
void test_io_LEDIO15_Repeat(void);
void test_io_LEDIO15_Button_Repeat(void);

#endif  // SYSTEMTESTS_IO_H
