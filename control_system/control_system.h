/*
 * control_system.h
 *
 *  Created on: 15/08/2017
 *      Author: haydn
 */
typedef int bool;
enum { false, true };

#define def_LUT_MAX_LEN 20 //must be the same or greater than te largest lut brk point or values length


typedef struct lut{
	int len;
	float output ;
	float prev_output ;
	float* input;//only assign for the first block
	float prev_input;//only assign for the first block
	struct transfer_function* prev_tf;
	struct transfer_function* next_tf;
	float vals[def_LUT_MAX_LEN];
	float break_points[def_LUT_MAX_LEN];
}lut;

typedef struct transfer_function{
	float in_k;
	float out_k;
	float T;
	float prev_output;
	float output;
	float gain;
	float* addition;
	float* input; //only assign for the first block.
	float prev_input; //only assign for the first block
	float integrator_lim_high;
	float integrator_lim_low;
	bool is_gain;
	bool is_addition;
	struct transfer_function* prev_tf;
	struct transfer_function* next_tf;
	lut* prev_lut;
	lut* next_lut;
}transfer_function;


lut init_lut(int len, float* input, float prev_input, float val_array[], float brk_array[]);

void iterate_control_system(transfer_function* tf, lut* lookup);

void eval_tf(transfer_function* tf);

transfer_function init_tf(float new_n1, float new_d1, float new_T, float* new_input,
		bool new_integrator, float new_gain, float* new_addition, float new_integrator_init,
		float int_lim_h, float int_lim_l);

void eval_lut(lut* lookup_table);


