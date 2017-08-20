/*
 * ENCE361
 *
 * PWM_GEN Module for Final Helicopter project
 *
 * Authors: Jun Qin (99756584), Khanh Nguyen Ly (88555900), Tao Ren (89494851)
 *
 * Last Date Modified: 5/6/2017
 *
 * Description:
 * PWM_GEN Module: This is used to generate a Pulse Width Modulator output waveform.
 * This can be applied to many different situations where a generator
 * waveform needs to be matched with the devices PWM waveform and this
 * module will keep it in sync and reproduce a identical waveform.
 * For this specific case it is used for generating waveforms to the
 * main rotor and the tail rotor of a Helicopter.
 */

//Built in Libraries
#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

//Macro libraries
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "inc/tm4c123gh6pm.h" // Definitions for the interrupt and register assignments.

//Driver Liraries
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

//Include modulised files
#include "PWM_GEN.h"


//Global variable called
uint32_t ui32Period;


void
initialisePWM (void)
{

    // Set the PWM clock rate (using the prescaler)
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    // As a precaution the peripherals used are reset
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_GPIOC); //Used for PWM output
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_GPIOF);
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_PWM0);  //Main Rotor PWM
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_PWM1); //Tail Rotor PWM


    //After the reset precaution the peripherals are then enabled
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM0);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM1);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIOC);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIOF);


    //Configuring the main rotor PC5 and tail rotor PF1
    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG5);
    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG1);


    //Set the main motor (PC5) and tail motor (PF1)
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE_C, PWM_MAIN_GPIO_PIN_5);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE_F, PWM_MAIN_GPIO_PIN_1);


    //Calculate the PWM period corresponding to the start frequency
    ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_RATE_HZ;


    //This sets the mode of the operation for a PWM generator e.g counting mode and synchronization mode.
    PWMGenConfigure(PWM_MAIN_BASE0, PWM_MAIN_GEN_3,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM_MAIN_BASE1, PWM_MAIN_GEN_2,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);


	//Gets the period of the specified PWM generator block which is defined as the number of PWM clock ticks
    PWMGenPeriodSet(PWM_MAIN_BASE0, PWM_MAIN_GEN_3, ui32Period);
    PWMGenPeriodSet(PWM_MAIN_BASE1, PWM_MAIN_GEN_2, ui32Period);


    //Gets the currently programmed pulse width for the specified PWM output.
    //Initially sets widths to 0.
    PWMPulseWidthSet(PWM_MAIN_BASE0, PWM_MAIN_OUTNUM_7, ui32Period * PWM_START_PC*1 / 100);
    PWMPulseWidthSet(PWM_MAIN_BASE1, PWM_MAIN_OUTNUM_5, ui32Period * PWM_START_PC*1 / 100);


    //This allows the PWM clock to drive the timer/counter for the specified generator block
    PWMGenEnable(PWM_MAIN_BASE0, PWM_MAIN_GEN_3);
    PWMGenEnable(PWM_MAIN_BASE1, PWM_MAIN_GEN_2);
}


void
initClocks (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    // Set the PWM clock rate (using the prescaler)
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
}


void
PWM_Output_Width_Set(uint32_t PWM_Base, uint32_t PWM_Out, uint32_t PWM_Width)
{
	PWMPulseWidthSet(PWM_Base, PWM_Out, PWM_Width);
}


void
PWM_Enable_Output(uint32_t PWM_Base, uint32_t PWM_OutBit)
{
	PWMOutputState(PWM_Base, PWM_OutBit, true);
}


void
PWM_Disable_Output(uint32_t PWM_Base, uint32_t PWM_OutBit)
{
	PWMOutputState(PWM_Base, PWM_OutBit, false);
}


void
initMotor(void){

    initialisePWM ();
	PWM_Enable_Output(PWM_MAIN_BASE0, PWM_MAIN_OUTBIT_7);
	PWM_Enable_Output(PWM_MAIN_BASE1, PWM_MAIN_OUTBIT_5);

}

int checkDuty(int cur_Duty)
{
	if (cur_Duty < 2) //Keeps the duty cycles between 2-98% range otherwise off.
	{
		cur_Duty = 0;
	}
	else if (cur_Duty > 98)
	{
		cur_Duty = 98;
	}
	return cur_Duty;
}



//---Main Rotor PWM: M0PWM7,PC5, J4-05
//---Tail Rotor PWM: M1PWM5, PF1, J3-10

void
MainRotorSet(int DutyCycle)
{
 DutyCycle =  checkDuty(DutyCycle);
 PWM_Output_Width_Set(PWM_MAIN_BASE0, PWM_MAIN_OUTNUM_7, ui32Period * DutyCycle / 100);
}

void
TailRotorSet(int DutyCycle){

 DutyCycle =  checkDuty(DutyCycle);
 PWM_Output_Width_Set(PWM_MAIN_BASE1, PWM_MAIN_OUTNUM_5, ui32Period * DutyCycle / 100);

}

