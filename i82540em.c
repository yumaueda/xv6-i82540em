#include "types.h"
#include "defs.h"
#include "x86.h"
#include "i82540em.h"
#include "pci.h"


static uint32_t read_reg(struct i82540em *nic, uint16_t offset)
{
    return *(volatile uint32_t *)(nic->mmio_base_addr + offset);
}


static void write_reg(struct i82540em *nic, uint16_t offset, uint32_t v)
{
    *(volatile uint32_t *)(nic->mmio_base_addr + offset) = v;
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
    // get MMIO base address
    get_mmio_base_addr(func, nic_i82540em);
    cprintf("82540em: MMIO base addr: 0x%x\n", nic_i82540em->mmio_base_addr);
    // read MAC address from EEPROM
    read_addr(nic_i82540em);
    cprintf("82540em: MAC addr: %x:%x:%x:%x:%x:%x\n", nic_i82540em->addr[0], nic_i82540em->addr[1], nic_i82540em->addr[2], nic_i82540em->addr[3], nic_i82540em->addr[4], nic_i82540em->addr[5]);

    return 0;
}