/**************************************************************/
/**
    @file    iofunc.h

    @brief


    Copyright 2020 Yabe.Kazuhiro

***************************************************************

*/
/**************************************************************/
#ifndef __IOFUNC_H__
#define __IOFUNC_H__


extern "C" {
    #include <cstdint>

    void io_write8(uint64_t adrs, uint8_t data);
    uint8_t io_read8(uint64_t adrs);

    void io_write16(uint64_t adrs, uint16_t data);
    uint16_t io_read16(uint64_t adrs);

    void io_write32(uint64_t adrs, uint32_t data);
    uint32_t io_read32(uint64_t adrs);

    void io_write64(uint64_t adrs, uint64_t data);
    uint64_t io_read64(uint64_t adrs);
}

#endif /* __IOFUNC_H__ */

