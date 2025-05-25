#ifndef OPENPEDALLER_H
#define OPENPEDALLER_H

#include "stdbool.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stddef.h>

#define GPS_TASK 1

typedef struct app_s {
    bool  gps_got_first_single;
    bool  gps_has_lock;
    uint32_t gps_no_satl;
    float gps_current_speed;
    float gps_current_alt;
    float gps_avg_speed;
    float gps_distance;
    float gps_previous_alt;
    float gps_alt_gained;
    float gps_alt_lost;
    uint32_t gps_time_hour;
    uint32_t gps_time_minute;
    uint32_t gps_time_second;

    float *gps_previous_speed;
    size_t gps_previous_speed_count;
    size_t gps_previous_speed_idx;
    //SemaphoreHandle_t event_read_lock;
    //SemaphoreHandle_t event_write_lock;
    //size_t event_read, event_write;
    //event_t events[EVENT_COUNT];
    TaskHandle_t handling_task;
} app_t;

#endif