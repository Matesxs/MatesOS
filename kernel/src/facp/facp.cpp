#include "facp.h"
#include "../library/string.h"
#include "../library/memory.h"
#include "../renderer/stat_logger.h"
#include "../IO/IO.h"
#include "../memory_management/PageTableManager.h"

namespace FACP
{
  uint32_t* SMI_CMD;
  uint8_t  ACPI_ENABLE;
  uint8_t  ACPI_DISABLE;
  uint32_t PM1a_CNT;
  uint32_t PM1b_CNT;
  uint16_t SLP_TYPa;
  uint16_t SLP_TYPb;
  uint8_t  PM1_CNT_LEN;
  uint16_t SLP_EN;
  uint16_t SCI_EN;

  uint32_t reset_flag;

  ACPI::FACPHeader *g_FACPHeader = nullptr;

  constexpr uint32_t bake(char a, char b , char c, char d) {
    return a | (b << 8) | (c << 16) | (d << 24);
  }

  void InitFACP(ACPI::FACPHeader *facp)
  {
    showInfo("FACP initializing");

    g_FACPHeader = facp;

    auto dsdt_addr = (uintptr_t) facp->DSDT;
    constexpr uint32_t DSDT_t = bake('D', 'S', 'D', 'T');

    if (*(uint32_t*)dsdt_addr != DSDT_t)
    {
      showFailed("FACP initialization failed - DSDT baked address missmatch");
      return;
    }

    char* S5Addr = (char*) dsdt_addr + 36;
    int dsdtLength = ((ACPI::SDTHeader*)dsdt_addr)->Length;
    dsdtLength *= 2;

    while (dsdtLength-- > 0)
    {
      if (memcmp(S5Addr, "_S5_", 4) == 0)
        break;
      S5Addr++;
    }

    if (dsdtLength <= 0) {
      showFailed("FACP initialization failed - _S5 not present in ACPI");
      return;
    }

    // check for valid AML structure
    if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 )
    {
      S5Addr += 5;
      S5Addr += ((*S5Addr &0xC0)>>6) + 2;   // calculate PkgLength size

      if (*S5Addr == 0x0A)
        S5Addr++;   // skip byteprefix
      SLP_TYPa = *(S5Addr) << 10;
      S5Addr++;

      if (*S5Addr == 0x0A)
        S5Addr++;   // skip byteprefix
      SLP_TYPb = *(S5Addr)<<10;

      SMI_CMD = (uint32_t*)(uintptr_t) facp->SMI_CMD;

      ACPI_ENABLE = facp->ACPI_ENABLE;
      ACPI_DISABLE = facp->ACPI_DISABLE;

      PM1a_CNT = facp->PM1a_CNT_BLK;
      PM1b_CNT = facp->PM1b_CNT_BLK;

      PM1_CNT_LEN = facp->PM1_CNT_LEN;

      SLP_EN = 1<<13;
      SCI_EN = 1;
    }
    else
    {
      showWarning("FACP Failed to parse _S5");
      SCI_EN = 0;
    }

    showSuccess("FACP Initialized");

    printStatsSpacing();
    printStats("FACP Flags: ");
    printStats(to_string_convert((uint64_t)facp->Flags, 2));
    statNewLine();

    printStatsSpacing();
    printStats("SD Info: ");
    printStats(to_string((uint64_t)SCI_EN));
    statNewLine();

    printStatsSpacing();
    printStats("Reset address: 0x");
    printStats(to_hstring(facp->RESET_REG.Address));
    printStats(", Reset value: ");
    printStats(to_string((uint64_t)facp->RESET_VALUE));
    printStats(", Reset flag: ");
    reset_flag = (facp->Flags & (1 << 10)) >> 10;
    printStats(to_string((uint64_t)reset_flag));
    printStats(", Address space: ");
    printStats(to_string((uint64_t)facp->RESET_REG.AddressSpace));
    statNewLine();
  }

  __attribute__((noreturn))
  void Shutdown()
  {
    if (SCI_EN == 1) 
    {
      outw(PM1a_CNT, SLP_TYPa | SLP_EN);
      if (PM1b_CNT != 0)
        outw(PM1b_CNT, SLP_TYPb | SLP_EN);
    }

    outw (0xB004, 0x2000);

    const char* s = "Shutdown";
    while (*s) outb (0x8900, *(s++));
    while (true) asm ("cli; hlt");
  }

  __attribute__((noreturn))
  void Reboot()
  {
    // Check if reset via FADT RESET_REG is supported
    if (reset_flag)
    {
      switch (g_FACPHeader->RESET_REG.AddressSpace)
      {
        case ACPI::IO:
          outb(g_FACPHeader->RESET_REG.Address, g_FACPHeader->RESET_VALUE);
          break;

        case ACPI::MMIO:
          *((volatile uint8_t *) ((uintptr_t)g_FACPHeader->RESET_REG.Address)) = g_FACPHeader->RESET_VALUE;
          break;

        case ACPI::PCI:
          // TODO: Implement
          break;
      
        default:
          break;
      }
    }

    while (true) asm ("cli; hlt");
  }
}