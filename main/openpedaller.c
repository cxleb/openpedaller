#include "openpedaller.h"
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/uart.h"
#include "esp_log.h"

#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

#include "peripherals/peripherals.h"
#include "ui/ui.h"

#include "nmea.h"
#include "gpgll.h"
#include "gpgga.h"
#include "gpvtg.h"

#define ECHO_UART_BAUD_RATE 9600
#define ECHO_UART_PORT_NUM 0
#define ECHO_TEST_TXD 43
#define ECHO_TEST_RXD 44
#define ECHO_TEST_CTS UART_PIN_NO_CHANGE
#define ECHO_TEST_RTS UART_PIN_NO_CHANGE
#define BUF_SIZE 1024

void app_notify_main(app_t *app, int task) {
    xTaskNotify(app->handling_task, task, eSetBits);
}

static SemaphoreHandle_t lvgl_mux = NULL;
static const char *TAG = "example";

bool example_lvgl_lock(int timeout_ms)
{
    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
}

void example_lvgl_unlock(void)
{
    xSemaphoreGive(lvgl_mux);
}

static void example_lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
    while (1) {
        task_delay_ms = lv_timer_handler();
        if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

void app_main(void)
{
    app_t* app = (app_t*) malloc(sizeof(app_t));
    app->gps_has_lock = false;
    app->gps_got_first_single = false;
    app->gps_no_satl = 0;
    app->gps_current_speed = 0;
    app->gps_current_alt = 0;
    app->gps_avg_speed = 0;
    app->gps_distance = 0;
    app->gps_alt_gained = 0;
    app->gps_alt_lost = 0;
    app->gps_time_hour = 0;
    app->gps_time_minute = 0;
    app->gps_time_second = 0;
    app->handling_task = xTaskGetCurrentTaskHandle();

    lcd_init();
    gps_init(app);
    sdcard_init();
    ui_t *ui = ui_init();

    // do an initial load to correct any errors
    ui_update_stats(ui, app);
    
    lvgl_mux = xSemaphoreCreateMutex();

    xTaskCreate(example_lvgl_port_task, "ui_task", 4096, NULL, 10, NULL);

    const TickType_t max_block_time = pdMS_TO_TICKS( 500 );
    BaseType_t result;

    uint32_t value = 0;
    while(1) {
        result = xTaskNotifyWait(pdFALSE, ULONG_MAX, &value, max_block_time);
        if (result != pdPASS) {
            continue;
        }
        if ((value & GPS_TASK) != 0) {
            ui_update_stats(ui, app);
        }
    }
}
