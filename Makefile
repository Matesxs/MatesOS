all:
	$(MAKE) image

.PHONY: init
init:
	sudo apt-get install gcc nasm
	sudo apt-get install qemu qemu-system-common

.PHONY: kernel
kernel:
	cd kernel && $(MAKE) kernel

.PHONY: image
image: bootloader
	cd kernel && $(MAKE) buildimg

.PHONY: rebuildkernel
rebuildkernel:
	cd kernel && $(MAKE) clean
	$(MAKE) kernel

.PHONY: bootloader
bootloader:
	cd gnu-efi && $(MAKE)
	cd gnu-efi && $(MAKE) bootloader

.PHONY: rebuild
rebuild: clean
	$(MAKE) all

.PHONY: clean
clean:
	cd gnu-efi && $(MAKE) clean
	cd kernel && $(MAKE) clean

.PHONY: run
run: bootloader
	cd kernel && $(MAKE) run

