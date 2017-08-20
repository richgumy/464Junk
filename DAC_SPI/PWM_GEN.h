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
 * The PWM_GEN header file contains all the defines and functions of the source file so it can be easily
 * applied to varies modules.
 *
 */

#ifndef PWM_GEN_H_
#define PWM_GEN_H_

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



/*******************************************
 *      PWM config details.
 *******************************************/
#define PWM_START_PC       10
#define PWM_RATE_HZ        150
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_4
#define PWM_DIVIDER  	   4



/*******************************************
 * PWM Hardware Details M0PWM7 (gen 3), M1PWM5 (Gen 2)
 *******************************************/
//---Main Rotor PWM: M0PWM7,PC5, J4-05
//---Tail Rotor PWM: M1PWM5, PF1, J3-10

#define PWM_MAIN_BASE0          PWM0_BASE
#define PWM_MAIN_BASE1          PWM1_BASE
#define PWM_MAIN_GEN_3          PWM_GEN_3
#define PWM_MAIN_GEN_2         PWM_GEN_2
#define PWM_MAIN_OUTNUM_7        PWM_OUT_7
#define PWM_MAIN_OUTNUM_5       PWM_OUT_5
#define PWM_MAIN_OUTBIT_7        PWM_OUT_7_BIT
#define PWM_MAIN_OUTBIT_5        PWM_OUT_5_BIT
#define PWM_MAIN_PERIPH_PWM0    SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_PWM1    SYSCTL_PERIPH_PWM1
#define PWM_MAIN_PERIPH_GPIOC  SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_PERIPH_GPIOF   SYSCTL_PERIPH_GPIOF
#define PWM_MAIN_GPIO_BASE_C   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_BASE_F   GPIO_PORTF_BASE
#define PWM_MAIN_GPIO_CONFIG5  GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_CONFIG1  GPIO_PF1_M1PWM5
#define PWM_MAIN_GPIO_PIN_5    GPIO_PIN_5
#define PWM_MAIN_GPIO_PIN_1    GPIO_PIN_1

//Global period variable used in the main source file
extern uint32_t ui32Period;

//List of all the functions in order

//initialise all Pulse Width Modulator functions
void initialisePWM (void);

//Sets the clock frequency of the TM4C123 Device and the PWM clock rate
void initClocks (void);

//Sets the pulse width of the PWM output
void PWM_Output_Width_Set(uint32_t PWM_Base, uint32_t PWM_Out, uint32_t PWM_Width);

//Enables the PWM output
void PWM_Enable_Output(uint32_t PWM_Base, uint32_t PWM_OutBit);

//Disables the PWM output
void PWM_Disable_Output(uint32_t PWM_Base, uint32_t PWM_OutBit);

void initMotor(void);

int checkDuty(int cur_Duty);

void MainRotorSet(int DutyCycle);

void TailRotorSet(int DutyCycle);

#endif /* PWM_GEN_H_ */
