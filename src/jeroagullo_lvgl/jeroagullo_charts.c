#include <zephyr/kernel.h>
#include <lvgl.h>
#include "jeroagullo_lvgl.h"
#include "sensors/accelerometer_handler.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(jeroagullo_charts);


#define THREAD_CHART_STACK_SIZE 500
#define THREAD_CHART_PRIORITY 4
K_THREAD_STACK_DEFINE(thread_chart_stack_area, THREAD_CHART_STACK_SIZE);
struct k_thread thread_chart_data;

k_tid_t thread_chart_id;

void lv_update_imu_chart(void *chart_ptr, void *unused2, void *unused3)
{        
        lv_obj_t *chart = (lv_obj_t *)chart_ptr;

        int setup_status = SetupAccelerometer();

        uint8_t i = 0;
        while(1) {
                lv_chart_series_t * ser = lv_chart_get_series_next(chart, NULL);
                while(ser) {
                        k_mutex_lock(&lvgl_mutex, K_FOREVER);
                        lv_chart_set_next_value(chart, ser, accel_last_value[i]);
                        k_mutex_unlock(&lvgl_mutex);
                        ser = lv_chart_get_series_next(chart, ser);
                        //LOG_INF("i->%d --> %d", i, accel_last_value[i]);
                        i++;
                }

                k_msleep(40);
                i = 0;
        }
}

/**
 * Show the value of the pressed points
 */
void lv_imu_chart(lv_obj_t *cont)
{
        /*Create a chart*/
        lv_obj_t * chart;
        chart = lv_chart_create(cont);
        lv_obj_set_size(chart, 400, 180);
        lv_obj_align(chart, LV_ALIGN_BOTTOM_RIGHT,0,0);

        //lv_obj_add_event_cb(chart, event_cb, LV_EVENT_ALL, NULL);
        lv_obj_refresh_ext_draw_size(chart);

        /*Zoom in a little in X*/
        //lv_chart_set_zoom_x(chart, 800);

        /*Add two data series*/
        lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_series_t * ser3 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

        // sampling rate 25 Hz -> 50 values = 2
        lv_chart_set_point_count(chart, 50);
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -4000, 4000);

        // remove the value points
        lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

        // add axis label
        lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 4, 5, 4, true, 100);

        lv_obj_t *legend = lv_label_create(chart);
        lv_label_set_text(legend, "x->blue\ny->green\nz->red");
        lv_obj_align_to(legend, chart, LV_ALIGN_BOTTOM_LEFT,0,0);

        thread_chart_id = k_thread_create(&thread_chart_data, thread_chart_stack_area,
                                                        K_THREAD_STACK_SIZEOF(thread_chart_stack_area),
                                                        lv_update_imu_chart,
                                                        chart, NULL, NULL,
                                                        THREAD_CHART_PRIORITY, 0, K_NO_WAIT);

        k_thread_suspend(thread_chart_id);
}