all:
	$(MAKE) bootloader
	$(MAKE) kernel

EFIAPPPATH = gnu-efi/x86_64/bootloader
EFIAPPFNAME = main.efi

BINDIR = kernel/bin
STATICSRCDIR = kernel/staticsrc

.PHONY: kernel
kernel:
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
	$(MAKE) bootloader
	$(MAKE) kernel

.PHONY: clean
clean:
	cd gnu-efi && $(MAKE) clean
	cd kernel && $(MAKE) clean

.PHONY: run
run: bootloader
	cd kernel && $(MAKE) run

