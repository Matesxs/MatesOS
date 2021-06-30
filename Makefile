OSNAME = MatesOS
KERNELDIR = kernel

KERNELPATH := $(KERNELDIR)/bin/kernel.elf

.PHONY: init all kernel kernel_debug rebuildkernel image rebuild clean cleanimages run run_only run_debug

all: image

init:
	sudo apt-get install gcc nasm gdb
	sudo apt-get install qemu qemu-system-common

kernel:
	cd kernel && $(MAKE) kernel

kernel_debug:
	cd kernel && $(MAKE) debug

image: cleanimages limine kernel
	mkdir -p iso_root
	mkdir -p iso_root/boot
	mkdir -p iso_root/static_data

	cp limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
	cp $(KERNELDIR)/bin/kernel.elf limine/BOOTX64.EFI iso_root/boot/
	cp -rf static_data/* iso_root/static_data/

	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(OSNAME).iso
	limine/limine-install $(OSNAME).iso
	rm -rf iso_root

rebuildimage: rebuildkernel image

rebuildkernel:
	cd kernel && $(MAKE) clean
	$(MAKE) kernel

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v2.4-binary --depth=1
	make -C limine

rebuild: clean
	$(MAKE) all

cleanimages:
	rm -rf iso_root
	rm -f $(OSNAME).iso

clean: cleanimages
	cd kernel && $(MAKE) clean
	rm -rf limine

run: image
	$(MAKE) run_only

run_only:
	qemu-system-x86_64 -smp 4 -machine q35 -cdrom $(OSNAME).iso -m 2G -cpu qemu64 -net none

run_debug: kernel_debug image
	qemu-system-x86_64 -s -S -smp 4 -machine q35 -cdrom $(OSNAME).iso -m 2G -cpu qemu64 -net none