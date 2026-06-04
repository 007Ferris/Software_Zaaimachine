//////////////////////////////////////////////////////////////////////////////
//
// UART740Lib.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	23-07-2025
//
// See also: https://github.com/rickkas7/SC16IS740RK
// and: https://github.com/rickkas7/SC16IS7xxRK
//
// UART datasheet: https://www.nxp.com/docs/en/data-sheet/SC16IS740_750_760.pdf
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"
#include <Wire.h>

///////////////////////////////////////////////////////////////////////////////
// application specific includes

#include "UART740Lib.h"
#include "I2CLib.h"
#include "Config.h"
#include "SerialPrintf.h"
#include "TaskSleep.h"


///////////////////////////////////////////////////////////////////////////////
// bool uart_Init(void)

bool uart_Init(void)
{
    bool i2cOK  = false;
    bool uartOK = false;

    i2cOK = i2c_Init();

    uartOK = uart_WriteRegister(UART_SPR, 0);    // try a write to the scratchpad register

    // set comm parameters, default = 9600, N, 8, 1
    uart_SetParameters(UART_BR_9600, OPTIONS_8N1);

    // enable FIFO's:
    uart_WriteRegister(UART_FCR, FCR_CLR_RCV_FIFO | FCR_CLR_TX_FIFO | FCR_FIFO_ENABLE);  

    return (i2cOK && uartOK);
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t uart_ReadRegister(uint8_t uartRegister)

uint8_t uart_ReadRegister(uint8_t subAddress)
{
    uint8_t value = 0;
    bool sendStop = false;
    uint16_t nrOfBytes = 1;

    Wire.beginTransmission(I2C_ADDRESS_UART);
    Wire.write(subAddress << 3);
    Wire.endTransmission(sendStop = false);
    
    Wire.requestFrom(I2C_ADDRESS_UART, nrOfBytes, (int)(sendStop = true));
    
    value = Wire.read();
   
    return value;
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t uart_WriteRegister(uint8_t subAddress, uint8_t data)

 bool uart_WriteRegister(uint8_t subAddress, uint8_t data)
{
    uint8_t result = 100;
    bool sendStop = false;

    Wire.beginTransmission(I2C_ADDRESS_UART);
    Wire.write(subAddress << 3);
    Wire.write(data);
    result = Wire.endTransmission(sendStop = true);

    #if (UART_DEBUG == 1)
    if (result != 0)
    {
        SerialPrintf("> UART write failed, I2C result code = %d\n", result);
    }
    #endif

    return (result == 0);
}

///////////////////////////////////////////////////////////////////////////////
// void uart_SetParameters(uint16_t baudRate, uint8_t options)
//
// refer to datasheet of SC16IS740 for all the bits and the register settings

void uart_SetParameters(uint32_t baudRate, uint8_t options)
{
    uint8_t  lcr = 0;
    uint16_t divisor = (UART_740_CLOCK_FREQ / (baudRate * 16)); // see datasheet

    #if (UART_DEBUG == 1)
    SerialPrintf("> UART clock = %.3f MHz, divisor = %d\n", (UART_740_CLOCK_FREQ/1e6), divisor);
    #endif

    // baudrate needs access via registers DLL and DLH, only accessible
    // with DLE bit set in LCR register:

    if ( (baudRate == UART_BR_38400) || (baudRate == UART_BR_19200) ||
         (baudRate == UART_BR_9600)  || (baudRate == UART_BR_4800)  ||
         (baudRate == UART_BR_57600) || (baudRate == UART_BR_115200) )
    {
        lcr = uart_ReadRegister(UART_LCR);
        lcr = lcr | LCR_DLE;
        uart_WriteRegister(UART_LCR, lcr);

        uart_WriteRegister(UART_DLH, highByte(divisor));
        uart_WriteRegister(UART_DLL, lowByte (divisor));

        #if (UART_DEBUG == 1)
        uint8_t dlh = uart_ReadRegister(UART_DLH);
        uint8_t dll = uart_ReadRegister(UART_DLL);
        SerialPrintf("> DLH = %d, DLL = %d\n", dlh, dll);
        #endif

        lcr = lcr & ~LCR_DLE;
        uart_WriteRegister(UART_LCR, lcr);
    }

    // settings for word length, parity and stopbits:
    uart_WriteRegister(UART_LCR, options);
    
    // check it...
    #if (UART_DEBUG == 1)
    lcr = uart_ReadRegister(UART_LCR);
    SerialPrintf("> LCR = 0x%02x\n", lcr);
    #endif
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t uart_BytesInReceiveFIFO(void)

uint8_t uart_BytesInRcvFIFO(void)
{
    uint8_t numBytes = 0;

    numBytes = uart_ReadRegister(UART_RXLVL);

    #if (UART_DEBUG == 1)
    SerialPrintf("> bytes in receive FIFO: %d\n", numBytes);
    #endif

    return numBytes;
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t uart_BytesInReceiveFIFO(void)

uint8_t uart_FreeSpaceinTxFIFO(void)
{
    uint8_t freeSpace = 0;

    freeSpace = uart_ReadRegister(UART_TXLVL);

    #if (UART_DEBUG == 1)
    SerialPrintf("> free space in transmit FIFO: %d\n", freeSpace);
    #endif

    return freeSpace;
}

///////////////////////////////////////////////////////////////////////////////
// int16_t uart_ReadByte(void)
 
int16_t uart_ReadByte(void)
{
    int16_t value = 0;

    if (uart_BytesInRcvFIFO() == 0)
    {
        value = -1;
    }
    else
    {
        value = uart_ReadRegister(UART_RHR);
    }

    return value;
}


///////////////////////////////////////////////////////////////////////////////
// void uart_SendByte(uint8_t value)

void uart_SendByte(uint8_t value)
{
    while (uart_FreeSpaceinTxFIFO() == 0)
    {
        taskSleep(0);
    }

    uart_WriteRegister(UART_THR, value);
}
