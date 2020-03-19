#include "types.h"
#include "defs.h"
#include "net.h"


int sys_sendframe(void)
{
    struct net *inet = 0;
    void *p_packet = 0;
    uint32_t packet_len = 0;

    if (arguint(2, &packet_len) < 0)
        return -1;
    if (argptr(1, p_packet, (int)packet_len) < 0)
        return -1;
    if (argptr(0, (void *)&inet, sizeof(*inet)) < 0)
        return -1;
    if (!sendframe(inet, p_packet, packet_len))
        return -1;

    return 0;
}