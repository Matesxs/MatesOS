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
#include "../memory_management/memory_entry.h"

void PrepareMemory(stivale2_struct_tag_memmap *memory_map)
{
  memory::g_Allocator = memory::PageFrameAllocator();
  memory::g_Allocator.ReadMemoryMap(memory_map->memmap, memory_map->entries);

  memory::g_PageTableManager = memory::PageTableManager(memory_map->memmap, memory_map->entries);

  uint64_t memorySize = GetMemorySize(memory_map->memmap, memory_map->entries);

  memory::g_PageTableManager.IndentityMapMemory((void *)memory_map->memmap[0].base, memorySize);

  uint64_t fbSize = (uint64_t)BasicRenderer::g_Renderer.GetFramebuffer().BufferSize + 0x1000;
  memory::g_Allocator.LockPages(BasicRenderer::g_Renderer.GetFramebuffer().BaseAddress, NEAREST_PAGE(fbSize) + 1);

  memory::g_PageTableManager.IndentityMapMemory(BasicRenderer::g_Renderer.GetFramebuffer().BaseAddress, fbSize);

  asm("mov %0, %%cr3"
      :
      : "r"(memory::g_PageTableManager.PML4));
}

IDTR idtr;
IDTDescEntry idt_entries[256];
void SetIDTGate(void *handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector)
{
  IDTDescEntry *interrupt = (IDTDescEntry *)(idtr.Base + entryOffset * sizeof(IDTDescEntry));
  interrupt->SetOffset((uint64_t)handler);
  interrupt->Type_Attr = type_attr;
  interrupt->Selector = selector;
}

void PrepareInterrupts()
{
  idtr.Limit = sizeof(IDTDescEntry) * 256 - 1;
  idtr.Base = (uint64_t)&idt_entries;

  SetIDTGate((void *)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void *)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void *)GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void *)KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void *)MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);
  SetIDTGate((void *)PITInt_Handler, 0x20, IDT_TA_InterruptGate, 0x08);

  asm("lidt %0"
      :
      : "m"(idtr));

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
  if (!APIC::MADTInit(madt))
    return;

  // io_apic_enable();
}

void PrepareACPI(stivale2_struct_tag_rsdp *rsdp)
{
  ACPI::RSDP2 *rsdpStructure = (ACPI::RSDP2 *)(rsdp->rsdp);
  if (rsdpStructure == nullptr)
  {
    Panic("RSDP Header not found");
  }

  if (strcmp((char *)rsdpStructure->Signature, "RSD PTR", 8) != 0)
  {
    Panic("Failed to verify RSDP Header");
  }

  showSuccess("RSDP Found");
  printStatsSpacing();
  printStats("Revision: ");
  printStats(to_string((uint64_t)rsdpStructure->Revision));
  statNewLine();

  ACPI::SDTHeader *rootHeader = NULL;
  void*(*findFunction)(ACPI::SDTHeader *, char *) = NULL;

  if (rsdpStructure->Revision >= 2)
  {
    showInfo("Using XSDT");

    ACPI::SDTHeader *xsdt = (ACPI::SDTHeader *)(rsdpStructure->XSDTAddress);
    if (xsdt == nullptr)
    {
      Panic("XSDT Header not found");
    }

    // Enumerate all headers
    if (!ACPI::EnumXSDT(xsdt))
    {
      Panic("No XSDT headers found");
      return;
    }

    rootHeader = xsdt;
    findFunction = &ACPI::FindXSDTTable;
  }
  else
  {
    showInfo("Using RSDT");

    ACPI::SDTHeader *rsdt = (ACPI::SDTHeader *)((uint64_t)rsdpStructure->RSDTAddress);

    // Enumerate all headers
    if (!ACPI::EnumRSDT(rsdt))
    {
      Panic("No RSDT headers found");
      return;
    }

    rootHeader = rsdt;
    findFunction = &ACPI::FindRSDTTable;
  }

  ACPI::MCFGHeader *mcfg = (ACPI::MCFGHeader *)findFunction(rootHeader, (char *)"MCFG");
  if (mcfg == nullptr)
  {
    showWarning("MCFG Table not found - skipping initialization of PCI driver");
  }
  else
  {
    showSuccess("MCFG Table found");
    PCI::EnumeratePCI(mcfg);
  }

  ACPI::FACPHeader *facp = (ACPI::FACPHeader *)findFunction(rootHeader, (char *)"FACP");
  if (facp == nullptr)
  {
    Panic("FACP Table not found");
  }
  else
  {
    showSuccess("FACP Table found");
    FACP::InitFACP(facp);
  }

  if (!CPU::feature::APIC())
    Panic("APIC not supported");

  ACPI::MADTHeader *madt = (ACPI::MADTHeader *)findFunction(rootHeader, (char *)"APIC");
  if (madt == nullptr)
    Panic("MADT Table not found - no APIC support!");
  else
  {
    showSuccess("MADT Table found");
    InitAPIC(madt);
  }
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
  if (CPU::feature::APIC())
    printStats("APIC ");
  if (CPU::feature::FPU())
    printStats("FPU ");
  if (CPU::feature::X2APIC())
    printStats("X2APIC ");
  if (CPU::feature::SSE())
    printStats("SSE ");
  statNewLine();
}

