/**
 * @brief Config Net library for Zephyr project.
 * @defgroup Config Net library 
 */

/**
 * @file config_network.h
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief This library handles the Net (ethernet or WIFI) connection.
 * @version 2.0
 * @date 2022-01-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * @addtogroup ConfigNetLib
 * @{
 */

#define CONFIG_NET_WIFI_LENGTH 100
extern char WIFI_SSID[100];
extern char WIFI_PASSWORD[100];

#ifndef __CONFIG_NET_H__
#define __CONFIG_NET_H__

void init_network(struct net_if *iface);

/**
 * @}
 */
#endif /*__CONFIG_NET_H__*/
