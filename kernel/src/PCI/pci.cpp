#include "pci.h"
#include "../ahci/ahci.h"
#include "../memory/heap.h"
#include "../renderer/stat_logger.h"
#include "../utils/driver.h"

namespace PCI
{
  void EnumerateFunction(uint64_t deviceAddress, uint64_t function)
  {
    uint64_t offset = function << 12;
    uint64_t functionAddress = deviceAddress + offset;

    memory::g_PageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);

    PCIDeviceheader *pciDeviceHeader = (PCIDeviceheader*)functionAddress;

    if (pciDeviceHeader->DeviceID == 0) return;
    if (pciDeviceHeader->DeviceID == 0xffff) return;

    printStats("   - ");
    printStats(GetVendorName(pciDeviceHeader->VendorID));
    printStats(" / ");
    printStats(GetDeviceName(pciDeviceHeader->VendorID, pciDeviceHeader->DeviceID));
    printStats(" / ");
    printStats(DeviceClasses[pciDeviceHeader->Class]);
    printStats(" / ");
    printStats(GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->Subclass));
    printStats(" / ");
    printStats(GetProgramIFName(pciDeviceHeader->Class, pciDeviceHeader->Subclass, pciDeviceHeader->ProgIF));
    statNewLine();

    switch (pciDeviceHeader->Class)
    {
      case 0x01: // Mass storage controller
        switch (pciDeviceHeader->Subclass)
        {
          case 0x06: // Serial ATA
            switch (pciDeviceHeader->ProgIF)
            {
              case 0x01: // AHCI 1.0 device
                driver::g_DriverManager.add_driver(new AHCI::AHCIDriver(pciDeviceHeader));
                break;
            }
            break;
        }
        break;
    }
  }

  void EnumerateDevice(uint64_t busAddress, uint64_t device)
  {
    uint64_t offset = device << 15;
    uint64_t deviceAddress = busAddress + offset;

    memory::g_PageTableManager.MapMemory((void*)deviceAddress, (void*)deviceAddress);

    PCIDeviceheader *pciDeviceHeader = (PCIDeviceheader*)deviceAddress;

    if (pciDeviceHeader->DeviceID == 0) return;
    if (pciDeviceHeader->DeviceID == 0xffff) return;

    for (uint64_t function = 0; function < 8; function++)
    {
      EnumerateFunction(deviceAddress, function);
    }
  }

  void EnumerateBus(uint64_t baseAddress, uint64_t bus)
  {
    uint64_t offset = bus << 20;
    uint64_t busAddress = baseAddress + offset;

    memory::g_PageTableManager.MapMemory((void*)busAddress, (void*)busAddress);

    PCIDeviceheader *pciDeviceHeader = (PCIDeviceheader*)busAddress;

    if (pciDeviceHeader->DeviceID == 0) return;
    if (pciDeviceHeader->DeviceID == 0xffff) return;

    for (uint64_t device = 0; device < 32; device++)
    {
      EnumerateDevice(busAddress, device);
    }
  }

  void EnumeratePCI(ACPI::MCFGHeader *mcfg)
  {
    int entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
    if (entries == 0) return;
    showSuccess("PCI Driver initialized");

    for (int t = 0; t < entries; t++)
    {
      ACPI::DeviceConfig *newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));
      
      for (uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++)
      {
        EnumerateBus(newDeviceConfig->BaseAddress, bus);
      }
    }
  }
}