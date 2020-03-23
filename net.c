#include "types.h"
#include "defs.h"
#include "net.h"
#include "i82540em.h"


static struct net *p_inet;


struct net *get_inet(void)
{
    return p_inet;
}


void register_inet(struct net *inet)
{
    cprintf("inet: registered: %x:%x:%x:%x:%x:%x\n",
        inet->addr[0],
        inet->addr[1],
        inet->addr[2],
        inet->addr[3],
        inet->addr[4],
        inet->addr[5]
    );

    p_inet = inet;
}


int set_ipv4_addr(uint32_t ipv4_addr)
{
    // TODO: send probe before assigning an IPv4 address.
    p_inet->ipv4_addr = ipv4_addr;
    return 0;
}


int netinit(void)
{
    uint32_t ipv4_addr = 0x1000020f;
    set_ipv4_addr(ipv4_addr);
    arpinit();
    return 0;
}
