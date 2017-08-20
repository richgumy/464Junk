/*
 * virtual_encoder.h
 *
 *  Created on: Aug 13, 2017
 *      Author: rje69
 * 
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#ifndef VIRTUAL_ENCODER_H_
#define VIRTUAL_ENCODER_H_

static int ENCODER_STATE = 00;

void initEncoderTimer(void);
void setState(void);
void updateEncoder(float angular_speed);
void initEncoderTimer(void);

#endif /* VIRTUAL_ENCODER_H_ */
