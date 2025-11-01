#include "jeroagullo_mqtt.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(jeroagullo_mqtt, LOG_LEVEL_DBG);

// publish thread variables
K_CONDVAR_DEFINE(mqtt_publish_condvar);
K_MUTEX_DEFINE(mqtt_publish_mutex);
k_tid_t thread_mqtt_publish_id;
char mqtt_publish_message[MQTT_PUBLISH_MESSAGE_LENGTH];

K_THREAD_STACK_DEFINE(thread_pring_stack_area, THREAD_PING_STACK_SIZE);
struct k_thread thread_ping_data;

K_THREAD_STACK_DEFINE(thread_publish_stack_area, THREAD_PUBLISH_STACK_SIZE);
struct k_thread thread_publish_data;

/* The mqtt client struct */
static struct mqtt_client client;

bool connected;

/* MQTT Broker details. */
static struct sockaddr_storage broker;

static struct zsock_pollfd fds[1];
static int nfds;

/* Buffers for MQTT client. */
static uint8_t rx_buffer[1024];
static uint8_t tx_buffer[1024];

#if defined(CONFIG_DNS_RESOLVER)
static struct zsock_addrinfo hints;
static struct zsock_addrinfo *haddr;
#endif

void prepare_fds()
{
	if (client.transport.type == MQTT_TRANSPORT_NON_SECURE) {
		fds[0].fd = client.transport.tcp.sock;
	}

	fds[0].events = ZSOCK_POLLIN;
	nfds = 1;
}

void clear_fds(void)
{
	nfds = 0;
}

int wait(int timeout)
{
	int ret = 0;

	if (nfds > 0) {
		ret = zsock_poll(fds, nfds, timeout);
		if (ret < 0) {
			LOG_ERR("poll error: %d", errno);
		}
	}

	return ret;
}

void mqtt_evt_handler(struct mqtt_client *const client,
        const struct mqtt_evt *evt)
{
	int err;

	switch (evt->type) {
	case MQTT_EVT_CONNACK:
		if (evt->result != 0) {
			LOG_ERR("MQTT connect failed %d", evt->result);
			break;
		}

		connected = true;
		LOG_INF("MQTT client connected!");

		break;

	case MQTT_EVT_DISCONNECT:
		LOG_INF("MQTT client disconnected %d", evt->result);

		connected = false;
		clear_fds();

		// in case of disconnection, try to connect again
		mqtt_try_to_connect();

		break;

	case MQTT_EVT_PUBACK:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBACK error %d", evt->result);
			break;
		}

		LOG_INF("PUBACK packet id: %u", evt->param.puback.message_id);

		break;

	case MQTT_EVT_PUBREC:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBREC error %d", evt->result);
			break;
		}

		LOG_INF("PUBREC packet id: %u", evt->param.pubrec.message_id);

		const struct mqtt_pubrel_param rel_param = {
			.message_id = evt->param.pubrec.message_id
		};

		err = mqtt_publish_qos2_release(client, &rel_param);
		if (err != 0) {
			LOG_ERR("Failed to send MQTT PUBREL: %d", err);
		}

		break;

	case MQTT_EVT_PUBCOMP:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBCOMP error %d", evt->result);
			break;
		}

		LOG_INF("PUBCOMP packet id: %u",
			evt->param.pubcomp.message_id);

		break;

	case MQTT_EVT_PINGRESP:
		LOG_INF("PINGRESP packet");
		break;

	default:
		break;
	}
}

int publish(enum mqtt_qos qos)
{
	struct mqtt_publish_param param;

	param.message.topic.qos = qos;
	param.message.topic.topic.utf8 = CONFIG_MQTT_TOPIC;
	param.message.topic.topic.size =
			strlen(param.message.topic.topic.utf8);
	param.message.payload.data = mqtt_publish_message;
	param.message.payload.len =
			strlen(param.message.payload.data);
	param.message_id = sys_rand32_get();
	param.dup_flag = 0U;
	param.retain_flag = 0U;

	return mqtt_publish(&client, &param);
}

#define RC_STR(rc) ((rc) == 0 ? "OK" : "ERROR")

#define PRINT_RESULT(func, rc) \
	LOG_INF("%s: %d <%s>", (func), rc, RC_STR(rc))

void broker_init(void)
{
	struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;

	broker4->sin_family = AF_INET;
	broker4->sin_port = htons(CONFIG_MQTT_SERVER_PORT);
#if defined(CONFIG_DNS_RESOLVER)
	inet_pton(AF_INET, CONFIG_MQTT_SERVER_ADDR, &broker4->sin_addr);
#else
    zsock_inet_pton(AF_INET, CONFIG_MQTT_SERVER_ADDR, &broker4->sin_addr);
#endif
}

