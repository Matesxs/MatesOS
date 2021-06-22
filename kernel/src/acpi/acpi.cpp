#include "acpi.h"
#include "../library/string.h"
#include "../library/memory.h"
#include "../renderer/stat_logger.h"
#include "../utils/panic.h"
#include "../utils/helpers.h"

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

    if (checksum((char*)sdtHeader, sdtHeader->Length) != 0) Panic("Failed SDT header checksum (XSDT)");
    if (sdtHeader->Length < sizeof(SDTHeader)) Panic("Impossible size of SDT header (XSDT)");

    int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / 8;
    if (entries > 0)
    {
      for (int t = 0; t < entries; t++)
      {
        ACPI::SDTHeader *newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * 8));

        if (checksum((char*)newSDTHeader, newSDTHeader->Length) != 0) Panic("Failed SDT header checksum");
        if (newSDTHeader->Length < sizeof(SDTHeader)) Panic("Impossible size of SDT header");

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