/**
 * @file config_network.c
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief This library handles the Net (ethernet or WIFI) connection.
 * @version 1.0
 * @date 2022-01-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * @addtogroup jeroagulloNetLib
 * @{
 */
#include <errno.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_mgmt.h>

#ifdef CONFIG_WIFI
#include <zephyr/net/wifi.h>
#include <zephyr/net/wifi_mgmt.h>
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(config_network, LOG_LEVEL_DBG);

#include "config_network.h"

static struct k_sem net_cb_sem;
static struct net_mgmt_event_callback mgmt_cb;

static void handler(struct net_mgmt_event_callback *cb,
		    uint32_t mgmt_event,
		    struct net_if *iface)
{
	int i = 0;

	if (mgmt_event != NET_EVENT_IPV4_ADDR_ADD) {
		return;
	}

	for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
		char buf[NET_IPV4_ADDR_LEN];

		if (iface->config.ip.ipv4->unicast[i].ipv4.addr_type !=
							NET_ADDR_DHCP) {
			continue;
		}

		LOG_INF("Your address: %s",
			net_addr_ntop(AF_INET,
			    &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
						  buf, sizeof(buf)));
		LOG_INF("Lease time: %u seconds",
			 iface->config.dhcpv4.lease_time);
		LOG_INF("Subnet: %s",
			net_addr_ntop(AF_INET,
				       &iface->config.ip.ipv4->unicast[i].netmask,
				       buf, sizeof(buf)));
		LOG_INF("Router: %s",
			net_addr_ntop(AF_INET,
						 &iface->config.ip.ipv4->gw,
						 buf, sizeof(buf)));
	}

	// Connected
	LOG_INF("Connection successed");
	k_sem_give(&net_cb_sem);
}


int init_network(struct net_if *iface){
	k_sem_init(&net_cb_sem, 0, 1);

	LOG_INF("STARTING NET...");

	LOG_INF("wait until the iface is initialized");
	k_msleep(5 * MSEC_PER_SEC);

	iface = net_if_get_default();

	if(iface == NULL){
		LOG_ERR("iface is NULL");
	}

	net_mgmt_init_event_callback(&mgmt_cb, handler,
				     NET_EVENT_IPV4_ADDR_ADD);

	net_mgmt_add_event_callback(&mgmt_cb);

	// start dhcp
	net_dhcpv4_start(iface);

#ifdef CONFIG_WIFI
	static struct wifi_connect_req_params wifi_conn_params;

    // Wifi parameters
	wifi_conn_params.ssid = WIFI_SSID;
	wifi_conn_params.ssid_length = strlen(WIFI_SSID);
	wifi_conn_params.psk  = WIFI_PASSWORD;
	wifi_conn_params.psk_length  = strlen(WIFI_PASSWORD);
	wifi_conn_params.channel = WIFI_CHANNEL_ANY;
	wifi_conn_params.security = WIFI_SECURITY_TYPE_PSK ;

	if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface,
			&wifi_conn_params, sizeof(struct wifi_connect_req_params))) {
		LOG_ERR("Connection failed");
		return false;
	} else {
		LOG_INF("Connection WIFI");
	}
#endif

	LOG_INF("Connection requested...");
	// Wait for connection during 20 seconds....
	int ret = k_sem_take(&net_cb_sem, K_SECONDS(30));

	if(ret == EAGAIN)
		LOG_ERR("Connection failed due to timeout");

	return ret;
}

/**
 * @}
 */