///////////////////////////////////////////////////////////////////////////////
//
// CommandParser.h
//
// Created: 06-06-2025
// Author:  Roel Smeets
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// function prototypes

bool cmd_ParseCommand(String commandLine, String paramName);
int32_t cmd_ParseInteger(String commandLine, String paramName, 
						 int32_t previousValue, int32_t lowerLimit, int32_t upperlimit);
float cmd_ParseFloat(String commandLine, String paramName, 
					 float previousValue, float lowerLimit, float upperlimit);

 #endif	// COMMANDPARSER_H
