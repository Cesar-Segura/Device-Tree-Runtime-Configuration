struct device_life_cycle_t {
    // defined in /kern/dev/interfaces/device.h 
};


struct device_driver_t {
    char *name;                         /* name of driver */
    // ^^used to match with the 'compatible' property of a device node in fdt

    struct device_life_cycle_t *life;   /* (L): device life-cycle interface (init, exit, ...) */
    void *call;                         /* (C): the interface provided */
    /**
     * ...
     * other info
     * ...
    */
};