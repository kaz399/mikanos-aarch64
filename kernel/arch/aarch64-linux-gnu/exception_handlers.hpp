/**
    @file    exception_handlers.cpp

*/

#pragma once


extern "C" {
  volatile void SyncHandler(unsigned int elr, unsigned int info);
  volatile void IRQHandler(unsigned int elr, unsigned int info);
  volatile void FIQHandler(unsigned int elr, unsigned int info);
  volatile void SerrorHandler(unsigned int elr, unsigned int info);
}

