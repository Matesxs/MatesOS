#include "interrupts.hpp"
#include "../utils/panic.hpp"
#include "../IO/IO.hpp"
#include "../userinput/keyboard.hpp"
#include "../scheduling/pit/pit.hpp"
#include "../userinput/mouse.hpp"

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame *frame)
{
  Panic("Page Fault Detected");
}

__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame *frame)
{
  Panic("Double Fault Detected");
}

__attribute__((interrupt)) void GPFault_Handler(interrupt_frame *frame)
{
  Panic("General Protection Fault Detected");
}

__attribute__((interrupt)) void KeyboardInt_Handler(interrupt_frame *frame)
{
  uint8_t scanCode = inb(0x60);
  HandleKeyboard(scanCode);
  PIC_EndMaster();
}

__attribute__((interrupt)) void MouseInt_Handler(interrupt_frame *frame)
{
  uint8_t mouseData = inb(0x60);
  HandlePS2Mouse(mouseData);
  PIC_EndSlave();
}

__attribute__((interrupt)) void PITInt_Handler(interrupt_frame *frame)
{
  PIT::Tick();
  PIC_EndMaster();
}

void RemapPIC()
{
  uint8_t a1, a2;

  a1 = inb(PIC1_DATA);
  waitb();
  a2 = inb(PIC2_DATA);
  waitb();

  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  waitb();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  waitb();

  outb(PIC1_DATA, 0x20);
  waitb();
  outb(PIC2_DATA, 0x28);
  waitb();

  outb(PIC1_DATA, 4);
  waitb();
  outb(PIC2_DATA, 2);
  waitb();

  outb(PIC1_DATA, ICW4_8086);
  waitb();
  outb(PIC2_DATA, ICW4_8086);
  waitb();

  outb(PIC1_DATA, a1);
  waitb();
  outb(PIC2_DATA, a2);
}

void PIC_EndMaster()
{
  outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave()
{
  outb(PIC2_COMMAND, PIC_EOI);
  outb(PIC1_COMMAND, PIC_EOI);
}