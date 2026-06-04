///////////////////////////////////////////////////////////////////////////////
//
// OLEDLibESP32.h
//
// Author:	 	Roel Smeets
// Edit date: 	13-03-2023
//				25-06-2025
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"


///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "Config.h"
#include "I2CLib.h"
#include "SSD1306Wire.h"
#include "OLEDLibESP32.h"

///////////////////////////////////////////////////////////////////////////////
// the OLED display object (file global)

static SSD1306Wire display(I2C_ADDRESS_OLED, I2C_SDA_PIN, I2C_SCL_PIN);

///////////////////////////////////////////////////////////////////////////////
// bool oled_Init(void)

bool oled_Init(void)
{
	bool result = false;
	
	result = display.init();

	display.setFont(ArialMT_Plain_10);
	display.setContrast(250);
	display.setBrightness(255);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.flipScreenVertically();

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// void oled_Clear(void)

void oled_Clear(void)
{
	display.clear();
}

//////////////////////////////////////////////////////////////////////////////
// oled_WriteLine(uint8_t row, const char *message, uint8_t align)

void oled_WriteLine(uint8_t line, const char *message, uint8_t align)
{
	uint8_t startCol = 0;
	
	switch (align)
	{
		case ALIGN_LEFT:
		startCol = 0;
		display.setTextAlignment(TEXT_ALIGN_LEFT);
		break;
		
		case ALIGN_CENTER:
		startCol = OLED_XSIZE / 2;
		display.setTextAlignment(TEXT_ALIGN_CENTER);
		break;
		
		case ALIGN_RIGHT:
		startCol = OLED_XSIZE;
		display.setTextAlignment(TEXT_ALIGN_RIGHT);
		break;
	}

	line = constrain(line, 0, OLED_NLINES - 1);

	display.drawString(startCol, line * OLED_LINEHEIGTH, message);
	display.display();
}
