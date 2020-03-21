#include "types.h"
#include "user.h"
#include "ether.h"


int main(void)
{
    char *data = "\x48\x65\x6C\x6C\x6F\x2C\x20\x57\x6F\x72\x6C\x64\x21"; // Hello, World!
    uint8_t dst_addr[ETHER_ADDR_LEN];
    memmove(dst_addr, ETHER_ADDR_BROADCAST, ETHER_ADDR_LEN);
    if (sendframe(dst_addr, data, 13, ETHER_TYPE_ARP) < 0) {
        printf(1, "failed\n");
        exit();
    }

    exit();
}
