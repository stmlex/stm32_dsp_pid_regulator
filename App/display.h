#ifndef DISPLAY_H
#define DISPLAY_H
#include "rtos_libs.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TaskHandle_t DisplayTask;
void display_task(void *pvParameters);
void lv_example_led_1(bool state);

#ifdef __cplusplus
}
#endif

#endif // !DISPLAY_H