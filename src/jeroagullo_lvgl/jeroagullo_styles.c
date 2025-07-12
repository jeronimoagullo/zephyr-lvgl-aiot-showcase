#include <zephyr/kernel.h>
#include <lvgl.h>
#include "jeroagullo_styles.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(jeroagullo_styles);

// for synchronization with lv_task_handler() calls
K_MUTEX_DEFINE(lvgl_mutex);

/* Styles */
lv_style_t style_btn;
lv_style_t style_btn2;
lv_style_t style_btn3;
lv_style_t style_tabview;
lv_style_t style_tab;
lv_style_t style_error;

/* Colors */
// jeroagullo colors
lv_color_t navy_color;
lv_color_t cyan_color;
lv_color_t honey_color;
lv_color_t kiwi_color;
lv_color_t sky_color;
lv_color_t gum_color;
lv_color_t tiger_color;

// jeroagullo gris colors
lv_color_t d10_color;
lv_color_t l10_color;
lv_color_t l20_color;
lv_color_t l30_color;
lv_color_t l40_color;
lv_color_t l50_color;

void init_jeroagullo_colors(){
        navy_color = lv_color_hex(0x061B2B);
        cyan_color = lv_color_hex(0x00C1DE);
        honey_color = lv_color_hex(0xF7B334);
        kiwi_color = lv_color_hex(0x85BD41);
        sky_color = lv_color_hex(0x84D4F7);
        gum_color = lv_color_hex(0xDA1684);
        tiger_color = lv_color_hex(0xF5801F);

        // jeroagullo gris colors
        d10_color = lv_color_hex(0x000712);
        l10_color = lv_color_hex(0x404B5C);
        l20_color = lv_color_hex(0x7F8691);
        l30_color = lv_color_hex(0xBFC3C8);
        l40_color = lv_color_hex(0xDEE1E2);
        l50_color = lv_color_hex(0xF7F8F8);
}

/**
 * @brief Create a style btn 1 object
 * 
 */
void create_style_btn_1(){
        lv_style_init(&style_btn);
        lv_style_set_radius(&style_btn, 10);
        lv_style_set_bg_opa(&style_btn, LV_OPA_COVER); 
        lv_style_set_bg_color(&style_btn, navy_color);

        lv_style_set_border_color(&style_btn, cyan_color);
        lv_style_set_border_opa(&style_btn, LV_OPA_40);
        lv_style_set_border_width(&style_btn, 2);

        lv_style_set_text_color(&style_btn, lv_color_white());
}

/**
 * @brief Create a style btn 2 object
 * 
 */
void create_style_btn_2(){
        lv_style_init(&style_btn2);
        lv_style_set_radius(&style_btn2, 10);
        lv_style_set_bg_opa(&style_btn2, LV_OPA_COVER);
        lv_style_set_bg_color(&style_btn2, cyan_color);
        lv_style_set_bg_grad_color(&style_btn2, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_bg_grad_dir(&style_btn2, LV_GRAD_DIR_HOR);

        lv_style_set_border_color(&style_btn2, navy_color);
        lv_style_set_border_opa(&style_btn2, LV_OPA_20);
        lv_style_set_border_width(&style_btn2, 2);

        lv_style_set_text_color(&style_btn2, lv_color_black());
}

/**
 * @brief Create a style btn 3 object
 * 
 */
void create_style_btn_3(){
        lv_style_init(&style_btn3);
        lv_style_set_radius(&style_btn3, 10);
        lv_style_set_bg_opa(&style_btn3, LV_OPA_80);
        lv_style_set_bg_color(&style_btn3, kiwi_color);
        lv_style_set_bg_grad_color(&style_btn3, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_bg_grad_dir(&style_btn3, LV_GRAD_DIR_VER);

        lv_style_set_border_color(&style_btn3, gum_color);
        lv_style_set_border_opa(&style_btn3, LV_OPA_40);
        lv_style_set_border_width(&style_btn3, 2);

        lv_style_set_text_color(&style_btn3, lv_color_black());
}

/**
 * @brief Create a style tab object
 * 
 */
void create_style_tab(){
        lv_style_init(&style_tab);
        lv_style_set_bg_opa(&style_tab, LV_OPA_80);
        lv_style_set_bg_color(&style_tab, navy_color);
        lv_style_set_text_color(&style_tab, lv_color_white());
        lv_style_set_border_side(&style_tab, LV_BORDER_SIDE_RIGHT);
}

/**
 * @brief Create a style tabview object
 * 
 */
void create_style_tabview(){
        lv_style_init(&style_tabview);
        lv_style_set_bg_opa(&style_tabview, LV_OPA_30);
        lv_style_set_bg_color(&style_tabview, cyan_color);
        lv_style_set_bg_grad_color(&style_tabview, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_bg_grad_dir(&style_tabview, LV_GRAD_DIR_VER);

        lv_style_set_text_color(&style_tabview, lv_color_black());

        /* Create jeroagullo tab style */
        create_style_tab();
}

/**
 * @brief Create a style error object
 * 
 */
void create_style_error(){
        lv_style_init(&style_error);
        lv_style_set_text_color(&style_error, lv_palette_main(LV_PALETTE_RED));
}



void style_init(void)
{
        /* create jeroagullo colors */
        init_jeroagullo_colors();

        /* Create jeroagullo button style 1*/
        create_style_btn_1();

        /* Create jeroagullo button style 2*/
        create_style_btn_2();

        /* Create jeroagullo button style 2*/
        create_style_btn_3();

        /* Create tabview style */
        create_style_tabview();

        /* Create error style */
        create_style_error();
}