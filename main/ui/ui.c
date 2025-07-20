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

void ui_stats_create(ui_t* ui)
{
    lv_style_t *style_label_speed = &ui->stats.style_label_speed;
    lv_style_init(style_label_speed);
    lv_style_set_text_font(style_label_speed, &roboto_72);

    lv_style_t *style_label_normal = &ui->stats.style_label_normal;
    lv_style_init(style_label_normal);
    lv_style_set_text_font(style_label_normal, &lv_font_montserrat_28);

    //lv_obj_t *actual_screen = lv_obj_create(NULL);
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
    lv_obj_align(screen, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_layout(screen, LV_LAYOUT_GRID);
    static int16_t column_dsc[] = {
        5,
        EXAMPLE_LCD_H_RES/2-11, 
        EXAMPLE_LCD_H_RES/2-11, 
        LV_GRID_TEMPLATE_LAST
    };
    static int16_t row_dsc[] = {
        5,
        30, 
        30, 
        90, 
        30, 
        90, 
        30, 
        90, 
        90, 
        LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(screen, column_dsc, row_dsc);

    int first_col = 1;
    int second_col = 2;
    int start_row = 1;

    lv_obj_t * label_gps_status = lv_label_create(screen);
    lv_label_set_text(label_gps_status, LV_SYMBOL_WARNING LV_SYMBOL_GPS);
    lv_obj_center(label_gps_status);
    lv_obj_add_style(label_gps_status, style_label_normal, 0);
    lv_obj_set_grid_cell(label_gps_status, LV_GRID_ALIGN_START, first_col, 1, LV_GRID_ALIGN_START, start_row + 0, 1);

    // Current Speed
    lv_obj_t * label_current_txt = lv_label_create(screen);
    lv_label_set_text(label_current_txt, "current: ");
    lv_obj_center(label_current_txt);
    lv_obj_add_style(label_current_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_current_txt, LV_GRID_ALIGN_START, first_col, 1, LV_GRID_ALIGN_CENTER, start_row + 1, 1);

    lv_obj_t * label_current_speed = lv_label_create(screen);
    lv_label_set_text(label_current_speed, "0");
    lv_obj_center(label_current_speed);
    lv_obj_add_style(label_current_speed, style_label_speed, 0);
    lv_obj_set_grid_cell(label_current_speed, LV_GRID_ALIGN_START, first_col, 1, LV_GRID_ALIGN_CENTER, start_row + 2, 1);

    // Average Speed
    lv_obj_t * label_avg_txt = lv_label_create(screen);
    lv_label_set_text(label_avg_txt, "average: ");
    lv_obj_center(label_avg_txt);
    lv_obj_add_style(label_avg_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_avg_txt, LV_GRID_ALIGN_START, second_col, 1, LV_GRID_ALIGN_CENTER, start_row + 1, 1);

    lv_obj_t * label_avg_speed = lv_label_create(screen);
    lv_label_set_text(label_avg_speed, "0");
    lv_obj_center(label_avg_speed);
    lv_obj_add_style(label_avg_speed, style_label_speed, 0);
    lv_obj_set_grid_cell(label_avg_speed, LV_GRID_ALIGN_START, second_col, 1, LV_GRID_ALIGN_CENTER, start_row + 2, 1);

    // Distance
    lv_obj_t * label_distance_txt = lv_label_create(screen);
    lv_label_set_text(label_distance_txt, "altitude: ");
    lv_obj_center(label_distance_txt);
    lv_obj_add_style(label_distance_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_distance_txt, LV_GRID_ALIGN_START, first_col, 1, LV_GRID_ALIGN_CENTER, start_row + 3, 1);

    lv_obj_t * label_distance = lv_label_create(screen);
    lv_label_set_text(label_distance, "0");
    lv_obj_center(label_distance);
    lv_obj_add_style(label_distance, style_label_speed, 0);
    lv_obj_set_grid_cell(label_distance, LV_GRID_ALIGN_START, first_col, 1, LV_GRID_ALIGN_CENTER, start_row + 4, 1);

    // Time
    lv_obj_t * label_time_txt = lv_label_create(screen);
    lv_label_set_text(label_time_txt, "time: ");
    lv_obj_center(label_time_txt);
    lv_obj_add_style(label_time_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_time_txt, LV_GRID_ALIGN_START, second_col, 1, LV_GRID_ALIGN_CENTER, start_row + 3, 1);

    lv_obj_t * label_time = lv_label_create(screen);
    lv_label_set_text(label_time, "00:00");
    lv_obj_center(label_time);
    lv_obj_add_style(label_time, style_label_speed, 0);
    lv_obj_set_grid_cell(label_time, LV_GRID_ALIGN_START, second_col, 1, LV_GRID_ALIGN_CENTER, start_row + 4, 1);

    // Alt Gained
    lv_obj_t * label_altitude_txt = lv_label_create(screen);
    lv_label_set_text(label_altitude_txt, "gain/loss: ");
    lv_obj_center(label_altitude_txt);
    lv_obj_add_style(label_altitude_txt, style_label_normal, 0);
    lv_obj_set_grid_cell(label_altitude_txt, LV_GRID_ALIGN_START, first_col, 1, LV_GRID_ALIGN_CENTER, start_row + 5, 1);

    lv_obj_t * label_alt_gained = lv_label_create(screen);
    lv_label_set_text(label_alt_gained, "0 " LV_SYMBOL_UP);
    lv_obj_center(label_alt_gained);
    lv_obj_add_style(label_alt_gained, style_label_speed, 0);
    lv_obj_set_grid_cell(label_alt_gained, LV_GRID_ALIGN_START, first_col, 1, LV_GRID_ALIGN_CENTER, start_row + 6, 1);

    // Alt Lost
    lv_obj_t * label_alt_lost = lv_label_create(screen);
    lv_label_set_text(label_alt_lost, "0 " LV_SYMBOL_DOWN);
    lv_obj_center(label_alt_lost);
    lv_obj_add_style(label_alt_lost, style_label_speed, 0);
    lv_obj_set_grid_cell(label_alt_lost, LV_GRID_ALIGN_START, second_col, 1, LV_GRID_ALIGN_CENTER, start_row + 6, 1);
    
    // Prev Screen
    lv_obj_t *prev_button = lv_btn_create(screen);
    lv_obj_set_grid_cell(prev_button, LV_GRID_ALIGN_CENTER, first_col, 1, LV_GRID_ALIGN_CENTER, start_row + 7, 1);
    lv_obj_t *prev_button_label = lv_label_create(prev_button);
    lv_label_set_text(prev_button_label, LV_SYMBOL_LEFT);
    lv_obj_add_style(prev_button_label, style_label_speed, 0);
    lv_obj_center(prev_button_label);

    // Next Screen
    lv_obj_t *next_button = lv_btn_create(screen);
    lv_obj_set_grid_cell(next_button, LV_GRID_ALIGN_CENTER, second_col, 1, LV_GRID_ALIGN_CENTER, start_row + 7, 1);
    lv_obj_t *next_button_label = lv_label_create(next_button);
    lv_label_set_text(next_button_label, LV_SYMBOL_RIGHT);
    lv_obj_add_style(next_button_label, style_label_speed, 0);
    lv_obj_center(next_button_label);

    ui->stats.screen = screen;
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

    ui_stats_create(ui);

    lv_scr_load(ui->stats.screen);

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