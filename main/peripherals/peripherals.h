#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "sdkconfig.h"
#include "../openpedaller.h"

#define Rotate_90    0
#define Rotate_NONO  1

#define AMOLED_Rotate Rotate_NONO

#define LCD_HOST    SPI2_HOST
#define TOUCH_HOST  I2C_NUM_0

#if CONFIG_LV_COLOR_DEPTH == 32
#define LCD_BIT_PER_PIXEL       (24)
#elif CONFIG_LV_COLOR_DEPTH == 16
#define LCD_BIT_PER_PIXEL       (16)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_LCD_CS            (GPIO_NUM_9)
#define EXAMPLE_PIN_NUM_LCD_PCLK          (GPIO_NUM_10) 
#define EXAMPLE_PIN_NUM_LCD_DATA0         (GPIO_NUM_11)
#define EXAMPLE_PIN_NUM_LCD_DATA1         (GPIO_NUM_12)
#define EXAMPLE_PIN_NUM_LCD_DATA2         (GPIO_NUM_13)
#define EXAMPLE_PIN_NUM_LCD_DATA3         (GPIO_NUM_14)
#define EXAMPLE_PIN_NUM_LCD_RST           (GPIO_NUM_21)
#define EXAMPLE_PIN_NUM_BK_LIGHT          (-1)

// The pixel number in horizontal and vertical
#if (AMOLED_Rotate == Rotate_90)
#define EXAMPLE_LCD_H_RES              600
#define EXAMPLE_LCD_V_RES              450
#else
#define EXAMPLE_LCD_H_RES              450
#define EXAMPLE_LCD_V_RES              600
#endif

#define EXAMPLE_USE_TOUCH              1 //Without tp ---- Touch off

#if EXAMPLE_USE_TOUCH
#define EXAMPLE_PIN_NUM_TOUCH_SCL         (GPIO_NUM_48)
#define EXAMPLE_PIN_NUM_TOUCH_SDA         (GPIO_NUM_47)
#define EXAMPLE_PIN_NUM_TOUCH_RST         (GPIO_NUM_3)
#define EXAMPLE_PIN_NUM_TOUCH_INT         (-1)

#endif

#define EXAMPLE_LVGL_BUF_HEIGHT        (EXAMPLE_LCD_V_RES/10)
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2

void lcd_init();
void gps_init(app_t *app);
void sdcard_init();

#endif