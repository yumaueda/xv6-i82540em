#ifndef __I82540EM__
#define __I82540EM__ 1


//-----------------
// Register offset
//-----------------
#define CTRL_8254X      0x0
#define EERD_8254X      0x14
#define ICR_8254X       0xC0
#define IMS_8254X       0xD0
#define IMC_8254X       0xD8
#define RCTL_8254X      0x100
#define TCTL_8254X      0x400
#define RDBAL_8254X     0x2800
#define RDBAH_8254X     0x2804
#define RDLEN_8254X     0x2808
#define RDH_8254X       0x2810
#define RDT_8254X       0x2818
#define TDBAL_8254X     0x3800
#define TDBAH_8254X     0x3804
#define TDLEN_8254X     0x3808
#define TDH_8254X       0x3810
#define TDT_8254X       0x3818
#define MTA_8254X       0x5200

//------
// CTRL
//------
#define CTRL_SLU        0x40

//------
// EERD
//------
#define EERD_ADDR_SHIFT 0x8
#define EERD_DATA_SHIFT 0x10
#define EERD_START      0x1
#define EERD_DONE       0x10

//-----------------
// ICR / IMS / IMC
//-----------------
#define ICR_TXQE        0x2
#define IMS_TXQE        ICR_TXQE
#define IMC_TXQE        ICR_TXQE
#define ICR_RXT0        0x80
#define IMS_RXT0        ICR_RXT0

//------
// RCTL
//------
#define RCTL_EN         0x2
#define RCTL_SBP        0x4
#define RCTL_UPE        0x8
#define RCTL_MPE        0x10
#define RCTL_LPE        0x20
#define RCTL_RDMTS_HALF 0x0
#define RCTL_BAM        0x8000
#define RCTL_BSIZE_2048 0x0
#define RCTL_SECRC      0x4000000

//------
// TCTL
//------
#define TCTL_EN         0x2
#define TCTL_PSP        0x8

//-----
// MTA
//-----
#define MTA_MAX         0x80

#define RX_RING_SIZE    0x10
#define TX_RING_SIZE    0x10
#define RX_BUFF_SIZE    0x800

//-------
// TDESC
//-------
#define TDESC_EOP       0x1
#define TDESC_RS        0x8


struct rx_descriptor{
    uint64_t buffer_addr;
    uint16_t length;
    uint16_t csum;
    uint8_t status;
    uint8_t errors;
    uint8_t special;
};


struct tx_descriptor{
    uint64_t buffer_addr;
    uint16_t length;
    uint8_t cso;
    uint8_t cmd;
    uint8_t status;
    uint8_t css;
    uint16_t special;
};


struct i82540em {
    uint32_t mmio_base_addr;
    uint32_t irq_line;
    uint8_t addr[6];
    struct rx_descriptor rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));;
    struct tx_descriptor tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));;
    struct net *inet;
};


#endif