#ifndef __ETHER__
#define __ETHER__ 1


#include "types.h"


#define ETHER_TYPE_LEN          2
#define ETHER_TYPE_EXPERIMENTAL 0x0101
#define ETHER_TYPE_IPV4         0x0800
#define ETHER_TYPE_ARP          0x0806
#define ETHER_TYPE_IPV6         0x86DD

#define ETHER_ADDR_LEN          6
#define ETHER_ADDR_BROADCAST    "\xFF\xFF\xFF\xFF\xFF\xFF"

#define ETHER_FRAME_MAX_LEN     1518
#define ETHER_FRAME_MIN_LEN     64
#define ETHER_HDR_LEN           14
#define ETHER_FCS_LEN           4
#define ETHER_DATA_MAX_LEN      (ETHER_FRAME_MAX_LEN - ETHER_HDR_LEN)
#define ETHER_DATA_MIN_LEN      (ETHER_FRAME_MIN_LEN - ETHER_HDR_LEN)


struct ether_hdr {
    uint8_t dst[ETHER_ADDR_LEN];
    uint8_t src[ETHER_ADDR_LEN];
    uint16_t type;
};


#endif