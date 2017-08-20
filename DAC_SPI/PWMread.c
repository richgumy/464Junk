/*
 * PWMread.c
 *
 *  Created on: Aug 5, 2017
 *      Author: rje69
 *
 *      TODO: This code has configured only CCP0 pin for Timer A. The CCP1 pin
 *      corresponding to Timer B is not configured. Also the incoming PWM signal
 *      has to be connected to both CCP0 and CCP1 pins.
 */

#include <stdint.h> // Variable definitions for the C99 standard.
#include <stdio.h> // Input and output facilities for the C99 standard.
#include <stdbool.h> // Boolean definitions for the C99 standard.
#include "inc/tm4c123gh6pm.h" // Definitions for the interrupt and register assignments.
#include "inc/hw_memmap.h" // Memory map definitions of the Tiva C Series device.
#include "inc/hw_types.h" // Definitions of common types and macros.
#include "driverlib/sysctl.h" // Definitions and macros for System Control API of DriverLib.
#include "driverlib/interrupt.h" // Defines and macros for NVIC Controller API of DriverLib.
#include "driverlib/gpio.h" // Definitions and macros for GPIO API of DriverLib.
#include "driverlib/timer.h" // Defines and macros for Timer API of DriverLib.
#include "driverlib/pin_map.h" //Mapping of peripherals to pins for all parts.
#include "driverlib/uart.h" // Definitions and macros for UART API of DriverLib.
#include "driverlib/adc.h" // Definitions for ADC API of DriverLib.
#include "driverlib/fpu.h" // Prototypes for the FPU manipulation routines.
#include "utils/uartstdio.h" // Prototypes for the UART console functions.
#include "driverlib/pwm.h"

#include "PWM_GEN.h" // Kev's code

// function prototypes
void init_timer(void);
void findPulseWidth(void);
void init_UART(void);

//Global variable called
uint32_t ui32Period;
uint32_t sys_clock;
uint32_t  start = 0, end = 0, length = 0;

int main(void)
{
    initClocks();

    initialisePWM();

    initMotor();

    TailRotorSet(50);

    // Enable the processor to respond to interrupts.
    IntMasterEnable();

    init_UART();

    UARTprintf("Welcome to PWM read\n");

    init_timer();

//    TimerEnable(TIMER0_BASE, TIMER_BOTH);
    TimerEnable(TIMER2_BASE, TIMER_A);

    //Now you have a timer

    //Stuff to add:
    // - rising and falling edge pinchange interrupt on pin(s) receiving pwm
    // - in that ISR, record timestamp using  TimerValueGet(TIMER2_BASE, TIMER_A);
    // - calculate timer ticks since last edge, use to calculate duty cycle
    // - you'll need to measure both period and pulse width, or assume fixed known period

    while(1);
}


void
init_timer(void)
{
    // Enable and configure Timer2 peripheral.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    // Initialize timer A to count up (32 bit?)
    TimerConfigure(TIMER2_BASE, (TIMER_CFG_PERIODIC_UP));


    // Set the value that the timers count to (0x9C3F = 39999)
//    TimerLoadSet(TIMER2_BASE, TIMER_A, 0xFFFFFFFF); //default value is already max.

    // Configure the pin that the timer A reads from (PB6) T0CCP0 and
    // timer B reads from (PB7) T0CCP1
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    GPIOPinConfigure(GPIO_PB6_T0CCP0 | GPIO_PB7_T0CCP1);
//    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7);


    // Registers a interrupt function to be called when timer B hits a neg edge event
//    IntRegister(INT_TIMER0B, findPulseWidth); // Changed interupt vector table to incl. findPulseWidth
    // Makes sure the interrupt is cleared
//    TimerIntClear(TIMER2_BASE, TIMER_CAPB_EVENT);
    // Enable the indicated timer interrupt source.
//    TimerIntEnable(TIMER2_BASE, TIMER_CAPB_EVENT);
    // The specified interrupt is enabled in the interrupt controller.
//    IntEnable(INT_TIMER2B);
}

// When negative edge is hit, record the values and find the difference, output to
// putty, timer interupt handler
void findPulseWidth(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
    start = TimerValueGet(TIMER0_BASE, TIMER_A);
    end = TimerValueGet(TIMER0_BASE, TIMER_B);
    length = end - start;

    UARTprintf("\nLENGTH = %d\n", length);
}

void init_UART(void)
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
