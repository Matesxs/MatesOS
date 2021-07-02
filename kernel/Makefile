LDS = kernel.ld
CC = gcc
ASMC = nasm
LD = ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-exceptions -Wall -Wfatal-errors -fno-stack-protector -Iinclude -fno-use-cxa-atexit -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fzero-initialized-in-bss
ASMFLAGS =
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := obj
BUILDDIR = bin

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

$(BUILDDIR):
	@echo !======= Creating build directory
	@mkdir $(BUILDDIR)

$(OBJDIR):
	@echo !======= Creating build object
	@mkdir $(OBJDIR)

.PHONY: debug clean

all: kernel

debug: CFLAGS += -g
debug: clean kernel

kernel: $(BUILDDIR)/kernel.elf

clean:
	@rm -rf $(BUILDDIR)
	@rm -rf $(OBJDIR)
