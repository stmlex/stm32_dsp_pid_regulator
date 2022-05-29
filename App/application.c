#include "main.h"
#include "application.h"
#include "log_libs.h"
#include "config.h"
#include "usbd_cdc_if.h"
#include "stdbool.h"
#include "cli_handle.h"
#include "pid.h"
#define PID_POLLING_PERIOD (10)

void LogLibsPrintCustom(char *buff, int n)
{
    CDC_Transmit_FS((uint8_t *)buff, n);
}

void LED_set(bool state)
{
    state ? HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void application(void)
{
    LOG_INFO("system reset");
    static uint32_t pid_timer_tick = 0;
    while (1)
    {
        CliReadTaskFunc();
        uint32_t tick = HAL_GetTick();
        if (tick >= (pid_timer_tick + PID_POLLING_PERIOD)) {
            pid_timer_tick = tick;
            pid_poll();
        }
    }
}