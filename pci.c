#include "types.h"
#include "defs.h"
#include "x86.h"
#include "pci.h"


// Forward declaration
static void check_bus(struct pci_bus *bus);


static struct pci_driver pci_devices[] = {
    { VENDOR_ID_INTEL, DEVICE_ID_82540EM, &i82540em_init},
    { 0, 0, 0 },
};


static void set_conf_addr(struct pci_func *func, uint32_t offset)
{
    uint32_t conf_addr_val = ENABLE_BIT | (uint32_t)func->bus->bus_num << 16 | (uint32_t)func->dev_num << 11 | (uint32_t)func->func_num << 8 | offset;
    outl(PCI_CONF_ADDR_IOPORT, conf_addr_val);
}


static void write_conf_data(struct pci_func *func, uint32_t offset, uint32_t v)
{
    set_conf_addr(func, offset);
    outl(PCI_CONF_DATA_IOPORT, v);
}


static uint32_t read_conf_data(struct pci_func *func, uint32_t offset)
{
    set_conf_addr(func, offset);
    return inl(PCI_CONF_DATA_IOPORT);
}


void func_enable(struct pci_func *func)
{
    write_conf_data(func, COMMAND, COMMAND_IO_ENABLE | COMMAND_MEM_ENABLE | COMMAND_MASTER_ENABLE);

    uint32_t barwidth = 4;
    uint32_t bar, oldv, v, base, size;
    uint32_t regnum;

    for (bar = BAR_START; bar < BAR_END; bar += barwidth) {
        barwidth = 4;
        oldv = read_conf_data(func, bar);
        write_conf_data(func, bar, 0xFFFFFFFF);

        if ((v = read_conf_data(func, bar)) == 0)
            continue;

        regnum = GET_BAR_REG_NUM(bar);
        if (GET_BAR_TYPE(v) == BAR_TYPE_MEM) {
            if (GET_BAR_MEM_TYPE(v) == BAR_MEM_TYPE_64)
                barwidth += 4;

            base = GET_BAR_MEM_ADDR(oldv);
            size = GET_BAR_MEM_SIZE(v);
            cprintf("mem: bar %d: %d bytes at 0x%x\n", regnum, size, base);
        }
        else {
            base = GET_BAR_IO_ADDR(oldv);
            size = GET_BAR_IO_SIZE(v);
            cprintf("io: bar %d: %d bytes at 0x%x\n", regnum, size, base);
        }

        func->reg_base[regnum] = base;
        func->reg_size[regnum] = size;

        // we have to restore the original value
        write_conf_data(func, bar, oldv);
    }
}


static void attach_matched_device(struct pci_func *func)
{
    uint32_t vdid = read_conf_data(func, VENDOR_ID);
    uint32_t vendor_id = GET_VENDOR_ID(vdid);
    uint32_t device_id = GET_DEVICE_ID(vdid);

    cprintf("vendor id: %x device id: %x\n", vendor_id, device_id);

    for (uint i = 0; pci_devices[i].attach_func; i++) {
        if (pci_devices[i].key1 == vendor_id && pci_devices[i].key2 == device_id) {
            pci_devices[i].attach_func(func);
        }
    }
}


static void attach_bridge(struct pci_func *func)
{
    struct pci_bus nbus;
    uint32_t sssp;
    memset(&nbus, 0, sizeof(nbus));
    sssp = read_conf_data(func, PRIM_BUS_NUM);
    nbus.bus_num = GET_SEC_BUS_NUM(sssp);
    check_bus(&nbus);
}


static void attach(struct pci_func *func)
{
    uint32_t cspr = read_conf_data(func, REV_ID);
    uint32_t subclass = GET_SUBCLASS(cspr);
    uint32_t class_code = GET_CLASS_CODE(cspr);

    cprintf("PCI: %x:%x:%x class: %x subclass: %x\n", (int)func->bus->bus_num, (int)func->dev_num, (int)func->func_num, (int)class_code, (int)subclass);

    if (subclass==SUBCLASS_PCI2PCI_BRIDGE && class_code==CLASS_CODE_BRIDGE) {
        attach_bridge(func);
    }
    else {
        attach_matched_device(func);
    }
}


static void check_bus(struct pci_bus *bus)
{
    struct pci_func func;
    memset(&func, 0, sizeof(func));
    func.bus = bus;

    uint32_t bhlc;
    uint32_t vdid;
    uint32_t header;
    uint32_t vendor_id;
    uint32_t mmii;

    for (func.dev_num = 0; func.dev_num < DEV_NUM_MAX; func.dev_num++) {
        func.func_num = 0;
        bhlc = read_conf_data(&func, CACHE_LINE_SIZE);
        vdid = read_conf_data(&func, VENDOR_ID);
        header = GET_HEADERTYPE_TYPE(bhlc);
        vendor_id = GET_VENDOR_ID(vdid);

        if (header > 0x1)
            continue; // CardBus Bridge is not supported

        if (vendor_id == 0xFFFF)
            continue; // Device doesn't exist

        for (func.func_num = 0; func.func_num < (IS_MULTIFUNC(bhlc) ? FUNC_NUM_MAX : 1); func.func_num++) {
            vdid = read_conf_data(&func, VENDOR_ID);
            vendor_id = GET_VENDOR_ID(vdid);
            mmii = read_conf_data(&func, INTERRUPT_LINE);
            func.irq_line = GET_INTERRUPT_LINE(mmii);
            if (vendor_id == 0xFFFF)
                continue;

            attach(&func);
        }
    }
}


void pciinit (void)
{
    static struct pci_bus root_bus;
    memset(&root_bus, 0, sizeof(root_bus));
    check_bus(&root_bus);
}