//////////////////////////////////////////////////////////////////////////////
//
// UART740Lib.h
//
// Authors: 	Roel Smeets
// Edit date: 	27-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef UART740_H
#define UART740_H

#include <inttypes.h>
#include "bits.h"

///////////////////////////////////////////////////////////////////////////////
// #defines

#define UART_DEBUG      0   // 1 = debug, 0 = no debug

///////////////////////////////////////////////////////////////////////////////
// register offsets for UART NXP SC16IS740 / SC16IS750 / SC16IS760

#define UART_RHR        0x00        // R
#define UART_THR        0x00        // W
#define UART_IER        0x01        // R/W
#define UART_FCR        0x02        // W
#define UART_IIR        0x02        // R
#define UART_LCR        0x03        // R/W
#define UART_MCR        0x04        // R/W
#define UART_LSR        0x05        // R
#define UART_MSR        0x06        // R
#define UART_SPR        0x07        // R/W
#define UART_TCR        0x06        // R/W
#define UART_TLR        0x07        // R/W
#define UART_TXLVL      0x08        // R
#define UART_RXLVL      0x09        // R
#define UART_RESERVED   0x0d        // -/-
#define UART_EFCR       0x0f        // R/W

// Special register set
// only accessible when LCR Divisor Latch Enable (DLE) = 1 and not 0xBF
// baud rate divisor High byte / Low byte
#define UART_DLL        0x00
#define UART_DLH        0x01

// Enhanced register set
// only accessible when LCR = 0xBF
#define UART_EFR        0x02
#define UART_XON1       0x04
#define UART_XON2       0x05
#define UART_XOFF1      0x06
#define UART_XOFF2      0x07

// only available on  SC16IS750/SC16IS760:
#define UART_IODIR      0x0a
#define UART_IOSTATE    0x0b
#define UART_IOINTENA   0x0c
#define UART_IOCONTROL  0x0e

///////////////////////////////////////////////////////////////////////////////
// baud rate definitions

#define UART_740_CLOCK_FREQ (18.432e6)  // clock frequency of xtal in Hz

#define UART_BR_4800        4800
#define UART_BR_9600        9600
#define UART_BR_19200       19200
#define UART_BR_38400       38400
#define UART_BR_57600       57600
#define UART_BR_115200      115200

///////////////////////////////////////////////////////////////////////////////
// bit definitions in registers

// LCR register
#define LCR_DLE             BIT_7 
#define LSR_THR_EMPTY       BIT_5

// FCR register
#define FCR_FIFO_ENABLE     BIT_0
#define FCR_CLR_TX_FIFO     BIT_1
#define FCR_CLR_RCV_FIFO    BIT_2

// MCR register
#define MCR_LOOPBACK        BIT_4

///////////////////////////////////////////////////////////////////////////////
// other defines

#define UART_FIFOSIZE       64

///////////////////////////////////////////////////////////////////////////////
// communication options bits/parity/stopbits
//
// copied from https://github.com/rickkas7/SC16IS740RK/blob/master/src/SC16IS740RK.h

#define OPTIONS_8N1     0b000011
#define OPTIONS_8E1     0b011011
#define OPTIONS_8O1     0b001011

#define OPTIONS_8N2     0b000111
#define OPTIONS_8E2     0b011111
#define OPTIONS_8O2     0b001111

#define OPTIONS_7N1     0b000010
#define OPTIONS_7E1     0b011010
#define OPTIONS_7O1     0b001010

#define OPTIONS_7N2     0b000110
#define OPTIONS_7E2     0b011110
#define OPTIONS_7O2     0b001110

///////////////////////////////////////////////////////////////////////////////
// function prototypes

bool uart_Init(void);
uint8_t uart_ReadRegister(uint8_t subAddress);
bool uart_WriteRegister(uint8_t subAddress, uint8_t data);
void uart_SetParameters(uint32_t baudRate, uint8_t options);

uint8_t uart_BytesInRcvFIFO(void);
uint8_t uart_FreeSpaceinTxFIFO(void);

int16_t uart_ReadByte(void);
void uart_SendByte(uint8_t value);

#endif  // UART740_H
