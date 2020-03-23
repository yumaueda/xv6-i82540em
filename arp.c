#include "types.h"
#include "defs.h"
#include "ether.h"
#include "net.h"
#include "arp.h"


static inline void arp_ether_ipv4_set_type(struct arp_frame *arp)
{
    arp->htype = ARP_HTYPE_ETHER;
    arp->ptype = ARP_PTYPE_IPV4;
}


static inline void arp_ether_ipv4_set_len(struct arp_frame *arp)
{
    arp->hlen = ARP_HLEN_LEN;
    arp->plen = ARP_PLEN_IPV4;
}


void gen_ether_garp(struct net *inet, struct arp_frame *arp)
{
    arp_ether_ipv4_set_type(arp);
    arp_ether_ipv4_set_len(arp);
    arp->oper = ARP_OPER_REQUEST;
    memmove(arp->sha, inet->addr, ETHER_ADDR_LEN);
    arp->spa = inet->ipv4_addr;
    memset(arp->tha, 0, sizeof(arp->tha));
    arp->tpa = inet->ipv4_addr;
}


int arpinit(void)
{
    struct net *inet;
    struct ether_hdr ehdr;
    struct arp_frame arp;
    void *frame;
    uint32_t frame_len;

    inet = get_inet();

    memmove(ehdr.src, inet->addr, ETHER_ADDR_LEN);
    memmove(ehdr.dst, ETHER_ADDR_BROADCAST, ETHER_ADDR_LEN);
    ehdr.type = ETHER_TYPE_ARP;

    memset(&arp, 0, sizeof(struct arp_frame));
    gen_ether_garp(inet, &arp);

    frame = kalloc();
    frame_len = gen_frame(frame, &ehdr, &arp, sizeof(arp));

    sendframe(inet, frame, frame_len);
    kfree(frame);

    return 0;
}
