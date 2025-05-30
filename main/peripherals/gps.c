
#include "esp_log.h"
#include "hal/i2c_types.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c.h>
#include "../openpedaller.h"
#include "nmea.h"
#include "gpgll.h"
#include "gpgga.h"
#include "gpvtg.h"

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

uint8_t i2c_read_buff(uint8_t addr,uint8_t reg,uint8_t *buf,uint8_t len)
{
  uint8_t ret;
  ret = i2c_master_write_read_device(I2C_NUM_0,addr,&reg,1,buf,len,1000);
  return ret;
}

bool available_bytes(uint16_t* num) {
    uint8_t bytes[2];
    uint8_t ret = i2c_read_buff(0x42, 0xFD, bytes, 2);
    if (ret != ESP_OK) {
        return 0;
    }
    *num = ((uint16_t)bytes[0]) << 8 | (uint16_t)bytes[1];
    return 1;
}

uint16_t read_device(uint16_t bytes_to_read, uint8_t *buf, uint32_t buf_size) {
    uint16_t to_read = bytes_to_read < buf_size ? bytes_to_read : buf_size;
    uint8_t ret = i2c_master_read_from_device(I2C_NUM_0, 0x42, buf, to_read , 1000);
    if (ret != ESP_OK) {
        return 0;
    }
    return to_read;
}

static void gps_task_2(void *arg) {
    
    app_t *app = (app_t*) arg;

    uint8_t *data = (uint8_t *) malloc(1024 + 1);
    char *sentence_buf = (char *) malloc(1024 + 1);
    size_t s_idx = 0;

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
        uint16_t available = 0;
        if(!available_bytes(&available)) {
            ESP_LOGE("gps", "Failed to get available bytes");
            continue;
        }
        ESP_LOGI("gps", "available bytes %d\n", available);

        if (available == 0) {
            continue;
        }

        uint16_t len = read_device(available, data, 1024);
        if(len == 0) {
            ESP_LOGE("gps", "Failed to read device");
            continue;    
        }

        ESP_LOGI("gps", "Read %d", len);

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
                ESP_LOGI("gps", "Processing sentence %s", sentence_buf);
                nmea_s* nmea = nmea_parse(sentence_buf, s_idx, 0);
                if (nmea != NULL) {
                    if (process_nmea(nmea, app)) {
                        num_processed++;
                    }
                } else {
                    ESP_LOGI("gps", "Invalid nmea sentence %s\n", sentence_buf);
                    //printf("Invalid NMEA sentence, %s", sentence_buf);
                }
                s_idx = 0;
            }
        }

        if (num_processed != 0) {
            app->gps_got_first_single = true;
        }
//
        // if we processed some gps messages, we need to update the ui
        if (num_processed != 0) {
            app_notify_main(app, GPS_TASK);
        }

        ESP_LOGI("gps", "Done processing batch");
    }
}

void gps_init(app_t *app) {
    xTaskCreate(gps_task_2, "gps_task", 8192, app, 10, NULL);

}