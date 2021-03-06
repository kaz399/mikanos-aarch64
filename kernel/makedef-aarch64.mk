ARCH_DIR            := arch/aarch64
MIKANOS_KERNEL_BASE := 0x40000000
CLANG_SYSROOT       := --sysroot=$(SYSROOT)

AS                  := clang
ASFLAGS             := --target=$(CLANG_TARGET)
#CCLD                := clang
#CCLD_FLAGS          := -nostdlib -fuse-ld=lld -target aarch64-linux-gnu $(CLANG_SYSROOT) $(addprefix -Xlinker ,$(LDFLAGS))
CCLD                := ld.lld
CCLD_FLAGS          := $(CLANG_SYSROOT) $(LDFLAGS)

ARCH_LIBS           := -L$(SYSROOT)/lib/gcc/aarch64-none-linux-gnu/10.2.1/ -lgcc
ARCH_OBJS           := $(ARCH_DIR)/iofunc.o $(ARCH_DIR)/pci.o $(ARCH_DIR)/halt.o $(ARCH_DIR)/libcxx_support.o
LIBS                := -lc -lc++ -lc++abi -lm -lunwind
OBJCOPY_TARGET      := elf64-aarch64
EDK2_ARCH           := AARCH64

