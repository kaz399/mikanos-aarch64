/**************************************************************/
/**
    @file    iofunc.c

    @brief


    Copyright 2020 Yabe.Kazuhiro

***************************************************************

*/
/**************************************************************/

#include <cstdint>

#include "iofunc.hpp"

extern int printk(const char* format, ...);

extern "C" {
    void io_write8(uint64_t address, uint8_t data)
    {
        // write_memory_barrier();
        //printk("address:%0p\n", address);
        volatile uint8_t *adrs = (volatile uint8_t *)address;
        *adrs = data;
    }

    uint8_t io_read8(uint64_t address)
    {
        //printk("read:%0p\n", address);
        volatile uint8_t *adrs = (volatile uint8_t *)address;
        uint16_t data = *adrs;
        //printk("data:%02x\n", data);
        // read_memory_barrier();
        return data;
    }

    void io_write16(uint64_t address, uint16_t data)
    {
        // write_memory_barrier();
        //printk("address:%0p\n", address);
        volatile uint16_t *adrs = (volatile uint16_t *)address;
        *adrs = data;
    }

    uint16_t io_read16(uint64_t address)
    {
        //printk("read:%0p\n", address);
        volatile uint16_t *adrs = (volatile uint16_t *)address;
        uint16_t data = *adrs;
        //printk("data:%04x\n", data);
        // read_memory_barrier();
        return data;
    }

    void io_write32(uint64_t address, uint32_t data)
    {
        // write_memory_barrier();
        //printk("address:%0p\n", address);
        volatile uint32_t *adrs = (volatile uint32_t *)address;
        *adrs = data;
    }

    uint32_t io_read32(uint64_t address)
    {
        //printk("read:%0p\n", address);
        volatile uint32_t *adrs = (volatile uint32_t *)address;
        uint32_t data = *adrs;
        //printk("data:%08x\n", data);
        // read_memory_barrier();
        return data;
    }

    void io_write64(uint64_t address, uint64_t data)
    {
        // write_memory_barrier();
        //printk("address:%0p\n", address);
        volatile uint64_t *adrs = (volatile uint64_t *)address;
        *adrs = data;
    }

    uint64_t io_read64(uint64_t address)
    {
        //printk("read:%0p\n", address);
        volatile uint64_t *adrs = (volatile uint64_t *)address;
        uint32_t data = *adrs;
        //printk("data:%016x\n", data);
        // read_memory_barrier();
        return data;
    }
}

