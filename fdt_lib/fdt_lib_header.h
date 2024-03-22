#ifndef _FDT_PARSE_LIB_HEADER_H_
#define _FDT_PARSE_LIB_HEADER_H_

/**
 * @brief Functions for accessing the values in the fdt header
*/
uint32_t fdt_get_header(const void *fdt, uint32_t field_offset);
uint64_t fdt_get_reserve_entry_header(const void *fdt, uint32_t field_offset);
uint32_t fdt_get_magic(const void *fdt);
uint32_t fdt_get_totalsize(const void *fdt) ;
uint32_t fdt_get_off_dt_struct(const void *fdt);
uint32_t fdt_get_off_dt_strings(const void *fdt);
uint32_t fdt_get_off_mem_rsvmap(const void *fdt);
uint32_t fdt_get_version(const void *fdt);
uint32_t fdt_get_last_comp_version(const void *fdt);
uint32_t fdt_get_boot_cpuid_phys(const void *fdt);
uint32_t fdt_get_size_dt_strings(const void *fdt);
uint32_t fdt_get_size_dt_struct(const void *fdt);

/**
 * @brief Get the contents of the header struct and store them in the return struct
*/
struct fdt_header *fdt_get_header_contents(const void *fdt);

/**
 * @brief Print the contents of the header struct
*/
void print_fdt_header(struct fdt_header *header);

#endif /* _FDT_PARSE_LIB_HEADER_H_ */