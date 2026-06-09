///////////////////////////////////////////////////////////////////////////////
//
// RTSW_Application.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	02-06-2025
//				10-08-2025
//				20-11-2025
//              30-04-2026 .. 05-05-2026 [Hans van der Linden]
//              27-05-2026 .. ..-..-.... [Felix Ahner]
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "WiFi.h"

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "InfoRTOS.h"
#include "IOLib.h"
#include "InterruptLib.h"
#include "LEDLib.h"
#include "ButtonLib.h"
#include "SPILib.h"
#include "I2CLib.h"
#include "OLEDLibESP32.h"
#include "I2CScanner.h"
#include "QC7366Lib.h"
#include "UART740Lib.h"
#include "DAC4922Lib.h"
#include "SPIeeprom.h"
#include "ADC3208Lib.h"

#include "ActuatorLib.h"

#include "SystemTests.h"

#include "Config.h"

#include "tasks/TaskHeartbeat.h"
#include "tasks/TaskCLIHandler.h"
#include "tasks/TaskCommandHandler.h"
#include "tasks/TaskADC.h"
#include "tasks/TaskBlink.h"
#include "tasks/TaskDisplay.h"

#include "tasks/machine/TaskMain.h"
#include "tasks/machine/safety.h"
#include "tasks/machine/io_handler.h"
#include "tasks/machine/motion_control.h"
#include "tasks/machine/alarm.h"
#include "tasks/machine/HMI.h"
#include "tasks/machine/diagnostics.h"

///////////////////////////////////////////////////////////////////////////////
// Global declarations, task handles

xTaskHandle handle_HartbeatTask	= NULL;
xTaskHandle handle_CLITask		= NULL;
xTaskHandle handle_CmdTask		= NULL;
xTaskHandle handle_ADCTask      = NULL;
xTaskHandle handle_DisplayTask  = NULL;
xTaskHandle handle_BlinkTask0   = NULL;
xTaskHandle handle_BlinkTask1   = NULL;

xTaskHandle h_TaskMain          = NULL;
xTaskHandle h_safety            = NULL;
xTaskHandle h_io_handler        = NULL;
xTaskHandle h_motion_control    = NULL;
xTaskHandle h_alarm             = NULL;
xTaskHandle h_HMI               = NULL;
xTaskHandle h_diagnostics       = NULL;



//─── semaphores ─────────────────────────────────────────────────────────

xSemaphoreHandle sem_motion_run             = NULL;         ///< Permits MotionController to drive
xSemaphoreHandle sem_seeding_run            = NULL;        ///< Permits seed valves to open
xSemaphoreHandle sem_comms_active           = NULL;       ///< Permits CAN TX burst
xSemaphoreHandle sem_configUpdated          = NULL;      ///< Permits HMI, diagnostics and alarm to be active as they are non essential tasks
xSemaphoreHandle sem_InstelWaarde_count     = NULL;     ///< Premits lenght preset
xSemaphoreHandle sem_InstelWaarde_count2    = NULL;    ///< Premits depth preset
xSemaphoreHandle sem_resetButton            = NULL;   ///< keeps track of resetbutton
xSemaphoreHandle sem_actionButton           = NULL;  ///< keeps track of actionbutton
xSemaphoreHandle sem_stopButton             = NULL; ///< keeps track of stopbutton

//─── Queues ─────────────────────────────────────────────────────────

xQueueHandle   alarm_queue                  = NULL;        ///< Any task → AlarmManager
xQueueHandle   can_tx_queue                 = NULL;       ///< Any task → CAN TX
xQueueHandle   can_rx_queue                 = NULL;      ///< CAN ISR/driver → CommManager
xQueueHandle   hmi_cmd_queue                = NULL;     ///< HMI → StateMachine
xQueueHandle   diag_queue                   = NULL;    ///< Any task → Diagnostics


//─── intergers ─────────────────────────────────────────────────────────

uint8_t semaMaxCount	    = 1;    //maximale semaphore
uint8_t semaInitialCount    = 0;    //initiale count semaphore

uint8_t capacitieveSensor   = GPIO_NUM_36;    //gpio36
uint8_t LEDsensor           = GPIO_NUM_39;   //gpio39
uint8_t IRsensor            = GPIO_NUM_34;  //gpio34

