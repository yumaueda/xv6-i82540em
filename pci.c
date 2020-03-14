#include "types.h"
#include "defs.h"
#include "x86.h"
#include "pci.h"


// Forward declaration
static void check_bus(struct pci_bus *bus);


static void set_conf_addr(struct pci_func *func, uint32_t offset)
{
    uint32_t conf_addr_val = ENABLE_BIT | (uint32_t)func->bus->bus_num << 16 | (uint32_t)func->dev_num << 11 | (uint32_t)func->func_num << 8 | offset;
    outl(PCI_CONF_ADDR_IOPORT, conf_addr_val);
}


static uint32_t read_conf_data(struct pci_func *func, uint32_t offset)
{
    set_conf_addr(func, offset);
    return inl(PCI_CONF_DATA_IOPORT);
}


static void attach(struct pci_func *func)
{
    // uint32_t vdid = read_conf_data(func, VENDOR_ID);
    // uint32_t cspr = read_conf_data(func, REV_ID);

    cprintf("[PCI] found a device at %d:%d:%d\n", (int)func->bus->bus_num, (int)func->dev_num, (int)func->func_num);
}


static void check_bus(struct pci_bus *bus)
{
    struct pci_func func;
    memset(&func, 0, sizeof(func));
    func.bus = bus;

    uint32_t bhlc;
    uint32_t vdid;

    for (func.dev_num = 0; func.dev_num < DEV_NUM_MAX; func.dev_num++) {
        bhlc = read_conf_data(&func, CACHE_LINE_SIZE);
        vdid = read_conf_data(&func, VENDOR_ID);

        if (GET_HEADERTYPE_TYPE(bhlc) > (uint32_t)1)
            continue; // CardBus Bridge is not supported
        if (GET_VENDOR_ID(vdid) == 0xFFFF)
            continue; // Device doesn't exist

        for (func.func_num = 0; func.func_num < (IS_MULTIFUNC(bhlc) ? FUNC_NUM_MAX : 1); func.func_num++) {
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