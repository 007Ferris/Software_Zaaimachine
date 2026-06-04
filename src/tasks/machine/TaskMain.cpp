///////////////////////////////////////////////////////////////////////////////
//
// TaskMain.cpp
//
// Created: 27-05-2026
// Author:  Felix Ahner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "Config.h"
#include "IOLib.h"
#include "ButtonLib.h"

void OLED_write(uint8_t writeFunction);
void ActuatorBasis();
///////////////////////////////////////////////////////////////////////////////
// void task_Main(void *pvParameters)

void task_Main(void *pvparameters)
{
    taskSleep(500);
    OLED_write(10);
    taskSleep(500);
    uint8_t MachineState = 0; //case: 0 NOODSTOP, 1 OPSTART, 2 Parameters instellen, 3 Parameters akkoord, 4 parameters uitvoeren, 5 Standby, 6 running, 7 stopping, 8 error

    ///
    while(true)
    {
        
        switch (MachineState) // gehele main frame in cases opdelen
        {
        case 0: /// NOODSTOP CASE RESET NEEDED
            /* code */
            machineRunning = false;

            SerialPrintf("> *** MachineState 0, NOODSTOP ***\n");
            OLED_write(6);
            //check for high signal ESTOP
            taskSleep(1000);
            OLED_write(3); // need reset input for whole process to start
            xSemaphoreTake(sem_resetButton, portMAX_DELAY);
            xSemaphoreGive(sem_resetButton);

            MachineState = 1;
            break;

        case 1:
            /* code */
            SerialPrintf("> *** MachineState 1, OPSTART ***\n");
            
            OLED_write(2);
            taskSleep(2000);
            // hier nog de opstart code
            ActuatorBasis();

            MachineState = 2;
            break;
        
        case 2:
            /* code */
            // wacht op instelwaardes
            SerialPrintf("> *** MachineState 2, Parameters instellen ***\n");

            instellenMachine = 1;

            xSemaphoreGive(sem_InstelWaarde_count); //zet instelwaardes vast zaaiafstand
            SerialPrintf("> machine parameters worden ingesteld...\n");
            taskSleep(100);
            xSemaphoreTake(sem_configUpdated, portMAX_DELAY);
            SerialPrintf("> Zaaiafstand ingesteld\n ");
            SerialPrintf("> Zaaidiepte ingesteld\n");
        
            instellenMachine = 0;

            OLED_write(1); //parameters zijn ingesteld
        

            taskSleep(2000);
            OLED_write(9);

            MachineState = 3;

            break;

        case 3:
            /* code */
            
             // akkoord gaan met parameters
            SerialPrintf("> *** MachineState 3, Parameters akkoord ***\n");
            if(button_IsPressed(ACTION_BUTTON) == true)
            {
                SerialPrintf("> Value's are accepted\n");
                MachineState = 4;
                    
            }
            else if(button_IsPressed(RESET_BUTTON) == true)
            {
                SerialPrintf("> Value's are rejected\n");

                MachineState = 2;
            }
            
            break;

        case 4:
            /* code */
            SerialPrintf("> *** MachineState 4, Parameters uitvoeren ***\n");

            // machine can now addapt the right distances for the given value's
        
            SerialPrintf("> Machine is addapting value's\n");
            OLED_write(2);
            xSemaphoreGive(sem_motion_run); // motion control will run and go to given value
            taskSleep(3000);
            xSemaphoreTake(sem_motion_run, portMAX_DELAY); // distance value's are set


            MachineState = 5;

            break;
        
        case 5:
            /* code */
            SerialPrintf("> *** MachineState 5, Standby ***\n");

            SerialPrintf("> Machine waits on start\n");
            OLED_write(4);  //start het planten
            xSemaphoreTake(sem_actionButton, portMAX_DELAY);
            xSemaphoreGive(sem_actionButton);

            
            MachineState = 6;

            break;

        case 6:
            /* code */
            SerialPrintf("> *** MachineState 6, Running ***\n");

            SerialPrintf("> Machine is starting to plant\n");
            xSemaphoreGive(sem_seeding_run);
            OLED_write(8);

            //machineRunning = true; //enable sensor checks

            SerialPrintf("> Machine is planting...\n");

            taskSleep(10000);
            SerialPrintf(">>> herhalen van programma!!!!!");


            MachineState = 0;

            break;

        default:
            break;
        }



    }

    //never get here
    vTaskDelete(NULL);
}