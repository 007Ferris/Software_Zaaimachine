///////////////////////////////////////////////////////////////////////////////
//
// motion_control.cpp
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
#include "QC7366Lib.h"
#include "ActuatorLib.cpp"

///////////////////////////////////////////////////////////////////////////////
// void task_motion_control(void *pvParameters)

void task_motion_control(void *pvparameters)
{
    ///
    ///
    ///
    while(true)
    {
        ///////////////////////////////////////////////////////////////////////
        ///
        ///Runnen van machine
        ///
        ///////////////////////////////////////////////////////////////////////
        xSemaphoreTake(sem_motion_run, portMAX_DELAY);
        SerialPrintf("> seeding started in morion_control.cpp\n");

        io_Init();
        qc_Init();
        act_Init();

        


    taskSleep(1000);
    }

    //never get here
    vTaskDelete(NULL);
}

void ActuatorBasis()
{
    // stel de actuator diepte in de basis positie
        ///////////////////////////////////////////////////////////////////////
        ///
        ///Instellen van machine diepte
        ///
        ///////////////////////////////////////////////////////////////////////
            SerialPrintf("> Parameters in motion_control.cpp\n");
            taskSleep(2000);





            SerialPrintf("> Parameters in motion_control.cpp klaar\n");
}