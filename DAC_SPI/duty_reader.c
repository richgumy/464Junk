/*
 * duty_reader.c
 *
 *  Created on: Aug 9, 2017
 *      Author: pqi13 & rje69
 */

//Built in libraries
#include "duty_reader.h"

#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
//#include "string.h"
#include "stdlib.h"
#include "math.h"


//Macro Libraries
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"

//Standard Libraries
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"

//Include files from driver Libraries
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

//Global Variables
static float duty = 0;

float time_rise1;
static float time_rise2 = 0;
float time_fall;
float period;
int Rise;

// Interrupt handler for rising and falling edges
void
PinChangeIntHandler(void)
{
	static int start_flag = 1;

	// Clear the interrupt for PB1 (documentation recommends doing this early)
	GPIOIntClear (GPIO_PORTB_BASE, GPIO_PIN_1);

	//Read the pin and storing it for the current value on the quadrature
	Rise = GPIOPinRead (GPIO_PORTB_BASE, GPIO_PIN_1);

	if (Rise && start_flag)
	{
		time_rise1 = TimerValueGet(TIMER2_BASE, TIMER_A);
		start_flag = 0;
	}
	else if (!Rise && !start_flag)
	{
		time_fall = TimerValueGet(TIMER2_BASE, TIMER_A);
	}
	else if (Rise && !start_flag)
	{
		time_rise2 = TimerValueGet(TIMER2_BASE, TIMER_A);
		period = time_rise2 - time_rise1;

		duty = (time_fall - time_rise1) / period;
		time_rise1 = time_rise2;
	}
}

void
initPwmReadPin(void)
{
    // As a precaution the peripherals used are reset
	SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOB);
	// Enable the port used for input = Port B
	SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);

	// Register the handler for Port B into the vector table
	GPIOIntRegister (GPIO_PORTB_BASE, PinChangeIntHandler);

	// Configure the pin used: input on PB0 (J1-03) (Channel A) and PB1 (J1-04) (Channel B)
	GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_1);

	// Configure the pins for current strength and pull down type
	GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
			GPIO_PIN_TYPE_STD_WPD);

	// Set up the pin change interrupt (both edges)
	GPIOIntTypeSet (GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES);

	// Enable the pin change interrupt on both PB0 and PB1
	GPIOIntEnable (GPIO_PORTB_BASE, GPIO_PIN_1);

	IntEnable (INT_GPIOB);
}

void
initTimer(void)
{
    // Enable and configure Timer2 peripheral.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    // Initialize timer A to count up (32 bit?)
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC_UP);

    TimerEnable(TIMER2_BASE, TIMER_A);
}

void
initConsole(void)
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


int updateDuty(void)
{
	//UARTprintf("Duty is %d \n", (int)duty);
	return duty;
}
