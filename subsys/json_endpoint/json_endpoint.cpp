//zephyr\samples\net\sockets\echo_client\src\udp.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <errno.h>
#include <stdio.h>

#include <string>

#include "json_endpoint.h"

LOG_MODULE_REGISTER(udp_client, LOG_LEVEL_INF);

#define CONFIG_PEER_PORT 4242
#define INVALID_SOCK (-1)

bool udp_started = false;
int sock = INVALID_SOCK;

#define UDP_RX_STACK_SIZE 8192
#define UDP_RX_PRIORITY 20
#define UDP_RX_START_DELAY_MS 1000

void udp_rx_handler();

K_THREAD_DEFINE(	udp_rx_thread, UDP_RX_STACK_SIZE, udp_rx_handler, NULL, NULL, NULL,
					UDP_RX_PRIORITY, 0, UDP_RX_START_DELAY_MS);

#define RECV_BUF_SIZE 1280
static char recv_buf[RECV_BUF_SIZE];

static json_endpoint_handler_t m_json_endpoint_handler = NULL;
json request;
json response;

bool safe_json_topic_paload_parse(std::string &payload, std::string &topic, json &request, std::string &error){
	size_t json_begin = payload.find("{");
	if(json_begin == std::string::npos){
		error = "topic::parse error no json map start found '{'";
		LOG_ERR("%s",error.c_str());
		return false;
	}
	topic = payload.substr(0,json_begin);
	std::string json_body = payload.substr(json_begin);
	try{
		request = json::parse(json_body);
	}catch(json::parse_error& ex){
		error = "json::parse threw an exception at byte " + std::to_string(ex.byte);
		LOG_ERR("%s",error.c_str());
		return false;
	}
	return true;
}

int start_udp(void)
{
	LOG_INF("start_udp() CONFIG_NET_IPV6");

	int ret = 0;
	struct sockaddr_in6 addr6;
	(void)memset(&addr6, 0, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_port = htons(CONFIG_PEER_PORT);

	sock = socket(addr6.sin6_family, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOG_ERR("Failed to create UDP socket : %d", errno);
		return -errno;
	}

	ret = bind(sock, (struct sockaddr *)&addr6, sizeof(addr6));
	if (ret < 0) {
		NET_ERR("Failed to bind UDP socket: ret= %d ; errno = %d",ret, errno);
		ret = -errno;
	}

	return ret;
}


int send_udp(uint8_t * data, uint8_t size)
{
	int ret;
    if(!udp_started){
        start_udp();
	    udp_started = true;
    }

	ret = send(sock, data, size, 0);

	LOG_DBG("UDP: Sent %d bytes", size);

	return ret < 0 ? -EIO : 0;

}

void udp_rx_handler(){

	LOG_INF("hello from udp_rx_handler()");
    if(!udp_started){//could be started by higher prio thread that wants to send
        start_udp();
	    udp_started = true;
    }

	while(true){
		LOG_INF("waiting for udp recv()");
		struct sockaddr client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		ssize_t received = recvfrom(sock, recv_buf,sizeof(recv_buf), 0,&client_addr, &client_addr_len);
		if (received < 0) {
			NET_ERR("UDP: Connection error %d", errno);
		}else{
			recv_buf[received] = '\0';
			LOG_INF("udp_rx_handler> received %d characters: %s\n",received,recv_buf);
		}

		if(m_json_endpoint_handler != NULL){
			ssize_t ret;
			std::string payload(recv_buf,received);
			std::string client,topic,error;
			bool valid = safe_json_topic_paload_parse(payload, topic, request, error);
			if(valid){
				response = "{}"_json;
				m_json_endpoint_handler(client, topic, request, response);
				std::string resp_text = topic+response.dump();
				ret = sendto(sock, resp_text.c_str(), resp_text.length(), 0, &client_addr, client_addr_len);
			}else{
				ret = sendto(sock, error.c_str(), error.length(), 0, &client_addr, client_addr_len);
			}
			if (ret < 0) {
				NET_ERR("UDP: Failed to send %d", errno);
				ret = -errno;
			}
		}


	}
}

void set_endpoint_handler(json_endpoint_handler_t handler)
{
	m_json_endpoint_handler = handler;
}
