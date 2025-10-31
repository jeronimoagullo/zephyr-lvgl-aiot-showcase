#include <lvgl.h>
#include "jeroagullo_styles.h"
#include "mqtt/jeroagullo_mqtt.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(jeroagullo_sliders);

static void slider_simple_event_cb(lv_event_t * e);
static void slider_double_event_cb(lv_event_t * e);
static void slider_arc_event_cb(lv_event_t * e);

lv_obj_t * slider1;
lv_obj_t * slider3;
lv_obj_t * arc;
lv_obj_t * btn;
lv_obj_t * btn2;

// The counters must be defined as global variables, otherwise the variable scope
// will be just the function in which we call `lv_obj_set_userdata`, resulting
// in a null pointer when calling `lv_obj_get_userdata` from other function
int btn_count1 = 0;
int btn_count2 = 0;

/**
 * A default slider with a label displaying the current value
 */
void lv_example_slider_1(lv_obj_t *cont)
{        
        lv_obj_t *slider_label = lv_label_create(cont);

        /*Create a slider in the center of the display*/
        slider1 = lv_slider_create(cont);
        
        lv_obj_align(slider1, LV_ALIGN_TOP_LEFT, 10, 50);
        lv_obj_add_event_cb(slider1, slider_simple_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);

        lv_label_set_text(slider_label, "0%");
        lv_obj_set_width(slider1, lv_obj_get_width(lv_scr_act())/2-30);

        lv_obj_align_to(slider_label, slider1, LV_ALIGN_OUT_TOP_MID, 0, -10);
}

static void slider_simple_event_cb(lv_event_t * e)
{
        lv_obj_t * slider = lv_event_get_target(e);
        lv_obj_t * label = lv_event_get_user_data(e);

        char buf[8];

        k_mutex_lock(&lvgl_mutex, K_FOREVER);
        lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
        lv_label_set_text(label, buf);
	k_mutex_unlock(&lvgl_mutex);

        k_mutex_unlock(&mqtt_publish_mutex);
        sprintf(mqtt_publish_message, "plain slider: %d%%", (int)lv_slider_get_value(slider));
        k_condvar_signal(&mqtt_publish_condvar);
        k_mutex_unlock(&mqtt_publish_mutex);
}



/**
 * Show the current value when the slider is pressed by extending the drawer
 *
 */
void lv_example_slider_3(lv_obj_t * cont)
{        
        /*Create a slider in the center of the display*/
        slider3 = lv_slider_create(cont);
        lv_obj_center(slider3);

        lv_slider_set_mode(slider3, LV_SLIDER_MODE_RANGE);
        lv_slider_set_value(slider3, 70, LV_ANIM_OFF);
        lv_slider_set_left_value(slider3, 20, LV_ANIM_OFF);

        lv_obj_add_event_cb(slider3, slider_double_event_cb, LV_EVENT_ALL, NULL);
        lv_obj_refresh_ext_draw_size(slider3);

        lv_obj_set_width(slider3, lv_obj_get_width(lv_scr_act())/2-30);

        lv_obj_align_to(slider3, slider1, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
}

static void slider_double_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

        if(code == LV_EVENT_VALUE_CHANGED){
                // mqtt publish data
                k_mutex_unlock(&mqtt_publish_mutex);
                sprintf(mqtt_publish_message, "double slider: %d - %d", (int)lv_slider_get_left_value(obj), (int)lv_slider_get_value(obj));
                k_condvar_signal(&mqtt_publish_condvar);
                k_mutex_unlock(&mqtt_publish_mutex);
        }

        /*Provide some extra space for the value*/
        if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
                k_mutex_lock(&lvgl_mutex, K_FOREVER);
                lv_event_set_ext_draw_size(e, 50);
                k_mutex_unlock(&lvgl_mutex);
        }
        else if(code == LV_EVENT_DRAW_PART_END) {
                lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
                if(dsc->part == LV_PART_INDICATOR) {
                        char buf[16];
                        lv_snprintf(buf, sizeof(buf), "%d - %d", (int)lv_slider_get_left_value(obj), (int)lv_slider_get_value(obj));
                        k_mutex_lock(&lvgl_mutex, K_FOREVER);
                        lv_point_t label_size;
                        lv_txt_get_size(&label_size, buf, LV_FONT_DEFAULT, 0, 0, LV_COORD_MAX, 0);
                        lv_area_t label_area;
                        label_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2 - label_size.x / 2;
                        label_area.x2 = label_area.x1 + label_size.x;
                        label_area.y2 = dsc->draw_area->y1 - 10;
                        label_area.y1 = label_area.y2 - label_size.y;

                        lv_draw_label_dsc_t label_draw_dsc;
                        lv_draw_label_dsc_init(&label_draw_dsc);
                        label_draw_dsc.color = lv_color_hex3(0x888);
                        lv_draw_label(dsc->draw_ctx, &label_draw_dsc, &label_area, buf, NULL);
                        k_mutex_unlock(&lvgl_mutex);
                }
        }
}


