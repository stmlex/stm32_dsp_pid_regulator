#ifndef DISPLAY_H
#define DISPLAY_H
#include "rtos_libs.h"

extern TaskHandle_t DisplayTask;
void display_task(void *pvParameters);

#endif // !DISPLAY_H