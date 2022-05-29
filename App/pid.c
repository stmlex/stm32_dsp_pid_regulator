#include "pid.h"
#include "arm_math.h"

static arm_pid_instance_f32 PID = {
    .Kp = 0.1f,
    .Ki = 0.001f,
    .Kd = 0.00f};

static float pid_setpoint = 0;

void pid_init(void)
{
    arm_pid_init_f32(&PID, 1);
}

void pid_set(float sp){
    pid_setpoint = sp;
}

void pid_poll(void)
{
    static float pid_out = 0;
    static float pid_error = 0;
    pid_error = pid_setpoint - pid_out;
    pid_out = arm_pid_f32(&PID, pid_error);
}