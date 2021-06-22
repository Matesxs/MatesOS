# MatesOS

## Table of contents
  - [Table of contents](#table-of-contents)
  - [General info](#general-info)
  - [Developement environment](#developement-environment)
  - [Project Folder Structure](#project-folder-structure)
  - [Makefile endpoints](#makefile-endpoints)
  - [Screenshots](#screenshots)

## General info
This project contains EFI based bootloader and kernel for custom x86_64 operating system mainly in C++. \
I am using this repository to play with idea of making own operating system and lear few things from it. \
Its still not finished and here is lack of support for hardware run of this OS.

Disclaimer: This repository is more like proof of concept than download and run! So run it on your own risk.

## Developement environment
### Software
- WSL (Ubuntu-20.04)
  - **nasm** for compiling assembler code
  - **gcc** for compiling C++ code
  - **ld** as a linker
  - **QEMU** for virtual machine

### Hardware
I7 - 9700KF \
32GB 3200MHz RAM \
RTX 2080S

## Project Folder Structure
- gnu-efi
  - bootloader
- kernel (folder for kernel code)
  - src (source code for kernel)
  - staticsrc (static files for kernel)
- OVMFbin (utility files for running in qemu virtual machine)

## Makefile endpoints
init - install dependencies for compiling this OS \
all - compile bootloader, kernel and create image file for usage in virtual machine \
kernel - compile kernel \
kernel_debug - compile kernel for debugging \
rebuildkernel - clean compiled object files and recompile kernel (used when changing header files or some stuck dependency) \
image - compile bootloader, kernel and create image file for usage in virtual machine \
rebuildimage - rebuild kernel and create image \
bootloader - compile bootloader \
rebuild - clean every compiled file and rebuild whole project \
clean - clean every compiled file \
cleanimages - clean OS images \
run - compile bootloader, kernel and create image file and run QEMU virtual machine using that image \
run_only - dont build anything only run from existing image file \
run_debug - clean compiled files, add -g flag for C++ compilation, compile bootloader, kernel and create image file and run QEMU virtual machine using that image and wait for gdb to connect on localhost:1234 (dont forgot to load symbol table from kernel/bin/kernel.elf) \

## Screenshots
![scr1](media/scr1.jpg?raw=true)