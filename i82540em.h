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

//------
// EERD
//------
#define EERD_ADDR_SHIFT 0x8
#define EERD_DATA_SHIFT 0x10
#define EERD_START      0x1
#define EERD_DONE       0x10

//-----
// ICR
//-----
#define ICR_RXT0        0x80


struct i82540em {
    uint32_t mmio_base_addr;
    uint8_t irq_line_num;
    uint8_t addr[6];
};


#endif