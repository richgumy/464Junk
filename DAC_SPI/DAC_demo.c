// DAC Demo program

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "drivers/buttons.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
// DAC header
#include "mcp4821.h"

//*****************************************************************************
//
//! This example uses the following peripherals and I/O signals:
//! - SSI0 peripheral
//! - GPIO Port A peripheral (for SSI0 pins)
//! - SSI0Clk - PA2
//! - SSI0Tx  - PA5
//
//*****************************************************************************


int
main(void)
{

    		while(1) // update DAC forever
    		{
        		initMCP4821();

        	    updateAnalogueOutput(1.8); // output analogue voltage
    		}

}
