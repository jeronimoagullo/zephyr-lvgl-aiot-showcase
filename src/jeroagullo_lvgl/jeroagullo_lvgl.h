#ifndef JEROAGULLO_STYLES_H
#define JEROAGULLO_STYLES_H

#include <zephyr/kernel.h>

extern struct k_mutex lvgl_mutex;

extern k_tid_t thread_chart_id;


/* Style definitions */
extern lv_style_t style_btn;
extern lv_style_t style_btn2;
extern lv_style_t style_btn3;
extern lv_style_t style_tabview;
extern lv_style_t style_tab;
extern lv_style_t style_error;

void style_init(void);

/* tabs definitions */
void lv_example_tabview_1(void);

/* Chart definitions */
void lv_update_imu_chart(void *chart_ptr, void *unused2, void *unused3);
void lv_imu_chart(lv_obj_t *cont);

/* Sliders and buttons definitions */
void lv_example_slider_1(lv_obj_t *cont);
void lv_example_slider_3(lv_obj_t * cont);
void lv_example_arc_1(lv_obj_t *cont);
void create_btn_1(lv_obj_t* tab, lv_obj_t * label_bottom);
void create_btn_2(lv_obj_t* tab, lv_obj_t * label_bottom);


#endif // JEROAGULLO_STYLES_H
