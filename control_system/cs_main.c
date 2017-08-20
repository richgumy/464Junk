#include "control_system_constants.h"
#include "control_system.h"

#include "mcp4821.h" /* DAC header */

#include <stdio.h>
#include <stdlib.h>     /* malloc, free, rand */

int main() {
	int steps = 300;
	float main_duty_cycle = 100;
	float tail_duty_cycle = 100;
	float gravity =-1*M_H* G;
	float Vout;
	
	//height cs start
	transfer_function main_rotor = init_tf(K_1, TAU_1, SAMPLE_TIME, &main_duty_cycle, 0, 0, NULL, 0, 0, 0);

	lut lut_Main_force_v_pwm = init_lut(def_LUT_MAIN_FORCE_V_PWM_LEN, NULL, 0, LUT_MAIN_FORCE_V_PWM_LUT, LUT_MAIN_FORCE_V_PWM_BRK);

	transfer_function gravity_constant = init_tf(0, 0, SAMPLE_TIME, NULL, 0, 0, &gravity, 0, 0, 0);
	transfer_function helicopter_mount = init_tf(K_2, TAU_2, SAMPLE_TIME, NULL, 0, 0, NULL, 0, 0, 0);
	transfer_function integrator_limited = init_tf(0, 1, SAMPLE_TIME, NULL, 1, 0, NULL, -1, 1, -1);

	lut percent_height_from_angle = init_lut(LUT_PERCENT_HEIGHT_FROM_ANGLE_LEN, NULL, 0, LUT_PERCENT_HEIGHT_FROM_ANGLE_LUT, LUT_PERCENT_HEIGHT_FROM_ANGLE_BRK);

	main_rotor.next_lut = &lut_Main_force_v_pwm;

	lut_Main_force_v_pwm.prev_tf = &main_rotor;
	lut_Main_force_v_pwm.next_tf = &gravity_constant;

	gravity_constant.prev_lut = &lut_Main_force_v_pwm;
	gravity_constant.next_tf = &helicopter_mount;

	helicopter_mount.prev_tf = &gravity_constant;
	helicopter_mount.next_tf = &integrator_limited;

	integrator_limited.prev_tf = &helicopter_mount;
	integrator_limited.next_lut = &percent_height_from_angle;

	percent_height_from_angle.prev_tf = &integrator_limited;
	//main rotor tf end

	//yaw cs start
	transfer_function main_rotor_1 = init_tf(K_3, TAU_3, SAMPLE_TIME, &main_duty_cycle, 0, 0, NULL, 0, 0, 0);
	transfer_function tail_rotor = init_tf(K_11, TAU_11, SAMPLE_TIME, &tail_duty_cycle, 0, 0, NULL, 0, 0, 0);

	lut tail_torque_v_pwm = init_lut(LUT_TAIL_TORQUE_V_PWM_LEN, NULL, 0, LUT_TAIL_TORQUE_V_PWM_LUT, LUT_TAIL_TORQUE_V_PWM_BRK);
	transfer_function nett_torque_on_helicopter = init_tf(0, 0, SAMPLE_TIME, NULL, 0, 0, &(main_rotor_1.output), 0, 0, 0);
	transfer_function helicopter_body = init_tf(K_12, TAU_12, SAMPLE_TIME, NULL, 0, 0, NULL, 0, 0, 0);
	transfer_function integrator = init_tf(0, 1, SAMPLE_TIME, NULL, 1, 0, NULL, 0, 0, 0);

	tail_rotor.next_lut = &tail_torque_v_pwm;

	tail_torque_v_pwm.prev_tf = &main_rotor_1;
	tail_torque_v_pwm.next_tf = &nett_torque_on_helicopter;

	nett_torque_on_helicopter.prev_lut = &tail_torque_v_pwm;
	nett_torque_on_helicopter.next_tf = &helicopter_body;

	helicopter_body.prev_tf = &nett_torque_on_helicopter;
	helicopter_body.next_tf = &integrator;

	integrator.prev_tf = &helicopter_body;
	//yaw cs end
	
	initMCP4821(); // initialise DAC
	

	for(int i=0; i<steps; i++){
		iterate_control_system(&main_rotor, NULL);
		iterate_control_system(&main_rotor_1, NULL); //order here matters
		iterate_control_system(&tail_rotor, NULL);
		Vout = updateHeight(percent_height_from_angle.output); // convert height to analogue voltage
		updateAnalogueOutput(Vout); // output analogue voltage
		printf("%% Height: %.10f  Yaw: %.10f \n", percent_height_from_angle.output, integrator.output);
	}
	
	
	
	return 0;
}

