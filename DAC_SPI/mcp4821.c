/*
 * mcp4821.c
 *
 *  Created on: Jul 31, 2017
 *      Author: rje69
 */

#include "mcp4821.h"


//*****************************************************************************
//
// Initialise the MCP4821 DAC pins
//
//*****************************************************************************
void initMCP4821(void)
{
    // The SSI0 peripheral must be enabled for use on port A.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Configure the pin muxing for SSI0 functions on port A2, and A5
	// respectively.
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    // Configure specific pins in use for SSI (SPI)
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_2);

    // Configure pin A3 as Chip Select (CS) output
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);

    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
	#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
		defined(TARGET_IS_TM4C129_RA1) ||                                         \
		defined(TARGET_IS_TM4C129_RA2)
		SSIConfigSetExpClk(SSI0_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_0,
						   SSI_MODE_MASTER, 1000000, 8);
	#else
		SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
						   SSI_MODE_MASTER, 1000000, 8);
	#endif

	// Enable the SSI0 module.
	SSIEnable(SSI0_BASE);
}


//*****************************************************************************
//
// Set analogue output voltage, Vout
//
//*****************************************************************************
void updateAnalogueOutput(float Vout)
{
	// Starting analogue voltage (Vout) in unscaled hex
	uint32_t unscaled_analog_byte1 = 0x07;
	uint32_t unscaled_analog_byte2 = 0x00;
	// Reference voltage for Vout calc
	float Vref = 2.048;

	// Buffer for transmitted data
	uint32_t pui32DataTx[NUM_SSI_DATA];
	// Index of buffer
	uint32_t ui32Index;

	uint32_t hex_value = 4096 * Vout / Vref;
	// MSByte bit mask then shift value to make 8bit number
	unscaled_analog_byte1 = (hex_value & 0xF00) >> 8;
	// 0x30 = 0011 = config bits, go to datasheet
	unscaled_analog_byte1 |= 0x30;

	// LSByte bit mask
	unscaled_analog_byte2 = hex_value & 0xFF;

    pui32DataTx[0] = unscaled_analog_byte1;
    pui32DataTx[1] = unscaled_analog_byte2;

    // Set CS to zero to allow DAC register to be written
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0);

    // Send 2 bytes of data
    for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
    {
		// Send the data using the "blocking" put function.  This function
		// will wait until there is room in the send FIFO before returning.
		// This allows you to assure that all the data you send makes it into
		// the send FIFO.
		SSIDataPut(SSI0_BASE, pui32DataTx[ui32Index]);
    }

    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
    while(SSIBusy(SSI0_BASE))
    {
    }

    // Set CS to high to ready DAC register for Vout
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}






