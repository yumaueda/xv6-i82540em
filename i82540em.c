#include "types.h"
#include "defs.h"
#include "ether.h"
#include "x86.h"
#include "i82540em.h"
#include "net.h"
#include "pci.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"


static uint32_t read_reg(struct i82540em *nic, uint16_t offset)
{
    return *(volatile uint32_t *)(nic->mmio_base_addr + offset);
}


static void write_reg(struct i82540em *nic, uint16_t offset, uint32_t v)
{
    *(volatile uint32_t *)(nic->mmio_base_addr + offset) = v;
}


void i82540em_soft_irq(void)
{
    cprintf("i82540em: interrupt\n");
}


static uint32_t tx(struct net *inet, void *p_packet, uint32_t packet_size)
{
    struct i82540em *nic = (struct i82540em *)inet->nic;
    uint32_t tail = read_reg(nic, TDT_8254X);
    struct tx_descriptor *td = &nic->tx_ring[tail];

    // send a packet
    td->buffer_addr = (uint64_t)V2P(p_packet);
    td->length = packet_size;
    td->status = 0;
    td->cmd = (
        TDESC_EOP |
        TDESC_RS  |
        0
    );

    // make tail points one beyond the last valid descriptor
    write_reg(nic, TDT_8254X, (tail + 1) % TX_RING_SIZE);

    // poll the status field
    while (!(td->status & 0xF))
        microdelay(1);

    return packet_size;
}


static void tx_init(struct i82540em *nic)
{
    // allocate region of memory for TX ring
    for (int i = 0; i < TX_RING_SIZE; i++) {
        memset(&nic->tx_ring[i], 0, sizeof(struct tx_descriptor));
    }
    uint64_t base = (uint64_t)V2P(nic->tx_ring);
    // set TDBAL/TDBAH
    write_reg(nic, TDBAL_8254X, (uint32_t)(base & 0xFFFFFFFF));
    write_reg(nic, TDBAH_8254X, (uint32_t)(base >> 32));
    // set TDLEN
    write_reg(nic, TDLEN_8254X, (uint32_t)(sizeof(struct tx_descriptor) * TX_RING_SIZE));
    // set TX head and tail
    write_reg(nic, TDH_8254X, 0);
    write_reg(nic, TDT_8254X, TX_RING_SIZE-1);
    // set TX control regsiter
    write_reg(nic, TCTL_8254X,
        TCTL_EN  |
        TCTL_PSP |
        0
    );
}


static void rx_init(struct i82540em *nic) {
    for (int i = 0; i < MTA_MAX; i++) // clear MTA
        write_reg(nic, MTA_8254X, 0);

    write_reg(nic, IMS_8254X, IMS_RXT0);
    read_reg(nic, ICR_8254X); // clear interrupts

    // allocate region of memory for RX ring
    for (int i = 0; i < RX_RING_SIZE; i++) {
        memset(&nic->rx_ring[i], 0, sizeof(struct rx_descriptor));
        nic->rx_ring[i].buffer_addr = (uint64_t)V2P(kalloc());
    }

    // set RDBAL/RDBAH
    uint64_t base = (uint64_t)V2P(nic->rx_ring);
    write_reg(nic, RDBAL_8254X, (uint32_t)(base & 0xFFFFFFFF));
    write_reg(nic, RDBAH_8254X, (uint32_t)(base >> 32));
    // set RDLEN
    write_reg(nic, RDLEN_8254X, (uint32_t)(sizeof(struct rx_descriptor) * RX_RING_SIZE));
    // set RX head and tail
    write_reg(nic, RDH_8254X, 0);
    write_reg(nic, RDT_8254X, RX_RING_SIZE-1);
    // set RX control register
    write_reg(nic, RCTL_8254X,
        RCTL_EN         |
        RCTL_SBP        |
        RCTL_UPE        |
        RCTL_MPE        |
        RCTL_LPE        |
        RCTL_RDMTS_HALF |
        RCTL_BAM        |
        RCTL_BSIZE_2048 |
        RCTL_SECRC      |
        0
    );
}


static uint16_t read_eeprom(struct i82540em *nic, uint8_t eerd_addr)
{
    uint32_t eerd;
    write_reg(nic, EERD_8254X, EERD_START | eerd_addr << EERD_ADDR_SHIFT);
    while(((eerd = read_reg(nic, EERD_8254X)) & EERD_DONE) == 0)
        microdelay(1);
    return (uint16_t)(eerd >> EERD_DATA_SHIFT);
}


static void read_addr(struct i82540em *nic)
{
    uint16_t data;
    for (int i = 0; i < 3; i++) {
        data = read_eeprom(nic, i);
        nic->addr[i*2+0] = data & 0xFF;
        nic->addr[i*2+1] = (data >> 0x8) & 0xFF;
    }
}


static void get_mmio_base_addr(struct pci_func *func, struct i82540em *nic)
{
    for (int i = 0; i < BAR_NUM_MAX; i++) {
        if (func->reg_base[i] > 0xFFFF) {
            nic->mmio_base_addr = func->reg_base[i];
            break;
        }
    }
}


int i82540em_init(struct pci_func *func)
{
    func_enable(func);

    struct i82540em *nic_i82540em = (struct i82540em *)kalloc();
    nic_i82540em->irq_line = func->irq_line;

    // get MMIO base address
    get_mmio_base_addr(func, nic_i82540em);
    cprintf("82540em: MMIO base addr: 0x%x\n", nic_i82540em->mmio_base_addr);

    // read MAC address from EEPROM
    read_addr(nic_i82540em);
    cprintf("82540em: MAC addr: %x:%x:%x:%x:%x:%x\n", nic_i82540em->addr[0], nic_i82540em->addr[1], nic_i82540em->addr[2], nic_i82540em->addr[3], nic_i82540em->addr[4], nic_i82540em->addr[5]);

    ioapicenable(nic_i82540em->irq_line, ncpu - 1); // register i/o apic
    write_reg(nic_i82540em, CTRL_8254X, read_reg(nic_i82540em, CTRL_8254X) | CTRL_SLU); // link up

    // RX/TX init
    rx_init(nic_i82540em);
    tx_init(nic_i82540em);

    // register inet
    struct net *inet = (struct net *)kalloc();
    memset(inet, 0, sizeof(struct net));
    inet->nic = nic_i82540em;
    memmove(inet->addr, nic_i82540em->addr, ETHER_ADDR_LEN);
    inet->send = tx;
    nic_i82540em->inet = inet;
    register_inet(inet);


    // send a test frame
    struct net *n_inet;
    struct ether_hdr ehdr;
    uint32_t frame_len;
    char *data = "\x48\x65\x6C\x6C\x6F\x2C\x20\x57\x6F\x72\x6C\x64\x21"; // Hello, World!

    n_inet = get_inet();

    memmove(ehdr.src, n_inet->addr, ETHER_ADDR_LEN);
    memmove(ehdr.dst, ETHER_ADDR_BROADCAST, ETHER_ADDR_LEN);
    ehdr.type = ETHER_TYPE_ARP;

    void *frame = kalloc();
    frame_len = gen_frame(frame, &ehdr, data, 13);

    sendframe(n_inet, frame, frame_len);

    return 0;
}