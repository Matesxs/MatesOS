#include "kernelUtil.h"
#include "../memory_management/heap.h"
#include "../renderer/basic_renderer.h"
#include "../library/memory.h"
#include "../library/string.h"
#include "../IO/IO.h"
#include "../gdt/gdt.h"
#include "../interrupts/IDT.h"
#include "../interrupts/interrupts.h"
#include "../renderer/stat_logger.h"
#include "helpers.h"
#include "../apic/madt.h"

void PrepareMemory(BootInfo *bootInfo)
{
  uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

  memory::g_Allocator = memory::PageFrameAllocator();
  memory::g_Allocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

  memory::g_Allocator.ReservePages(&_KernelStart, kernelPages);

  memory::PageTable *PML4 = (memory::PageTable*)memory::g_Allocator.RequestPage();
  memset(PML4, 0, 0x1000);

  memory::g_PageTableManager = memory::PageTableManager(PML4);

  for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t += 0x1000)
  {
    memory::g_PageTableManager.MapMemory((void *)t, (void *)t);
  }

  uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
  uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
  memory::g_Allocator.ReservePages((void *)fbBase, fbSize / 0x1000 + 1);
  for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096)
  {
    memory::g_PageTableManager.MapMemory((void *)t, (void *)t);
  }

  asm("mov %0, %%cr3" : : "r"(PML4));
}

IDTR idtr;
void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector)
{
  IDTDescEntry *interrupt = (IDTDescEntry*)(idtr.Offset + entryOffset * sizeof(IDTDescEntry));
  interrupt->SetOffset((uint64_t)handler);
  interrupt->Type_Attr = type_attr;
  interrupt->Selector = selector;
}

void PrepareInterrupts()
{
  idtr.Limit = 0x0fff;
  void *newPage = memory::g_Allocator.RequestPage();
  if (newPage == NULL)
  {
    showFailed("Interrupts memory allocation failed");
    halt();
  }

  idtr.Offset = (uint64_t)newPage;

  SetIDTGate((void*)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)PITInt_Handler, 0x20, IDT_TA_InterruptGate, 0x08);

  asm ("lidt %0" : : "m" (idtr));

  RemapPIC();

  showSuccess("Interrupts initialized");
}

void InitAPIC(ACPI::SDTHeader *xsdt)
{
  ACPI::MADTHeader *madt = (ACPI::MADTHeader*)ACPI::FindTable(xsdt, (char*)"APIC");
  if (madt == nullptr)
  {
    showFailed("MADT Header not found");
  }
  else
  {
    showSuccess("MADT Header found");
    io_apic_enable();
    if (!APIC::MADTInit(madt)) return;
  }
}

void PrepareACPI(BootInfo *bootInfo)
{
  ACPI::SDTHeader *xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
  if (xsdt == nullptr)
  {
    showFailed("SDT Header not found");
    return;
  }
  else showSuccess("SDT Header found");

  ACPI::MCFGHeader *mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");
  if (mcfg == nullptr)
  {
    showFailed("MCFG Header not found");
  }
  else
  {
    showSuccess("MCFG Header found");
    PCI::EnumeratePCI(mcfg);
  }

  InitAPIC(xsdt);
}

void InitializeKernel(BootInfo *bootInfo)
{
  BasicRenderer::InitGlobalBasicRenderer(bootInfo->framebuffer, bootInfo->psf1_Font, BasicRenderer::BR_WHITE, BasicRenderer::__BACKGROUND_COLOR);
  setLoggerStart(50, 50);

  GDTDescriptor gdtDescriptor;
  gdtDescriptor.Size = sizeof(GDT) - 1;
  gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
  LoadGDT(&gdtDescriptor);

  PrepareMemory(bootInfo);
  BasicRenderer::g_Renderer.ClearScreen();
  showSuccess("Memory initialized");
  showSuccess("Frame buffer initialized");

  if (memory::CreateHeap((void*)0x0000100000000000, 0x10)) showSuccess("Heap initialized");
  else
  {
    showFailed("Heap initialization failed");
    halt();
  }

  PrepareInterrupts();

  InitPS2Mouse();
  showSuccess("Mouse initialized");

  outb(PIC1_DATA, 0b11111000);
  outb(PIC2_DATA, 0b11101111);

  PrepareACPI(bootInfo);

  // Load all drivers
  driver::g_DriverManager.activate_all();

  showSuccess("Kernel initialized successfully");

  statNewLine();
  ShowStats();

  // enable maskable interrupts
  asm ("sti");
}