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
}

