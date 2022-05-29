#include "main.h"
#include "application.h"
#include "log_libs.h"
#include "config.h"
#include "usbd_cdc_if.h"
#include "stdbool.h"
#include "cli_handle.h"
#include "pid.h"
#include "rtos_libs.h"

#define PID_POLLING_PERIOD (10)

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

static void app_task(void *pvParameters)
{
    LOG_INFO("system_reset");
    FRTOS_TaskCreateStatic(CliReadTaskFunc, "cli_read", CLI_READ_TASK_STACK_SIZE, NULL, CLI_READ_TASK_PRIORITY, CliReadTask);
    for (;;)
    {
        vTaskDelay(100);
    }
}

void application(void)
{
    LOG_INFO("system reset");
    FRTOS_TaskCreateStatic(app_task, "App", APP_TASK_STACK_SIZE, NULL, APP_TASK_PRIORITY, AppTask);
    vTaskStartScheduler();
    while (true)
    {
    }
}