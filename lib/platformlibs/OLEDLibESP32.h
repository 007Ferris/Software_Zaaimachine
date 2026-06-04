/*
 * OLEDLib.h
 *
 * Created: 14-3-2023 06:24:46
 *  Author: Roel Smeets
 */ 


#ifndef OLEDLIB_H_
#define OLEDLIB_H_

///////////////////////////////////////////////////////////////////////////////
// #defines

#define ALIGN_LEFT				0
#define ALIGN_RIGHT				1
#define ALIGN_CENTER			2

#define OLED_NLINES				4

#define OLED_XSIZE				128
#define OLED_YSIZE				64

#define OLED_LINEHEIGTH 		(OLED_YSIZE / OLED_NLINES)

///////////////////////////////////////////////////////////////////////////////
// function prototypes

bool oled_Init(void);
void oled_Clear(void);
void oled_WriteLine(uint8_t line, const char *message, uint8_t align);

#endif /* OLEDLIB_H_ */