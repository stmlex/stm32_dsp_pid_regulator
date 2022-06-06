#include "display.h"
#include "config.h"
#include "lvgl.h"
#include "ssd1306.h"

TaskHandle_t DisplayTask;
TimerHandle_t LVGLtickTimer;
void LVGLtickTimer_cb(TimerHandle_t xTimer)
{
    lv_tick_inc(LV_TICK_PERIOD);
}

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISP_HOR_RES * DISP_VER_RES / 10]; /*Declare a buffer for 1/10 screen size*/

static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void display_task(void *pvParameters)
{
    FRTOS_TimerCreateStatic("lvgl_tick", LV_TICK_PERIOD, FRTOS_TIMER_AUTORELOAD, NULL, LVGLtickTimer_cb, LVGLtickTimer);
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISP_HOR_RES * DISP_VER_RES / 10); /*Initialize the display buffer.*/
    lv_disp_drv_init(&disp_drv);                                                    /*Basic initialization*/
    disp_drv.flush_cb = my_disp_flush;                                              /*Set your driver function*/
    disp_drv.draw_buf = &draw_buf;                                                  /*Assign the buffer to the display*/
    disp_drv.hor_res = DISP_HOR_RES;                                             /*Set the horizontal resolution of the display*/
    disp_drv.ver_res = DISP_VER_RES;                                             /*Set the vertical resolution of the display*/
    lv_disp_drv_register(&disp_drv);                                                /*Finally register the driver*/
    xTimerStart(LVGLtickTimer, 0);
    for (;;)
    {
        vTaskDelay(LV_TIMER_HANDLER_PERIOD);
        lv_timer_handler();
    }
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x, y;
    /*It's a very slow but simple implementation.
     *`set_pixel` needs to be written by you to a set pixel on the screen*/
    for (y = area->y1; y <= area->y2; y++)
    {
        for (x = area->x1; x <= area->x2; x++)
        {
            //TODO
            //set_pixel(x, y, *color_p);
            ssd1306_DrawPixel(x,y,lv_color_to1(*color_p));
            color_p++;
        }
    }

    lv_disp_flush_ready(disp); /* Indicate you are ready with the flushing*/
}