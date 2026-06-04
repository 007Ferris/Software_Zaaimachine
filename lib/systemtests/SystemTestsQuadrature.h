//////////////////////////////////////////////////////////////////////////////
//
// SystemTestsQuadrature.h
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEMTESTS_QUADRATURE_H
#define SYSTEMTESTS_QUADRATURE_H

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <inttypes.h>

///////////////////////////////////////////////////////////////////////////////
// function prototypes

uint16_t test_qc_RW(uint8_t qcChannel);
void test_qc_RW_Repeat(uint8_t channel);
void test_qc_CountIndex_Repeat(uint8_t channel);

void test_Quadrature_Registers_RW(void);
void test_Quadrature_CountIndex(void);

void test_qc_CountExternal_Repeat(void);
void test_qc_IndexExternal_Repeat(void);

void test_qc_IndexExternal(uint8_t channel);

void test_qc_GenerateCounts(uint32_t nrOfCounts, bool countUp);
void test_qc_SetIndexSignal(void);

void test_qc_DisplayCounts(uint8_t qcChannel);
uint16_t test_qc_CountSimulated(bool verbose);
uint16_t test_qc_IndexSimulated(uint8_t channel, bool verbose);

#endif  // SYSTEMTESTS_QUADRATURE_H
