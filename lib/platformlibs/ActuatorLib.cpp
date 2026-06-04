///////////////////////////////////////////////////////////////////////////////
//
// ActuatorLib.cpp
//
// Linear actuator control: stall-homing against the inner end stop and
// absolute positioning in millimetres using the quadrature encoder.
//
///////////////////////////////////////////////////////////////////////////////
 
///////////////////////////////////////////////////////////////////////////////
// system includes
 
#include <stdlib.h>     // labs()
#include <stdbool.h>
#include <inttypes.h>
 
///////////////////////////////////////////////////////////////////////////////
// application includes
 
#include "IOLib.h"          // io_SetBit()
#include "QC7366Lib.h"      // qc_* encoder counter
#include "TaskSleep.h"      // taskSleep()
#include "ActuatorLib.h"
 
///////////////////////////////////////////////////////////////////////////////
// motor primitives  (static = private to this module)
///////////////////////////////////////////////////////////////////////////////
 
// Stop the motor. Direction lines low first, then disable.
static void act_Stop(void)
{
    io_SetBit(ACT_OUT_IN1, false);
    io_SetBit(ACT_OUT_IN2, false);
    io_SetBit(ACT_OUT_ENA, false);
}
 
// Drive the rod OUT (extend / push). Set direction first, enable last so we
// never get a short pulse in the wrong direction.
// If the rod moves the wrong way, simply swap IN1 and IN2 here (and in retract).
static void act_DriveExtend(void)
{
    io_SetBit(ACT_OUT_IN1, true);
    io_SetBit(ACT_OUT_IN2, false);
    io_SetBit(ACT_OUT_ENA, true);
}
 
// Drive the rod IN (retract / pull towards the home stop).
static void act_DriveRetract(void)
{
    io_SetBit(ACT_OUT_IN1, false);
    io_SetBit(ACT_OUT_IN2, true);
    io_SetBit(ACT_OUT_ENA, true);
}
 
///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////
 
// Distance from home as a positive count, independent of encoder sign.
static int32_t act_AbsCounts(void)
{
    return (int32_t) labs( qc_ReadCountRegister(ACT_QC_CHANNEL) );
}
 
///////////////////////////////////////////////////////////////////////////////
// void act_Init(void)
//
// Prepares the actuator. io_Init() and qc_Init() must already have been called
// once during system start-up (they are shared with the rest of the board).
///////////////////////////////////////////////////////////////////////////////
 
void act_Init(void)
{
    act_Stop();
    qc_EnableCounter(ACT_QC_CHANNEL);
}
 
///////////////////////////////////////////////////////////////////////////////
// bool act_Home(void)
//
// Drives the rod fully into the cylinder (0 %) by pulling it in until it
// presses against the inner end stop. The internal limit switch of the
// actuator cuts its own motor at the end, and we detect the resulting stall
// (encoder count stops changing). On success the encoder is zeroed so that
// "fully retracted" == position 0.
//
// Returns true when homed, false on safety timeout.
///////////////////////////////////////////////////////////////////////////////
 
bool act_Home(void)
{
    int32_t  lastCount = 0;
    int32_t  nowCount  = 0;
    uint32_t stillMs   = 0;
    uint32_t elapsedMs = 0;
    bool     homed     = false;
 
    qc_EnableCounter(ACT_QC_CHANNEL);
 
    act_DriveRetract();                 // pull the rod inwards
    lastCount = qc_ReadCountRegister(ACT_QC_CHANNEL);
 
    while (true)
    {
        taskSleep(ACT_SAMPLE_MS);
        elapsedMs += ACT_SAMPLE_MS;
 
        nowCount = qc_ReadCountRegister(ACT_QC_CHANNEL);
 
        // Count how long the rod has effectively not moved.
        if (labs(nowCount - lastCount) < ACT_STALL_COUNTS)
        {
            stillMs += ACT_SAMPLE_MS;
        }
        else
        {
            stillMs = 0;                // still moving -> reset
        }
        lastCount = nowCount;
 
        // Ignore "no movement" while the motor is still spinning up, unless
        // it was already sitting on the stop at start-up (handled by timeout).
        if (elapsedMs < ACT_STARTUP_GRACE_MS)
        {
            stillMs = 0;
        }
 
        // Hit the inner end stop -> we are home.
        if (stillMs >= ACT_STALL_TIME_MS)
        {
            homed = true;
            break;
        }
 
        // Safety: never keep driving forever.
        if (elapsedMs >= ACT_HOME_TIMEOUT_MS)
        {
            homed = false;
            break;
        }
    }
 
    act_Stop();
    taskSleep(ACT_SETTLE_MS);
 
    if (homed)
    {
        qc_ClearCountRegister(ACT_QC_CHANNEL);  // fully retracted = 0
    }
 
    return homed;
}
 
