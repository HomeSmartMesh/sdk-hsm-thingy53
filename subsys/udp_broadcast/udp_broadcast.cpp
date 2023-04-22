//zephyr\samples\net\sockets\echo_client\src\udp.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <stdio.h>

#include "udp_broadcast.h"

LOG_MODULE_REGISTER(udp_broadcast, LOG_LEVEL_INF);

static bool udp_bcast_started = false;

#define CONFIG_PEER_PORT 4141
#if !defined(CONFIG_NET_CONFIG_PEER_IPV6_ADDR)
#define CONFIG_NET_CONFIG_PEER_IPV6_ADDR "ff03::1"
#endif
#define INVALID_SOCK (-1)

int sock = INVALID_SOCK;

int start_bcast_udp(void)
{
	int ret = 0;
	struct sockaddr_in6 addr6;

	LOG_INF("start_bcast_udp() CONFIG_NET_IPV6");
	addr6.sin6_family = AF_INET6;
	addr6.sin6_port = htons(CONFIG_PEER_PORT);
	inet_pton(AF_INET6, CONFIG_NET_CONFIG_PEER_IPV6_ADDR,&addr6.sin6_addr);

	sock = socket(addr6.sin6_family, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOG_ERR("Failed to create UDP socket : %d", errno);
		return -errno;
	}

	/* Call connect so we can use send and recv. */
	ret = connect(sock, (struct sockaddr *)&addr6, sizeof(addr6));
	if (ret < 0) {
		LOG_ERR("Cannot connect to UDP remote : %d", errno);
		ret = -errno;
	}

	return ret;
}


int send_udp_buffer(uint8_t * data, uint8_t size)
{
    if(!udp_bcast_started){
        start_bcast_udp();
	    udp_bcast_started = true;
    }
	int ret = send(sock, data, size, 0);
	if(ret < 0){
		LOG_ERR("could not send : %d", ret);
	}
	LOG_DBG("UDP: Sent %d bytes", size);
	return ret < 0 ? -EIO : 0;
}

int send_udp(std::string &data){
    if(!udp_bcast_started){
        start_bcast_udp();
	    udp_bcast_started = true;
    }
	int ret = send(sock, data.c_str(), data.length(), 0);
	if(ret < 0){
		LOG_ERR("could not send : %d", ret);
	}
	LOG_DBG("UDP: Sent %d bytes", data.length());
	return ret < 0 ? -EIO : 0;
}
