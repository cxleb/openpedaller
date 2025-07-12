#include "ui.h"

#include "core/lv_obj.h"
#include "core/lv_obj_pos.h"
#include "extra/layouts/flex/lv_flex.h"
#include "extra/layouts/grid/lv_grid.h"
#include "font/lv_font.h"
#include "peripherals/peripherals.h"
#include "lvgl.h"

// defined in martian_mono_80.c
extern const lv_font_t martian_mono_80;
extern const lv_font_t roboto_72;

static void event_cb(lv_event_t * e)
{
    LV_LOG_USER("Clicked");

    static uint32_t cnt = 1;
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
    cnt++;
}

/**
 * Add click event to a button
 */
void lv_example_event_click(ui_t* ui)
{
    lv_style_t *style_label_speed = &ui->stats.style_label_speed;
    lv_style_init(style_label_speed);
    lv_style_set_text_font(style_label_speed, &roboto_72);

    lv_style_t *style_label_normal = &ui->stats.style_label_normal;
    lv_style_init(style_label_normal);
    lv_style_set_text_font(style_label_normal, &lv_font_montserrat_28);

    lv_obj_t *main_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_cont, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
    lv_obj_align(main_cont, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_layout(main_cont, LV_LAYOUT_GRID);

    static int16_t column_dsc[] = {450/2-1, 450/2-1, LV_GRID_TEMPLATE_LAST};
    static int16_t row_dsc[] = {30, 30, 90, 30, 90, 30, 90, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(main_cont, column_dsc, row_dsc);
    //lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);

    //lv_obj_t * btn = lv_btn_create(lv_scr_act());
    //lv_obj_set_size(btn, 100, 50);
    //lv_obj_center(btn);
    //lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label_gps_status = lv_label_create(main_cont);
    lv_label_set_text(label_gps_status, LV_SYMBOL_WARNING LV_SYMBOL_GPS);
    lv_obj_center(label_gps_status);
    lv_obj_add_style(label_gps_status, style_label_normal, 0);
    lv_obj_set_grid_cell(label_gps_status, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    // Current Speed
    lv_obj_t * label_current_txt = lv_label_create(main_cont);
    lv_label_set_text(label_current_txt, "current: ");
    lv_obj_center(label_current_txt);
    lv_obj_add_style(label_current_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_current_txt, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_t * label_current_speed = lv_label_create(main_cont);
    lv_label_set_text(label_current_speed, "0");
    lv_obj_center(label_current_speed);
    lv_obj_add_style(label_current_speed, style_label_speed, 0);
    lv_obj_set_grid_cell(label_current_speed, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    // Average Speed
    lv_obj_t * label_avg_txt = lv_label_create(main_cont);
    lv_label_set_text(label_avg_txt, "average: ");
    lv_obj_center(label_avg_txt);
    lv_obj_add_style(label_avg_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_avg_txt, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_t * label_avg_speed = lv_label_create(main_cont);
    lv_label_set_text(label_avg_speed, "0");
    lv_obj_center(label_avg_speed);
    lv_obj_add_style(label_avg_speed, style_label_speed, 0);
    lv_obj_set_grid_cell(label_avg_speed, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    // Distance
    lv_obj_t * label_distance_txt = lv_label_create(main_cont);
    lv_label_set_text(label_distance_txt, "altitude: ");
    lv_obj_center(label_distance_txt);
    lv_obj_add_style(label_distance_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_distance_txt, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_t * label_distance = lv_label_create(main_cont);
    lv_label_set_text(label_distance, "0");
    lv_obj_center(label_distance);
    lv_obj_add_style(label_distance, style_label_speed, 0);
    lv_obj_set_grid_cell(label_distance, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    // Time
    lv_obj_t * label_time_txt = lv_label_create(main_cont);
    lv_label_set_text(label_time_txt, "time: ");
    lv_obj_center(label_time_txt);
    lv_obj_add_style(label_time_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_time_txt, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_t * label_time = lv_label_create(main_cont);
    lv_label_set_text(label_time, "00:00");
    lv_obj_center(label_time);
    lv_obj_add_style(label_time, style_label_speed, 0);
    lv_obj_set_grid_cell(label_time, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    // Alt Gained
    lv_obj_t * label_altitude_txt = lv_label_create(main_cont);
    lv_label_set_text(label_altitude_txt, "gain/loss: ");
    lv_obj_center(label_altitude_txt);
    lv_obj_add_style(label_altitude_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_altitude_txt, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    lv_obj_t * label_alt_gained = lv_label_create(main_cont);
    lv_label_set_text(label_alt_gained, "0 " LV_SYMBOL_UP);
    lv_obj_center(label_alt_gained);
    lv_obj_add_style(label_alt_gained, style_label_speed, 0);
    lv_obj_set_grid_cell(label_alt_gained, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);

    // Alt Lost
    lv_obj_t * label_alt_lost = lv_label_create(main_cont);
    lv_label_set_text(label_alt_lost, "0 " LV_SYMBOL_DOWN);
    lv_obj_center(label_alt_lost);
    lv_obj_add_style(label_alt_lost, style_label_speed, 0);
    lv_obj_set_grid_cell(label_alt_lost, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);

    

    ui->main_cont = main_cont;
    ui->stats.label_gps_status = label_gps_status;
    ui->stats.label_current_speed = label_current_speed;
    ui->stats.label_avg_speed = label_avg_speed;
    ui->stats.label_distance = label_distance;
    ui->stats.label_time = label_time;
    ui->stats.label_alt_gained = label_alt_gained;
    ui->stats.label_alt_lost = label_alt_lost;
}

ui_t *ui_init() {
    ui_t* ui = (ui_t*) malloc(sizeof(ui_t));

    lv_example_event_click(ui);

    return ui;
}

void ui_update_stats(ui_t *ui, app_t *app) {
    if (app->gps_got_first_single) {
        if (app->gps_has_lock) {
            lv_label_set_text_fmt(ui->stats.label_gps_status, LV_SYMBOL_GPS " No. %lu", app->gps_no_satl);
        } else {
            lv_label_set_text_fmt(ui->stats.label_gps_status,  LV_SYMBOL_WARNING LV_SYMBOL_GPS " No. %lu", app->gps_no_satl);
        }
    } else {
        lv_label_set_text(ui->stats.label_gps_status, "Waiting for GPS");
    }
    lv_label_set_text_fmt(ui->stats.label_current_speed, "%.0f", app->gps_current_speed);
    lv_label_set_text_fmt(ui->stats.label_avg_speed, "%.0f", app->gps_avg_speed);
    lv_label_set_text_fmt(ui->stats.label_distance, "%.0f", app->gps_current_alt);
    lv_label_set_text_fmt(ui->stats.label_time, "%2lu:%2lu", app->gps_time_minute, app->gps_time_second);
    lv_label_set_text_fmt(ui->stats.label_alt_gained, "%.0f " LV_SYMBOL_UP, app->gps_alt_gained);
    lv_label_set_text_fmt(ui->stats.label_alt_lost, "%.0f " LV_SYMBOL_DOWN, app->gps_alt_lost);
}