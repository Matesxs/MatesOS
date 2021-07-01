#pragma once

#include <stdint.h>

#define IO_CHIPSET_ADDRESS_REGISTER	0x22
#define IO_CHIPSET_DATA_REGISTER	0x23
#define IO_IMCR_REGISTER_ADDRESS	0x70
#define IO_IMCR_8259_DIRECT			0x00
#define IO_IMCR_VIA_APIC			0x01

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void waitb();

void outw(uint16_t port, uint16_t val);
uint16_t inw(uint16_t port);

void outl(uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);

// Enables APIC
extern "C" void io_apic_enable();
void io_pic_disable();
extern "C" void io_wait();