///////////////////////////////////////////////////////////////////////////////
// bool act_MovePositionMm(uint16_t targetMm)
//
// Moves the rod to an absolute position, in mm, measured from the home
// (fully-retracted) position. After act_Home() this means "push the rod out
// by targetMm". The request is clamped to the actuator stroke.
//
// The move stops when the target count is reached, OR early if the rod stalls
// against a mechanical end before the target (e.g. if ACT_RESOLUTION is not yet
// calibrated), OR on safety timeout.
//
// Returns true if the target position was reached, false otherwise.
///////////////////////////////////////////////////////////////////////////////
 
bool act_MovePositionMm(uint16_t targetMm)
{
    int32_t  targetCounts = 0;
    int32_t  nowCount     = 0;
    int32_t  lastCount    = 0;
    uint32_t stillMs      = 0;
    uint32_t elapsedMs    = 0;
    bool     extending    = false;
    bool     reached      = false;
 
    // Clamp to the physical stroke so we never command past the end.
    if (targetMm > ACT_MAX_STROKE_MM)
    {
        targetMm = ACT_MAX_STROKE_MM;
    }
    targetCounts = (int32_t) targetMm * ACT_RESOLUTION;
 
    nowCount = act_AbsCounts();
 
    // Decide direction. Already there? done.
    if (targetCounts > nowCount)
    {
        extending = true;
        act_DriveExtend();
    }
    else if (targetCounts < nowCount)
    {
        extending = false;
        act_DriveRetract();
    }
    else
    {
        return true;
    }
 
    lastCount = qc_ReadCountRegister(ACT_QC_CHANNEL);
 
    while (true)
    {
        taskSleep(ACT_SAMPLE_MS);
        elapsedMs += ACT_SAMPLE_MS;
 
        nowCount = act_AbsCounts();
 
        // Target reached?
        if (extending && (nowCount >= targetCounts))
        {
            reached = true;
            break;
        }
        if (!extending && (nowCount <= targetCounts))
        {
            reached = true;
            break;
        }
 
        // Stall detection (hit a mechanical end before the target).
        {
            int32_t raw = qc_ReadCountRegister(ACT_QC_CHANNEL);
            if (labs(raw - lastCount) < ACT_STALL_COUNTS)
            {
                stillMs += ACT_SAMPLE_MS;
            }
            else
            {
                stillMs = 0;
            }
            lastCount = raw;
        }
 
        if (elapsedMs < ACT_STARTUP_GRACE_MS)
        {
            stillMs = 0;
        }
 
        if (stillMs >= ACT_STALL_TIME_MS)       // stuck before target
        {
            reached = false;
            break;
        }
 
        if (elapsedMs >= ACT_MOVE_TIMEOUT_MS)    // safety cap
        {
            reached = false;
            break;
        }
    }
 
    act_Stop();
    return reached;
}
 
///////////////////////////////////////////////////////////////////////////////
// position read-back
///////////////////////////////////////////////////////////////////////////////
 
int32_t act_GetPositionCounts(void)
{
    return act_AbsCounts();
}
 
float act_GetPositionMm(void)
{
    return (float) act_AbsCounts() / (float) ACT_RESOLUTION;
}