#ifndef __PCI__
#define __PCI__ 1


#include "types.h"


//-----------------
// I/O port number
//-----------------
#define PCI_CONF_ADDR_IOPORT      0xCF8
#define PCI_CONF_DATA_IOPORT      0xCFC

//----------------
// CONFIG_ADDRESS
//----------------
#define BUS_NUM_MAX               0x100
#define DEV_NUM_MAX               0x20
#define FUNC_NUM_MAX              0x8
#define ENABLE_BIT_SHIFT          0x1F
#define ENABLE_BIT                (1 << ENABLE_BIT_SHIFT)

//-----------------
// Register offset
//-----------------
#define VENDOR_ID                 0x0
#define COMMAND                   0x4
#define REV_ID                    0x8
#define CACHE_LINE_SIZE           0xC
#define BAR_START                 0x10
#define PRIM_BUS_NUM              0x18
#define BAR_END                   0x28
#define INTERRUPT_LINE            0x3C

//-----------
// Vendor ID
//-----------
#define VENDOR_ID_MASK            0xFFFF
#define GET_VENDOR_ID(vdid)       ((vdid) & VENDOR_ID_MASK)

#define VENDOR_ID_INTEL           0x8086

//-----------
// Device ID
//-----------
#define DEVICE_ID_MASK            0xFFFF
#define DEVICE_ID_SHIFT           0x10
#define GET_DEVICE_ID(vdid)       (((vdid) >> DEVICE_ID_SHIFT) & DEVICE_ID_MASK)

#define DEVICE_ID_82540EM         0x100E

//---------
// Command
//---------
#define COMMAND_IO_ENABLE         0x1
#define COMMAND_MEM_ENABLE        0x2
#define COMMAND_MASTER_ENABLE     0x4
#define COMMAND_SPECIAL_ENABLE    0x8
#define COMMAND_INVALIDATE_ENABLE 0x10

//----------
// Subclass
//----------
#define SUBCLASS_MASK             0xFF
#define SUBCLASS_SHIFT            0x10
#define GET_SUBCLASS(cspr)        (((cspr) >> SUBCLASS_SHIFT) & SUBCLASS_MASK)

#define SUBCLASS_PCI2PCI_BRIDGE   0x04
#define SUBCLASS_ETHERNET         0x0

//------------
// Class code
//------------
#define CLASS_CODE_MASK           0xFF
#define CLASS_CODE_SHIFT          0x18
#define GET_CLASS_CODE(cspr)      (((cspr) >> CLASS_CODE_SHIFT) & CLASS_CODE_MASK)

#define CLASS_CODE_BRIDGE         0x6

//-------------
// Header type
//-------------
#define HEADER_MASK               0xFF
#define HEADER_SHIFT              0x10
#define GET_HEADERTYPE(bhlc)      ((((bhlc) >> HEADER_SHIFT) & HEADER_MASK))
#define GET_HEADERTYPE_TYPE(bhlc) (GET_HEADERTYPE(bhlc) & 0x7f)
#define IS_MULTIFUNC(bhlc)        ((GET_HEADERTYPE(bhlc) & 0x80) != 0)

//-----
// BAR
//-----
#define BAR_NUM_MAX               0x6

#define BAR_TYPE_MASK             0x1
#define BAR_TYPE_MEM              0x0
#define GET_BAR_TYPE(v)           ((v) & BAR_TYPE_MASK)

#define BAR_MEM_TYPE_32           0x0
#define BAR_MEM_TYPE_64           0x2
#define BAR_MEM_TYPE_MASK         0x3
#define BAR_MEM_TYPE_SHIFT        0x1
#define GET_BAR_MEM_TYPE(v)       (((v) >> BAR_MEM_TYPE_SHIFT) & BAR_MEM_TYPE_MASK)

#define GET_BAR_REG_NUM(bar)      (((bar) - BAR_START) / 4)

#define BAR_MEM_INFO_MASK         0xFFFFFFF0
#define GET_BAR_MEM_ADDR(oldv)    ((oldv) & BAR_MEM_INFO_MASK)
#define GET_BAR_MEM_SIZE(v)       (~((v) & BAR_MEM_INFO_MASK) + 1)

#define BAR_IO_INFO_MASK          0xFFFFFFFC
#define GET_BAR_IO_ADDR(oldv)     ((oldv) & BAR_IO_INFO_MASK)
#define GET_BAR_IO_SIZE(v)        (~((v) & BAR_IO_INFO_MASK) + 1)

//----------------------
// Secondary bus number
//----------------------
#define SEC_BUS_NUM_MASK          0xFF
#define SEC_BUS_NUM_SHIFT         0x8
#define GET_SEC_BUS_NUM(sssp)     (((sssp) >> SEC_BUS_NUM_SHIFT) & SEC_BUS_NUM_MASK)

//----------------
// Interrupt line
//----------------
#define INTERRUPT_LINE_MASK       0xFF
#define GET_INTERRUPT_LINE(mmii)  ((mmii) & INTERRUPT_LINE_MASK)


struct pci_func {
    struct pci_bus *bus;
    uint32_t dev_num;
    uint32_t func_num;
    uint32_t reg_base[BAR_NUM_MAX];
    uint32_t reg_size[BAR_NUM_MAX];
    uint32_t irq_line;
};

struct pci_bus {
    struct pci_func *parent_bridge;
    uint32_t bus_num;
};

struct pci_driver {
    uint32_t key1;
    uint32_t key2;
    int (*attach_func)(struct pci_func *func);
};


#endif