MIKANOS_ARCH_DEFINE := ARCH_AARCH64
MIKANOS_ARCH_TARGET := aarch64-linux-gnu
MIKANOS_KERNEL_BASE := 0x40000000

CLANG_SYSROOT       :=

CLANG_VERSION       := -11
CPP                 := clang-cpp$(CLANG_VERSION)
CC                  := clang$(CLANG_VERSION)
CXX                 := clang++$(CLANG_VERSION)
LD                  := ld.lld$(CLANG_VERSION)
OBJCOPY             := llvm-objcopy$(CLANG_VERSION)
AS                  := $(CC)

ARCH_INC_PATH       := -I$(KERNEL_DIR) \
			-I$(KERNEL_DIR)/arch/$(MIKANOS_ARCH_TARGET) \
			-I$(DEVENV_DIR)/$(MIKANOS_ARCH_TARGET)/include/c++/v1 \
			-I$(DEVENV_DIR)/$(MIKANOS_ARCH_TARGET)/include \
			-I$(DEVENV_DIR)/$(MIKANOS_ARCH_TARGET)/include/freetype2
ARCH_LIB_PATH       := -L$(DEVENV_DIR)/$(MIKANOS_ARCH_TARGET)/lib

ARCH_CPPFLAGS       := --target=$(MIKANOS_ARCH_TARGET) $(ARCH_INC_PATH) -D$(MIKANOS_ARCH_DEFINE)=1
ARCH_CFLAGS         := --target=$(MIKANOS_ARCH_TARGET) $(ARCH_INC_PATH) -D$(MIKANOS_ARCH_DEFINE)=1
ARCH_CXXFLAGS       := --target=$(MIKANOS_ARCH_TARGET) $(ARCH_INC_PATH) -D$(MIKANOS_ARCH_DEFINE)=1
ARCH_LDFLAGS        := --image-base $(MIKANOS_KERNEL_BASE) $(ARCH_LIB_PATH)
ARCH_ASFLAGS        := --target=$(MIKANOS_ARCH_TARGET)
ARCH_OBJCOPYFLAGS   := -O elf64-aarch64

ARCH_OBJS           := iofunc.o pci.o halt.o libcxx_support.o
ARCH_LIBS           := -lc++abi -lm -lunwind -lgcc

EDK2_ARCH_TARGET    := AARCH64

