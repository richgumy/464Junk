/*
 * virtual_encoder.c
 *
 *  Created on: Aug 13, 2017
 *      Author: rje69
 * 
 * A set of functions which translate yaw speed into virtual quadrature encoder
 * signals A and B, to simulate the helicopter encoder setup. The encoder has
 * 112 slots in total -> 224 edges -> 2*Pi / 224 = 0.02805rad/edge
 * 
 */

#include "virtual_encoder.h"
#include "duty_reader.h"


#define NUM_STATES 4

// Global vars
int sequence[] = {0x0,0x2,0x3,0x1}; // in binary.. {00,10,11,01}
const float rad_per_slot = 28.0499; // divide by 1,000 to get actual rad/slot


int seq_index = 0; // Initialise state to index to 0 (i.e. state = 00 = AB)

uint32_t prev_time = 0;

/*
 * Setup Timer0 for the virtual encoder
 */
void
initEncoderTimer(void)
{
	// Set up encoder timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) // Ensure peripheral's ready
	{
	}

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP);

	TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

/*
 * Setup GPIO pins for signals A and B for the virtual encoder
 */
void
initEncoderSignals(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); // Signal A}
}

/*
 * Set virtual encoder signals A and B depending on state,
 * using PB0 for A and PB1 for B.
 */
void
setState(void)
{
	int A = (0x2 & ENCODER_STATE) >> 1;
	int B = (0x1 & ENCODER_STATE) << 1;
//	UARTprintf("AB=%d%d  ",A,B);

	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, A | B);
}

/*
 * Changes the states of the A and B signals on the encoder
 * depending on the sign and magnitude of the angular speed
 * input. (i.e. changes period between )
 */
void
updateEncoder(float angular_speed)
{
	float delta_t = rad_per_slot/angular_speed;

	//	UARTprintf("Delta:%d  ",(int)delta_t);
	uint32_t current_time = TimerValueGet(TIMER0_BASE, TIMER_A);
//	UARTprintf("Current:%u  ",current_time);
//	UARTprintf("Previous:%u  ",prev_time);
//	UARTprintf("Index:%d  State:%x\n",seq_index,ENCODER_STATE);
	uint32_t time_since_state_change = current_time - prev_time;
	time_since_state_change = time_since_state_change/1600; // Clock is 16MHz
//	UARTprintf("Time since state change:%u\n",time_since_state_change);

	if (time_since_state_change >= delta_t && delta_t)
	{
		 if (angular_speed > 0)
		 {
			 seq_index = seq_index < NUM_STATES-1 ? seq_index++ : 0; // loop if indexing outside of array
			 prev_time = current_time;
		 }
		 if (angular_speed < 0)
		 {
			 seq_index = seq_index > 0 ? seq_index-- : NUM_STATES-1;
			 prev_time = current_time;
		 }
	}
	ENCODER_STATE = sequence[seq_index];
}
