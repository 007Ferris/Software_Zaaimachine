//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsQuadrature.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "TaskSleep.h"
#include "ButtonLib.h"
#include "SerialPrintf.h"
#include "SystemTestsQuadrature.h"
#include "QC7366Lib.h"
#include "IOLib.h"
#include "SystemTests.h"

///////////////////////////////////////////////////////////////////////////////
// uint8_t GetQuadratureChannel(void)

uint8_t GetQuadratureChannel(void)
{
	uint8_t channel = 0;

	do 
	{
		channel = ReadNumber();
		if (channel >= 3)
		{
			SerialPrintf("> quadrature channel must be in range 0..2\n");
		}
	} while (channel >= 3);

	return channel;
}

///////////////////////////////////////////////////////////////////////////////
// void test_Quadrature_Registers_RW(void)

void test_Quadrature_Registers_RW(void)
{
	uint8_t choice  = 0;
	uint8_t channel = 2;

	while (true)
	{
		SerialPrintf("* make a selection:\n");
		SerialPrintf("1.    select quadrature channel, current = %d\n", channel);
		SerialPrintf("2.    R/W test\n");

		choice = ReadChoice(2);

		switch (choice)
		{
		case 0: // up one level in menu
			return;
		case 1:
			SerialPrintf("> enter channel number (0..2, 2 = both channels) > ");
			channel = GetQuadratureChannel();
			SerialPrintf("%d\n", channel);
			break;
		case 2:
			test_qc_RW_Repeat(channel);
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_qc_RW_Repeat(uint8_t channel)

void test_qc_RW_Repeat(uint8_t channel)
{
	uint32_t errorCount = 0;
	uint32_t pass = 0;

	while (true)
	{
		if (channel == 2)
		{
			errorCount += test_qc_RW(0);
			errorCount += test_qc_RW(1);
		}
		else
		{
			errorCount += test_qc_RW(channel);
		}
		test_ShowPass(++pass, 10);
		test_ErrorReport("Quadrature registers R/W test", pass, errorCount, 100);

		if (StopTest())	break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_Quadrature_CountIndex(void)

void test_Quadrature_CountIndex(void)
{
	uint8_t choice  = 0;
	uint8_t channel = 2;	// 2 = test channel 0 AND 1

	while (true)
	{
		SerialPrintf("* make a selection:\n");
		SerialPrintf("1.    select quadrature channel, current = %d\n", channel);
		SerialPrintf("2.    count & index test\n");

		choice = ReadChoice(2);

		switch (choice)
		{
		case 0: // up one level in menu
			return;
		case 1:
			SerialPrintf("> enter channel number (0..2, 2 = both channels) > ");
			channel = GetQuadratureChannel();
			SerialPrintf("%d\n", channel);
			break;
		case 2:
			test_qc_CountIndex_Repeat(channel);
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_qc_CountIndex_Repeat(uint8_t channel)

void test_qc_CountIndex_Repeat(uint8_t channel)
{
	uint32_t errorCount = 0;
	uint32_t pass = 0;
	bool verbose = true;
	char msg[80];

	if (channel == 2)
	{
		sprintf(msg, "Quadrature count & index test channel 0 and 1");
	}
	else
	{
		sprintf(msg, "Quadrature count & index test channel %d", channel);
	}

	while (true)
	{
		errorCount += test_qc_CountSimulated(verbose); // channel 0 and 1 clocked parallel!!

		if (channel == 2)
		{
			errorCount += test_qc_IndexSimulated(0, verbose);
			errorCount += test_qc_IndexSimulated(1, verbose);
		}
		else
		{
			errorCount += test_qc_IndexSimulated(channel, verbose);
		}
		test_ShowPass(++pass, 1);
		test_ErrorReport(msg, pass, errorCount, 1);

		if (StopTest())	break;
	}

}

///////////////////////////////////////////////////////////////////////////////
// uint16_t test_qc_RW(uint8_t channel)

uint16_t test_qc_RW(uint8_t channel)
{
	uint32_t count = 0;
	uint32_t outputRegister = 0;
	uint32_t countRegister  = 0;
	uint8_t  statusRegister = 0;
	uint8_t  modeRegister0  = 0;
	uint8_t  modeRegister1  = 0;
	uint16_t errors = 0;
	uint16_t ix = 0;

    uint8_t mode = MODE_QC_1 | MODE_FREERUNNING | INDEX_RESETCNTR | INDEX_ASYNC | FILTERCLOCK_DIV_2;

	qc_WriteModeRegister(channel, QC_MODE_REGISTER_0, mode);
	modeRegister0 = qc_ReadModeRegister(channel, QC_MODE_REGISTER_0);
	modeRegister1 = qc_ReadModeRegister(channel, QC_MODE_REGISTER_1);

	qc_WriteModeRegister(channel, QC_MODE_REGISTER_1, CNTMODE_4);
	qc_EnableCounter(channel);

	for (ix = 0; ix <= 0xff; ix++)
	{
		statusRegister = qc_ReadStatusRegister(channel);
		modeRegister0  = qc_ReadModeRegister(channel, QC_MODE_REGISTER_0);
		modeRegister1  = qc_ReadModeRegister(channel, QC_MODE_REGISTER_1);

		count = (ix << 24) | (ix << 16) | (ix << 8) | (ix << 0);
		qc_WriteDataRegister(channel, count);
		qc_TransferDataRegisterToCountRegister(channel);
		qc_TranferCountRegisterToOutputRegister(channel);
		countRegister  = qc_ReadCountRegister(channel);
		outputRegister = qc_ReadOutputRegister(channel);
		if (outputRegister != countRegister)
		{
			SerialPrintf("> QC error channel %d: countregister = 0x%08x, outputregister = 0x%08x\n", 
						channel, countRegister, outputRegister);
						errors++;
		}
		// SerialPrintf("channel %d: CNT = 0x%08x, OTR = 0x%08x, STR = 0x%02x, MDR0 = 0x%02x, MDR1 = 0x%02x\n",
		// 			 channel, countRegister, outputRegister, statusRegister,
		// 			 modeRegister0, modeRegister1);
		// taskSleep(500);
	}

	// SerialPrintf("> quadrature R/W errors channel %d: %d\n", channel, errors);

	return errors;
}

///////////////////////////////////////////////////////////////////////////////
// void test_qc_CountExternal_Repeat(void)

void test_qc_CountExternal_Repeat(void)
{
	uint32_t countRegister = 0;
	uint8_t  qcChannel = 0;
	
	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_EnableCounter(qcChannel);
		qc_ClearCountRegister(qcChannel);
	}

	while (true)
	{
		for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
		{
			countRegister = qc_ReadCountRegister(qcChannel);
			SerialPrintf("> QC channel %d: count = %8d\n", qcChannel, countRegister);
		}
		taskSleep(500);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_qc_IndexExternal(uint8_t channel)

void test_qc_IndexExternal(uint8_t channel)
{
	uint8_t sr = 0;

	SerialPrintf("> QC channel %d: waiting for index bit...\n", channel);

	while (qc_IsIndexSet(channel) == false)
	{
	}

	sr = qc_ReadStatusRegister(channel);
	SerialPrintf("> QC channel %d: status register = 0x%02x\n", channel, sr);
	SerialPrintf("> QC channel %d: index bit is set, resetting...\n", channel);

	qc_ClearStatusRegister(channel);

	sr = qc_ReadStatusRegister(channel);
	SerialPrintf("> QC channel %d: status register after reset = 0x%02x\n", channel, sr);
}

///////////////////////////////////////////////////////////////////////////////
// uint16_t test_qc_IndexSimulated(uint8_t channel, bool verbose)

uint16_t test_qc_IndexSimulated(uint8_t channel, bool verbose)
{
	uint8_t sr = 0;
	uint16_t errors = 0;

	qc_ClearStatusRegister(channel);

	if (qc_IsIndexSet(channel))
	{
		SerialPrintf("> QC channel %d: index bit is already set\n", channel);
		errors++;
	}

	if (verbose)
	{
		SerialPrintf("> QC channel %d: setting index signal\n", channel);
	}
	test_qc_SetIndexSignal();

	if (verbose)
	{
		SerialPrintf("> QC channel %d: waiting for index bit...\n", channel);
	}

	while (qc_IsIndexSet(channel) == false)
	{
	}

	if (verbose)
	{
		SerialPrintf("> QC channel %d: index bit is set!\n", channel);
	}

	if (verbose)
	{
		sr = qc_ReadStatusRegister(channel);
		SerialPrintf("> QC channel %d: status register = 0x%02x\n", channel, sr);
	}

	qc_ClearStatusRegister(channel);
	if (qc_IsIndexSet(channel))
	{
		SerialPrintf("> QC channel %d: index bit is NOT cleared after reset!\n", channel);
		errors++;
	}

	if (verbose)
	{
		sr = qc_ReadStatusRegister(channel);
		SerialPrintf("> QC channel %d: status register after reset = 0x%02x\n", channel, sr);
	}

	if (verbose)
	{
		SerialPrintf("> quadrature index bit errors channel %d: %d\n", channel, errors);
	}

	return errors;
}

///////////////////////////////////////////////////////////////////////////////
// void test_qc_IndexExternal_Repeat(void)

void test_qc_IndexExternal_Repeat(void)
{
	uint8_t channel = 0;

	while (true)
	{
		for (channel = 0; channel <= QC_MAX_CHANNEL; channel++)
		{
			test_qc_IndexExternal(channel);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// void test_qc_GenerateCounts(uint32_t nrOfCounts, bool countUp)
//
// generate positive or negative counts with digital outputs:
// 		IO bit 0 = A signal
// 		IO bit 1 = B signal
// 		IO bit 2 = index bit

#define QC_A		0
#define QC_B		1
#define QC_INDEX	2

void test_qc_GenerateCounts(uint32_t nrOfCounts, bool countUp)
{
	uint8_t s1  = countUp ? QC_A : QC_B;
	uint8_t s2  = countUp ? QC_B : QC_A;
	uint32_t ix = 0;

	for (ix = 0; ix < nrOfCounts; ix++)
	{
		io_SetBit(s1, true);
		io_SetBit(s2, true);
		io_SetBit(s1, false);
		io_SetBit(s2, false);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void test_qc_SetIndexSignal(void)
//
// index signal is active LOW!

void test_qc_SetIndexSignal(void)
{
	io_SetBit(QC_INDEX, true);
	io_SetBit(QC_INDEX, false);
	io_SetBit(QC_INDEX, true);
}

///////////////////////////////////////////////////////////////////////////////
// void test_qc_DisplayCounts(uint8_t qcChannel)

void test_qc_DisplayCounts(uint8_t qcChannel)
{
	int32_t countRegister = 0;

	countRegister = qc_ReadCountRegister(qcChannel);
	SerialPrintf("> QC channel %d: count = %d\n", qcChannel, countRegister);
}

///////////////////////////////////////////////////////////////////////////////
//	uint16_t test_qc_CountSimulated(bool verbose)
//
// test both channels at the same time, they are clocked in parallel!

uint16_t test_qc_CountSimulated(bool verbose)
{
	int32_t countRegister = 0;
	uint8_t qcChannel 	  = 0;
	int32_t counts  = 1000000;
	uint16_t errors = 0;
	bool countUp = true;

	io_SetBit(QC_INDEX, true);	// index bit is active LOW, must be set INactive!!!

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_EnableCounter(qcChannel);
		qc_ClearCountRegister(qcChannel);
	}

	// step 1: count up from zero
	if (verbose)
	{
		SerialPrintf("> QC - 1: generating %d counts up...\n", counts);
	}

	test_qc_GenerateCounts(counts, countUp = true);

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		if (verbose)
		{
			test_qc_DisplayCounts(qcChannel);
		}
		countRegister = qc_ReadCountRegister(qcChannel);
		if (countRegister != counts)
		{
			SerialPrintf("> QC channel %d count error: count = %d, expected: %d\n",
						 qcChannel, countRegister, counts);
			errors++;
		}
		else
		{
			if (verbose)
			{
				SerialPrintf("> QC channel %d count OK [%d]\n", qcChannel, countRegister);
			}
		}
	}

	// step 2: count down to zero again
	if (verbose)
	{
		SerialPrintf("> QC - 2: generating %d counts down...\n", counts);
	}
	test_qc_GenerateCounts(counts, countUp = false);

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		if (verbose)
		{
			test_qc_DisplayCounts(qcChannel);
		}
		countRegister = qc_ReadCountRegister(qcChannel);
		if (countRegister != 0)
		{
			SerialPrintf("> QC channel %d count error: count = %d, expected: 0\n",
						 qcChannel, countRegister);
			errors++;
		}
		else
		{
			if (verbose)
			{
				SerialPrintf("> QC channel %d count OK [%d]\n", qcChannel, countRegister);
			}
		}
	}

	// step 3: count down to negative count
	if (verbose)
	{
		SerialPrintf("> QC - 3: generating %d counts down...\n", counts);
	}
	test_qc_GenerateCounts(counts, countUp = false);

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		if (verbose)
		{
			test_qc_DisplayCounts(qcChannel);
		}
		countRegister = qc_ReadCountRegister(qcChannel);
		if (countRegister != -counts)
		{
			SerialPrintf("> QC channel %d count error: count = %d, expected: %d\n",
						 qcChannel, countRegister, -counts);
			errors++;
		}
		else
		{
			if (verbose)
			{
				SerialPrintf("> QC channel %d count OK [%d]\n", qcChannel, countRegister);
			}
		}
	}

	// step 4: count up to zero again
	if (verbose)
	{
		SerialPrintf("> QC - 4: generating %d counts up...\n", counts);
	}
	test_qc_GenerateCounts(counts, countUp = true);

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		if (verbose)
		{
			test_qc_DisplayCounts(qcChannel);
		}
		countRegister = qc_ReadCountRegister(qcChannel);
		if (countRegister != 0)
		{
			SerialPrintf("> QC channel %d count error: count = %d, expected: 0\n",
						 qcChannel, countRegister);
			errors++;
		}
		else
		{
			if (verbose)
			{
				SerialPrintf("> QC channel %d count OK [%d]\n", qcChannel, countRegister);
			}
		}
	}

	if (verbose)
	{
		SerialPrintf("> quadrature count errors: %d\n", errors);
	}

	return errors;
}
