TARGET = kernel.elf

KERNEL_DIR:=$(CURDIR)
OBJCOPY_VER:=-11

ifeq ($(MIKANOS_ARCH), ARCH_X64)
    ARCH_DIR=arch/x64
    MIKANOS_KERNEL_BASE:=0x100000
    CLANG_SYSROOT:=
    ARCH_LIBS:=
    ARCH_OBJS:=$(ARCH_DIR)/pci.o $(ARCH_DIR)/asmfunc.o $(ARCH_DIR)/newlib_support.o $(ARCH_DIR)/halt.o $(ARCH_DIR)/libcxx_support.o
endif

ifeq ($(MIKANOS_ARCH), ARCH_AARCH64)
    ARCH_DIR=arch/aarch64
    MIKANOS_KERNEL_BASE:=0x40000000
    CLANG_SYSROOT:=--sysroot=$(SYSROOT)
    ARCH_LIBS:=-L$(SYSROOT)/lib/gcc/aarch64-none-linux-gnu/10.2.1/ -lgcc
    ARCH_OBJS:=$(ARCH_DIR)/iofunc.o $(ARCH_DIR)/pci.o $(ARCH_DIR)/halt.o $(ARCH_DIR)/libcxx_support.o
endif

OBJS = main.o graphics.o mouse.o font.o hankaku.o console.o logger.o \
       usb/memory.o usb/device.o usb/xhci/ring.o usb/xhci/trb.o usb/xhci/xhci.o \
       usb/xhci/port.o usb/xhci/device.o usb/xhci/devmgr.o usb/xhci/registers.o \
       usb/classdriver/base.o usb/classdriver/hid.o usb/classdriver/keyboard.o \
       usb/classdriver/mouse.o
OBJS += $(ARCH_OBJS)
DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))

CPPFLAGS += -I$(KERNEL_DIR)
CFLAGS   += -O2 $(CLANG_SYSROOT) -Wall -g --target=$(CLANG_TARGET) -ffreestanding -mno-red-zone \
	    -I$(KERNEL_DIR) -I$(KERNEL_DIR)/$(ARCH_DIR) -D$(MIKANOS_ARCH)=1
CXXFLAGS += -O2 $(CLANG_SYSROOT) -Wall -g --target=$(CLANG_TARGET) -ffreestanding -mno-red-zone \
            -fno-exceptions -fno-cxx-exceptions -fno-rtti -std=c++17 \
	    -I$(KERNEL_DIR) -I$(KERNEL_DIR)/$(ARCH_DIR) -D$(MIKANOS_ARCH)=1
LDFLAGS  += --entry KernelMain -z norelro  --image-base $(MIKANOS_KERNEL_BASE) $(ARCH_LIBS) --static

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	find $(KERNEL_DIR) -name '*.o' -type f -delete
	rm -f $(DEPENDS)

kernel.elf: $(OBJS) Makefile
	clang -nostdlib -fuse-ld=lld $(CXXFLAGS) $(addprefix -Xlinker ,$(LDFLAGS)) -o kernel.elf $(OBJS) -lc -lc++ -lc++abi -lunwind

%.o: %.cpp Makefile
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.%.d: %.cpp
	clang++ $(CPPFLAGS) $(CXXFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.cpp=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.c Makefile
	clang $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.%.d: %.c
	clang $(CPPFLAGS) $(CFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.c=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.asm Makefile
	clang --target=$(CLANG_TARGET) -o $@ $<

hankaku.bin: hankaku.txt
	../tools/makefont.py -o $@ $<

hankaku.o: hankaku.bin
	llvm-objcopy$(OBJCOPY_VER) -I binary -O elf64-aarch64 $< $@

.%.d: %.bin
	touch $@

.PHONY: depends $(DEPENDS)
depends:
	$(MAKE) $(DEPENDS)

-include $(DEPENDS)
