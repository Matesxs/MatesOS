#include "interrupts.h"
#include "panic.h"
#include "../IO/IO.h"
#include "../userinput/keyboard.h"
#include "../scheduling/pit/pit.h"
#include "../userinput/mouse.h"

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame *frame)
{
  Panic("Page Fault Detected");
  while (true)
  {
    asm("hlt");
  }
}

__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame *frame)
{
  Panic("Double Fault Detected");
  while (true)
  {
    asm("hlt");
  }
}

__attribute__((interrupt)) void GPFault_Handler(interrupt_frame *frame)
{
  Panic("General Protection Fault Detected");
  while (true)
  {
    asm("hlt");
  }
}

__attribute__((interrupt)) void KeyboardInt_Handler(interrupt_frame *frame)
{
  uint8_t scanCode = ReadBus(0x60);
  HandleKeyboard(scanCode);
  PIC_EndMaster();
}

__attribute__((interrupt)) void MouseInt_Handler(interrupt_frame *frame)
{
  uint8_t mouseData = ReadBus(0x60);
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

  a1 = ReadBus(PIC1_DATA);
  IOWait();
  a2 = ReadBus(PIC2_DATA);
  IOWait();

  WriteBus(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  IOWait();
  WriteBus(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  IOWait();

  WriteBus(PIC1_DATA, 0x20);
  IOWait();
  WriteBus(PIC2_DATA, 0x28);
  IOWait();

  WriteBus(PIC1_DATA, 4);
  IOWait();
  WriteBus(PIC2_DATA, 2);
  IOWait();

  WriteBus(PIC1_DATA, ICW4_8086);
  IOWait();
  WriteBus(PIC2_DATA, ICW4_8086);
  IOWait();

  WriteBus(PIC1_DATA, a1);
  IOWait();
  WriteBus(PIC2_DATA, a2);
}

void PIC_EndMaster()
{
  WriteBus(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave()
{
  WriteBus(PIC2_COMMAND, PIC_EOI);
  WriteBus(PIC1_COMMAND, PIC_EOI);
}