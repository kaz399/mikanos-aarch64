TARGET = kernel.elf
KERNEL_DIR:=$(CURDIR)

OBJS = main.o graphics.o mouse.o font.o hankaku.o console.o logger.o \
       usb/memory.o usb/device.o usb/xhci/ring.o usb/xhci/trb.o usb/xhci/xhci.o \
       usb/xhci/port.o usb/xhci/device.o usb/xhci/devmgr.o usb/xhci/registers.o \
       usb/classdriver/base.o usb/classdriver/hid.o usb/classdriver/keyboard.o \
       usb/classdriver/mouse.o
LIBS = -lc -lc++
CPPFLAGS += -nostdlibinc -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS
CFLAGS   += -O2 -Wall -g -ffreestanding -mno-red-zone
CXXFLAGS += -O2 -Wall -g -ffreestanding -mno-red-zone \
            -fno-exceptions -fno-cxx-exceptions -fno-rtti -std=c++17
LDFLAGS  += --entry KernelMain -z norelro --static

# include target specific definitions
include makedef-$(TARGET_ARCH).mk

CPPFLAGS     += $(ARCH_CPPFLAGS)
CFLAGS       += $(ARCH_CFLAGS)
CXXFLAGS     += $(ARCH_CXXFLAGS)
LDFLAGS      += $(ARCH_LDFLAGS)
ASFLAGS      += $(ARCH_ASFLAGS)
OBJCOPYFLAGS += $(ARCH_OBJCOPYFLAGS)

OBJS := $(OBJS) $(foreach obj,$(ARCH_OBJS),$(join arch/$(MIKANOS_ARCH_TARGET)/,$(obj)))
DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))

LIBS := $(LIBS) $(ARCH_LIBS)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	find $(KERNEL_DIR) -name '*.o' -type f -delete
	rm -f $(DEPENDS)

kernel.elf: $(OBJS) Makefile
	$(LD) $(LDFLAGS) -o kernel.elf $(OBJS) $(LIBS)

%.o: %.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.%.d: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.cpp=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.%.d: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.c=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@


%.o: %.asm Makefile
	$(AS) $(ASFLAGS) -o $@ $<

hankaku.bin: hankaku.txt
	../tools/makefont.py -o $@ $<

hankaku.o: hankaku.bin
	$(OBJCOPY) -I binary $(OBJCOPYFLAGS) $< $@

.%.d: %.bin
	touch $@

loader:
	(cd ../../edk2 && build -a $(EDK2_ARCH_TARGET) -t CLANG38 -p MikanLoaderPkg/MikanLoaderPkg.dsc)

.PHONY: depends $(DEPENDS)
depends:
	$(MAKE) $(DEPENDS)

-include $(DEPENDS)
