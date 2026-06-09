///////////////////////////////////////////////////////////////////////////////
//
// io_handler.cpp
//
// Created: 27-05-2026
// Author:  Felix Ahner
//
///////////////////////////////////////////////////////////////////////////////

// system #includes

#include <Arduino.h>


///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "io_handler.h"
#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "Config.h"
#include "IOLib.h"
#include "ADC3208Lib.h"
#include "ButtonLib.h"

bool checkZaadInhoud();
uint64_t lastIRtrigger();
bool SeedInWheel();


static const ButtonBinding buttons[] = {
    { RESET_BUTTON,  &sem_resetButton,  "RESET"  },
    { ACTION_BUTTON, &sem_actionButton, "ACTION" },
    { STOP_BUTTON,   &sem_stopButton,   "STOP"   },
}; // tabel voor makkelijke toevoeging van knoppen


void updateOLED(uint8_t zaaiAfstand_mm, uint8_t zaaiDiepte_mm);
void OLED_write(uint8_t writeFunction);
///////////////////////////////////////////////////////////////////////////////
// void task_io_handler(void *pvParameters)

void task_io_handler(void *pvparameters)
{
    taskSleep(500); //opstart sleep
    uint8_t channel_afstandADC = 4;     //channel adc 1
    uint8_t channel_diepteADC  = 5;     //channel adc 2
    uint32_t ADC_Value1         = 0;    //adc waarde potmeter
    uint32_t ADC_Value2         = 0;    //adc waarde potmeter


    static uint32_t last_inhoud_check_ms = 0; // laatste inhouds check
    const uint32_t INHOUD_CHECK_INTERVAL_MS = 60000; // interval tussen checks (60sec)

    uint64_t last_IR_read_ms = 0;
    bool inhoudSensor = false;
    bool seedingWheel = false;


    ///
    while(true)
    {
        if(instellenMachine == 1)
        {
            xSemaphoreTake(sem_InstelWaarde_count, portMAX_DELAY);
            
            while(button_IsPressed(ACTION_BUTTON) == false)
            {
            //get afstand in mm
        
            ADC_Value1 = adc_ReadRaw(channel_afstandADC);
            zaaiAfstand_mm = map(ADC_Value1, 0, 4095, 1, MAX_ZAAIAFSTAND);

            ADC_Value2 = adc_ReadRaw(channel_diepteADC);
            zaaiDiepte_mm = map(ADC_Value2, 0, 4095, 1, MAX_ZAAIDIEPTE);
        
            updateOLED(zaaiAfstand_mm, zaaiDiepte_mm);
            }

            DiepteInstelling = zaaiDiepte_mm;
            xSemaphoreGive(sem_configUpdated);
            instellenMachine = 0;
            taskSleep(500);
        }   

        /////////////////////////////////////
        //knoppen controle
        /////////////////////////////////////

        // if(button_IsPressed(RESET_BUTTON) == true)
        // {
        //     SerialPrintf("*** RESET BUTTON PRESSED ***\n");
        //     xSemaphoreTake(sem_resetButton, 0);
        //     xSemaphoreGive(sem_resetButton);
        //     taskSleep(100);
        //     xSemaphoreTake(sem_actionButton, 100);
        // }
        // else if (button_IsPressed(ACTION_BUTTON) == true)
        // {
        //     SerialPrintf("*** ACTION BUTTON PRESSED ***\n");
        //     xSemaphoreTake(sem_actionButton, 0);
        //     xSemaphoreGive(sem_actionButton);
        //     taskSleep(100);
        //     xSemaphoreTake(sem_actionButton, 100);
        // }
        // else if (button_IsPressed(STOP_BUTTON) == true)
        // {
        //     SerialPrintf("*** STOP BUTTON PRESSED ***\n");
        //     xSemaphoreTake(sem_stopButton, 0);
        //     xSemaphoreGive(sem_stopButton);
        //     taskSleep(100);
        //     xSemaphoreTake(sem_stopButton, 100);
        // }

        for (const auto &b : buttons) // for loop auto definitie, adress van element van buttons
        {
            if (button_IsPressed(b.button)) // check de verschillende buttons
            {
                SerialPrintf("*** %s BUTTON PRESSED ***\n", b.name); // display buttonpredded + name
                xSemaphoreGive(*b.sem); // give consumer button semaphore
                taskSleep(100);   // debounce so we don't fire again next pass
                break;
            }
        }
        
        /////////////////////////////////////
        //sensoren controle
        /////////////////////////////////////

        if(machineRunning == true)
        {

            //check sensoren
            uint32_t now = millis();
            if (now - last_inhoud_check_ms >= INHOUD_CHECK_INTERVAL_MS) //check of interval al vergaan is (1min)
            {
                last_inhoud_check_ms = now;
                inhoudSensor = checkZaadInhoud(); // check zaad inhoud
                SerialPrintf("> Zaadinhoud %s\n", inhoudSensor ? "OK" : "*** LEEG ***");

                if (!inhoudSensor)
                {
                    // error: geen zaad in opslag
                    //SerialPrintf(">>> SENSOR EMPTY: MachineState was %d, &MS=%p\n",
                    //MachineState, (void*)&MachineState);

                    SerialPrintf("*** go to error ****\n");

                    MachineState = 0;
                    SerialPrintf(">>> SENSOR EMPTY: MachineState now %d\n", MachineState);
                }
            }

            last_IR_read_ms = lastIRtrigger();
        
            if(seedPlanted)
            {
                seedingWheel = SeedInWheel();
                SerialPrintf("> Zaadwielinhoud %s\n", seedingWheel ? "OK" : "*** LEEG ***");

                if(!seedingWheel)
                {
                    //error geen zaad in het zaaiwiel
                    MachineState = 0;
                }
                seedPlanted = !seedPlanted;
            }

        }
    }

    //never get here
    vTaskDelete(NULL);
}
bool checkZaadInhoud() // capacitieve sensor uitlezen
{
    bool current = 0;
    
    current = digitalRead(capacitieveSensor);
    if(current == 1)
    {
        return true;
    }

return false;
}
uint64_t lastIRtrigger() // bijhouden hoeveel ms vergaan zijn per zaaiproces
{

return 0;

}

bool SeedInWheel() // Checken of er een zaadje in het draaiwiel is
{
    bool current = 0;

    current = digitalRead(LEDsensor);
    if(current == 1)
    {
        return true;
    }

return false;
}