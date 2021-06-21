#include "IO.h"

void outb(uint16_t port, uint8_t val)
{
  asm volatile("outb %0, %1"
               :
               : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
  uint8_t ret;
  asm volatile("inb %1, %0"
               : "=a"(ret)
               : "Nd"(port));
  return ret;
}

void outw(uint16_t port, uint16_t val)
{
  asm volatile("outw %0, %1"
               :
               : "a"(val), "Nd"(port));
}

uint16_t inw(uint16_t port)
{
  uint16_t ret;
  asm volatile("inw %1, %0"
               : "=a"(ret)
               : "Nd"(port));
  return ret;
}

void outl(uint16_t port, uint32_t val)
{
  asm volatile("outl %0, %1"
               :
               : "a"(val), "Nd"(port));
}

uint32_t inl(uint16_t port)
{
  uint32_t ret;
  asm volatile("inl %1, %0"
               : "=a"(ret)
               : "Nd"(port));
  return ret;
}

void waitb()
{
  asm volatile("outb %%al, $0x80"
               :
               : "a"(0));
}

void io_pic_disable()
{
  // Redirect
	outb(IO_CHIPSET_ADDRESS_REGISTER, IO_IMCR_REGISTER_ADDRESS);
	io_wait();
	outb(IO_CHIPSET_DATA_REGISTER, IO_IMCR_VIA_APIC);
	io_wait();
	// Disable
	outb(PIC1_DATA, 0xff);
	io_wait();
	outb(PIC2_DATA, 0xff);
}