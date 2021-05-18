
OSNAME = MatesOS

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
CC = gcc
ASMC = nasm
LD = ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-exceptions -Wall -fno-stack-protector -Iinclude -fno-use-cxa-atexit -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASMFLAGS =
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))

$(OBJDIR)/interrupts/interrupts.o: $(SRCDIR)/interrupts/interrupts.cpp | $(OBJDIR)
	@ echo !======= COMPILING $^
	@ mkdir -p $(@D)
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@ echo !======= COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm | $(OBJDIR)
	@ echo !======= COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

$(BUILDDIR)/kernel.elf: $(OBJS) | $(BUILDDIR)
	@ echo !======= LINKING $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)

kernel: $(BUILDDIR)/kernel.elf

$(BUILDDIR):
	@echo !======= Creating build directory
	@mkdir $(BUILDDIR)

$(OBJDIR):
	@echo !======= Creating build object
	@mkdir $(OBJDIR)

clean:
	@rm -rf $(BUILDDIR)
	@rm -rf $(OBJDIR)

buildimg: kernel
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	mformat -i $(BUILDDIR)/$(OSNAME).img -f 1440 ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img staticsrc/fonts/zap-light16.psf ::

run: buildimg
	qemu-system-x86_64 -machine q35 -drive file=$(BUILDDIR)/$(OSNAME).img -m 2G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=$(OVMFDIR)/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=$(OVMFDIR)/OVMF_VARS-pure-efi.fd -net none
