#ifndef JEROAGULLO_STYLES_H
#define JEROAGULLO_STYLES_H

#include <zephyr/kernel.h>

extern struct k_mutex lvgl_mutex;

extern k_tid_t thread_chart_id;

extern lv_style_t style_btn;
extern lv_style_t style_btn2;
extern lv_style_t style_btn3;
extern lv_style_t style_tabview;
extern lv_style_t style_tab;
extern lv_style_t style_error;

void style_init(void);

#endif // JEROAGULLO_STYLES_H
