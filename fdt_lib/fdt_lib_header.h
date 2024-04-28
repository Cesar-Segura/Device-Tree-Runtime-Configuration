#ifndef _FDT_LIB_HEADER_H_
#define _FDT_LIB_HEADER_H_

/**
 * @brief Functions for accessing the values in the fdt header
 * 
 * @param fdt pointer to the beginning of the device tree
*/

/** @brief Get the "magic" field of the fdt_header struct. */
uint32_t fdt_get_magic(const void *fdt);
/** @brief Get the "totalsize" field of the fdt_header struct. */
uint32_t fdt_get_totalsize(const void *fdt);
/** @brief Get the "off_dt_struct" field of the fdt_header struct. */
uint32_t fdt_get_off_dt_struct(const void *fdt);
/** @brief Get the "off_dt_strings" field of the fdt_header struct. */
uint32_t fdt_get_off_dt_strings(const void *fdt);
/** @brief Get the "off_mem_rsvmap" field of the fdt_header struct. */
uint32_t fdt_get_off_mem_rsvmap(const void *fdt);
/** @brief Get the "version" field of the fdt_header struct. */
uint32_t fdt_get_version(const void *fdt);
/** @brief Get the "last_comp_version" field of the fdt_header struct. */
uint32_t fdt_get_last_comp_version(const void *fdt);
/** @brief Get the "boot_cpuid_phys" field of the fdt_header struct. */
uint32_t fdt_get_boot_cpuid_phys(const void *fdt);
/** @brief Get the "size_dt_strings" field of the fdt_header struct. */
uint32_t fdt_get_size_dt_strings(const void *fdt);
/** @brief Get the "size_dt_struct" field of the fdt_header struct. */
uint32_t fdt_get_size_dt_struct(const void *fdt);

/**
 * @brief Get the contents of the header struct and store them in the return struct
 * 
 * @param fdt pointer to beginning of fdt in memory
 * @param header pointer to a (unpopulated) fdt_header struct
*/
void fdt_get_header_contents(const void *fdt, struct fdt_header *header);

#endif /* _FDT_LIB_HEADER_H_ */