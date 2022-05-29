#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C" {
#endif

void pid_poll(void);
void pid_set(float sp);

#ifdef __cplusplus
}
#endif

#endif // !PID_H