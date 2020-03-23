#ifndef __ARP__
#define __ARP__ 1


#define ARP_HTYPE_ETHER  0x0100
#define ARP_PTYPE_IPV4   0x0008
#define ARP_HLEN_LEN     0x6
#define ARP_PLEN_IPV4    0x4
#define ARP_OPER_REQUEST 0x0100
#define ARP_OPER_REPLY   0x0200
#define ATE_MAX_NUM      0x100


struct arp_frame {
    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t oper;
    uint8_t  sha[ARP_HLEN_LEN];
    uint32_t spa;
    uint8_t  tha[ARP_HLEN_LEN];
    uint32_t tpa;
};


struct arp_table_entry {
    uint8_t  tha[ARP_HLEN_LEN];
    uint32_t tpa;
};


struct arp_table {
    struct arp_table_entry ate[ATE_MAX_NUM];
};


#endif
