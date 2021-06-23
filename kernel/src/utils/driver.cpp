#include <stddef.h>
#include "driver.h"
#include "../renderer/stat_logger.h"
#include "../utils/helpers.h"
#include "../library/string.h"

namespace driver
{
  DriverManager g_DriverManager = DriverManager();

  Driver::Driver() {}

  Driver::~Driver(){}

  bool Driver::activate()
  {
    return false;
  }

  DriverManager::DriverManager()
  {
    num_of_drivers = 0;
  }

  void DriverManager::activate_all()
  {
    bool failed = false;

    showInfo("Loading drivers");
    printStatsSpacing();
    printStats(to_string(num_of_drivers));
    printStats(" drivers to load");
    statNewLine();

    for (uint64_t i = 0; i < num_of_drivers; i++)
    {
      if (drivers[i] != NULL)
      {
        if (!drivers[i]->activate()) failed = true;
      }
    }

    if (failed) showWarning("Drivers load failed");
    else showSuccess("Drivers loaded");
  }

  void DriverManager::add_driver(Driver *driver)
  {
    if (num_of_drivers == DRIVER_NUM)
      return;

    drivers[num_of_drivers] = driver;
    num_of_drivers++;
  }

  Driver *DriverManager::get_by_index(uint64_t index)
  {
    if (index > (num_of_drivers - 1)) return NULL;
    return drivers[index];
  }

  uint64_t DriverManager::get_num_of_drivers()
  {
    return num_of_drivers;
  }
}