uint8_t zaaiAfstand_mm      = 0;    // gemapte waarde van adc -> io_handler
uint8_t zaaiDiepte_mm       = 0;    // gemapte waarde van adc -> io_handler

volatile uint8_t MachineState        = 0; //case: 0 NOODSTOP, 1 OPSTART, 2 Parameters instellen, 3 Parameters akkoord, 4 parameters uitvoeren, 5 Standby, 6 running, 7 stopping, 8 error
uint8_t DiepteInstelling    = 0;        // basis diepte diepte instelling

bool instellenMachine       = false;    // is machine in instel stand of niet
bool machineRunning         = false;    // is machine aan het planten
bool seedPlanted            = false;    // counts true if seedwheel rotates 90 degre

char messageSize[30];

//─── function declaration ─────────────────────────────────────────────────────────

void create_tasks();


///////////////////////////////////////////////////////////////////////////////
// wrapper, simplified version of xTaskCreatePinnedToCore

bool platformTaskCreate(TaskFunction_t taskCode, void *taskParameters,
                        const char * const taskName, 
                        TaskHandle_t * const taskHandle)
{
    BaseType_t taskResult = pdFAIL;
    bool taskOK = false;
  
    taskResult = xTaskCreatePinnedToCore(taskCode, taskName, 
                RTOS_DEFAULT_STACKSIZE, taskParameters, 1, taskHandle, CORE_1);
    
    info_RegisterTaskByName(taskName);

    taskOK = (taskResult == pdPASS);
    SerialPrintf("> task [%s] creation %s\n", taskName, taskOK ? "OK" : "FAILED");

    return taskOK;
}


///////////////////////////////////////////////////////////////////////////////
// bool platformInit(void)

