#include "acpi.hpp"
#include "../library/string.hpp"
#include "../library/memory.hpp"
#include "../renderer/stat_logger.hpp"
#include "../utils/panic.hpp"
#include "../utils/helpers.hpp"

namespace ACPI
{
  void* FindXSDTTable(SDTHeader *sdtHeader, char *signature)
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

  int EnumXSDT(SDTHeader *sdtHeader)
  {
    showInfo("Enumerating XSDT");

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

    return entries;
  }

  void* FindRSDTTable(SDTHeader *sdtHeader, char *signature)
  {
    int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / sizeof(uint32_t);
    for (int t = 0; t < entries; t++)
    {
      ACPI::SDTHeader *newSDTHeader = (ACPI::SDTHeader*)*(uint32_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * sizeof(uint32_t)));

      if (strcmp(signature, (char*)newSDTHeader->Signature, 4) != 0) continue;
      return newSDTHeader;
    }

    return nullptr;
  }

  int EnumRSDT(SDTHeader *sdtHeader)
  {
    showInfo("Enumerating RSDT");

    if (checksum((char*)sdtHeader, sdtHeader->Length) != 0) Panic("Failed SDT header checksum (RSDT)");
    if (sdtHeader->Length < sizeof(SDTHeader)) Panic("Impossible size of SDT header (RSDT)");

    int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / sizeof(uint32_t);
    if (entries > 0)
    {
      for (int t = 0; t < entries; t++)
      {
        ACPI::SDTHeader *newSDTHeader = (ACPI::SDTHeader*)*(uint32_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * sizeof(uint32_t)));

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

    return entries;
  }
}