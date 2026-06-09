///////////////////////////////////////////////////////////////////////////////
//
// Config.h
//
// Authors: 	Roel Smeets
// Edit date: 	02-06-2025
//              27-05-2026 .. ..-..-.... [Felix Ahner]
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
///////////////////////////////////////////////////////////////////////////////
// define during development for testing individual devices on the board,
// 0 = regular operation, 1 = systems test only

#define SYSTEMTEST_ONLY		0

///////////////////////////////////////////////////////////////////////////////
// DAC constants for MCP4922 & output stage

#define N_DAC_BITS			12
#define N_DAC_CHANNELS		4

#define DAC_MIN_CHANNEL		0 
#define DAC_MAX_CHANNEL		(N_DAC_CHANNELS - 1)

#define DAC_MIN_VALUE 		0
#define DAC_MAX_VALUE 		((1 << N_DAC_BITS) - 1)

#define DAC_SPAN            20.0    // -10 .. + 10 volt
#define DAC_RESOLUTION	    (DAC_SPAN / (DAC_MAX_VALUE + 1))

#define DAC_MIN_VOLTAGE 	-10.0
#define DAC_MAX_VOLTAGE 	(DAC_MIN_VOLTAGE + ((DAC_MAX_VALUE) * (DAC_RESOLUTION)))


///////////////////////////////////////////////////////////////////////////////
// ADC constants & definitions for MCP3208 8 channel ADC

#define N_ADC_CHANNELS      8
#define N_ADC_BITS		    12

#define ADC_MIN_CHANNEL		0
#define ADC_MAX_CHANNEL		(N_ADC_CHANNELS - 1)

#define ADC_MIN_VALUE 	    0
#define ADC_MAX_VALUE       ((1 << N_ADC_BITS) - 1)

#define ADC_REFERENCE_VOLTAGE   2.5

// definitions for channels 0..3 with input range from -10 .. +10 volt

#define ADC03_MIN_VOLTAGE       -10.0
#define ADC03_RESOLUTION        ((8.0 * ADC_REFERENCE_VOLTAGE) / (ADC_MAX_VALUE + 1))
#define ADC03_MAX_VOLTAGE		(ADC03_MIN_VOLTAGE + ((ADC_MAX_VALUE) * (ADC03_RESOLUTION)))

// definitions for channels 4..7 with input range from 0 .. +2.5 volt

#define ADC47_RESOLUTION        (ADC_REFERENCE_VOLTAGE / (ADC_MAX_VALUE + 1))
#define ADC47_MIN_VOLTAGE	    0.0
#define ADC47_MAX_VOLTAGE		((ADC_MAX_VALUE) * (ADC47_RESOLUTION))

///////////////////////////////////////////////////////////////////////////////
// conversion factors

#define VOLT_TO_MV			(1e3)
#define MV_TO_VOLT			(1e-3)

///////////////////////////////////////////////////////////////////////////////
// RTOS defines for xTaskCreatePinnedToCore

#define	RTOS_DEFAULT_STACKSIZE	4096    // RTOS task stack size in bytes
#define CORE_0	    0                   // pin to core 0 of ESP32, Core 0 = Protocol CPU (WiFi/BT Stack)
#define CORE_1	    1                   // pin to core 1 of ESP32, user apps

///////////////////////////////////////////////////////////////////////////////
// I2C address definitions

#define I2C_ADDRESS_OLED		0x3C	    // I2C address of OLED display
#define I2C_ADDRESS_UART        0x4D        // I2C address of UART 
#define I2C_ADDRESS_EEPROM		0x50		// I2C address of the external (!!) EEPROM on the I2C bus

// ─── Alarm codes ─────────────────────────────────────────────────────────────
static constexpr uint16_t ALARM_INIT_FAIL    = 0x0001;
static constexpr uint16_t ALARM_ESTOP_ACTIVE = 0x0002;
static constexpr uint16_t ALARM_FAULT_ENTRY  = 0x0003;
static constexpr uint16_t ALARM_STATE_CHANGE = 0x0004;

// ─── Alarm severity ───────────────────────────────────────────────────────────
enum class AlarmSeverity : uint8_t
{
    INFO    = 0,    ///< Informational — no action required
    WARNING = 1,    ///< Attention required — operation may continue
    FAULT   = 2,    ///< Operation suspended — intervention required
    ESTOP   = 3     ///< Emergency — immediate safe shutdown
};

// ─── Alarm message (posted to AlarmQueue) ────────────────────────────────────
struct AlarmMessage
{
    uint16_t       code;
    AlarmSeverity  severity;
    uint32_t       timestamp_ms;
    char           description[32];
};

// ─── CAN message envelope ─────────────────────────────────────────────────────
struct CanMessage
{
    uint32_t  id;
    uint8_t   data[8];
    uint8_t   dlc;
    bool      is_extended;
};

// ─── HMI command ─────────────────────────────────────────────────────────────
enum class HmiCommand : uint8_t
{
    START    = 0,
    STOP,
    PAUSE,
    RESUME,
    RESET,
    ACK_ALARM,
    ESTOP_RESET
};

// ─── Diagnostic event ─────────────────────────────────────────────────────────
struct DiagEvent
{
    uint32_t  timestamp_ms;
    uint8_t   task_id;
    uint8_t   event_code;
    uint32_t  value;
};


// ─── Declareren globale variabelen ─────────────────────────────────────────────────────────
// Semaphores
extern SemaphoreHandle_t sem_motion_run;
extern SemaphoreHandle_t sem_seeding_run;
extern SemaphoreHandle_t sem_comms_active;
extern SemaphoreHandle_t sem_configUpdated;
extern SemaphoreHandle_t sem_InstelWaarde_count;
extern SemaphoreHandle_t sem_InstelWaarde_count2;
extern SemaphoreHandle_t sem_resetButton;
extern SemaphoreHandle_t sem_actionButton;
extern SemaphoreHandle_t sem_stopButton;

// Queues
extern QueueHandle_t alarm_queue;
extern QueueHandle_t can_tx_queue;
extern QueueHandle_t can_rx_queue;
extern QueueHandle_t hmi_cmd_queue;
extern QueueHandle_t diag_queue;

// bools
extern bool instellenMachine;
extern bool machineRunning;
extern bool seedPlanted;

//Input
extern uint8_t capacitieveSensor;  //gpio36
extern uint8_t LEDsensor;  //gpio39
extern uint8_t IRsensor;  //gpio34


//Output


//gobal variabels
#define RESET_BUTTON 0
#define ACTION_BUTTON 1
#define STOP_BUTTON 2

extern uint8_t DiepteInstelling;
extern volatile uint8_t MachineState;


#endif	// CONFIG_H
