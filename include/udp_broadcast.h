
#ifndef __UDP_BROADCAST_H__
#define __UDP_BROADCAST_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int send_udp(uint8_t * data, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /*__UDP_BROADCAST_H__*/
