#include "drivers.h"
// include header file for some_driver.h
// for non pointer table: 
//      { .name = "driver name", .life = driver_life, .call = driver_call },

// global lookup table that matches strings to device_driver_t 
struct device_drivert_t *table[] = {
    &driver1,
    &driver2,
    &arm_pl011_driver
};

// char *tablename[] = {
//     "driver1",
//     "driver2",
//     "pl011"
// };
// use driver->name to match names to drivers in get_driver

struct device_driver_t *get_driver(char *name)
{
    return 0;
}