void lv_example_arc_1(lv_obj_t *cont)
{
        lv_obj_t * label = lv_label_create(cont);

        /*Create an Arc*/
        arc = lv_arc_create(cont);
        lv_obj_set_size(arc, 140, 140);
        lv_arc_set_rotation(arc, 135);
        lv_arc_set_bg_angles(arc, 0, 270);
        lv_arc_set_value(arc, 10);
        lv_obj_center(arc);
        lv_obj_add_event_cb(arc, slider_arc_event_cb, LV_EVENT_VALUE_CHANGED, label);

        lv_obj_align_to(arc, slider3, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

        /*Manually update the label for the first time*/
        lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
}

static void slider_arc_event_cb(lv_event_t * e)
{
        lv_obj_t * arc = lv_event_get_target(e);
        lv_obj_t * label = lv_event_get_user_data(e);

        k_mutex_lock(&lvgl_mutex, K_FOREVER);
        lv_label_set_text_fmt(label, "%d%%", lv_arc_get_value(arc));
        lv_obj_align_to(label, arc, LV_ALIGN_CENTER, 0, 0);
        k_mutex_unlock(&lvgl_mutex);

        // mqtt publish data
        k_mutex_unlock(&mqtt_publish_mutex);
        sprintf(mqtt_publish_message, "arc slider: %d%%", lv_arc_get_value(arc));
        k_condvar_signal(&mqtt_publish_condvar);
        k_mutex_unlock(&mqtt_publish_mutex);
}


static void event_btn_cb(lv_event_t * e)
{
        lv_obj_t * btn = lv_event_get_target(e);
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t * label = lv_event_get_user_data(e);

        //static uint32_t cnt1[3] = {1};

        switch(code) {
        case LV_EVENT_PRESSED:
                k_mutex_lock(&lvgl_mutex, K_FOREVER);
                lv_label_set_text(label, "The last button event:\nLV_EVENT_PRESSED");
                k_mutex_unlock(&lvgl_mutex);
                break;
        case LV_EVENT_CLICKED:
                k_mutex_lock(&lvgl_mutex, K_FOREVER);
                lv_label_set_text(label, "The last button event:\nLV_EVENT_CLICKED");
                lv_obj_t *btn_label = lv_obj_get_child(btn, 0);

                int *count = (int*)lv_obj_get_user_data(btn);
                *count += 1;
                lv_label_set_text_fmt(btn_label, "%"LV_PRIu32, *count);

                k_mutex_unlock(&lvgl_mutex);
                break;
        case LV_EVENT_LONG_PRESSED:
                k_mutex_lock(&lvgl_mutex, K_FOREVER);
                lv_label_set_text(label, "The last button event:\nLV_EVENT_LONG_PRESSED");
                k_mutex_unlock(&lvgl_mutex);
                break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
                k_mutex_lock(&lvgl_mutex, K_FOREVER);
                lv_label_set_text(label, "The last button event:\nLV_EVENT_LONG_PRESSED_REPEAT");
                k_mutex_unlock(&lvgl_mutex);
                break;
        default:
                break;
        }
}

/**
 *      Creates the button 1
 */
void create_btn_1(lv_obj_t* tab, lv_obj_t * label_bottom){
        
        btn = lv_btn_create(tab);
        lv_obj_t * btn_label = lv_label_create(btn);

        lv_label_set_text(btn_label, "button 1");
        lv_obj_center(btn_label);

        lv_obj_add_style(btn, &style_btn, 0);
        lv_obj_align_to(btn, slider1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

        lv_obj_set_user_data(btn, &btn_count1);
        lv_obj_add_event_cb(btn, event_btn_cb, LV_EVENT_ALL, label_bottom);
}

/**
 *      Creates the button 2
 */
void create_btn_2(lv_obj_t* tab, lv_obj_t * label_bottom){
        
        btn2 = lv_btn_create(tab);
        lv_obj_t * btn_label2 = lv_label_create(btn2);

        lv_label_set_text(btn_label2, "button 2");
        lv_obj_center(btn_label2);

        lv_obj_add_style(btn2, &style_btn2, 0);
        lv_obj_align_to(btn2, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

        lv_obj_set_user_data(btn2, &btn_count2);
        lv_obj_add_event_cb(btn2, event_btn_cb, LV_EVENT_ALL, label_bottom);

}