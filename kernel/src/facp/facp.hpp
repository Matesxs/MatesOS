#pragma once

#include "stddef.h"
#include "stdint.h"
#include "../acpi/acpi.hpp"

namespace FACP
{
  void InitFACP(ACPI::FACPHeader *facp);
  void Shutdown();
  void Reboot();

  extern ACPI::FACPHeader *g_FACPHeader;
}