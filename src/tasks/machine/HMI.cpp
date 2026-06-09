///////////////////////////////////////////////////////////////////////////////
//
// HMI.cpp
//
// Created: 27-05-2026
// Author:  Felix Ahner
//
///////////////////////////////////////////////////////////////////////////////

// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "Config.h"
#include "IOLib.h"
#include "OLEDLibESP32.h"

///////////////////////////////////////////////////////////////////////////////
// void task_HMI(void *pvParameters)

void task_HMI(void *pvparameters)
{

    taskSleep(500); //opstart sleep
    
    ///
    while(true)
    {
        //hmi structuur naar AmigA
    }

    //never get here
    vTaskDelete(NULL);
}

void updateOLED(uint8_t zaaiAfstand, uint8_t zaaiDiepte)
{
        //display afstand in mm
    char buf1[22];
    char buf2[22];



    sniprintf(buf1, sizeof(buf1), " %d", zaaiAfstand);
    sniprintf(buf2, sizeof(buf2), " %d", zaaiDiepte);

    oled_Clear();
    oled_WriteLine(0, "ZaaiAfstand(mm)",    ALIGN_CENTER);
	oled_WriteLine(1, buf1,                 ALIGN_CENTER);
	oled_WriteLine(2, "ZaaiDiepte(mm)",     ALIGN_CENTER);
    oled_WriteLine(3, buf2,                 ALIGN_CENTER);
    taskSleep(500); //update of OLED delay
    
}
void OLED_write(uint8_t writeFunction)//case: 1 parameter, 2 wachten, 3 press reset, 4 press action, 5 press stop, 6 Estop, 7 Error, 8 machine working, 9 akkoord action, 10 clear
{
    switch (writeFunction)
    {
    case 1:
        /* code */
        oled_Clear();
        oled_WriteLine(0, "Parameters",     ALIGN_CENTER);
	    oled_WriteLine(1, "Zijn",           ALIGN_CENTER);
	    oled_WriteLine(2, "Ingesteld",      ALIGN_CENTER);

        break;
    case 2:

        oled_Clear();
        oled_WriteLine(0, "Machine aan",    ALIGN_CENTER);
	    oled_WriteLine(1, "het instellen",  ALIGN_CENTER);
	    oled_WriteLine(3, "Wachten...",     ALIGN_CENTER);

        break;
    case 3:
        oled_Clear();
        oled_WriteLine(0, "Input needed",   ALIGN_CENTER);
	    oled_WriteLine(1, "press button:",  ALIGN_CENTER);
	    oled_WriteLine(3, "RESET",          ALIGN_CENTER);

        break;
    case 4:
        oled_Clear();
        oled_WriteLine(0, "Input needed",   ALIGN_CENTER);
	    oled_WriteLine(1, "press button:",  ALIGN_CENTER);
	    oled_WriteLine(3, "ACTION",         ALIGN_CENTER);
        break;
    case 5:
        oled_Clear();
        oled_WriteLine(0, "Input needed",   ALIGN_CENTER);
	    oled_WriteLine(1, "press button:",  ALIGN_CENTER);
	    oled_WriteLine(3, "STOP",           ALIGN_CENTER);
        break;
    case 6:
        oled_Clear();
        oled_WriteLine(0, "***ERROR***",    ALIGN_CENTER);
	    oled_WriteLine(1, "ESTOP PRESSED",  ALIGN_CENTER);
        oled_WriteLine(2, "MACHINE STOPPED",ALIGN_CENTER);
	    oled_WriteLine(3, "............",   ALIGN_CENTER);
        break;
    case 7:
        oled_Clear();
        oled_WriteLine(0, "***ERROR***",    ALIGN_CENTER);
	    oled_WriteLine(1, "MACHINE ERROR",  ALIGN_CENTER);
        oled_WriteLine(2, "MACHINE STOPPED",ALIGN_CENTER);
	    oled_WriteLine(3, "............",   ALIGN_CENTER);
        break;
    case 8:
        oled_Clear();
        oled_WriteLine(0, "Zaaimachine",    ALIGN_CENTER);
	    oled_WriteLine(1, "Aan het",        ALIGN_CENTER);
        oled_WriteLine(2, "Zaaien",         ALIGN_CENTER);
	    oled_WriteLine(3, "............",   ALIGN_CENTER);
        break;
    case 9:
        oled_Clear();
        oled_WriteLine(0, "Parameter Akkoord?", ALIGN_CENTER);
	    oled_WriteLine(1, "PRESS ACTION",       ALIGN_CENTER);
        oled_WriteLine(2, "OR",                 ALIGN_CENTER);
	    oled_WriteLine(3, "RESET BUTTON",       ALIGN_CENTER);
        break;
    case 10:
        oled_Clear();
    break;
    }
}