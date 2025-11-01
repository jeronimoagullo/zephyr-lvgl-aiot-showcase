
#ifndef JEROAGULLO_MQTT_H
#define JEROAGULLO_MQTT_H

/* Expose a C friendly interface for main functions. */
#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/random/random.h>
#include <stdio.h>

#define THREAD_PING_STACK_SIZE 1000
#define THREAD_PING_PRIORITY 3

#define THREAD_PUBLISH_STACK_SIZE 1000
#define THREAD_PUBLISH_PRIORITY 2

extern struct k_condvar mqtt_publish_condvar;
extern struct k_mutex mqtt_publish_mutex;
extern k_tid_t thread_mqtt_publish_id;

#define MQTT_PUBLISH_MESSAGE_LENGTH 50
extern char mqtt_publish_message[MQTT_PUBLISH_MESSAGE_LENGTH];


/* The SERVER_HOSTNAME must match the Common Name (CN) of the server ca.crt */
#define SERVER_HOSTNAME         	CONFIG_MQTT_SERVER_ADDR
/* Port of the server. 1883 for no TLS*/
#define CONFIG_MQTT_SERVER_PORT                     1883

/* configuration setting of mqtt protocol */
#define APP_CONNECT_TIMEOUT_MS		2000
#define APP_PING_MSECS			CONFIG_MQTT_KEEPALIVE * MSEC_PER_SEC
#define APP_CONNECT_TRIES		3

/* The mqtt client struct */
extern bool connected;

// macros
#define SUCCESS_OR_EXIT(rc) { if (rc != 0) { return 1; } }
#define SUCCESS_OR_BREAK(rc) { if (rc != 0) { break; } }

// Function definition
void prepare_fds();
void clear_fds(void);
int wait(int timeout);
void mqtt_evt_handler(struct mqtt_client *const client,
        const struct mqtt_evt *evt);
int publish(enum mqtt_qos qos);
void client_init();
int mqtt_try_to_connect();
int process_mqtt_and_sleep(int timeout);
int ping();
int disconnect();
#if defined(CONFIG_DNS_RESOLVER)
int get_mqtt_broker_addrinfo(void);
#endif


#ifdef __cplusplus
}
#endif

#endif //JEROAGULLO_MQTT_H