all:
	cd gnu-efi && $(MAKE)
	cd gnu-efi && $(MAKE) bootloader
	cd kernel && $(MAKE) kernel
	cd kernel && $(MAKE) buildimg

EFIAPPPATH = gnu-efi/x86_64/bootloader
EFIAPPFNAME = main.efi

BINDIR = kernel/bin
STATICSRCDIR = kernel/staticsrc

.PHONY: rebuildkernel
rebuildkernel:
	cd kernel && $(MAKE) clean
	cd kernel && $(MAKE) setup
	cd kernel && $(MAKE) kernel
	cd kernel && $(MAKE) buildimg

.PHONY: kernel
kernel:
	cd kernel && $(MAKE) kernel
	cd kernel && $(MAKE) buildimg

.PHONY: bootloader
bootloader:
	cd gnu-efi && $(MAKE) bootloader

.PHONY: rebuild
rebuild: clean
	cd kernel && $(MAKE) setup
	cd gnu-efi && $(MAKE)
	cd gnu-efi && $(MAKE) bootloader
	cd kernel && $(MAKE) kernel
	cd kernel && $(MAKE) buildimg

.PHONY: clean
clean:
	cd gnu-efi && $(MAKE) clean
	cd kernel && $(MAKE) clean

.PHONY: createboot
createboot: rebuildall
	@rm -rf bootFiles/efi/boot
	@mkdir -p bootFiles/efi/boot
	@cp $(EFIAPPPATH)/$(EFIAPPFNAME) bootFiles/efi/boot/bootx64.efi
	@cp $(STATICSRCDIR)/fonts/zap-ext-light16.psf bootFiles/zap-ext-light16.psf
	@cp $(BINDIR)/kernel.elf bootFiles/kernel.elf

