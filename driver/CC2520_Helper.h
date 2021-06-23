#ifndef RPI3_CC2520_API_CC2520_HELPER_H
#define RPI3_CC2520_API_CC2520_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CC2520.h"

typedef struct {
    unsigned int dev_id;
    unsigned short dst_pan_id;
    unsigned short dst_addr;
    unsigned short src_pan_id;
    unsigned short src_addr;
} CC2520_addr_t;

void CC2520_Send(unsigned char *buf, unsigned int len, CC2520_addr_t* addr);

#ifdef __cplusplus
}
#endif

#endif //RPI3_CC2520_API_CC2520_HELPER_H
