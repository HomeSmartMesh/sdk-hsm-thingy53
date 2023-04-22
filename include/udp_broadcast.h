
#ifndef __UDP_BROADCAST_H__
#define __UDP_BROADCAST_H__

#include <stdint.h>
#include <string>

int send_udp_buffer(uint8_t * data, uint8_t size);
int send_udp(std::string &data);

#endif /*__UDP_BROADCAST_H__*/
