//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsUART.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	30-07-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "Config.h"
#include "UART740Lib.h"
#include "SystemTestsUART.h"
#include "SerialPrintf.h"
#include "TaskSleep.h"
#include "SystemTests.h"

///////////////////////////////////////////////////////////////////////////////
// void test_uart_RW_Repeat(void)

void test_uart_RW_Repeat(void)
{
	uint32_t errorCount = 0;
	uint32_t pass = 0;

	while (true)
	{
		errorCount += test_uart_RWTest();
		test_ShowPass(++pass, 10);
		test_ErrorReport("UART R/W test", pass, errorCount, 100);

		if (StopTest()) break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_uart_LoopbackTest_Repeat(bool internal)

void test_uart_LoopbackTest_Repeat(bool internal)
{
	uint32_t errorCount = 0;
	uint32_t pass = 0;
	char msg[40];

	sprintf(msg, "UART %sternal loopback test", internal ? "in" : "ex");

	while (true)
	{
		errorCount += test_uart_LoopbackTest(internal);
		
		test_ShowPass(++pass, 1);
		test_ErrorReport(msg, pass, errorCount, 10);

		if (StopTest()) break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t test_uart_RWTest(void)
//
// perform R/W test on SPR register (scatch pad register)

uint16_t test_uart_RWTest(void)
{
    uint16_t errors = 0;
    uint8_t ix = 0;
    uint8_t dataRead = 0;
    bool writeOK = false;

    for (ix = 0; ix < 0xff; ix++)
    {
        writeOK  = uart_WriteRegister(UART_SPR, ix);
        dataRead = uart_ReadRegister(UART_SPR);

        if (writeOK == false)
        {
            SerialPrintf("> UART R/W error: WriteRegister error\n");
            errors++;
        }

        if (dataRead != ix)
        {
            SerialPrintf("> UART R/W data error: write = 0x%02x, read = 0x%02x\n", ix, dataRead);
            errors++;
        }
    }

	// SerialPrintf("> UART R/W errors: %d\n", errors);
	
    return errors;
}


///////////////////////////////////////////////////////////////////////////////
// uint16_t test_uart_RWTest(void)
//
// loopback test with FIFO's, either internal on-chip or external
// via external connector

uint16_t test_uart_LoopbackTest(bool internalLoopback)
{
    uint16_t errors   = 0;
    uint8_t  nBlocks  = 0;
    uint8_t  block    = 0;
    uint8_t  data     = 0;
    uint8_t  expected = 0;
    uint8_t  nBytesReceived = 0;
    uint8_t  mcr = 0;
    uint8_t  ix  = 0;

    uart_SetParameters(UART_BR_115200, OPTIONS_8N1);
    
    mcr = uart_ReadRegister(UART_MCR);
    if (internalLoopback)
    {
        mcr = mcr | MCR_LOOPBACK;
    }
    else
    {
        mcr = mcr & ~MCR_LOOPBACK;
    }
    uart_WriteRegister(UART_MCR, mcr);

    nBlocks = 256 / UART_FIFOSIZE;  // 8 bit values, range from 0..255
    // SerialPrintf("> UART [%s] writing %d blocks\n", internalLoopback ? "INT" : "EXT", nBlocks);

    for (block = 0; block < nBlocks; block++)
    {
        data = 0;
        for (ix = 0; ix < UART_FIFOSIZE; ix++)
        {
            data = ix + (block*UART_FIFOSIZE);
            uart_WriteRegister(UART_THR, data);
        }

        // transmission time required for 64 bytes from Tx FIFO to Rc FIFO  =
        // 64 * 1 ms (@9600 baud) = app. 64 ms (maximum)

        taskSleep(70);

        nBytesReceived = uart_BytesInRcvFIFO();

        if (nBytesReceived != UART_FIFOSIZE)
        {
            SerialPrintf("> UART [%s] %d bytes in Rcv FIFO, expected: %d\n", internalLoopback ? "INT" : "EXT",
                nBytesReceived, UART_FIFOSIZE);
            errors++;
        }

        for (ix = 0; ix < UART_FIFOSIZE; ix++)
        {
            expected = ix + (block*UART_FIFOSIZE);
            data = uart_ReadRegister(UART_RHR);
            if (data != expected)
            {
                SerialPrintf("> UART [%s] receive data error: data = %d, expected: %d\n", 
                    internalLoopback ? "INT" : "EXT", data, expected);
                errors++;
            }
        }

        nBytesReceived = uart_BytesInRcvFIFO(); // FIFO should be empty
        if (nBytesReceived != 0)
        {
            SerialPrintf("> UART [%s] %d bytes in Rcv FIFO, expected: 0\n", 
                internalLoopback ? "INT" : "EXT", nBytesReceived);
            errors++;
        }        

    }

	// SerialPrintf("> UART loopback errors [%s]: %d\n", internalLoopback ? "INT" : "EXT", errors);

    return errors;
}

///////////////////////////////////////////////////////////////////////////////
// void test_uart_StreamData_Repeat(void)

void test_uart_StreamData_Repeat(void)
{
	uint8_t data = 0;
	uint8_t mcr = 0;
	uint8_t sleepTime = 50;

	uart_SetParameters(UART_BR_115200, OPTIONS_8N1);

	mcr = uart_ReadRegister(UART_MCR);
	mcr = mcr & ~MCR_LOOPBACK;
	uart_WriteRegister(UART_MCR, mcr);

	while (true)
	{
		for (data = '0'; data <= '9'; data++)
		{
			uart_WriteRegister(UART_THR, data);
			taskSleep(sleepTime);
		}
		uart_WriteRegister(UART_THR, CHAR_CR);
		uart_WriteRegister(UART_THR, CHAR_LF);

		for (data = 'A'; data <= 'Z'; data++)
		{
			uart_WriteRegister(UART_THR, data);
			taskSleep(sleepTime);
		}
		uart_WriteRegister(UART_THR, CHAR_CR);
		uart_WriteRegister(UART_THR, CHAR_LF);

		for (data = 'a'; data <= 'z'; data++)
		{
			uart_WriteRegister(UART_THR, data);
			taskSleep(sleepTime);
		}
		uart_WriteRegister(UART_THR, CHAR_CR);
		uart_WriteRegister(UART_THR, CHAR_LF);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void test_uart_Echo_Repeat(void)

void test_uart_Echo_Repeat(void)
{
	uint8_t nBytesReceived = 0;
	uint8_t mcr = 0;
	char dataBuffer[2];

	uart_SetParameters(UART_BR_115200, OPTIONS_8N1);

	mcr = uart_ReadRegister(UART_MCR);
	mcr = mcr & ~MCR_LOOPBACK;
	uart_WriteRegister(UART_MCR, mcr);

	memset(dataBuffer, 0, sizeof(dataBuffer));

	while (true)
	{
        do
		{
			nBytesReceived = uart_BytesInRcvFIFO();
			SerialPrintf("> bytes in receive FIFO: %d\n", nBytesReceived);
			taskSleep(500);
		} while (nBytesReceived == 0);

		while (uart_BytesInRcvFIFO() != 0)
		{
			dataBuffer[0] = uart_ReadRegister(UART_RHR);
			uart_WriteRegister(UART_THR, dataBuffer[0]);
			SerialPrintf("%s", dataBuffer);
		}
	}
}
