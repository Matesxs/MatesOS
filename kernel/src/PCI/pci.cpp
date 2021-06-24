#include "pci.h"
#include "../ahci/ahci.h"
#include "../memory_management/heap.h"
#include "../renderer/stat_logger.h"
#include "../utils/driver.h"
#include "../utils/helpers.h"
#include "../utils/panic.h"

// #define DEBUG

#define MAX_NUMBER_OF_LANES 32

namespace PCI
{
  static struct PCIDeviceheader* s_pci_devices[MAX_NUMBER_OF_LANES];
  static size_t s_pci_devices_num = 0;

  void EnumerateFunction(uint64_t deviceAddress, uint64_t function)
  {
    uint64_t offset = function << 12;
    uint64_t functionAddress = deviceAddress + offset;

    memory::g_PageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);

    PCIDeviceheader *pciDeviceHeader = (PCIDeviceheader*)functionAddress;

    if (pciDeviceHeader->DeviceID == 0) return;
    if (pciDeviceHeader->DeviceID == 0xffff) return;

    if (s_pci_devices_num >= MAX_NUMBER_OF_LANES) return;
    s_pci_devices[s_pci_devices_num] = pciDeviceHeader;
    s_pci_devices_num++;
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
    showInfo("PCI initializing");

    if (checksum((char*)&mcfg->Header, mcfg->Header.Length)) Panic("Failed SDT header checksum in MCFG header");
    if (mcfg->Header.Length < sizeof(ACPI::SDTHeader)) Panic("Impossible size of SDT header in MCFG header");

    uint64_t entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
    if (entries == 0)
    {
      showWarning("No PCI buses found");
      return;
    }

    // PCIDeviceheader *device1 = EnumerateFunction(mcfg, 0, 0, 0, 0);
    // PCIDeviceheader *device2 = EnumerateFunction(mcfg, 0, 0, 0, 1);
    // PCIDeviceheader *device3 = EnumerateFunction(mcfg, 0, 0, 0, 2);
    // PCIDeviceheader *device4 = EnumerateFunction(mcfg, 0, 0, 0, 3);

    for (uint64_t t = 0; t < entries; t++)
    {
      ACPI::DeviceConfig *newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));
      
      for (uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++)
      {
        EnumerateBus(newDeviceConfig->BaseAddress, bus);
      }
    }

    for (size_t i = 0; i < s_pci_devices_num; i++)
    {
      PCIDeviceheader *pciDeviceHeader = s_pci_devices[i];

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
        case DC_MassStorageController:
          switch (pciDeviceHeader->Subclass)
          {
            case MSCSC_SATA: // Serial ATA
              switch (pciDeviceHeader->ProgIF)
              {
                case SATAPI_AHCI_1_0_Device: // AHCI 1.0 device
                  driver::g_DriverManager.add_driver(new AHCI::AHCIDriver(pciDeviceHeader));
                  break;
              }
              break;
          }
          break;

        case DC_SerialBusController:
          switch (pciDeviceHeader->Subclass)
          {
            case SBCSC_USBController:
              switch (pciDeviceHeader->ProgIF)
              {
                default:
                  break;
              }
              break;
          }
          break;
      }
    }

    showSuccess("PCI initialized");
  }

  PCIDeviceheader *EnumerateFunction(ACPI::MCFGHeader *mcfg, uint64_t entryIndex, uint64_t busIndex, uint64_t deviceIndex, uint64_t functionIndex)
  {
    uint64_t entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);

    #ifdef DEBUG
      printStats("  -- ");
      printStats("Bus Entries: ");
      printStats(to_string(entries));
      statNewLine();
    #endif

    if (entries <= entryIndex) return NULL;
    if (deviceIndex >= 32) return NULL;
    if (functionIndex >= 8) return NULL;

    ACPI::DeviceConfig *newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * entryIndex));
    uint64_t bus = newDeviceConfig->StartBus + busIndex;

    #ifdef DEBUG
      printStats("  -- ");
      printStats("Bus start: ");
      printStats(to_string((uint64_t)newDeviceConfig->StartBus));
      printStats(" Bus end: ");
      printStats(to_string((uint64_t)newDeviceConfig->EndBus));
      printStats(" Selected bus: ");
      printStats(to_string(bus));
      statNewLine();
    #endif

    if (bus >= newDeviceConfig->EndBus) return NULL;

    uint64_t offset1 = bus << 20;
    uint64_t busAddress = newDeviceConfig->BaseAddress + offset1;

    memory::g_PageTableManager.IndentityMapMemory((void*)busAddress);

    PCIDeviceheader *pciDevicesHeader = (PCIDeviceheader*)busAddress;

    #ifdef DEBUG
      printStats("  -- ");
      printStats("L1 - Bus Address:      0x");
      printStats(to_hstring(busAddress));
      printStats(" DeviceID: ");
      printStats(to_string((uint64_t)pciDevicesHeader->DeviceID));
      printStats(" VendorID: ");
      printStats(to_string((uint64_t)pciDevicesHeader->VendorID));
      statNewLine();
    #endif

    if (pciDevicesHeader->DeviceID == 0) return NULL;
    if (pciDevicesHeader->DeviceID == 0xffff) return NULL;

    uint64_t offset2 = deviceIndex << 15;
    uint64_t deviceAddress = busAddress + offset2;

    memory::g_PageTableManager.IndentityMapMemory((void*)deviceAddress);

    PCIDeviceheader *pciDeviceHeader = (PCIDeviceheader*)deviceAddress;

    #ifdef DEBUG
      printStats("  -- ");
      printStats("L2 - Device Address:   0x");
      printStats(to_hstring(deviceAddress));
      printStats(" DeviceID: ");
      printStats(to_string((uint64_t)pciDeviceHeader->DeviceID));
      printStats(" VendorID: ");
      printStats(to_string((uint64_t)pciDeviceHeader->VendorID));
      statNewLine();
    #endif

    if (pciDeviceHeader->DeviceID == 0) return NULL;
    if (pciDeviceHeader->DeviceID == 0xffff) return NULL;

    uint64_t offset3 = functionIndex << 12;
    uint64_t functionAddress = deviceAddress + offset3;

    memory::g_PageTableManager.IndentityMapMemory((void*)functionAddress);

    PCIDeviceheader *pciDeviceHeaderFunction = (PCIDeviceheader*)functionAddress;

    #ifdef DEBUG
      printStats("  -- ");
      printStats("L3 - Function Address: 0x");
      printStats(to_hstring(functionAddress));
      printStats(" DeviceID: ");
      printStats(to_string((uint64_t)pciDeviceHeaderFunction->DeviceID));
      printStats(" VendorID: ");
      printStats(to_string((uint64_t)pciDeviceHeaderFunction->VendorID));
      statNewLine();
    #endif

    if (pciDeviceHeaderFunction->DeviceID == 0) return NULL;
    if (pciDeviceHeaderFunction->DeviceID == 0xffff) return NULL;

    return pciDeviceHeaderFunction;
  }
}