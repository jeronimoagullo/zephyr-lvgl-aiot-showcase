/**
 * @file main.c
 * @author Jerónimo Agulló Ocampos (jeronimoagullo@jeroagullo.com)
 * @brief 	This app creates a button which has 3 events (press, release and click).
 * 			While the button is pressed, the text changes to "pressed" and changes to
 * 			"released" when it is released. When we click it, the counter is reseted
 *
 * @version 1.0
 * @date 2023-01-12
 *
 * @copyright Copyright (c) jeroagullo 2023
 *
 */
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>

#include "config.h"
#include "jeroagullo_lvgl/jeroagullo_lvgl.h"
#include "mqtt/jeroagullo_mqtt.h"
#include "config_network.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

void main(void)
{
	int ret = 0;
	const struct device *display_dev;
	struct net_if *iface = NULL;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

	// initialize the LVGL styles
	style_init();

	/*
	 *  Boot sequence
	 */

	// Add Zephyr logo
	LV_IMG_DECLARE(Zephyr_RTOS_logo_100pp_a8);
	lv_obj_t * img1 = lv_img_create(lv_scr_act());
	lv_img_set_src(img1, &Zephyr_RTOS_logo_100pp_a8);
	lv_obj_align(img1, LV_ALIGN_TOP_MID, 0, 50);

	// Add label for displaying booting log
	lv_obj_t * log_label = lv_label_create(lv_scr_act());
	lv_obj_align_to(log_label, img1, LV_ALIGN_OUT_BOTTOM_LEFT, -100, 10);
	lv_label_set_recolor(log_label, true); // Allow recoloring for fail/success

	// Initialize Networking
	lv_label_set_text(log_label, "[ ] Initializing network");
	lv_task_handler();
	ret = init_network(iface);
	char log_results[300] = "";

	if (ret == 0) {
		strcat(log_results, "#008000 [-] Network successed#\n");
	} else {
		strcat(log_results, "#ff0000 [x] Network failed#\n");
	}

	strcat(log_results, "[ ] Initializing TensorFlow Lite model\n");

	lv_label_set_text(log_label, log_results);
	lv_task_handler();
	k_msleep(1 * MSEC_PER_SEC);

	// Initialize TensorFlow
	LOG_INF("Starting TensorFlow Model");

	lv_label_set_text(log_label, 	"[*] Network successed\n"
					"[*] TensorFlow Lite model successed\n"
					"[ ] Initializing MQTT connection");
	lv_task_handler();
	k_msleep(1 * MSEC_PER_SEC);

	// Initialize MQTT

	LOG_INF("Starting mqtt connection");
	ret = mqtt_try_to_connect();

	if (ret != 0){
		char mqtt_error[100];
		sprintf(mqtt_error, "Please, check broker %s:%d\nRebooting system", CONFIG_MQTT_SERVER_ADDR, CONFIG_MQTT_SERVER_PORT);
		lv_label_set_text(log_label, 	"[*]Network successed\n"
						"[*]TensorFlow Lite model successed\n"
						"[x]MQTT connection Error");
		lv_obj_t *mqtt_error_label = lv_label_create(lv_scr_act());
		lv_obj_add_style(mqtt_error_label, &style_error, 0);
		lv_label_set_text(mqtt_error_label, mqtt_error);
		lv_obj_align_to(mqtt_error_label, log_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
		lv_task_handler();
		k_msleep(5 * MSEC_PER_SEC);
		sys_reboot(SYS_REBOOT_WARM);
	}


	lv_label_set_text(log_label, 	"[*]Network successed\n"
					"[*]TensorFlow Lite model successed\n"
					"[*]MQTT connection successed\n"
					"Launching app...");
	lv_task_handler();

	k_msleep(1 * MSEC_PER_SEC);

	// Delete booting objects
	lv_obj_del(img1);
	lv_obj_del(log_label);

	// Add display objects
	lv_example_tabview_1();
	display_blanking_off(display_dev);
	lv_obj_t * count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_LEFT, 360, 0);

	char count_str[22] = {0};
	int mseconds = 0;

	// foot object with boot time
	while (1) {
		mseconds = k_uptime_get();
		sprintf(count_str, "%.2d:%.2d:%.2d:%.3d",
			(mseconds/1000)/3600, (mseconds/1000)/60, (mseconds/1000)%60, mseconds%1000);
		lv_label_set_text(count_label, count_str);

		k_mutex_lock(&lvgl_mutex, K_FOREVER);
		lv_task_handler();
		k_mutex_unlock(&lvgl_mutex);

		k_msleep(10);
	}

}
