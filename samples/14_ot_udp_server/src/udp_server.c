//zephyr\samples\net\sockets\echo_client\src\udp.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <errno.h>
#include <stdio.h>

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

bool udp_rx_receive(ssize_t* received){

	struct sockaddr client_addr;
	//*received = recv(sock, recv_buf, sizeof(recv_buf), MSG_WAITALL);
	socklen_t client_addr_len = sizeof(client_addr);
	*received = recvfrom(sock, recv_buf,sizeof(recv_buf), 0,&client_addr, &client_addr_len);

	//ret = sendto(data->udp.sock, data->udp.recv_buffer, received, 0,
	//			&client_addr, client_addr_len);
	//if (ret < 0) {
	//	NET_ERR("UDP: Failed to send %d", errno);
	//	ret = -errno;
	//	break;
	//}


	if (*received < 0) {
		NET_ERR("UDP: Connection error %d", errno);
		return false;
	}
	else{
		return true;
	}
}

void udp_rx_handler(){
	ssize_t nb_rec;

	LOG_INF("hello from udp_rx_handler()");
    if(!udp_started){//could be started by higher prio thread that wants to send
        start_udp();
	    udp_started = true;
    }

	while(true){

		LOG_INF("waiting for udp recv()");
		if(udp_rx_receive(&nb_rec)){
			LOG_INF("received %d characters",nb_rec);
		}

		LOG_INF("udp rx sleeping 10 sec");
		k_sleep(K_MSEC(10000));
	}
}
