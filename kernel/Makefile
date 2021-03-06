TARGET = kernel.elf

OBJS = main.o graphics.o mouse.o font.o hankaku.o console.o logger.o \
       usb/memory.o usb/device.o usb/xhci/ring.o usb/xhci/trb.o usb/xhci/xhci.o \
       usb/xhci/port.o usb/xhci/device.o usb/xhci/devmgr.o usb/xhci/registers.o \
       usb/classdriver/base.o usb/classdriver/hid.o usb/classdriver/keyboard.o \
       usb/classdriver/mouse.o
DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))
CPPFLAGS += -I$(KERNEL_DIR)
CFLAGS   += -O2 $(CLANG_SYSROOT) -Wall -g --target=$(CLANG_TARGET) -ffreestanding -mno-red-zone \
	    -I$(KERNEL_DIR) -I$(KERNEL_DIR)/$(ARCH_DIR) -D$(MIKANOS_ARCH)=1
CXXFLAGS += -O2 $(CLANG_SYSROOT) -Wall -g --target=$(CLANG_TARGET) -ffreestanding -mno-red-zone \
            -fno-exceptions -fno-cxx-exceptions -fno-rtti -std=c++17 \
	    -I$(KERNEL_DIR) -I$(KERNEL_DIR)/$(ARCH_DIR) -D$(MIKANOS_ARCH)=1
LDFLAGS  += --entry KernelMain -z norelro  --image-base $(MIKANOS_KERNEL_BASE) --static

# include target specific definitions
include makedef-$(TARGET_ARCH).mk

OBJS := $(OBJS) $(ARCH_OBJS)
KERNEL_DIR:=$(CURDIR)
OBJCOPY_VER:=-11

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	find $(KERNEL_DIR) -name '*.o' -type f -delete
	rm -f $(DEPENDS)

kernel.elf: $(OBJS) Makefile
	#clang -nostdlib -fuse-ld=lld $(CXXFLAGS) $(addprefix -Xlinker ,$(LDFLAGS)) -o kernel.elf $(OBJS) $(LIBS)
	$(CCLD) $(CCLD_FLAGS) -o kernel.elf $(OBJS) $(LIBS) $(ARCH_LIBS)

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
	$(AS) $(ASFLAGS) -o $@ $<

hankaku.bin: hankaku.txt
	../tools/makefont.py -o $@ $<

hankaku.o: hankaku.bin
	llvm-objcopy$(OBJCOPY_VER) -I binary -O $(OBJCOPY_TARGET) $< $@

.%.d: %.bin
	touch $@

loader:
	(cd ../../edk2 && build -a $(EDK2_ARCH) -t CLANG38 -p MikanLoaderPkg/MikanLoaderPkg.dsc)

.PHONY: depends $(DEPENDS)
depends:
	$(MAKE) $(DEPENDS)

-include $(DEPENDS)
