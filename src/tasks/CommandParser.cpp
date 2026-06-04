///////////////////////////////////////////////////////////////////////////////
//
// CommandParser.cpp
//
// Authors: 	Roel Smeets
// Date: 		06-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application specific #includes

#include "SerialPrintf.h"
#include "tasks/CommandParser.h"

///////////////////////////////////////////////////////////////////////////////
// bool cmd_ParseCommand(String commandLine, String command)
// 
// See if "command" is present in commandLine. Returns true if found, false
// if not

bool cmd_ParseCommand(String commandLine, String command)
{
	const char *delimiters = " ";	// space ' ' is delimiter between tokens
	char *token = NULL;				// points to token in commandLine
	char *stringToParse = (char*)commandLine.c_str();
	char *commandName	= (char*)command.c_str();	// conversion needed from String type
	uint8_t commandLength = command.length();
	bool found	= false;

	// get pointer to first token in commandLine: thread safe version of strtok!
	token = strtok_r(stringToParse, delimiters, &stringToParse);

	// search for command in the commandLine until at the end or found
	while ((token != NULL) && (found == false))
	{
		// if EXACT command found:
		if ((strcmp(token, commandName) == 0)	&& (commandLength == strlen(token)))	
		{
			found = true;
		}
		
		// find next parameter name in commandLine
		token = strtok_r(NULL, delimiters, &stringToParse);
	}

	// SerialPrintf("> found = %s\n", found ? "true" : "false");

	return found;
}

///////////////////////////////////////////////////////////////////////////////
// int32_t cmd_ParseInteger(String commandLine, String paramName, 
//							int32_t previousValue, 
//							int32_t lowerLimit, int32_t upperlimit)
//
// parse integer value from a command line, e.g.:
// "duty=10 mode=1 frequency = +40 imax=6.3"
// modevalue = cmd_ParseInteger(line, "mode", prevMode, 0, 6);
//
// see also cmd_ParseFloat for float implementation

int32_t cmd_ParseInteger(String commandLine, String paramName, 
						 int32_t previousValue, int32_t lowerLimit, int32_t upperlimit)
{
	int32_t value = 0;
	const char *delimiters = " =";	// space ' 'and '=' are delimiters between tokens
	char *token = NULL;				// points to token in commandLine
	bool found	= false;
	char *name	= (char*)paramName.c_str();			// conversion needed from String type
	uint8_t paramNameLength = paramName.length();	// length of paramName
	char *stringToParse = (char*)commandLine.c_str();

	// get pointer to first token in commandLine: thread safe version!
	token = strtok_r(stringToParse, delimiters, &stringToParse);

	// search for paramName in the commandLine until at the end or found
	while ((token != NULL) && (found == false))
	{
		// if EXACT paramName found:
		if ((strcmp(token, name) == 0)	&& (paramNameLength == strlen(token)))	
		{
			found = true;
			token = strtok(NULL, delimiters);	// get ptr to value string
			if (token != NULL)					// get value if value string present
			{
				char *p = NULL;					// required  for checking valid value string
				value = strtol(token, &p, 10);
				if (*p != '\0')					// if number string is invalid
				{
					value = previousValue;		// then keep previous value
				}
			}
			else // if no value string present: keep previous value
			{
				value = previousValue;
			}
		}
		
		// find next parameter name in commandLine
		token = strtok(NULL, delimiters);
	}

	// if the paramName was not found in the commandLine, keep previous value
	if (found == false)
	{
		value = previousValue;
	}

	// keep the value between the user defined limits:
	value = constrain(value, lowerLimit, upperlimit);

	return value;
}

///////////////////////////////////////////////////////////////////////////////
// float cmd_ParseFloat(String commandLine, String paramName, 
//					 	float previousValue, 
//						float lowerLimit, float upperlimit)
//
// parse float value from a command line, see cmd_ParseInteger


float cmd_ParseFloat(String commandLine, String paramName, 
					 float previousValue, float lowerLimit, float upperlimit)
{
	float value = 0.0;
	const char *delimiters = " =";	// space ' 'and '=' are delimiters between tokens
	char *token = NULL;				// points to token in commandLine
	bool found	= false;
	char *name	= (char*)paramName.c_str();			// conversion needed from String type
	uint8_t paramNameLength = paramName.length();	// length of paramaName

	// get pointer to first token in commandLine:
	token = strtok((char*)commandLine.c_str(), delimiters);

	// search for paramName in the commandLine until at the end or found
	while ((token != NULL) && (found == false))
	{
		// if EXACT paramName found:
		if ((strcmp(token, name) == 0)	&& (paramNameLength == strlen(token)))	
		{
			found = true;
			token = strtok(NULL, delimiters);	// get ptr to value string
			if (token != NULL)					// get value if value string present
			{
				char *p = NULL;					// required  for checking valid value string
				value = strtod(token, &p);
				if ((*p) != '\0')				// if number string is invalid
				{
					value = previousValue;		// then keep previous value
				}
			}
			else // if no value string present: keep previous value
			{
				value = previousValue;
			}
		}
		
		// find next parameter name in commandLine
		token = strtok(NULL, delimiters);
	}

	// if the paramName was not found in the commandLine, keep previous value
	if (found == false)
	{
		value = previousValue;
	}

	// keep the value between the user defined limits:
	value = constrain(value, lowerLimit, upperlimit);

	return value;
}
