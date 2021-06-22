#pragma once

#include "stddef.h"
#include "stdint.h"
#include "../acpi/acpi.h"

namespace FACP
{
  void InitFACP(ACPI::FACPHeader *facp);
  void Shutdown();
  void Reboot();

  extern ACPI::FACPHeader *g_FACPHeader;
}