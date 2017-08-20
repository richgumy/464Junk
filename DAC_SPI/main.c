/*
 * main.c
 *
 *  Created on: Aug 9, 2017
 *      Author: rje69
 */

#include "duty_reader.h"
#include "virtual_encoder.h"

int
main (void)
{
	initEncoderTimer();
	initEncoderSignals();
	initConsole();
	float omega = 6.18;

	while(1)
	{
		updateEncoder(omega);
		setState();
//		omega = omega < 100? omega + 0.01 : 0.01;
//		UARTprintf("State:%d\n", ENCODER_STATE);
	}
}
