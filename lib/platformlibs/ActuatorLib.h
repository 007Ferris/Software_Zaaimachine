///////////////////////////////////////////////////////////////////////////////
//
// ActuatorLib.h
//
// Control library for the linear actuator with quadrature (Hall) encoder.
//
//  - act_Home()            : retract the rod fully into the cylinder (0 %) and
//                            zero the encoder count. Run this once at start-up.
//  - act_MovePositionMm()  : push the rod out to an absolute position in mm,
//                            measured from the fully-retracted home position.
//
// Encoder is read through the LS7366R quadrature counter (QC7366Lib).
// Motor is driven through the H-bridge digital outputs (IOLib).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ACTUATORLIB_H
#define ACTUATORLIB_H

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <inttypes.h>
#include <stdbool.h>
#include "IOLib.h"
#include "Config.h"

///////////////////////////////////////////////////////////////////////////////
// configuration #define's  --  CHECK / CALIBRATE THESE FOR YOUR HARDWARE
///////////////////////////////////////////////////////////////////////////////

// Encoder resolution. PLACEHOLDER value - measure it later:
// drive a known distance, read the counts, RESOLUTION = counts / mm.
// NOTE: depends on the LS7366R count mode (1x/2x/4x) set in qc_Init().
#define ACT_RESOLUTION          40          // counts per mm

// Mechanical stroke of the actuator (TRU 3373200 = 100 mm). Used to clamp
// requested positions so we never command past the physical end.
#define ACT_MAX_STROKE_MM       100         // mm

// Which LS7366R channel the actuator encoder is wired to.
#define ACT_QC_CHANNEL          0

// H-bridge control lines = digital OUTPUT bit numbers in IOLib.
// Schematic: Digital_OUT1 -> ENA, Digital_OUT2 -> IN1, Digital_OUT3 -> IN2.
// !! VERIFY these bit numbers against your actual wiring !!
#define ACT_OUT_ENA             25 // enable / on-off
#define ACT_OUT_IN1             26 // direction A
#define ACT_OUT_IN2             27 // direction B

///////////////////////////////////////////////////////////////////////////////
// timing / detection #define's

#define ACT_SAMPLE_MS           50          // how often we read the encoder
#define ACT_STALL_COUNTS        4           // movement below this = "not moving"
#define ACT_STALL_TIME_MS       300         // no movement this long = end stop
#define ACT_STARTUP_GRACE_MS    500         // ignore stall while motor spins up
#define ACT_SETTLE_MS           150         // pause after stopping at a stop
#define ACT_HOME_TIMEOUT_MS     20000       // safety cap for homing
#define ACT_MOVE_TIMEOUT_MS     20000       // safety cap for a move

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void    act_Init(void);                         // stop motor, prepare counter
bool    act_Home(void);                         // retract fully, set position 0
bool    act_MovePositionMm(uint16_t targetMm);  // move to absolute mm from home

int32_t act_GetPositionCounts(void);            // current distance from home (counts)
float   act_GetPositionMm(void);                // current distance from home (mm)

#endif // ACTUATORLIB_H
