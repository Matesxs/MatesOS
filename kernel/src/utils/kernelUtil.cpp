#include "kernelUtil.h"
#include "../memory/heap.h"
#include "performance.h"
#include "../renderer/basic_renderer.h"
#include "../utils/cstr.h"
#include "../IO/IO.h"
#include "../gdt/gdt.h"
#include "../interrupts/IDT.h"
#include "../interrupts/interrupts.h"
#include "../renderer/stat_logger.h"

void PrepareMemory(BootInfo *bootInfo)
{
  uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

  GlobalAllocator = PageFrameAllocator();
  GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

  GlobalAllocator.LockPages(&_KernelStart, kernelPages);

  PageTable *PML4 = (PageTable *)GlobalAllocator.RequestPage();
  memset(PML4, 0, 0x1000);

  g_PageTableManager = PageTableManager(PML4);

  for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t += 0x1000)
  {
    g_PageTableManager.MapMemory((void *)t, (void *)t);
  }

  uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
  uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
  GlobalAllocator.LockPages((void *)fbBase, fbSize / 0x1000 + 1);
  for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096)
  {
    g_PageTableManager.MapMemory((void *)t, (void *)t);
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
  void *newPage = GlobalAllocator.RequestPage();
  if (newPage == NULL)
  {
    showFailed("Interrupts memory allocation failed");
    return;
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

void PrepareACPI(BootInfo *bootInfo)
{
  ACPI::SDTHeader *xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
  if (xsdt == nullptr)
  {
    showFailed("SDT Header not found");
    return;
  }
  else
  {
    showSuccess("SDT Header found");
  }

  ACPI::MCFGHeader *mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");
  if (mcfg == nullptr)
  {
    showFailed("MCFG Header not found");
    return;
  }
  else
  {
    showSuccess("MCFG Header found");
    PCI::EnumeratePCI(mcfg);
  }
}

void InitializeKernel(BootInfo *bootInfo)
{
  InitGlobalBasicRenderer(bootInfo->framebuffer, bootInfo->psf1_Font, BR_WHITE, __BACKGROUND_COLOR);
  setLoggerStart(50, 50);

  GDTDescriptor gdtDescriptor;
  gdtDescriptor.Size = sizeof(GDT) - 1;
  gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
  LoadGDT(&gdtDescriptor);

  PrepareMemory(bootInfo);
  GlobalBasicRenderer.ClearScreen();
  GlobalBasicRenderer.SetCursor(50, 50);
  GlobalBasicRenderer.SetColor(BR_WHITE);
  showSuccess("Memory initialized");

  uint64_t heapInitPages = InitializeHeap((void*)0x0000100000000000, 0x10);
  if (heapInitPages == 0)
    showFailed("Heap initialization failed");
  else
    showSuccess("Heap initialized");

  PrepareInterrupts();

  InitPS2Mouse();
  showSuccess("Mouse initialized");

  WriteBus(PIC1_DATA, 0b11111000);
  WriteBus(PIC2_DATA, 0b11101111);

  PrepareACPI(bootInfo);

  showSuccess("Kernel initialized successfully");

  GlobalBasicRenderer.NewLine();
  GlobalBasicRenderer.NewLine();

  asm ("sti");
}