#include <stddef.h>
#include "driver.h"
#include "../renderer/stat_logger.h"

namespace driver
{
  Driver::Driver()
  {
  }

  Driver::~Driver()
  {
  }

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
    showInfo("Loading drivers");
    for (uint64_t i = 0; i < num_of_drivers; i++)
    {
      if (drivers[i] != NULL)
        drivers[i]->activate();
    }
    showSuccess("Drivers loaded");
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
    if (index > (num_of_drivers - 1))
      return NULL;
    return drivers[index];
  }

  DriverManager g_DriverManager = DriverManager();
}
