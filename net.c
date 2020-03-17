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