void InitializeKernel(stivale2_struct *bootloader_info)
{
  memset(&_WritableStart, 0, (uint64_t)&_WritableEnd - (uint64_t)&_WritableStart);

  stivale_enumerate(bootloader_info);

  stivale2_struct_tag_rsdp *rsdp = (stivale2_struct_tag_rsdp *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_RSDP_ID);
  stivale2_struct_tag_memmap *memory_map = (stivale2_struct_tag_memmap *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MEMMAP_ID);
  // stivale2_struct_tag_smp *smp = (stivale2_struct_tag_smp *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_SMP_ID);
  stivale2_struct_tag_framebuffer *fb = (stivale2_struct_tag_framebuffer *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
  stivale2_struct_tag_modules *modules = (stivale2_struct_tag_modules *)stivale2_get_tag(bootloader_info, STIVALE2_STRUCT_TAG_MODULES_ID);

  stivale2_module fontModule;
  stivale2_get_module(modules, &fontModule, "font");

  PSF1_FONT *font = (PSF1_FONT *)fontModule.begin;

  FrameBuffer frameBuffer = {
      .BaseAddress = (void *)fb->framebuffer_addr,
      .BufferSize = (size_t)fb->framebuffer_pitch * fb->framebuffer_height,
      .Width = fb->framebuffer_width,
      .Height = fb->framebuffer_height,
      .PixPitch = fb->framebuffer_pitch};

  BasicRenderer::InitGlobalBasicRenderer(&frameBuffer, font, BasicRenderer::BR_WHITE, BasicRenderer::__BACKGROUND_COLOR);
  BasicRenderer::g_Renderer.ClearScreen();
  BasicRenderer::g_Renderer.SetCursor(50, 0);
  showSuccess("Frame buffer initialized");
  printStatsSpacing();
  printStats("Address: 0x");
  printStats(to_hstring((uint64_t)frameBuffer.BaseAddress));
  printStats(", Resolution: ");
  printStats(to_string((uint64_t)frameBuffer.Width));
  printStats("x");
  printStats(to_string((uint64_t)frameBuffer.Height));
  printStats(", Size: ");
  printStats(to_string(frameBuffer.BufferSize));
  statNewLine();

  CPU::feature::cpu_enable_features();
  showSuccess("CPU Info loaded");
  printCPUInfo();

  GDTDescriptor gdtDescriptor;
  gdtDescriptor.Size = sizeof(GDT) - 1;
  gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
  LoadGDT(&gdtDescriptor);

  PrepareMemory(memory_map);
  showSuccess("Memory initialized");

  memory::CreateHeap((void *)0xffff800000000000, 0x10);
  showSuccess("Heap initialized");

  PrepareInterrupts();
  showSuccess("Interrupts prepared");

  InitPS2Mouse();
  showSuccess("Mouse initialized");

  outb(PIC1_DATA, 0b11111000);
  outb(PIC2_DATA, 0b11101111);

  PrepareACPI(rsdp);

  // Load all drivers
  if (driver::g_DriverManager.get_num_of_drivers() > 0) driver::g_DriverManager.activate_all();
  else showWarning("No drivers to load");

  showSuccess("Kernel initialized successfully");

  memory::WalkHeap();

  statNewLine();
  ShowOSStats();

  // enable maskable interrupts
  asm("sti");
}