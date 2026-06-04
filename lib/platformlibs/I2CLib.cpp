//////////////////////////////////////////////////////////////////////////////
//
// I2CLib.cpp
//
// Authors: 	Roel Smeets (Avans)
// Edit date: 	25-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include <Wire.h>
#include "I2CLib.h"
#include "SerialPrintf.h"

///////////////////////////////////////////////////////////////////////////////
// file global data

static bool G_IsI2CInitialised = false;


///////////////////////////////////////////////////////////////////////////////
// void i2c_Init(void)
//
// initializes I2C channel on the NodeMCU
// returns true if successfully assigned, false on fail

bool i2c_Init(void)
{
	uint8_t i2cStatus = I2C_BUS_OK;
	bool wireOK	= true;		// assume success
	bool initOK	= true;		// assume success
	bool busOK	= true;		// assume success

	if (G_IsI2CInitialised == false)	// prevents multiple inits
	{
		// always do a hardware bit-bang reset of the I2C bus on 
		// init in case of lock-ups...
		
		wireOK 		= Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
		i2cStatus 	= i2c_ClearBus(I2C_SCL_PIN, I2C_SDA_PIN);
		busOK = (i2cStatus == I2C_BUS_OK) || (i2cStatus == I2C_BUS_REPAIRED);

		initOK = wireOK & busOK;

		G_IsI2CInitialised = initOK;
	}
	
	return initOK;
}


///////////////////////////////////////////////////////////////////////////////
//  uint8_t i2c_ClearBus(uint8_t sclPin, uint8_t sdaPin)
//
// resets the entire I2C bus & all devices. Otherwise, only power down 
// is an option... :-(

static uint8_t G_I2CClockRepairCount = 0;

uint8_t i2c_ClearBus(uint8_t sclPin, uint8_t sdaPin)
{
	// Make sdaPin (data) and sclPin (clock) pins Inputs with pull-up.

	pinMode(sdaPin, INPUT_PULLUP);
	pinMode(sclPin, INPUT_PULLUP);

	delay(100);
	
	// If SCL is held low, NodeMCU cannot become the I2C master:
	
	bool isSCLLow = (digitalRead(sclPin) == LOW);
	if (isSCLLow) 
	{	
		// Make sdaPin (data) and sclPin (clock) pins inputs with pull-up.
		
		pinMode(sdaPin, INPUT_PULLUP);
		pinMode(sclPin, INPUT_PULLUP);

		// I2C bus error. Could not clear sclPin, clock line held low	
		return I2C_ERR_CLK_LOW;		
	}
		
	bool isSDALow = (digitalRead(sdaPin) == LOW);

	// if SCL is high and SDA is high, bus is ok:
	
	if ((isSCLLow == false) && (isSDALow == false))
	{
		return I2C_BUS_OK;
	}
	
	int clockCount = 40; // > 2 x 9 clock, see spec of I2C bus

	//  if sdaPin is Low, send dummy clock pulses to the slave:
	
	while (isSDALow && (clockCount > 0)) 
	{ 
		clockCount--;
		
		// Note: I2C bus is open collector so do NOT drive sclPin or sdaPin high.

		pinMode(sclPin, INPUT);		// release sclPin pull-up so that when made output it will be LOW
		pinMode(sclPin, OUTPUT);	// then clock sclPin Low

		delayMicroseconds(20); 		//  for > 5us
		
		pinMode(sclPin, INPUT); 		// release sclPin LOW
		pinMode(sclPin, INPUT_PULLUP);	// turn on pullup resistors again
		
		// do not force high as slave may be holding it low for clock stretching.
		// the > 5us is so that even the slowest I2C devices are handled.
		
		delayMicroseconds(20); //  for >5us

		G_I2CClockRepairCount++;
		
		isSCLLow = (digitalRead(sclPin) == LOW); // Check if sclPin is low

		int counter = 20;
		while (isSCLLow && (counter > 0)) //  loop waiting for sclPin to become high only wait 2sec.
		{  
			counter--;
			delay(100);
			isSCLLow = (digitalRead(sclPin) == LOW);
		}
		
		// still low after 2 sec == error:
		
		if (isSCLLow) 
		{ 
			// Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
			pinMode(sdaPin, INPUT_PULLUP);
			pinMode(sclPin, INPUT_PULLUP);
			
			// I2C bus error, could not clear the bus. sclPin clock line held low 
			// by slave clock stretch for > 2sec
			
			return I2C_ERR_CLK_LOW_SLAVE; 
		}
		isSDALow = (digitalRead(sdaPin) == LOW); //   and check sdaPin input again and loop
	}	
	
	if (isSDALow) // still low
	{ 
		// Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
		pinMode(sdaPin, INPUT_PULLUP);
		pinMode(sclPin, INPUT_PULLUP);
		
		// I2C bus error. Could not clear. sdaPin data line held low.
		
		return I2C_ERR_SDA_LOW; 
	}

	// else pull sdaPin line low for Start or Repeated Start

	pinMode(sdaPin, INPUT);		// remove pullup
	pinMode(sdaPin, OUTPUT);	// and then make it LOW i.e. send an I2C Start or Repeated start control.

	// When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
	/// A Repeat Start is a Start occurring after a Start with no intervening Stop.

	delayMicroseconds(20); 			// wait > 5us
	
	pinMode(sdaPin, INPUT); 		// remove output low
	pinMode(sdaPin, INPUT_PULLUP);	// and make sdaPin high i.e. send I2C STOP control.
	
	delayMicroseconds(20); // wait > 5us
	
	// Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
	pinMode(sdaPin, INPUT_PULLUP); 
	pinMode(sclPin, INPUT_PULLUP);

	return I2C_BUS_REPAIRED; // repaired, I2C bus OK now
}

