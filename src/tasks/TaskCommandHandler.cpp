///////////////////////////////////////////////////////////////////////////////
//
// TaskCLIHandler.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	02-06-2025
//
// Joel Brigida
// CDA 4102: Computer Architecture
// Implementation file for Task2: determine valid input.
// June 28, 2023
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "tasks/CommandParser.h"
#include "InfoRTOS.h"
#include "tasks/TaskCLIHandler.h"
#include "tasks/TaskCommandHandler.h"

///////////////////////////////////////////////////////////////////////////////
// void task_CommandHandler(void* param)

void task_CommandHandler(void* param)
{
    char buffer[BUF_LEN];   	// string buffer for terminal message

    while (true)
    {
		memset(buffer, 0, BUF_LEN);	// Clear input buffer

        if(cliMessageQueue != NULL)
        {
            if(xQueueReceive(cliMessageQueue, buffer, 0) == pdTRUE)
            {
				// SerialPrintf(">>> read from queue: [%s]\n", buffer);

				if (cmd_ParseCommand(buffer, "stats"))
				{
					// SerialPrintf("stats command\n");
					info_Tasks();
				}
				else if (cmd_ParseCommand(buffer, "cpu"))
				{
					// SerialPrintf("cpu command\n");
					// int32_t cmd_ParseInteger(String commandLine, String paramName, 
					// 	 int32_t previousValue, int32_t lowerLimit, int32_t upperlimit)
					info_CPU();
				}
				else if (cmd_ParseCommand(buffer, "ver"))
				{
					info_Version();
				}
				else
                {
					// SerialPrintf("> invalid command\n");
                }
            }
        }
		taskSleep(0);
    } 
}
