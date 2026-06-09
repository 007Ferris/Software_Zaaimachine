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
#include "ActuatorLib.h"

///////////////////////////////////////////////////////////////////////////////
// void task_motion_control(void *pvParameters)

void task_motion_control(void *pvparameters)
{
    taskSleep(500); //opstart sleep
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
        SerialPrintf("> instellen started in morion_control.cpp\n");

        /* actuator naar positie brengen */
        
        // io_init, qc_init en act_init geinitialiseerd in  platformInit(void)

        // act_MovePositionMm(DiepteInstelling);   /// push out to 35 mm DiepteInstelling = global ingeseldt vanuit io_handler
    


        taskSleep(3000); //test_sleep
        xSemaphoreGive(sem_motion_run);



    taskSleep(1000); //test_sleep
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

            SerialPrintf("> Home pos in motion_control.cpp\n");
            taskSleep(2000); //test_sleep

            /// act_Home();

            SerialPrintf("> Home pos in motion_control.cpp klaar\n");
}