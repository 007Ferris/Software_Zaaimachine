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

void updateOLED(uint8_t zaaiAfstand_mm, uint8_t zaaiDiepte_mm);
void OLED_write(uint8_t writeFunction);
///////////////////////////////////////////////////////////////////////////////
// void task_io_handler(void *pvParameters)

void task_io_handler(void *pvparameters)
{
    taskSleep(500);
    uint8_t channel_afstandADC = 4;     //channel adc 1
    uint8_t channel_diepteADC  = 5;     //channel adc 2
    uint32_t ADC_Value1         = 0;    //adc waarde potmeter
    uint32_t ADC_Value2         = 0;    //adc waarde potmeter
    uint8_t zaaiAfstand_mm      = 0;    // gemapte waarde van adc  
    uint8_t zaaiDiepte_mm       = 0;    // gemapte waarde van adc

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
            zaaiAfstand_mm = map(ADC_Value1, 0, 4095, 0, MAX_ZAAIAFSTAND);

            ADC_Value2 = adc_ReadRaw(channel_diepteADC);
            zaaiDiepte_mm = map(ADC_Value2, 0, 4095, 0, MAX_ZAAIDIEPTE);
        
            updateOLED(zaaiAfstand_mm, zaaiDiepte_mm);
            taskSleep(500); 
                
            }
            instellenMachine = 0;
            xSemaphoreGive(sem_configUpdated);
            taskSleep(500);
        }   

        /////////////////////////////////////
        //knoppen controle
        /////////////////////////////////////

        if(button_IsPressed(RESET_BUTTON) == true)
        {
            SerialPrintf("*** RESET BUTTON PRESSED ***\n");
            xSemaphoreGive(sem_resetButton);
            taskSleep(100);
            xSemaphoreTake(sem_actionButton, 10);
        }
        else if (button_IsPressed(ACTION_BUTTON) == true)
        {
            SerialPrintf("*** ACTION BUTTON PRESSED ***\n");
            xSemaphoreGive(sem_actionButton);
            taskSleep(100);
            xSemaphoreTake(sem_actionButton, 10);
        }
        else if (button_IsPressed(STOP_BUTTON) == true)
        {
            SerialPrintf("*** STOP BUTTON PRESSED ***\n");
            xSemaphoreGive(sem_stopButton);
            taskSleep(100);
            xSemaphoreTake(sem_stopButton, 10);
        }
        
        


        if(machineRunning == true)
        {
            //check sensoren
            inhoudSensor = checkZaadInhoud();
            SerialPrintf("> Zaadinhuod %s\n", inhoudSensor ? "OK" : "*** LEEG ***");
            if(inhoudSensor == false)
            {
                //error geen zaad in opslag
                OLED_write(7);
            }

            last_IR_read_ms = lastIRtrigger();
        

            seedingWheel = SeedInWheel();
            SerialPrintf("> Zaadwielinhoud %s\n", seedingWheel ? "OK" : "*** LEEG ***");
            if(seedingWheel == false)
            {
                //error geen zaad in het zaaiwiel
                OLED_write(7);
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