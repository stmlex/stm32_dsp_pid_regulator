#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_MEM_SIZE      (3U * 1024U)        /*Size memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#define DISP_HOR_RES          (128)
#define DISP_VER_RES          (64)
#define LV_ANTIALIAS         0       /*1: Enable anti-aliasing*/

#define LV_COLOR_DEPTH      1                     /*Color depth: 1/8/16/32*/
#define USE_LV_SHADOW           0               /*1: Enable shadows*/
#define USE_LV_GROUP            0               /*1: Enable object groups (for keyboards)*/
#define USE_LV_GPU              0               /*1: Enable GPU interface*/
#define USE_LV_REAL_DRAW        0               /*1: Enable function which draw directly to the frame buffer instead of VDB (required if LV_VDB_SIZE = 0)*/
#define USE_LV_FILESYSTEM       0               /*1: Enable file system (might be required for images*/
#define USE_LV_LOG      0   /*Enable/disable the log module*/
// I turned off all the other themes
#define USE_LV_THEME_MONO       1       /*Mono color theme for monochrome displays*/
#define LV_USE_THEME_DEFAULT 0

//#define LV_USE_LOG 3
//#define LV_LOG_LEVEL LV_LOG_LEVEL_ERROR
//#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_BTN        1

#endif // !LV_CONF_H