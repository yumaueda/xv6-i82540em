#include "types.h"
#include "defs.h"
#include "ether.h"
#include "net.h"


int sys_sendframe(void)
{
    struct net *inet = get_inet();
    struct ether_hdr ehdr;
    uint8_t *dst_addr;
    void *p_data;
    uint32_t data_len;
    uint16_t data_type;
    void *frame;
    uint32_t frame_len;


    if (argushort(3, &data_type) < 0)
        return -1;
    if (arguint(2, &data_len) < 0)
        return -1;
    if (argptr(1, (void *)&p_data, (int)data_len) < 0)
        return -1;
    if (argptr(0, (void *)&dst_addr, ETHER_ADDR_LEN) < 0)
        return -1;

    memmove(ehdr.src, inet->addr, ETHER_ADDR_LEN);
    memmove(ehdr.dst, dst_addr, ETHER_ADDR_LEN);
    ehdr.type = ETHER_TYPE_ARP;

    frame = kalloc();
    frame_len = gen_frame(frame, &ehdr, p_data, data_len);

    if (!sendframe(inet, frame, frame_len))
        return -1;

    kfree(frame);
    return 0;
}
