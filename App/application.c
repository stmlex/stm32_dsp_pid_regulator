#include "main.h"
#include "application.h"
#include "log_libs.h"
#include "config.h"
#include "usbd_cdc_if.h"
#include "stdbool.h"
#include "cli_handle.h"
#include "pid.h"
#include "rtos_libs.h"
#include "pid.h"
#include "display.h"
#include "ssd1306.h"

bool host_com_port_open = false;
void LogLibsPrintCustom(char *buff, int n)
{
    if (!host_com_port_open)
    {
        return;
    }
    uint32_t attempts = 0;
    while (attempts++ < CDC_TRANSMIT_ATTEMPTS)
    {
        if (CDC_Transmit_FS((uint8_t *)buff, n) == USBD_OK)
        {
            return;
        }
    }
}

void LED_set(bool state)
{
    state ? HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

TaskHandle_t CliReadTask;
TaskHandle_t AppTask;
TimerHandle_t PidPollTimer;

void PidPollTimer_cb(TimerHandle_t xTimer)
{
    pid_poll();
}

static void app_task(void *pvParameters)
{
    FRTOS_TaskCreateStatic(CliReadTaskFunc, "cli_read", CLI_READ_TASK_STACK_SIZE, NULL, CLI_READ_TASK_PRIORITY, CliReadTask);
    FRTOS_TaskCreateStatic(display_task, "display", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, DisplayTask);
    FRTOS_TimerCreateStatic("pid_poll", PID_POLLING_PERIOD, FRTOS_TIMER_AUTORELOAD, NULL, PidPollTimer_cb, PidPollTimer);
    pid_init();
   
    xTimerStart(PidPollTimer, 0);
    for (;;)
    {
        vTaskDelay(100);
    }
}

void application(void)
{
    LOG_INFO("system reset");
    FRTOS_TaskCreateStatic(app_task, "App", APP_TASK_STACK_SIZE, NULL, APP_TASK_PRIORITY, AppTask);
    ssd1306_Init();
    vTaskStartScheduler();
    while (true)
    {
    }
}