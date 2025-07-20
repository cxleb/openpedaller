#ifndef UI_H
#define UI_H

#include "lvgl.h"
#include "openpedaller.h"

enum SCREEN {
    SCREEN_STATS
};

typedef struct ui_stats_s {
    lv_obj_t *screen;

    lv_obj_t *label_gps_status;
    lv_obj_t *label_current_speed;
    lv_obj_t *label_avg_speed;
    lv_obj_t *label_distance;
    lv_obj_t *label_time;
    lv_obj_t *label_alt_gained;
    lv_obj_t *label_alt_lost;

    lv_style_t style_label_normal;
    lv_style_t style_label_speed;
} ui_stats_t;

typedef struct ui_s {
    ui_stats_t stats;
} ui_t;

ui_t *ui_init();
void ui_update_stats(ui_t *ui, app_t *app);

#endif