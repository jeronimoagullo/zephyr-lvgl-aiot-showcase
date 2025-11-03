#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_ACCELEROMETER_HANDLER_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_ACCELEROMETER_HANDLER_H_

/* Expose a C friendly interface for main functions. */
#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_ACCEL_STACK_SIZE 1000
#define THREAD_ACCEL_PRIORITY -1

#define kChannelNumber 3

// tensor of 128 values * 3 inputs
#define BUFLEN 128 * 3
extern float accel_fifo_buf[BUFLEN];
extern int accel_last_value[3];

int SetupAccelerometer();
void ReadAccelerometer();

#ifdef __cplusplus
}
#endif

#endif /* TENSORFLOW_LITE_MICRO_EXAMPLES_MAGIC_WAND_ACCELEROMETER_HANDLER_H_ */
