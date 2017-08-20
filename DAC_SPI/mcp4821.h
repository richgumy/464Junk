/*
 * mcp4821.h
 *
 *  Created on: Jul 31, 2017
 *      Author: rje69
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

#ifndef MCP4821_H_
#define MCP4821_H_

// Number of bytes transmitted
#define NUM_SSI_DATA            2

//*****************************************************************************
//
// @function - Initialise the MCP4821 DAC pins
//
//*****************************************************************************
extern void initMCP4821(void);

//*****************************************************************************
//
// @function - Set analogue output voltage, Vout
// @param - Enter a floating point (3dp) analogue voltage
//
//*****************************************************************************
extern void updateAnalogueOutput(float Vout);

#endif /* MCP4821_H_ */
