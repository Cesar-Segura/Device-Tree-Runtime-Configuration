#include "fdt_lib.h"
#include "fdt_lib_header.h"

#define attr_size(header, attr) (sizeof( ((header *)0)->attr ))

/**
 * @brief Get a fdt_header attribute.
 * 
 * @param fdt pointer to beginning of fdt in memory
 * @param field_offset offset of the attribute within the fdt_header
*/
static uint32_t fdt_get_header(const void *fdt, uint32_t field_offset)
{
    return convert_32_to_big_endian((const uint32_t *)((uint8_t *)fdt + field_offset));
}

uint32_t fdt_get_magic(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, magic) * 0); 
}

uint32_t fdt_get_totalsize(const void *fdt) 
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, totalsize) * 1); 
}

uint32_t fdt_get_off_dt_struct(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, off_dt_struct) * 2); 
}

uint32_t fdt_get_off_dt_strings(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, off_dt_strings) * 3);
}

uint32_t fdt_get_off_mem_rsvmap(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, off_mem_rsvmap) * 4);
}

uint32_t fdt_get_version(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, version) * 5);
}

uint32_t fdt_get_last_comp_version(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, last_comp_version) * 6);
}

uint32_t fdt_get_boot_cpuid_phys(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, boot_cpuid_phys) * 7);
}

uint32_t fdt_get_size_dt_strings(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, size_dt_strings) * 8); 
}

uint32_t fdt_get_size_dt_struct(const void *fdt)
{
    return fdt_get_header(fdt, attr_size(struct fdt_header, size_dt_struct) * 9); 
}

void fdt_get_header_contents(const void *fdt, struct fdt_header *header)
{
    header->magic = fdt_get_magic(fdt);
    header->totalsize = fdt_get_totalsize(fdt);
    header->boot_cpuid_phys = fdt_get_boot_cpuid_phys(fdt);
    header->last_comp_version = fdt_get_last_comp_version(fdt);
    header->off_dt_strings = fdt_get_off_dt_strings(fdt);
    header->off_dt_struct = fdt_get_off_dt_struct(fdt);
    header->off_mem_rsvmap = fdt_get_off_mem_rsvmap(fdt);
    header->size_dt_strings = fdt_get_size_dt_strings(fdt);
    header->size_dt_struct = fdt_get_size_dt_struct(fdt);
    header->version = fdt_get_version(fdt);
}