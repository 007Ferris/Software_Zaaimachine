///////////////////////////////////////////////////////////////////////////////
//
// TaskCLIHandler.h
//
// Authors: 	Roel Smeets
// Edit date: 	25-06-2025
//
///////////////////////////////////////////////////////////////////////////////


#ifndef TASKCLIHANDLER_H_
#define TASKCLIHANDLER_H_

#define BUF_LEN		80 	// Buffer Length setting for user CLI terminal
#define QUEUESIZE	5   // 5 elements in any Queue

extern  QueueHandle_t cliMessageQueue;

typedef struct 
{                       // Struct for CLI input
    char msg[20]; 
} CLI_MESSAGE;

void task_CLIHandler(void *param) ;

#endif	// TASKCLIHANDLER_H_