//============================================================================
// Name        : controller.cpp
// Author      : Haydn
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>     /* malloc, free, rand */
#include "control_system.h"

void eval_lut(lut* lookup_table){

	if(((lookup_table->input) && *(lookup_table->input) >= lookup_table->break_points[lookup_table->len-1]*0.99) || //if start block
			((lookup_table->prev_tf->output) >= ((lookup_table->break_points[lookup_table->len-1])*0.99))){
		lookup_table->output = lookup_table->vals[lookup_table->len-1];
	}
	else if(((lookup_table->input) && *(lookup_table->input) >= (lookup_table->break_points[lookup_table->len-1]*0.99)) ||//if start block
			(lookup_table->prev_tf->output <= lookup_table->break_points[0]*0.99)){
		lookup_table->output = lookup_table->vals[0];
	}

	else for(int j = 0; j< lookup_table->len; j++){

		if(((lookup_table->input) && *(lookup_table->input) >= (lookup_table->break_points[j]) &&
				*(lookup_table->input) <= (lookup_table->break_points[j])) || //if start block
				(lookup_table->prev_tf->output >= lookup_table->break_points[j] &&
				lookup_table->prev_tf->output < lookup_table->break_points[j+1])){ //break point met

			float brk_interpolate = 0;
			brk_interpolate = ((lookup_table->prev_tf->output-lookup_table->break_points[j])/(lookup_table->break_points[j+1]-lookup_table->break_points[j]));
			if ( brk_interpolate < 0.0 ) brk_interpolate = -brk_interpolate;
			lookup_table->output = lookup_table->vals[j]+ (brk_interpolate * (lookup_table->vals[j+1]- lookup_table->vals[j]));
			break;
		}
	}
}

transfer_function init_tf(float new_n1, float new_d1, float new_T, float* new_input,
		bool new_integrator, float new_gain, float* new_addition, float new_integrator_init,
		float int_lim_h, float int_lim_l){


	transfer_function new_tf;
	new_tf.in_k = 0;
	new_tf.out_k = 0;
	new_tf.T = new_T;
	new_tf.prev_output = 0;
	new_tf.output = 0;
	new_tf.gain = 0;
	new_tf.addition = NULL;
	new_tf.input = new_input;
	new_tf.prev_input = 0;
	new_tf.integrator_lim_high = 0;
	new_tf.integrator_lim_low = 0;
	new_tf.is_gain = false;
	new_tf.is_addition = false;
	new_tf.prev_tf = NULL;
	new_tf.next_tf = NULL;
	new_tf.prev_lut = NULL;
	new_tf.next_lut = NULL;

	if(new_integrator){
		new_tf.in_k = 1/(2*(1/new_T));
		new_tf.out_k = 1;
		new_tf.output = new_integrator_init;
		new_tf.prev_output = new_integrator_init;
		new_tf.integrator_lim_high = int_lim_h;
		new_tf.integrator_lim_low = int_lim_l;
	}

	else if(new_gain != 0){
		new_tf.gain = new_gain;
	}

	else if(new_addition != NULL){
		new_tf.addition = new_addition;
	}

	else{ //transfer function
		new_tf.in_k = new_n1/(2*new_d1*(1/new_T)+1); //tustin transform
		new_tf.out_k = (2*((1/new_T)*new_d1)-1)/(2*((1/new_T)*new_d1)+1);
	}

	return new_tf;
}

void eval_tf(transfer_function* tf){
	tf->prev_output = tf->output;
	if(tf->addition){ //addition block
		if(tf->prev_tf){
			tf->output = *(tf->addition) + (tf->prev_tf->output);
		}
		else{
			tf->output = *(tf->addition) + (tf->prev_lut->output);
		}
	}
	else if(tf->gain){
		if(tf->prev_tf){
			tf->output = (tf->gain)* *(tf->input);
		}
		else{
			tf->output = (tf->gain)* *(tf->input);
		}
	}
	else if(tf->input){//start_block
		tf->output = (tf->in_k)*((tf->prev_input) + *(tf->input)) + (tf->out_k) * (tf->prev_output);
	}
	else if(tf->prev_tf){ //transfer function was before
		tf->output = (tf->in_k)*((tf->prev_tf->output) + (tf->prev_tf->prev_output)) + (tf->out_k) * (tf->prev_output);
	}
	else{ //lut was before
		tf->output = (tf->in_k)*((tf->prev_lut->output) + (tf->prev_lut->prev_output)) + (tf->out_k) * (tf->prev_output);

	}
	if((tf->integrator_lim_high) != 0){
		if((tf->output) > (tf->integrator_lim_high)){
			tf->output = tf->integrator_lim_high;
		}
		if((tf->output) < (tf->integrator_lim_low)){
			tf->output = tf->integrator_lim_low;
		}
	}
	if(tf->input){
		tf->prev_input = *(tf->input);
	}

}

void iterate_control_system(transfer_function* tf, lut* lookup){

	bool lut_next = false;
	bool tf_next = false;
	bool run_cs = true;
	if(tf){ tf_next = true;}
	if(lut_next) {lut_next = true;}

	while(run_cs){

		//if current block points to no block then end of control system has been reached
		if((((tf) && ((tf->next_tf == NULL) && (tf->next_lut == NULL))) || ((lookup) && (lookup->next_tf == NULL)))){
			run_cs = false;
		}

		if(tf_next){
			eval_tf(tf);

			if(tf->next_tf){
				tf = tf->next_tf;
				tf_next  = true;
				lut_next = false;
			}
			else if(tf->next_lut){
				lookup = tf->next_lut;
				tf_next  = false;
				lut_next = true;
			}
		}
		else if (lut_next){ //lut
			eval_lut(lookup);
			lookup->prev_output = lookup->output;
			if(lookup->next_tf){
				tf = lookup->next_tf;
				tf_next  = true;
				lut_next = false;
			}
		}
	}
}

lut init_lut(int len, float* input, float prev_input, float val_array[], float brk_array[]){

	lut new_lut;
	new_lut.len = len;
	new_lut.output = 0;
	new_lut.prev_output = 0;
	new_lut.input = NULL;
	new_lut.prev_input = 0;
	new_lut.prev_tf = NULL;
	new_lut.next_tf = NULL;

	for(int i=0; i<len; i++){
		new_lut.vals[i] = val_array[i];
		new_lut.break_points[i] = brk_array[i];
	}
	return new_lut;
}



