OSNAME = MatesOS
KERNELDIR = kernel
GNUEFI = gnu-efi
OVMFDIR = OVMFbin

BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi
KERNELPATH := $(KERNELDIR)/bin/kernel.elf

.PHONY: init all kernel kernel_debug rebuildkernel image bootloader rebuild clean cleanimages run run_debug

all: image

init:
	sudo apt-get install gcc nasm gdb
	sudo apt-get install qemu qemu-system-common

kernel:
	cd kernel && $(MAKE) kernel

kernel_debug:
	cd kernel && $(MAKE) debug

image: cleanimages bootloader kernel
	dd if=/dev/zero of=$(OSNAME).img bs=512 count=93750
	mformat -i $(OSNAME).img -f 1440 ::
	mmd -i $(OSNAME).img ::/EFI
	mmd -i $(OSNAME).img ::/EFI/BOOT
	mmd -i $(OSNAME).img ::/STATIC_SOURCES
	mmd -i $(OSNAME).img ::/STATIC_SOURCES/FONTS
	mcopy -i $(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i $(OSNAME).img startup.nsh ::
	mcopy -i $(OSNAME).img $(KERNELPATH) ::
	mcopy -i $(OSNAME).img static_data/fonts/* ::/STATIC_SOURCES/FONTS
	cp $(OSNAME).img $(OSNAME).hdd

rebuildimage: rebuildkernel image

rebuildkernel:
	cd kernel && $(MAKE) clean
	$(MAKE) kernel

bootloader:
	cd gnu-efi && $(MAKE)
	cd gnu-efi && $(MAKE) bootloader

rebuild: clean
	$(MAKE) all

cleanimages:
	rm -f $(OSNAME).img
	rm -f $(OSNAME).hdd

clean: cleanimages
	cd gnu-efi && $(MAKE) clean
	cd kernel && $(MAKE) clean
	rm -rf $(OVMFDIR)

$(OVMFDIR):
	git clone git@github.com:Matesxs/OVMFbin.git

run: OVMFbin image
	qemu-system-x86_64 -smp 2 -machine q35 -drive file=$(OSNAME).img -m 2G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=$(OVMFDIR)/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=$(OVMFDIR)/OVMF_VARS-pure-efi.fd -net none

run_debug: OVMFbin kernel_debug image
	qemu-system-x86_64 -s -S -smp 2 -machine q35 -drive file=$(OSNAME).img -m 2G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=$(OVMFDIR)/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=$(OVMFDIR)/OVMF_VARS-pure-efi.fd -net none