///////////////////////////////////////////////////////////////////////////////
//  uint8_t i2c_GetRepairCount(void)

uint8_t i2c_GetRepairCount(void)
{
	return G_I2CClockRepairCount;
}

///////////////////////////////////////////////////////////////////////////////
//  const char *i2c_GetErrorMessage(uint8_t i2cErrorCode)

const char *i2c_GetErrorMessage(uint8_t i2cErrorCode)
{
	const char *msg = NULL;
	
	if (i2cErrorCode == I2C_BUS_OK)
	{
		msg = "I2C - BUS OK";
	}
	else if (i2cErrorCode == I2C_BUS_REPAIRED)
	{
		msg = "I2C - BUS REPAIRED";
	}
	else if (i2cErrorCode == I2C_ERR_CLK_LOW)
	{
		msg = "I2C - CLK LINE LOW";
	}
	else if (i2cErrorCode == I2C_ERR_SDA_LOW)
	{
		msg = "I2C - SDA LINE LOW";
	}
	else if (i2cErrorCode == I2C_ERR_CLK_LOW_SLAVE)
	{
		msg = "I2C - SLAVE CLK LINE LOW";
	}
	else
	{
		msg = "I2C - INVALID ERROR CODE";
	}
	
	return msg;
}

//////////////////////////////////////////////////////////////////////////////
//  uint8_t i2c_ClearBus(void)
//
// for documentation & background of this code, see: 
//
// https://spellfoundry.com/2020/06/25/reliable-embedded-systems-recovering-arduino-i2c-bus-lock-ups/
// http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html


/*
int I2C_ClearBus(int sdaPin, int sclPin) {
	#if defined(TWCR) && defined(TWEN)
	TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the sdaPin and sclPin pins directly
	#endif
	pinMode(sdaPin, INPUT_PULLUP); // Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
	pinMode(sclPin, INPUT_PULLUP);
	#ifndef ARDUINO_nRF52832_BARE_MODULE
	delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
	// skip this delay for bare module NRF5 chips as it will cause high power and prevent monitoring current consumption
	#endif
	// up of the DS3231 module to allow it to initialize properly,
	// but is also assists in reliable programming of FioV3 boards as it gives the
	// IDE a chance to start uploaded the program
	// before existing sketch confuses the IDE by sending Serial data.

	boolean SCL_LOW = (digitalRead(sclPin) == LOW); // Check is sclPin is Low.
	if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master.
		pinMode(sdaPin, INPUT_PULLUP); // Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
		pinMode(sclPin, INPUT_PULLUP);
		return 1; //I2C bus error. Could not clear sclPin clock line held low
	}

	boolean SDA_LOW = (digitalRead(sdaPin) == LOW);  // vi. Check sdaPin input.
	int clockCount = 20; // > 2x9 clock

	while (SDA_LOW && (clockCount > 0)) { //  vii. If sdaPin is Low,
		clockCount--;
		// Note: I2C bus is open collector so do NOT drive sclPin or sdaPin high.
		pinMode(sclPin, INPUT); // release sclPin pullup so that when made output it will be LOW
		pinMode(sclPin, OUTPUT); // then clock sclPin Low
		delayMicroseconds(10); //  for >5us
		pinMode(sclPin, INPUT); // release sclPin LOW
		pinMode(sclPin, INPUT_PULLUP); // turn on pullup resistors again
		// do not force high as slave may be holding it low for clock stretching.
		delayMicroseconds(10); //  for >5us
		// The >5us is so that even the slowest I2C devices are handled.
		SCL_LOW = (digitalRead(sclPin) == LOW); // Check if sclPin is Low.
		int counter = 20;
		while (SCL_LOW && (counter > 0)) {  //  loop waiting for sclPin to become High only wait 2sec.
			counter--;
			delay(100);
			SCL_LOW = (digitalRead(sclPin) == LOW);
		}
		if (SCL_LOW) { // still low after 2 sec error
			pinMode(sdaPin, INPUT_PULLUP); // Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
			pinMode(sclPin, INPUT_PULLUP);
			return 2; // I2C bus error. Could not clear. sclPin clock line held low by slave clock stretch for >2sec
		}
		SDA_LOW = (digitalRead(sdaPin) == LOW); //   and check sdaPin input again and loop
	}
	if (SDA_LOW) { // still low
		pinMode(sdaPin, INPUT_PULLUP); // Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
		pinMode(sclPin, INPUT_PULLUP);
		return 3; // I2C bus error. Could not clear. sdaPin data line held low
	}

	// else pull sdaPin line low for Start or Repeated Start
	pinMode(sdaPin, INPUT); // remove pullup.
	pinMode(sdaPin, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
	// When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
	/// A Repeat Start is a Start occurring after a Start with no intervening Stop.
	delayMicroseconds(10); // wait >5us
	pinMode(sdaPin, INPUT); // remove output low
	pinMode(sdaPin, INPUT_PULLUP); // and make sdaPin high i.e. send I2C STOP control.
	delayMicroseconds(10); // x. wait >5us
	pinMode(sdaPin, INPUT_PULLUP); // Make sdaPin (data) and sclPin (clock) pins Inputs with pullup.
	pinMode(sclPin, INPUT_PULLUP);
	return 0; // all ok
}

*/
