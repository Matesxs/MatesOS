#pragma once

#include <stdint.h>

namespace driver
{
#define DRIVER_NUM 512

  class Driver
  {
  public:
    Driver();
    ~Driver();
    virtual bool activate();
  };

  class DriverManager
  {
  private:
    Driver *drivers[DRIVER_NUM];
    uint64_t num_of_drivers;

  public:
    DriverManager();
    void add_driver(Driver *driver);
    void activate_all();
    Driver *get_by_index(uint64_t index);
    uint64_t get_num_of_drivers();
  };

  extern DriverManager g_DriverManager;
}