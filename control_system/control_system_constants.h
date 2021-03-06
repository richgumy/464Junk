#define def_LUT_MAIN_FORCE_V_PWM_LEN 17
#define def_LUT_PERCENT_HEIGHT_FROM_ANGLE_LEN 11
#define def_LUT_TAIL_TORQUE_V_PWM_LEN 16


int steps = 500;
float SAMPLE_TIME = 0.01;
float G = 9.8067;
float K_1 = 1;
float K_11 = 1;
float K_12 = 18000;
float K_2 = 1.5300;
float K_3 = 0.000028300;
float M_H = 0.0853;
float R_H = 200.50000;
float TAU_1 = 0.1000;
float TAU_11 = 0.1000;
float TAU_12 = 0.3700;
float TAU_2 = 0.6380;
float TAU_3 = 0.1000;

int LUT_MAIN_FORCE_V_PWM_LEN = def_LUT_MAIN_FORCE_V_PWM_LEN;
float LUT_MAIN_FORCE_V_PWM_LUT[def_LUT_MAIN_FORCE_V_PWM_LEN] = {0, 0.0268, 0.1609, 0.3142, 0.4748, 0.6190, 0.7626, 0.8796, 1.0104, 1.1098,1.2155, 1.3227, 1.4339, 1.5124, 1.6039, 1.6824, 1.7707};
float LUT_MAIN_FORCE_V_PWM_BRK[def_LUT_MAIN_FORCE_V_PWM_LEN] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80};

int LUT_PERCENT_HEIGHT_FROM_ANGLE_LEN = def_LUT_PERCENT_HEIGHT_FROM_ANGLE_LEN;
float LUT_PERCENT_HEIGHT_FROM_ANGLE_LUT[def_LUT_PERCENT_HEIGHT_FROM_ANGLE_LEN] = {46.5203, 57.0280, 67.6609, 78.3881, 89.1783, 100.0000, 110.8217, 121.6119, 132.3391, 142.9720, 153.4797};
float LUT_PERCENT_HEIGHT_FROM_ANGLE_BRK[def_LUT_PERCENT_HEIGHT_FROM_ANGLE_LEN] = {-1.0000, -0.8000, -0.6000, -0.4000 , -0.2000 , 0, 0.2000, 0.4000, 0.6000, 0.8000, 1.0000};

int LUT_TAIL_TORQUE_V_PWM_LEN = def_LUT_TAIL_TORQUE_V_PWM_LEN;
float LUT_TAIL_TORQUE_V_PWM_LUT[def_LUT_TAIL_TORQUE_V_PWM_LEN] = {0, 0.0003, 0.0005, 0.0007, 0.0010, 0.0012, 0.0014, 0.0016, 0.0018, 0.0020, 0.0022, 0.0023, 0.0025, 0.0026, 0.0027, 0.0029};
float LUT_TAIL_TORQUE_V_PWM_BRK[def_LUT_TAIL_TORQUE_V_PWM_LEN] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75};

