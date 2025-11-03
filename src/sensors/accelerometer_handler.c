/*
 * Copyright 2019 The TensorFlow Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accelerometer_handler.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(accelerometer_handler, LOG_LEVEL_DBG);

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>


K_THREAD_STACK_DEFINE(thread_accel_stack_area, THREAD_ACCEL_STACK_SIZE);
struct k_thread thread_accel_data;

float accel_fifo_buf[BUFLEN] = { 0.0f };
int accel_last_value[3] = { 0 };

const char *label = NULL;
const struct device *sensor = NULL;
int current_index = 0;

bool initial = true;

int SetupAccelerometer()
{
	sensor = DEVICE_DT_GET_ONE(invensense_mpu6050);

	if (sensor == NULL) {
		LOG_ERR("Failed to get accelerometer: %s\n",
				     sensor->name);
		return -1;
	}

	LOG_INF("Got accelerometer: %s\n", sensor->name);

	LOG_INF("Starting accelerometer thread");
	k_tid_t thread_accel_id = k_thread_create(&thread_accel_data, thread_accel_stack_area,
							K_THREAD_STACK_SIZEOF(thread_accel_stack_area),
							ReadAccelerometer,
							NULL, NULL, NULL,
							THREAD_ACCEL_PRIORITY, 0, K_NO_WAIT);

	k_thread_start(thread_accel_id);

	return 0;
}

/**
 * @brief Construct a new k timer define object
 * This timer sets the sample rate of the sensor	
 */
K_TIMER_DEFINE(sensor_timer, NULL, NULL);
#define SAMPLE_TIME_MS 40

void ReadAccelerometer()
{
	int rc;
	struct sensor_value accel[3];
	int index = 0;

	// Initialization of the timer for sensor sampling
	k_timer_init(&sensor_timer, NULL, NULL);

	LOG_INF("------> Starting ReadAccelerometer thread");

	while(1){
		rc = sensor_sample_fetch(sensor);
		if (rc < 0) {
			LOG_ERR("Fetch failed\n");
			continue;
		}

		rc = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, accel);
		if (rc < 0) {
			LOG_ERR("ERROR: channgel get failed: %d\n", rc);
			continue;
		}

		//TODO mutex
		accel_fifo_buf[index++] = (float)sensor_value_to_double(&accel[0]) / 9.8 * 1000.0;
		accel_fifo_buf[index++] = (float)sensor_value_to_double(&accel[1]) / 9.8 * 1000.0;
		accel_fifo_buf[index++] = (float)sensor_value_to_double(&accel[2]) / 9.8 * 1000.0;

		accel_last_value[0] = (int)(sensor_value_to_double(&accel[0]) / 9.8 * 1000.0);
		accel_last_value[1] = (int)(sensor_value_to_double(&accel[1]) / 9.8 * 1000.0);
		accel_last_value[2] = (int)(sensor_value_to_double(&accel[2]) / 9.8 * 1000.0);

		/*printf("index: %d, accel-> x: %f, y: %f, z: %f\n", index, accel_fifo_buf[index-3], 
			accel_fifo_buf[index-2], accel_fifo_buf[index-1]);*/

		if(index >= BUFLEN){
			index = 0;
		}
	
		/* Timer to match the sample rate */
		k_timer_start(&sensor_timer, K_MSEC(SAMPLE_TIME_MS), K_NO_WAIT);
		k_timer_status_sync(&sensor_timer);
	}

}
