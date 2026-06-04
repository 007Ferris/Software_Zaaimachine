//////////////////////////////////////////////////////////////////////////////
//
// SystemTests.h
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEMTESTS_H
#define SYSTEMTESTS_H

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void RunSystemTests(void);
void RunSystemTestsMenu(void);

bool StopTest(void);
uint32_t ReadNumber(void);
uint8_t  ReadChoice(uint8_t maxChoice);

void test_ShowPass(uint32_t pass, uint32_t moduloCount);
void test_ErrorReport(const char *nameOfTest, uint32_t pass, uint32_t nErrors, uint32_t modulo);
void test_WaitForButton(void);
void test_BatchTest(void);

#endif  // SYSTEMTESTS_H
