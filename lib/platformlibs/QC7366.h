///////////////////////////////////////////////////////////////////////////////
//
// QC7366.h
//
// Created: 31-08-2025
// Author:  Roel Smeets
//
// class library for LS7366 Quadrature Counter using SPI bus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef QC7366_H_
#define QC7366_H_

///////////////////////////////////////////////////////////////////////////////
// #includes

#include <inttypes.h>
#include <SPI.h>

///////////////////////////////////////////////////////////////////////////////
// enums

typedef enum 
{
	QC_MODE_REGISTER_0,
	QC_MODE_REGISTER_1,
} mode_register_t;


//////////////////////////////////////////////////////////////////////////////
// class definition of QC7366 quadrature counter

class QC7366Counter
{
	// public section, constants
public:

// public section, functions
public:
	QC7366Counter(uint8_t spiDeviceNumber);	// spiDeviceNumber must be 0 or 1 on NodeMCU

	bool begin(void);
	uint8_t readModeRegister(mode_register_t modeRegister);
	void writeModeRegister(mode_register_t modeRegister, uint8_t valueMDR);
	void disableCounter(void);
	void clearCountRegister(void);
	
	// private section, constants
private:
	static constexpr uint8_t REG_MDR0 =	(0x1 << 3);
	static constexpr uint8_t REG_MDR1 = (0x2 << 3);
	static constexpr uint8_t REG_DTR  = (0x3 << 3);
	static constexpr uint8_t REG_CNTR =	(0x4 << 3);
	static constexpr uint8_t REG_OTR  = (0x5 << 3);
	static constexpr uint8_t REG_STR  =	(0x6 << 3);

	static constexpr uint8_t OP_CLR	= (0x0 << 6);		// clear register
	static constexpr uint8_t OP_READ = 	(0x1 << 6);		// read register
	static constexpr uint8_t OP_WRITE = (0x2 << 6);		// write register
	static constexpr uint8_t OP_LOAD = 	(0x3 << 6);		// load register

	static constexpr uint8_t READ_MDR0  = (OP_READ | REG_MDR0);
	static constexpr uint8_t READ_MDR1 = (OP_READ | REG_MDR1);

	static constexpr uint8_t MODE_QC_1 = 0x01;
	static constexpr uint8_t MODE_QC_2 = 0x02;
	static constexpr uint8_t MODE_QC_4 = 0x03;

	static constexpr uint8_t MODE_FREERUNNING =	(0x0 << 2);
	static constexpr uint8_t MODE_SINGLECYCLE = (0x1 << 2);

	static constexpr uint8_t INDEX_DISABLE =  (0x0 << 4);
	static constexpr uint8_t INDEX_RESETCNTR = (0x2 << 4);

	static constexpr uint8_t CNT_DISABLE  = (0x1 << 2);
	
	static constexpr uint8_t INDEX_ASYNC = (0x0 << 6);
	
	static constexpr uint8_t FILTERCLOCK_DIV_2 = (0x1 << 7);

	static constexpr uint8_t WRITE_MDR0 = (OP_WRITE | REG_MDR0); 
	static constexpr uint8_t WRITE_MDR1 = (OP_WRITE | REG_MDR1);
	// static constexpr uint8_t ;
	// static constexpr uint8_t ;
	// static constexpr uint8_t ;

	
	// private section, functions
private:

// private section, variables
private:
	SPISettings _spisettings;	  // must still be implemented, now using global setting
	uint8_t _spiDeviceNumber;
};

#endif	// QC7366_H_
