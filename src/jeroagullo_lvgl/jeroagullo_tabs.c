#include <lvgl.h>
#include <zephyr/kernel.h>

#include "jeroagullo_lvgl.h"
#include "mqtt/jeroagullo_mqtt.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(jeroagullo_tabs);

#define TAB_WELCOME     0
#define TAB_SLIDERS     1
#define TAB_GRAPH       2
#define TAB_TFLITE      3
#define TAB_CONFIG      4


// NOTE: there is a bug in which the tabview changes when you press the button, so the returning value 
// of lv_tabview_get_tab_act is a weird number, working only by "sliding" instead of "clicking"
// it is solved using the tabview as user data parameter
static void event_tabview_cb(lv_event_t *e){

        lv_obj_t * tabview = lv_event_get_user_data(e);
        uint16_t tabIdx = lv_tabview_get_tab_act(tabview);

        LOG_INF("Current Active Tab : %d", tabIdx);

        switch(tabIdx){
                case TAB_WELCOME:
                        //k_thread_suspend(thread_inferenceTFLite_id);
                        k_thread_suspend(thread_chart_id);
                        k_thread_suspend(thread_mqtt_publish_id);
                break;
                case TAB_SLIDERS:
                        //k_thread_suspend(thread_inferenceTFLite_id);
                        k_thread_suspend(thread_chart_id);
                        k_thread_resume(thread_mqtt_publish_id);
                break;
                case TAB_GRAPH:
                        //k_thread_suspend(thread_inferenceTFLite_id);
                        k_thread_resume(thread_chart_id);
                        k_thread_suspend(thread_mqtt_publish_id);
                break;
                case TAB_TFLITE:
                        //k_thread_resume(thread_inferenceTFLite_id);
                        k_thread_suspend(thread_chart_id);
                        k_thread_suspend(thread_mqtt_publish_id);
                break;
                case TAB_CONFIG:
                        //k_thread_resume(thread_inferenceTFLite_id);
                        k_thread_suspend(thread_chart_id);
                        k_thread_suspend(thread_mqtt_publish_id);
                break;
        }
}

void create_tab1(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "Welcome!!!\n\n"
                        "It is the display PoC by jeroagullo\n"
                        "to demostrate the capabilities\n"
                        "of using a display with Zephyr.\n\n"
                        "Please, move around scrolling to the right\n"
                        "or touching the top tabs.\n"
                        "Each tab contains a different feature.\n" 
                        "Everything was designed with the Zephyr style,\n"
                        "but it could be your company's style ;D\n"
                        "\n"
                        "I challenge you to scroll down\n"
                        "\n"
                        "\n"
                        "\n"
                        "\n"
                        "\n"
                        "\n"
                        "Scroll a bit more..."
                        "\n"
                        "\n"
                        "\n"
                        "\n"
                        "\n"
                        "\n"
                        "Great! you did it!");

        // Add image
        LV_IMG_DECLARE(Zephyr_RTOS_logo_100pp_a8);
        lv_obj_t * img1 = lv_img_create(tab);
        lv_img_set_src(img1, &Zephyr_RTOS_logo_100pp_a8);
        lv_obj_align(img1, LV_ALIGN_TOP_RIGHT, -20, 0);
}
/**
 * @brief Create a tab2 object
 *              This tab shows the sliders and buttons (with mqtt)
 * 
 * @param tab 
 */

void create_tab2(lv_obj_t* tab){

        // create top text
        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab shows different sliders and buttons");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // create bottom text
        lv_obj_t * label_bottom = lv_label_create(tab);
        lv_label_set_text(label_bottom, "Touch a button");
        lv_obj_align(label_bottom, LV_ALIGN_BOTTOM_LEFT, 20, 0);


        // create sliders
        lv_example_slider_1(tab);
        lv_example_slider_3(tab);
        lv_example_arc_1(tab);

        // create buttons
        create_btn_1(tab, label_bottom);
        create_btn_2(tab, label_bottom);
        
}

void create_tab3(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab graphs the IMU data (mg @ 25Hz)");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        lv_imu_chart(tab);
}

void create_tab4(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab is booked for TensorFlow Lite demo");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        //create_tflite_tab(tab);
}

/**
 * @brief Create a tab5 object
 *              Config tag to
 * 
 * @param tab 
 */
void create_tab5(lv_obj_t* tab){
        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "You can check and modify the configuration of device");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
}


void lv_example_tabview_1(void)
{
        /*Create a Tab view object*/
        lv_obj_t * tabview;
        tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 30);

        lv_obj_add_style(tabview,  &style_tabview, 0);

        /*Add tabs (the tabs are page (lv_page) and can be scrolled*/
        lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "welcome");
        lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "sliders");
        lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "graph");
        lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "TFLite");
        lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "config");

        lv_obj_add_event_cb(tabview, event_tabview_cb, LV_EVENT_VALUE_CHANGED, tabview); // LV_EVENT_VALUE_CHANGED || LV_EVENT_CLICKED

        lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
        lv_obj_add_style(tab_btns, &style_tab, 0);

        create_tab1(tab1);
        create_tab2(tab2);
        create_tab3(tab3);
        create_tab4(tab4);
        create_tab5(tab5);

        //lv_obj_add_event_cb(tabview, event_tabview_cb, LV_EVENT_CLICKED, NULL); // LV_EVENT_VALUE_CHANGED || LV_EVENT_CLICKED
        //lv_obj_add_event_cb(tabview, event_tabview_cb, LV_EVENT_ALL, NULL); // LV_EVENT_VALUE_CHANGED || LV_EVENT_CLICKED

}

