///////////////////////////////////////////////////////////////////////////////
//
// TaskDisplay.cpp
//
// Authors: 	Hans van der Linden
// Edit date: 	04-05-2026
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "OLEDLibESP32.h"
#include "Config.h"

#include "TaskDisplay.h"

///////////////////////////////////////////////////////////////////////////////
// defines

#define STRING_LENGTH (10+1)
#define QUEUE_LENGTH 12
#define SCREEN_UPDATE_INTERVAL_MS 800


//variabels
char screenBuffer[OLED_NLINES][STRING_LENGTH];
TimerHandle_t displayTimer = NULL;
bool displayTimerHasTimedOut = false;

void display_TimerCallback(TimerHandle_t timer);
static void writeToScreenBuffer(uint8_t lineNr, char *str);
static void showScreenBuffer();
static void clearScreenBuffer();

///////////////////////////////////////////////////////////////////////////////
// type definitions

typedef struct {
    uint8_t lineNr;
    char message[STRING_LENGTH];
} display_msg_t;

///////////////////////////////////////////////////////////////////////////////
// queue handle for passing messages to the display
QueueHandle_t displayMessageQueue = NULL;

///////////////////////////////////////////////////////////////////////////////
// void task_Display(void* pvParameters)

void task_Display(void *pvParameters)
{
    // Local variables
    display_msg_t message;

    // Create message queue
    displayMessageQueue = xQueueCreate(QUEUE_LENGTH, sizeof(display_msg_t));

     displayTimer = xTimerCreate("display_timer",
        pdMS_TO_TICKS(SCREEN_UPDATE_INTERVAL_MS),
        pdTRUE,
        (void *) NULL,
        (TimerCallbackFunction_t) display_TimerCallback);
        xTimerStart(displayTimer, portMAX_DELAY);

    while (true)
    {
		memset(&message, 0, sizeof(display_msg_t));	// Clear input buffer

        if(displayMessageQueue != NULL)
        {
            if(xQueueReceive(displayMessageQueue, &message, portMAX_DELAY) == pdTRUE)
            {
                // SerialPrintf("DEBUG msg received line %d msg %s\n", message.lineNr, message.message);
               // oled_Clear();
                //oled_WriteLine(message.lineNr, message.message, ALIGN_RIGHT);
                writeToScreenBuffer(message.lineNr, message.message);

            }
            if(displayTimerHasTimedOut)
            {
                displayTimerHasTimedOut = false;

                showScreenBuffer();
            }
        }
    }

    // Should never go here!
	vTaskDelete(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// bool display_SendFreqData(uint32_t potNr, double freq)

bool display_SendFreqData(uint32_t potNr, double freq)
{
    if (displayMessageQueue == NULL) return false;
    display_msg_t msg;
    msg.lineNr = 2 + potNr;
    //char str[STRING_LENGTH];
    snprintf(msg.message, STRING_LENGTH, "%5.2f", freq);
    return xQueueSend(displayMessageQueue, (void *) &msg, 400) == pdTRUE;
}

static void clearScreenBuffer()
{
    uint8_t lineNr = 0;
    for (lineNr = 0; lineNr < OLED_NLINES; lineNr++)
    {
        snprintf(screenBuffer[lineNr], STRING_LENGTH, "            ");
    }
}

static void writeToScreenBuffer(uint8_t lineNr, char *str)
{
    snprintf(screenBuffer[lineNr], STRING_LENGTH, "%s", str);
}

static void showScreenBuffer()
{
    int lineNr = 0;
    oled_Clear();
    for(lineNr = 0; lineNr < OLED_NLINES; lineNr++)
    {
        oled_WriteLine(lineNr, screenBuffer[lineNr], ALIGN_LEFT);
    }
}

void display_TimerCallback(TimerHandle_t timer)

{

    displayTimerHasTimedOut = true; // signal to the task via this variable

}