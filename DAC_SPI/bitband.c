

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
//!
//! This example uses the following peripherals and I/O signals:
//! - SSI0 peripheral
//! - GPIO Port A peripheral (for SSI0 pins)
//! - SSI0Clk - PA2
//! - SSI0Tx  - PA5
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of SSI0.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//
//*****************************************************************************

//********************************************************
// Constants
//********************************************************
#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 10

volatile uint8_t slowTick = false; // global tick state (for delays)

//*******************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*******************************************************************
void
SysTickIntHandler (void)
{
	static uint8_t tickCount = 0;
	const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;

    if (++tickCount >= ticksPerSlow)
    {						// Signal a slow tick
    	tickCount = 0;
    	slowTick = true;
    }

}

void
initSysTick (void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet (SysCtlClockGet () / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister (SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable ();
    SysTickEnable ();
}

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

////*****************************************************************************
////
//// Set analogue output voltage, Vout
////
////*****************************************************************************
//void updateAnalogueOutput(float Vout)
//{
//	uint32_t pui32DataTx[NUM_SSI_DATA];
//	uint32_t ui32Index;
//
//	uint32_t hex_value = 4096 * Vout / Vref;
//	// MSByte bit mask then shift value to make 8bit number
//	unscaled_analog_byte1 = (hex_value & 0xF00) >> 8;
//	// 0x30 = 0011 = config bits, go to datasheet
//	unscaled_analog_byte1 |= 0x30;
//
//	// LSByte bit mask
//	unscaled_analog_byte2 = hex_value & 0xFF;
//
//    pui32DataTx[0] = unscaled_analog_byte1;
//    pui32DataTx[1] = unscaled_analog_byte2;
//
//    // Set CS to zero to allow DAC register to be written
//    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0);
//
//    // Send 2 bytes of data
//    for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
//    {
//		// Send the data using the "blocking" put function.  This function
//		// will wait until there is room in the send FIFO before returning.
//		// This allows you to assure that all the data you send makes it into
//		// the send FIFO.
//		SSIDataPut(SSI0_BASE, pui32DataTx[ui32Index]);
//    }
//
//    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
//    while(SSIBusy(SSI0_BASE))
//    {
//    }
//
//    // Set CS to high to ready DAC register for Vout
//    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
//}
//
////*****************************************************************************
////
//// Initialise the MCP4821 DAC pins
////
////*****************************************************************************
//void initMCP4821 (void)
//{
//    // The SSI0 peripheral must be enabled for use on port A.
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//
//	// Configure the pin muxing for SSI0 functions on port A2, and A5
//	// respectively.
//    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
//    GPIOPinConfigure(GPIO_PA5_SSI0TX);
//
//    // Configure specific pins in use for SSI (SPI)
//    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_2);
//
//    // Configure pin A3 as Chip Select (CS) output
//    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
//
//    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
//    // system clock supply, idle clock level low and active low clock in
//    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
//    // For SPI mode, you can set the polarity of the SSI clock when the SSI
//    // unit is idle.  You can also configure what clock edge you want to
//    // capture data on.  Please reference the datasheet for more information on
//    // the different SPI modes.
//    //
//	#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
//		defined(TARGET_IS_TM4C129_RA1) ||                                         \
//		defined(TARGET_IS_TM4C129_RA2)
//		SSIConfigSetExpClk(SSI0_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_0,
//						   SSI_MODE_MASTER, 1000000, 8);
//	#else
//		SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
//						   SSI_MODE_MASTER, 1000000, 8);
//	#endif
//
//	// Enable the SSI0 module.
//	SSIEnable(SSI0_BASE);
//}

//*****************************************************************************
//
// Configure SSI0 in master Freescale (SPI) mode.  This example will send out
// 2 bytes of data.
//
//*****************************************************************************
int
main(void)
{
	//
	// Set the clocking to run directly from the external crystal/oscillator.
	// TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
	// crystal on your board.
	//
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
				   SYSCTL_XTAL_16MHZ);

	initSysTick ();

    // Initialise buttons
    ButtonsInit();

    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for SSI operation.
    //
    InitConsole();

//    while(1)
//    {
//    		uint8_t ui8CurButtonState, ui8PrevButtonState;
//
//    	    ui8CurButtonState = ui8PrevButtonState = 0;
//
//    	    int buttonPushed = 0;
//
//    		while(!buttonPushed)
//    		{
//    	        //
//    	        // Poll the debounced state of the buttons.
//    	        //
//    	        ui8CurButtonState = ButtonsPoll(0, 0);
//
//    	        // Make functions for these processes
//    	        if (slowTick)
//    	        {
//    				if(ui8CurButtonState != ui8PrevButtonState)
//    				{
//    		            if((ui8CurButtonState & ALL_BUTTONS) != 0)
//    		            {
//    		                if((ui8CurButtonState & ALL_BUTTONS) == LEFT_BUTTON)
//    		                {
//    		                	UARTprintf("Left Button is pressed.\n");
//    		                	buttonPushed = 1;
//    		                	// Decrement Voltage
//    		                	unscaled_analog_byte2 = unscaled_analog_byte2 > 0? unscaled_analog_byte2-- : 0;
//    		                }
//    		                if((ui8CurButtonState & ALL_BUTTONS) == RIGHT_BUTTON)
//    		                {
//    		                	UARTprintf("Right Button is pressed.\n");
//    		                	buttonPushed = 1;
//    		                	// Increment voltage else...
//    		                	// Limit volatge value so the value doesn't loop back to zero
//    		                	// i.e. 0xFF + 0x01 = 0x100, which is seen as 0x00 for the voltage
//    		                	unscaled_analog_byte2 = unscaled_analog_byte2 < 0xFF? unscaled_analog_byte2++ : 0xFF;
//    		                }
//    		            }
//    				}
//    				slowTick = false;
//    	        }
//
//    		}
//
//    		buttonPushed = 0;
//
//    	    // Display the setup on the console.
//    	    //
//    	    UARTprintf("SSI ->\n");
//    	    UARTprintf("  Mode: SPI\n");
//    	    UARTprintf("  Data: 8-bit\n\n");

//    	    //
//    	    // The SSI0 peripheral must be enabled for use.
//    	    //
//    	    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
//
//    	    //
//    	    // For this example SSI0 is used with PortA[5:2].  The actual port and pins
//    	    // used may be different on your part, consult the data sheet for more
//    	    // information.  GPIO port A needs to be enabled so these pins can be used.
//    	    // TODO: change this to whichever GPIO port you are using.
//    	    //
//    	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//
//    	    //
//    	    // Configure the pin muxing for SSI0 functions on port A2, and A5.
//    	    // This step is not necessary if your part does not support pin muxing.
//    	    // TODO: change this to select the port/pin you are using.
//    	    //
//    	    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
//    	    GPIOPinConfigure(GPIO_PA5_SSI0TX);
//
//    	    //
//    	    // Configure the GPIO settings for the SSI pins.  This function also gives
//    	    // control of these pins to the SSI hardware.  Consult the data sheet to
//    	    // see which functions are allocated per pin.
//    	    // The pins are assigned as follows:
//    	    //      PA5 - SSI0Tx
//    	    //      PA4 - SSI0Rx
//    	    //      PA3 - SSI0Fss
//    	    //      PA2 - SSI0CLK
//    	    // TODO: change this to select the port/pin you are using.
//    	    //
//    	    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_2);
//
//
//    	    // Set CS (ChipSelect) to low to ready DAC for data
//
//    	    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
//
//    	    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0);
//
//    	    //
//    	    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
//    	    // system clock supply, idle clock level low and active low clock in
//    	    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
//    	    // For SPI mode, you can set the polarity of the SSI clock when the SSI
//    	    // unit is idle.  You can also configure what clock edge you want to
//    	    // capture data on.  Please reference the datasheet for more information on
//    	    // the different SPI modes.
//    	    //
//    	    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
//    	                       SSI_MODE_MASTER, 1000000, 8);
//
//    	    //
//    	    // Enable the SSI0 module.
//    	    //
//    	    SSIEnable(SSI0_BASE);

    		while(1)
    		{
        		initMCP4821();

        	    updateAnalogueOutput(1.8); // output volts
    		}


//
//    	    //
//    	    // Initialize the data to send!!!
//    	    //
//    	    pui32DataTx[0] = 0x30 | unscaled_analog_byte1; // 0x30 -> 0011 = config bits go to datasheet
//    	    pui32DataTx[1] = unscaled_analog_byte2;
//
//    	    uint32_t MSB = unscaled_analog_byte1 << 8;
//
//    	    MSB = MSB+unscaled_analog_byte2;
//
//    	    float value = 1000*Vref*(float)MSB/4096;
//
//    	    int new_val = value;
//
//    	    int twoDP = new_val % 1000;
//
//    	    UARTprintf("Analogue Output Voltage: %x  %d.%d volts\n",MSB,(int)value/1000,twoDP);
//
//    	    //
//    	    // Display indication that the SSI is transmitting data.
//    	    //
//    	    // UARTprintf("Sent:\n  ");
//
//    	    //
//    	    // Send 2 bytes of data.
//    	    //
//    	    for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
//    	    {
//
//    	        //
//    	        // Send the data using the "blocking" put function.  This function
//    	        // will wait until there is room in the send FIFO before returning.
//    	        // This allows you to assure that all the data you send makes it into
//    	        // the send FIFO.
//    	        //
//    	        SSIDataPut(SSI0_BASE, pui32DataTx[ui32Index]);
//    	    }
//
//    	    //
//    	    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
//    	    //
//    	    while(SSIBusy(SSI0_BASE))
//    	    {
//    	    }
//
//    	    // Set CS to high to ready DAC for Vout
//    	    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
//
//    }

}
