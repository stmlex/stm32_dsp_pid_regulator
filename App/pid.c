#include "pid.h"
#include "arm_math.h"

static arm_pid_instance_f32 PID = {
    .Kp = 0.9f,
    .Ki = 0.08f,
    .Kd = 0.00f};

volatile static float pid_setpoint = 0;

void pid_init(void)
{
    arm_pid_init_f32(&PID, 1);
}

void pid_set(float sp){
    pid_setpoint = sp;
}

void pid_poll(void)
{
    volatile static float pid_out = 0;
    volatile static float pid_error = 0;
    pid_error = pid_setpoint - pid_out;
    pid_out = arm_pid_f32(&PID, pid_error);
}