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
#include "../facp/facp.h"
#include "../utils/panic.h"
#include "../cpu/features.h"

void PrepareMemory(BootInfo *bootInfo)
{
  uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

  memory::g_Allocator = memory::PageFrameAllocator();
  memory::g_Allocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

  memory::g_Allocator.LockPages(&_KernelStart, kernelPages);

  memory::PageTable *PML4 = (memory::PageTable*)memory::g_Allocator.RequestPage();
  if (PML4 == NULL) Panic("Failed to alocate PML4 memory page");
  memset(PML4, 0, 0x1000);

  memory::g_PageTableManager = memory::PageTableManager(PML4);

  uint64_t memorySize = GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize);
  for (uint64_t t = (uint64_t)bootInfo->mMap->physAddr; t < memorySize; t += 0x1000)
  {
    memory::g_PageTableManager.IndentityMapMemory((void *)t);
  }

  uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
  uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
  memory::g_Allocator.LockPages((void *)fbBase, fbSize / 0x1000 + 1);
  for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096)
  {
    memory::g_PageTableManager.IndentityMapMemory((void *)t);
  }

  asm("mov %0, %%cr3" : : "r"(PML4));
}

IDTR idtr;
IDTDescEntry idt_entries[256];
void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector)
{
  IDTDescEntry *interrupt = (IDTDescEntry*)(idtr.Base + entryOffset * sizeof(IDTDescEntry));
  interrupt->SetOffset((uint64_t)handler);
  interrupt->Type_Attr = type_attr;
  interrupt->Selector = selector;
}

void PrepareInterrupts()
{
  idtr.Limit = sizeof(IDTDescEntry) * 256 - 1;
  idtr.Base = (uint64_t)&idt_entries;

  SetIDTGate((void*)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void*)PITInt_Handler, 0x20, IDT_TA_InterruptGate, 0x08);

  asm ("lidt %0" : : "m" (idtr));

  RemapPIC();

  showSuccess("Interrupts initialized");
  printStatsSpacing();
  printStats("Base address: 0x");
  printStats(to_hstring(idtr.Base));
  statNewLine();
  printStatsSpacing();
  printStats("Limit: ");
  printStats(to_string((uint64_t)idtr.Limit));
  statNewLine();
}

void InitAPIC(ACPI::SDTHeader *xsdt)
{
  if (!CPU::feature::APIC()) Panic("APIC not supported");

  ACPI::MADTHeader *madt = (ACPI::MADTHeader*)ACPI::FindTable(xsdt, (char*)"APIC");
  if (madt == nullptr) Panic("MADT Table not found - no APIC support!");
  else
  {
    showSuccess("MADT Table found");
    if (!APIC::MADTInit(madt)) return;
    // io_apic_enable();
  }
}

void PrepareACPI(BootInfo *bootInfo)
{
  ACPI::SDTHeader *xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
  if (xsdt == nullptr)
  {
    Panic("XSDT Table (ACPI) not found");
  }
  else showSuccess("XSDT Table (ACPI) found");

  // Enumerate all headers in ACPI
  if (!ACPI::EnumACPI(xsdt))
  {
    Panic("No ACPI headers found");
    return;
  }

  ACPI::MCFGHeader *mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");
  if (mcfg == nullptr)
  {
    showWarning("MCFG Table not found - skipping initialization of PCI driver");
  }
  else
  {
    showSuccess("MCFG Table found");
    PCI::EnumeratePCI(mcfg);
  }

  ACPI::FACPHeader *facp = (ACPI::FACPHeader*)ACPI::FindTable(xsdt, (char*)"FACP");
  if (facp == nullptr)
  {
    Panic("FACP Table not found");
  }
  else
  {
    showSuccess("FACP Table found");
    FACP::InitFACP(facp);
  }

  InitAPIC(xsdt);
}

void InitializeKernel(BootInfo *bootInfo)
{
  memset(&_BSSStart, 0, (uint64_t)&_KernelEnd - (uint64_t)&_BSSStart);

  BasicRenderer::InitGlobalBasicRenderer(bootInfo->framebuffer, bootInfo->psf1_Font, BasicRenderer::BR_WHITE, BasicRenderer::__BACKGROUND_COLOR);
  BasicRenderer::g_Renderer.ClearScreen();
  BasicRenderer::g_Renderer.SetCursor(50, 0);
  showSuccess("Frame buffer initialized");

  CPU::feature::cpu_enable_features();
  CPU::CPUInfo cpuInfo = CPU::GetCPUInfo();
  showSuccess("CPU Info loaded");

  printStatsSpacing();
  printStats("Vendor: ");
  printStats(cpuInfo.VendorString);
  printStats(" (");
  printStats(to_string((uint64_t)cpuInfo.VendorID));
  printStats(")");
  statNewLine();

  printStatsSpacing();
  printStats("Brand: ");
  printStats(cpuInfo.BrandString);
  statNewLine();

  printStatsSpacing();
  printStats("Features: ");
  if (CPU::feature::APIC()) printStats("APIC ");
  if (CPU::feature::FPU()) printStats("FPU ");
  if (CPU::feature::X2APIC()) printStats("X2APIC ");
  if (CPU::feature::SSE()) printStats("SSE ");
  statNewLine();

  GDTDescriptor gdtDescriptor;
  gdtDescriptor.Size = sizeof(GDT) - 1;
  gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
  LoadGDT(&gdtDescriptor);

  PrepareMemory(bootInfo);
  showSuccess("Memory initialized");

  memory::CreateHeap((void*)0x0000100000000000, 0x20);
  showSuccess("Heap initialized");

  PrepareInterrupts();

  InitPS2Mouse();
  showSuccess("Mouse initialized");

  outb(PIC1_DATA, 0b11111000);
  outb(PIC2_DATA, 0b11101111);

  PrepareACPI(bootInfo);

  // Load all drivers
  if (driver::g_DriverManager.get_num_of_drivers() > 0) driver::g_DriverManager.activate_all();
  else showWarning("No drivers to load");

  showSuccess("Kernel initialized successfully");

  statNewLine();
  ShowOSStats();

  // enable maskable interrupts
  asm ("sti");
}