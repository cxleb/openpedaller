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

static int process_nmea(nmea_s* data, app_t *app) {
    app->gps_got_first_single = true;
    if (data->type == NMEA_GPGGA) {
        printf("GPGGA Sentence\n");
        nmea_gpgga_s *gpgga = (nmea_gpgga_s *) data;
        // when we get our first correct gps signal we need to set gps has signal
        app->gps_no_satl = gpgga->n_satellites;
         
        if (app->gps_no_satl > 0) {
            app->gps_has_lock = true;
        }

        app->gps_previous_alt = app->gps_current_alt;
        app->gps_current_alt = gpgga->altitude;
        float alt_delta = app->gps_current_alt - app->gps_previous_alt;
        if (alt_delta > 0) {
            app->gps_alt_gained += alt_delta;
        } else {
            app->gps_alt_lost += -alt_delta;
        }
        return true;
    } else if (NMEA_GPGLL == data->type) {
        nmea_gpgll_s *gpgll = (nmea_gpgll_s *) data;

        //gpgll->
        //gpgll.
        app->gps_time_hour = gpgll->time.tm_hour;
        app->gps_time_minute = gpgll->time.tm_min;
        app->gps_time_second = gpgll->time.tm_sec;

       printf("GPGLL Sentence\n");
       //printf("Longitude:\n");
       //printf("  Degrees: %d\n", gpgll->longitude.degrees);
       //printf("  Minutes: %f\n", gpgll->longitude.minutes);
       //printf("  Cardinal: %c\n", (char) gpgll->longitude.cardinal);
       //printf("Latitude:\n");
       //printf("  Degrees: %d\n", gpgll->latitude.degrees);
       //printf("  Minutes: %f\n", gpgll->latitude.minutes);
       //printf("  Cardinal: %c\n", (char) gpgll->latitude.cardinal);
       return true;
    } else if (NMEA_GPVTG == data->type) {
        nmea_gpvtg_s *gpvtg = (nmea_gpvtg_s *) data;
        printf("GPVTG Sentence\n");
        //gpvtg->
        app->gps_current_speed = gpvtg->gndspd_kmph;
        return true;
    }
    return false;
}

static void gps_task(void *arg)
{
    app_t *app = (app_t*) arg;
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    //ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE + 1);
    char *sentence_buf = (char *) malloc(BUF_SIZE + 1);
    size_t s_idx = 0;

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // Read data from the UART
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        
        int num_sentence = 0, num_processed = 0;
        bool finished_sentence;
        for (int i = 0; i < len; i++) {
            finished_sentence = false;
            // start copy from data to sentence util we get a \n
            while(data[i] != '\n' && i < len) {
                sentence_buf[s_idx++] = data[i];
                i++;
            }
            // if the final byte was a new line copy it and set the flag
            if (data[i] == '\n') {
                sentence_buf[s_idx++] = '\n';
                sentence_buf[s_idx] = 0;
                finished_sentence = true;
                num_sentence++;
            }

            if (finished_sentence) {
                nmea_s* nmea = nmea_parse(sentence_buf, s_idx, 0);
                if (nmea != NULL) {
                    if (process_nmea(nmea, app)) {
                        num_processed++;
                    }
                } else {
                    //printf("Invalid NMEA sentence, %s", sentence_buf);
                }
                s_idx = 0;
            }
        }

        if (num_processed != 0) {
            app->gps_got_first_single = true;
        }

        // if we processed some gps messages, we need to update the ui
        if (num_processed != 0) {
            app_notify_main(app, GPS_TASK);
        }

        printf("Done processing batch\n");
    }
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
    ui_t *ui = ui_init();

    // do an initial load to correct any errors
    ui_update_stats(ui, app);
    
    lvgl_mux = xSemaphoreCreateMutex();

    xTaskCreate(gps_task, "gps_task", 8192, app, 10, NULL);
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
