#include "madt.hpp"
#include "../memory_management/PageTableManager.hpp"
#include "../renderer/stat_logger.hpp"
#include "../library/string.hpp"
#include "../utils/helpers.hpp"
#include "../utils/panic.hpp"

namespace APIC
{
  static ACPI::MADTHeader* s_madt;
  static MADTInformation s_madt_info = {
	  .processors_num = 0,
	  .ioapics_num = 0,
	  .isos_num = 0,
  };

  bool MADTInit(ACPI::MADTHeader* madt)
  {
    showInfo("MADT Initializing");

    if (checksum((char*)&madt->Header, madt->Header.Length)) Panic("Failed SDT header checksum in MADT header");
    if (madt->Header.Length < sizeof(ACPI::SDTHeader)) Panic("Impossible size of SDT header in MADT header");

    s_madt = madt;
    memory::g_PageTableManager.IndentityMapMemory((void*)madt, madt->Header.Length);

    void* local_apic_ptr = (void*)(uint64_t)madt->LocalApicAddress;
    size_t t = 0;

    while (t < madt->Header.Length - sizeof(ACPI::MADTHeader))
    {
      MADTHeader *header = (MADTHeader*)((uint64_t)madt + sizeof(ACPI::MADTHeader) + t);
      t += header->length;

      switch (header->type)
      {
        case MADT_TYPE_LOCAL_PROCESSOR:
        {
          MADTLocalProcessor *localProcessor = (MADTLocalProcessor*)header;
          s_madt_info.processors[s_madt_info.processors_num].local_processor = localProcessor;
          s_madt_info.processors_num++;
          break;
        }

        case MADT_TYPE_IOAPIC:
        {
          MADTIOAPIC *ioapic = (MADTIOAPIC*)header;
          void *ioapic_ptr = (void*)(uint64_t)ioapic->ioapic_address;

          memory::g_PageTableManager.IndentityMapMemory(ioapic_ptr);

          s_madt_info.ioapics[s_madt_info.ioapics_num].ioapic = ioapic;
          s_madt_info.ioapics_num++;
          break;
        }

        case MADT_TYPE_INTERRUPT_SOURCE_OVERRIDE:
        {
          MADTInterruptSourceOverride* iso = (MADTInterruptSourceOverride*)header;
          s_madt_info.isos[s_madt_info.isos_num] = iso;
          s_madt_info.isos_num++;
          break;
        }

        case MADT_TYPE_NONMASKABLE_INTERRUPT:
        {
          MADTNonMaskableInterrupt* nmi = (MADTNonMaskableInterrupt*)header;
          s_madt_info.nmis[s_madt_info.nmis_num] = nmi;
          s_madt_info.nmis_num++;

          printStatsSpacing();
          printStats("Non-Maskable Interrupt ID: ");
          printStats(to_string((uint64_t)nmi->local_apic_id));
          statNewLine();
          break;
        }

        case MADT_TYPE_LOCAL_APIC_ADDRESS_OVERRIDE:
        {
          MADTLocalAPICAddressOverride* local_apic_override = (MADTLocalAPICAddressOverride*)header;
          local_apic_ptr = (void*)local_apic_override->local_apic_address;

          printStatsSpacing();
          showWarning("Local PIC Address changed");
          printStats("New address: 0x");
          printStats(to_hstring(local_apic_override->local_apic_address));
          statNewLine();
          break;
        }

        default:
        {
          showWarning("Found unknown MADT header");
          printStatsSpacing();
          printStats("Type: ");
          printStats(to_string((uint64_t)header->type));
          break;
        }
      }
    }

    memory::g_PageTableManager.IndentityMapMemory(local_apic_ptr);
    s_madt_info.local_apic_ptr = local_apic_ptr;
    
    if (s_madt_info.local_apic_ptr != nullptr)
    {
      showSuccess("MADT Initialized");

      printStatsSpacing();
      printStats("Local APIC Address: 0x");
      printStats(to_hstring((uint64_t)s_madt_info.local_apic_ptr));
      printStats(", ID: ");
      printStats(to_string((uint64_t)lapic_get_id()));
      statNewLine();

      printStatsSpacing();
      printStats("IOAPIC Count: ");
      printStats(to_string(s_madt_info.ioapics_num));
      statNewLine();

      printStatsSpacing();
      printStats("ISO Count: ");
      printStats(to_string(s_madt_info.isos_num));
      statNewLine();
      return true;
    }
    else
    {
      showFailed("MADT Initialization failed - Bootstrap processor APIC Address not found");
      return false;
    }
  }

  ACPI::MADTHeader* madt_get() {
	  return s_madt;
  }

  MADTInformation* madt_get_info() {
	  return &s_madt_info;
  }
}