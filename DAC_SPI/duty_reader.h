/*
 * yaw.h
 *
 *  Created on: Jul 26, 2017
 *      Author: pqi13
 */

#include <stdint.h>

#ifndef DUTY_READER_H_
#define DUTY_READER_H_

void PinChangeIntHandler(void);
void initPwmReadPin(void);
void initTimer(void);
int updateDuty(void);
void initConsole(void);

#endif /* DUTY_READER_H_ */
