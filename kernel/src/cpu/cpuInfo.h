#pragma once

#include "cpuid.h"

namespace CPU
{
  struct CPUInfo
  {
    CPUIDVendorID VendorID;
    char VendorString[13];
    char BrandString[49];
    CPUIDFeatures features;
  };

  CPUInfo GetCPUInfo();

  extern CPUInfo g_CPUInfo;
}