void client_init()
{
	mqtt_client_init(&client);

	broker_init();

	/* Username and password for mqtt authentication */
#if defined(CONFIG_MQTT_USE_AUTH)
	struct mqtt_utf8 userName = MQTT_UTF8_LITERAL(CONFIG_MQTT_USER_NAME);
	struct mqtt_utf8 userPassword = MQTT_UTF8_LITERAL(CONFIG_MQTT_PASSWORD);
#endif

	/* MQTT client configuration */
	client.broker = &broker;
	client.evt_cb = mqtt_evt_handler;
	client.client_id.utf8 = (uint8_t *)CONFIG_MQTT_CLIENTID;
	client.client_id.size = strlen(CONFIG_MQTT_CLIENTID);

#if defined(CONFIG_MQTT_USE_AUTH)
	client.user_name = &userName;
	client.password = &userPassword;
#else
	client.user_name = NULL;
	client.password = NULL;
#endif

	client.protocol_version = MQTT_VERSION_3_1_1;

	/* MQTT buffers configuration */
	client.rx_buf = rx_buffer;
	client.rx_buf_size = sizeof(rx_buffer);
	client.tx_buf = tx_buffer;
	client.tx_buf_size = sizeof(tx_buffer);
	client.transport.type = MQTT_TRANSPORT_NON_SECURE;

}

int process_mqtt_and_sleep(int timeout)
{
	int64_t remaining = timeout;
	int64_t start_time = k_uptime_get();
	int rc;

	while (remaining > 0 && connected) {
		if (wait(remaining)) {
			rc = mqtt_input(&client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		rc = mqtt_live(&client);
		if (rc != 0 && rc != -EAGAIN) {
			PRINT_RESULT("mqtt_live", rc);
			return rc;
		} else if (rc == 0) {
			rc = mqtt_input(&client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		remaining = timeout + start_time - k_uptime_get();
	}

	return 0;
}

void ping_thread(){
	int ret = 0;

	while(1){
		LOG_INF("mqtt ping socket: %d", client.transport.tcp.sock);
		ret = mqtt_ping(&client);
		LOG_INF("mqtt_ping %d", ret);
		SUCCESS_OR_BREAK(ret);

		ret = process_mqtt_and_sleep(APP_PING_MSECS);
		SUCCESS_OR_BREAK(ret);
	}
}

void publish_thread(){
	int ret = 0;
	LOG_DBG("Starting mqtt publish thread");

	while(1){
		
		//k_mutex_lock(&mqtt_publish_mutex, K_FOREVER);
		k_condvar_wait(&mqtt_publish_condvar, &mqtt_publish_mutex, K_FOREVER);

		ret = publish(MQTT_QOS_0_AT_MOST_ONCE);
		SUCCESS_OR_BREAK(ret);

		k_mutex_unlock(&mqtt_publish_mutex);
	}
}

/* In this routine we block until the connected variable is 1 */
int mqtt_try_to_connect()
{
	int rc, i = 0;

	while ((i < APP_CONNECT_TRIES) && !connected) {

		client_init(&client);

		rc = mqtt_connect(&client);
		if (rc != 0) {
			PRINT_RESULT("mqtt_connect", rc);
			k_sleep(K_MSEC(1 * MSEC_PER_SEC));
			i++;
			continue;
		}

		prepare_fds(client);

		if (wait(APP_CONNECT_TIMEOUT_MS)) {
			mqtt_input(&client);
		}

		if (!connected) {
			mqtt_abort(&client);
		}
	}

	if (connected) {
		// If connected creates the ping and publish threads

		LOG_INF("MQTT connection success. MQTT socket: %d", client.transport.tcp.sock);

		k_tid_t thread_ping_id = k_thread_create(&thread_ping_data, thread_pring_stack_area,
                                 K_THREAD_STACK_SIZEOF(thread_pring_stack_area),
                                 ping_thread,
                                 NULL, NULL, NULL,
                                 THREAD_PING_PRIORITY, 0, K_NO_WAIT);

		thread_mqtt_publish_id = k_thread_create(&thread_publish_data, thread_publish_stack_area,
                                 K_THREAD_STACK_SIZEOF(thread_publish_stack_area),
                                 publish_thread,
                                 NULL, NULL, NULL,
                                 THREAD_PUBLISH_PRIORITY, 0, K_NO_WAIT);

		k_thread_start(thread_ping_id);
		k_thread_start(thread_mqtt_publish_id);

		return 0;
	}

	return -EINVAL;
}


int disconnect(){
    return mqtt_disconnect(&client);
}

int ping(){
    return mqtt_ping(&client);
}

#if defined(CONFIG_DNS_RESOLVER)
int get_mqtt_broker_addrinfo(void)
{
	int retries = 3;
	int rc = -EINVAL;

	while (retries--) {
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = 0;

		rc = zsock_getaddrinfo(SERVER_HOSTNAME, "8883",
				       &hints, &haddr);
		if (rc == 0) {
			LOG_INF("DNS resolved for %s:%d", SERVER_HOSTNAME, CONFIG_MQTT_SERVER_PORT);
			return 0;
		}

		LOG_ERR("DNS not resolved for %s:%d, retrying", SERVER_HOSTNAME, CONFIG_MQTT_SERVER_PORT);
	}

	return rc;
}
#endif

