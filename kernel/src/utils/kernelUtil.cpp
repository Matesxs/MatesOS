#include "kernelUtil.hpp"
#include "../memory_management/heap.hpp"
#include "../renderer/basic_renderer.hpp"
#include "../library/memory.hpp"
#include "../library/string.hpp"
#include "../IO/IO.hpp"
#include "../gdt/gdt.hpp"
#include "../interrupts/IDT.hpp"
#include "../interrupts/interrupts.hpp"
#include "../renderer/stat_logger.hpp"
#include "helpers.hpp"
#include "../apic/madt.hpp"
#include "../facp/facp.hpp"
#include "../utils/panic.hpp"
#include "../cpu/features.hpp"

// #define __FORCE_RSDT__

void PrepareMemory(BootInfo *bootInfo)
{
  uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

  memory::g_Allocator = memory::PageFrameAllocator();
  memory::g_Allocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

  memory::g_Allocator.LockPages(&_KernelStart, kernelPages);

  memory::PageTable *PML4 = (memory::PageTable*)memory::g_Allocator.RequestPage();
  if (PML4 == NULL) return Panic("Failed to alocate PML4 memory page");
  memset(PML4, 0, 0x1000);

  memory::g_PageTableManager = memory::PageTableManager(PML4);

  uint64_t memorySize = GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize);

  memory::g_PageTableManager.IndentityMapMemory(bootInfo->mMap->physAddr, memorySize);

  uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
  memory::g_Allocator.LockPages(bootInfo->framebuffer->BaseAddress, fbSize / 0x1000 + 1);

  memory::g_PageTableManager.IndentityMapMemory(bootInfo->framebuffer->BaseAddress, fbSize);

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

void InitAPIC(ACPI::MADTHeader *madt)
{
  if (!APIC::MADTInit(madt)) return;
  // io_apic_enable();
  // io_pic_disable()
}

void PrepareACPI(BootInfo *bootInfo)
{
  ACPI::RSDP2 *rsdp = (ACPI::RSDP2*)bootInfo->rsdp;
  if (rsdp == nullptr) return Panic("RSDP header not found");
  showInfo("RSDP Header found");
  printStatsSpacing();
  printStats("Revision: ");
  printStats(to_string((uint64_t)rsdp->Revision));
  statNewLine();

  ACPI::SDTHeader *rootHeader;
  void *(*FindTable)(ACPI::SDTHeader*, char*);

#ifdef __FORCE_RSDT__
  showInfo("Using RSDT");
  rootHeader = (ACPI::SDTHeader*)(uint64_t)(bootInfo->rsdp->RSDTAddress);
  FindTable = &ACPI::FindRSDTTable;

  ACPI::EnumRSDT(rootHeader);
#else
  if (rsdp->Revision >= 2)
  {
    // XSDT available
    showInfo("Using XSDT");
    rootHeader = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
    FindTable = &ACPI::FindXSDTTable;

    ACPI::EnumXSDT(rootHeader);
  }
  else
  {
    // XSDT not available
    showInfo("Using RSDT");
    rootHeader = (ACPI::SDTHeader*)(uint64_t)(bootInfo->rsdp->RSDTAddress);
    FindTable = &ACPI::FindRSDTTable;

    ACPI::EnumRSDT(rootHeader);
  }
#endif

  if (rootHeader == nullptr) return Panic("Root ACPI header not found");

  ACPI::MCFGHeader *mcfg = (ACPI::MCFGHeader*)FindTable(rootHeader, (char*)"MCFG");
  if (mcfg == nullptr) showWarning("MCFG Table not found - skipping initialization of PCI driver");
  else PCI::EnumeratePCI(mcfg);

  ACPI::FACPHeader *facp = (ACPI::FACPHeader*)FindTable(rootHeader, (char*)"FACP");
  if (facp == nullptr) return Panic("FACP Table not found");
  FACP::InitFACP(facp);

  if (!CPU::feature::APIC()) return Panic("APIC not supported");
  ACPI::MADTHeader *madt = (ACPI::MADTHeader*)FindTable(rootHeader, (char*)"APIC");
  if (madt == nullptr) return Panic("MADT Table not found - no APIC support!");
  InitAPIC(madt);
}

void printCPUInfo()
{
  CPU::CPUInfo cpuInfo = CPU::GetCPUInfo();

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
}

void InitializeKernel(BootInfo *bootInfo)
{
  memset(&_WritableStart, 0, (uint64_t)&_WritableEnd - (uint64_t)&_WritableStart);

  BasicRenderer::InitGlobalBasicRenderer(bootInfo->framebuffer, Point{4, 0}, bootInfo->psf1_Font, 8, 16, BasicRenderer::BR_WHITE, BasicRenderer::__BACKGROUND_COLOR);
  BasicRenderer::g_Renderer.ClearScreen();
  showSuccess("Frame buffer initialized");

  CPU::feature::cpu_enable_features();
  showSuccess("CPU Info loaded");
  printCPUInfo();

  GDTDescriptor gdtDescriptor;
  gdtDescriptor.Size = sizeof(GDT) - 1;
  gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
  LoadGDT(&gdtDescriptor);

  PrepareMemory(bootInfo);
  showSuccess("Memory initialized");

  memory::CreateHeap((void*)0x0000100000000000, 0x10);
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

  memory::WalkHeap();

  statNewLine();
  ShowOSStats();

  // enable maskable interrupts
  asm ("sti");
}