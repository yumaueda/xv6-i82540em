#include "types.h"
#include "defs.h"
#include "ether.h"
#include "net.h"
#include "i82540em.h"


uint32_t sendframe(struct net *inet, void *p_packet, uint32_t packet_size)
{
    if (packet_size > ETHER_DATA_MAX_LEN || !p_packet)
        return 0;

    if (packet_size < ETHER_DATA_MIN_LEN)
        return 0;

    uint32_t (*send)(struct net *inet, void *p_packet, uint32_t packet_size) = inet->send;
    send(inet, p_packet, packet_size);
    return packet_size;
}


uint32_t gen_frame(void *dst, struct ether_hdr *ehdr, void *p_data, uint32_t data_len)
{
    if (data_len > ETHER_DATA_MAX_LEN || !p_data)
        return 0;

    uint8_t frame[ETHER_FRAME_MAX_LEN];
    memset(frame, 0, sizeof(frame));
    struct ether_hdr *hdr = (struct ether_hdr *)frame;
    void *data = hdr + 1;

    memmove((void *)&hdr->dst, (void *)&ehdr->dst, ETHER_ADDR_LEN);
    memmove((void *)&hdr->src, (void *)&ehdr->src, ETHER_ADDR_LEN);
    memmove((void *)&hdr->type, (void *)&ehdr->type, ETHER_TYPE_LEN);
    memmove((void *)data, (void *)p_data, data_len);

    if (data_len < ETHER_DATA_MIN_LEN) {
        memset(data+data_len, 0, ETHER_DATA_MIN_LEN-data_len); // needs padding!
        memmove(dst, (void *)frame, ETHER_FRAME_MIN_LEN);
        return (uint32_t)ETHER_FRAME_MIN_LEN;
    }
    else {
        memmove(dst, (void *)frame, ETHER_HDR_LEN + data_len);
        return (uint32_t)(ETHER_HDR_LEN + data_len);
    }
}
