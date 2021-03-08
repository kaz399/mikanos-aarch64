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
    void io_write32(uint64_t adrs, uint32_t data);
    uint32_t io_read32(uint64_t adrs);
}

#endif /* __IOFUNC_H__ */

