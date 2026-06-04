///////////////////////////////////////////////////////////////////////////////
//
// NodeMCU32Blink_v01.cpp
//
// Authors: 	Roel Smeets (Avans)
// Edit date: 	02-06-2025
//
// Joel Brigida
// CDA 4102: Computer Architecture
// Implementation file for Reading User Input
// June 28, 2023
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "tasks/TaskCLIHandler.h"

///////////////////////////////////////////////////////////////////////////////
// global queue handle for passing messages

QueueHandle_t cliMessageQueue = NULL;

///////////////////////////////////////////////////////////////////////////////
// void task_CLIHandler(void *param)

void task_CLIHandler(void *param) 
{
    CLI_MESSAGE cliMessage;
    char input = 0;                                     
    char buffer[BUF_LEN];   
    uint8_t index = 0;     
	bool eol = false;

    memset(buffer, 0, BUF_LEN);   

    while (true)
    {       
        if(Serial.available() > 0)
        {
            input = Serial.read();  
			eol = (input == '\r') || (input == '\n');

            if (index < (BUF_LEN - 1))
            {
				if (eol == false)
				{
	                buffer[index] = input;
	                index++;
				}
				else
				{
	                buffer[index] = '\0';	// terminate input string
					while (Serial.available()) Serial.read();
				}
            }

            if (eol)	// end of line = user presses ENTER key
            {
                strcpy(cliMessage.msg, buffer);
				// SerialPrintf("> sending to queue: [%s]", cliMessage.msg);
                xQueueSend(cliMessageQueue, (void *)(&cliMessage.msg), 100);	// Send to cliMessageQueue for handler
                memset(buffer, 0, BUF_LEN);
                index = 0;
            }
            else // echo each character back to the serial terminal
            {
                // SerialPrintf("%c", input);
            }
        }

		taskSleep(20);	// yield to other tasks
    }
}
