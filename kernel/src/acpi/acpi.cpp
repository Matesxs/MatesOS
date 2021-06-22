#include "acpi.h"
#include "../library/string.h"
#include "../library/memory.h"
#include "../renderer/stat_logger.h"

namespace ACPI
{
  void* FindTable(SDTHeader *sdtHeader, char *signature)
  {
    int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / 8;
    for (int t = 0; t < entries; t++)
    {
      ACPI::SDTHeader *newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * 8));

      if (strcmp(signature, (char*)newSDTHeader->Signature, 4) != 0) continue;
      return newSDTHeader;
    }

    return nullptr;
  }

  int EnumACPI(SDTHeader *sdtHeader)
  {
    showInfo("Enumerating ACPI");

    int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / 8;
    if (entries > 0)
    {
      for (int t = 0; t < entries; t++)
      {
        ACPI::SDTHeader *newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * 8));

        char headerSignature[5];
        memcpy(headerSignature, newSDTHeader->Signature, 4);
        headerSignature[4] = '\0';

        printStatsSpacing();
        printStats(headerSignature);
        printStats(" / Revision: ");
        printStats(to_string((uint64_t)newSDTHeader->Revision));
        printStats(" / OEM Revision: ");
        printStats(to_string((uint64_t)newSDTHeader->OEMRevision));
        printStats(" / Creator revision: ");
        printStats(to_string((uint64_t)newSDTHeader->CreatorRevision));
        statNewLine();
      }
    }

    showInfo("ACPI Enumerated");

    return entries;
  }
}