#include "madt.h"
#include "../memory/PageTableManager.h"
#include "../renderer/stat_logger.h"
#include "../library/cstr.h"

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
    s_madt = madt;
    memory::g_PageTableManager.IndentityMapMemory((void*)madt, madt->Header.Length);

    void* local_apic_ptr = (void*)(uint64_t)madt->LocalApicAddress;
    size_t t = 0;

    showInfo("MADT Initializing");

    while (t < madt->Header.Length - sizeof(ACPI::MADTHeader))
    {
      MADTRecord *record = (MADTRecord*)((uint64_t)madt + sizeof(ACPI::MADTHeader) + t);
      t += record->length;

      switch (record->type)
      {
        case MADT_TYPE_LOCAL_PROCESSOR:
        {
          MADTLocalProcessor *localProcessor = (MADTLocalProcessor*)record;

          s_madt_info.processors[s_madt_info.processors_num] = LocalAPICProcessor{.local_processor = localProcessor};
          s_madt_info.processors_num++;
          break;
        }

        case MADT_TYPE_IOAPIC:
        {
          MADTIOAPIC *ioapic = (MADTIOAPIC*)record;
          void *ioapic_ptr = (void*)(uint64_t)ioapic->ioapic_address;

          memory::g_PageTableManager.IndentityMapMemory(ioapic_ptr);

          s_madt_info.ioapics[s_madt_info.ioapics_num].ioapic = ioapic;
          s_madt_info.ioapics_num++;
          break;
        }

        case MADT_TYPE_INTERRUPT_SOURCE_OVERRIDE:
        {
          MADTInterruptSourceOverride* iso = (MADTInterruptSourceOverride*)record;
          s_madt_info.isos[s_madt_info.isos_num] = iso;
				  s_madt_info.isos_num++;
				  break;
        }

        case MADT_TYPE_NONMASKABLE_INTERRUPT:
        {
          MADTNonMaskableInterrupt* nmi = (MADTNonMaskableInterrupt*)record;
          s_madt_info.nmis[s_madt_info.nmis_num] = nmi;
          s_madt_info.nmis_num++;

          printStats("   - ");
          printStats("Non-Maskable Interrupt ID: ");
          printStats(to_string((uint64_t)nmi->local_apic_id));
          statNewLine();
          break;
        }

        case MADT_TYPE_LOCAL_APIC_ADDRESS_OVERRIDE:
        {
          MADTLocalAPICAddressOverride* local_apic_override = (MADTLocalAPICAddressOverride*)record;
          local_apic_ptr = (void*)local_apic_override->local_apic_address;

          printStats("   - ");
          showWarning("Local PIC Address changed");
          printStats("New address: 0x");
          printStats(to_hstring(local_apic_override->local_apic_address));
          statNewLine();
          break;
        }
      }
    }

    memory::g_PageTableManager.IndentityMapMemory(local_apic_ptr);
    s_madt_info.local_apic_ptr = local_apic_ptr;
    
    if (s_madt_info.local_apic_ptr != nullptr)
    {
      showSuccess("MADT Initialized");
      printStats("   - ");
      printStats("Bootstrap processor APIC Address: 0x");
      printStats(to_hstring((uint64_t)local_apic_ptr));
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