bool platformInit(void)
{
    Serial.begin(115200);
    SerialPrintf("> RTSW Test Setup\n");
	SerialPrintf("> build: %s\n", __TIMESTAMP__);
	SerialPrintf("> running setup\n");

    bool i2cOK  = false;
    bool oledOK = false;
    bool uartOK = false;
    bool result = true;
    uint8_t nDevices = 0;

    io_Init();
    led_Init();
    button_Init();
    spi_Init();
    qc_Init();
	dac_Init();
   	adc_Init();
    act_Init();


    i2cOK  = i2c_Init();
    oledOK = oled_Init();
    uartOK = uart_Init();

	result = i2cOK && oledOK && uartOK;

    nDevices = i2c_ScanBus();

    SerialPrintf("> I2C  init %s\n", i2cOK  ? "OK" : "*** FAILED ***");
    SerialPrintf("> OLED init %s\n", oledOK ? "OK" : "*** FAILED ***");
    SerialPrintf("> UART init %s\n", uartOK ? "OK" : "*** FAILED ***");

    SerialPrintf("> I2C devices found: %d\n", nDevices);

    #if (SYSTEMTEST_ONLY == 1)
    SerialPrintf("> DAC resolution = %10.6f Volt\n", DAC_RESOLUTION);
    SerialPrintf("> DAC min        = %10.6f Volt\n", DAC_MIN_VOLTAGE);
    SerialPrintf("> DAC max        = %10.6f Volt\n", DAC_MAX_VOLTAGE);
    #endif

    cliMessageQueue = xQueueCreate(QUEUESIZE, sizeof(CLI_MESSAGE));

    result &= platformTaskCreate(task_Heartbeat,      NULL, "task_heartbeat", &handle_HartbeatTask);
    result &= platformTaskCreate(task_CLIHandler,     NULL, "task_cli",       &handle_CLITask);
    result &= platformTaskCreate(task_CommandHandler, NULL, "task_cmd",       &handle_CmdTask);

    // manually register default system tasks:
    info_RegisterTaskByName("main");
    info_RegisterTaskByName("esp_timer");
    info_RegisterTaskByName("IDLE0");
    info_RegisterTaskByName("IDLE1");
    info_RegisterTaskByName("Tmr Svc");
    info_RegisterTaskByName("ipc0");
    info_RegisterTaskByName("ipc1");
    info_RegisterTaskByName("loopTask");

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// void setup()

void setup()
{

    bool result = false;

    result = platformInit();

   	SerialPrintf("> setup done: %s\n", (result == true) ? "OK" : "FAILED");


    oled_Clear();  // ← everything else after
	oled_WriteLine(0, "Starting",           ALIGN_CENTER);
	oled_WriteLine(1, "Tasks",              ALIGN_CENTER);
	oled_WriteLine(2, "Application:",       ALIGN_CENTER);
	oled_WriteLine(3, "Zaaimachine",        ALIGN_CENTER);
    


    // blink_params_t blinkParams0 = {
    //     3,
    //     BLINK_INPUT_POT0,
    //     0.03,
    //     3.0
    // };
    // result &= platformTaskCreate(task_Blink, (void *) &blinkParams0, "task_blink0", &handle_BlinkTask0);
    // SerialPrintf("> Blink task 0 init %s\n", result ? "OK" : "*** FAILED ***");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Combinatiezaaimachine taken
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sem_motion_run = xSemaphoreCreateCounting(semaMaxCount, semaInitialCount);
    sem_seeding_run = xSemaphoreCreateCounting(semaMaxCount, semaInitialCount);
    sem_comms_active = xSemaphoreCreateCounting(semaMaxCount, semaInitialCount);
    sem_configUpdated = xSemaphoreCreateCounting(semaMaxCount, semaInitialCount);
    sem_InstelWaarde_count = xSemaphoreCreateCounting(semaMaxCount, semaInitialCount);  
    sem_InstelWaarde_count2 = xSemaphoreCreateCounting(semaMaxCount, semaInitialCount);
    sem_resetButton = xSemaphoreCreateBinary();
    sem_actionButton = xSemaphoreCreateBinary();
    sem_stopButton = xSemaphoreCreateBinary();

    alarm_queue   = xQueueCreate(10, sizeof(AlarmMessage));
    can_tx_queue  = xQueueCreate(10, sizeof(CanMessage));
    can_rx_queue  = xQueueCreate(10, sizeof(CanMessage));
    hmi_cmd_queue = xQueueCreate(5,  sizeof(HmiCommand));
    diag_queue    = xQueueCreate(10, sizeof(DiagEvent));    





    //////////////////////////////////////////////////////////////////////////////////////////////////

    result &= platformTaskCreate(task_Main, NULL, "Task_Main", &h_TaskMain);
    SerialPrintf("> Main task init %s\n", result ? "OK" : "*** FAILED ***");
    vTaskPrioritySet(h_TaskMain, 10);   

    result &= platformTaskCreate(task_safety, NULL, "Task_Safety", &h_safety);
    SerialPrintf("> Safety task init %s\n", result ? "OK" : "*** FAILED ***");
    vTaskPrioritySet(h_safety, 20);

    result &= platformTaskCreate(task_io_handler, NULL, "Task_io_Handler", &h_io_handler);
    SerialPrintf("> IO_Handler task init %s\n", result ? "OK" : "*** FAILED ***");
    vTaskPrioritySet(h_io_handler, 10);

    result &= platformTaskCreate(task_motion_control, NULL, "Task_Motion_Control", &h_motion_control);
    SerialPrintf("> Motion_Control task init %s\n", result ? "OK" : "*** FAILED ***");
    vTaskPrioritySet(h_motion_control, 10);

    result &= platformTaskCreate(task_alarm, NULL, "Task_Alarm", &h_alarm);
    SerialPrintf("> Alarm task init %s\n", result ? "OK" : "*** FAILED ***");
    vTaskPrioritySet(h_alarm, 8);

    result &= platformTaskCreate(task_HMI, NULL, "Task_HMI", &h_HMI);
    SerialPrintf("> HMI task init %s\n", result ? "OK" : "*** FAILED ***");
    vTaskPrioritySet(h_HMI, 6);

    result &= platformTaskCreate(task_diagnostics, NULL, "Task_diagnostics", &h_diagnostics);
    SerialPrintf("> diagnostics task init %s\n", result ? "OK" : "*** FAILED ***");
    vTaskPrioritySet(h_diagnostics, 6);

    SerialPrintf("> Starting create_tasks\n");


	info_Tasks();
    

    #if (SYSTEMTEST_ONLY == 1)
	SerialPrintf("> running system tests...\n");
	// RunSystemTests();	// for separate HW testing
	RunSystemTestsMenu();	// via user menu
	#endif

}


///////////////////////////////////////////////////////////////////////////////
// void loop()

void loop()
{
    // Intentionally left empty, all work will be done by the running tasks
}


