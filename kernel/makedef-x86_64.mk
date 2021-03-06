ARCH_DIR            := arch/x86_64
MIKANOS_KERNEL_BASE := 0x100000
CLANG_SYSROOT       :=

AS                  := nasm
ASFLAGS             := -f elf64
CCLD                := ld.lld
CCLD_FLAGS          := $(LDFLAGS)

ARCH_LIBS           =
ARCH_OBJS           = $(ARCH_DIR)/pci.o $(ARCH_DIR)/asmfunc.o $(ARCH_DIR)/newlib_support.o $(ARCH_DIR)/halt.o $(ARCH_DIR)/libcxx_support.o
LIBS                := -lc -lc++
OBJCOPY_TARGET      := elf64-x86-64
EDK2_ARCH           := X64

