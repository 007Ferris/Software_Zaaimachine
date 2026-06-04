//////////////////////////////////////////////////////////////////////////////
//
// InterruptLib.cpp
//
// Authors: 	Roel Smeets (Avans)
// Edit date: 	28-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "InterruptLib.h"


/////////////////////////////////////////////////////////////////////////////
// bool interrupt_Disable(int16_t gpioNumber)

bool interrupt_Disable(int16_t gpioNumber)
{
	esp_err_t result = ESP_OK;

	result = gpio_intr_disable( (gpio_num_t) gpioNumber);

	return (result == ESP_OK);
}

/////////////////////////////////////////////////////////////////////////////
// bool interrupt_Enable(int16_t gpioNumber)

bool interrupt_Enable(int16_t gpioNumber)
{
	esp_err_t result = ESP_OK;

	result = gpio_intr_enable((gpio_num_t) gpioNumber);

	return (result == ESP_OK);
}


///////////////////////////////////////////////////////////////////////////////
// bool interrupt_AttachHandler(INTERRUPTHANDLER handler, int16_t 
//								gpioNumber, uint8_t flags)

bool interrupt_AttachHandler(INTERRUPTHANDLER handler, int16_t gpioNumber, uint8_t interruptFlag)
{
	int16_t interruptNumber = digitalPinToInterrupt(gpioNumber);
	bool attachOK = false;
	
	if ((interruptNumber != NOT_AN_INTERRUPT) && interrupt_IsValidFlag(interruptFlag))
	{
		attachInterrupt(interruptNumber, handler, interruptFlag);
		attachOK = true;
	}

	return attachOK;
}

///////////////////////////////////////////////////////////////////////////////
// bool interrupt_DetachHandler(int16_t gpioNumber)

bool interrupt_DetachHandler(int16_t gpioNumber)
{
	int16_t interruptNumber = digitalPinToInterrupt(gpioNumber);
	bool detachOK = false;

	if (interruptNumber != NOT_AN_INTERRUPT)
	{
		detachInterrupt(interruptNumber);
		detachOK = true;
	}

	return detachOK;
}

///////////////////////////////////////////////////////////////////////////////
// bool interrupt_IsValidFlag(uint8_t interruptFlag)

bool interrupt_IsValidFlag(uint8_t flag)
{
	bool valid = false;

	if 	( (flag == RISING) 	|| (flag == FALLING) || (flag == CHANGE) ||
		  (flag == ONLOW)	|| (flag == ONHIGH)  || 
		  (flag == ONLOW_WE)|| (flag == ONHIGH_WE)
		)
	{
		valid = true;
	}

	return valid;
}
