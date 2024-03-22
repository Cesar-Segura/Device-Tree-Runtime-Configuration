#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "fdt_lib.h" 

uint32_t fdt_get_header(const void *fdt, uint32_t field_offset)
{
    return convert_32_to_big_endian((const uint32_t *)((uint8_t *)fdt + field_offset));
}

uint64_t fdt_get_reserve_entry_header(const void *fdt, uint32_t field_offset)
{
    return convert_64_to_big_endian((const uint64_t *)((uint8_t *)fdt + field_offset)); 
}

uint32_t fdt_get_magic(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, magic)); 
}

uint32_t fdt_get_totalsize(const void *fdt) 
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, totalsize)); 
}

uint32_t fdt_get_off_dt_struct(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, off_dt_struct)); 
}

uint32_t fdt_get_off_dt_strings(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, off_dt_strings));
}

uint32_t fdt_get_off_mem_rsvmap(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, off_mem_rsvmap));
}

uint32_t fdt_get_version(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, version));
}

uint32_t fdt_get_last_comp_version(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, last_comp_version));
}

uint32_t fdt_get_boot_cpuid_phys(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, boot_cpuid_phys));
}

uint32_t fdt_get_size_dt_strings(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, size_dt_strings)); 
}

uint32_t fdt_get_size_dt_struct(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, size_dt_struct)); 
}

struct fdt_header *fdt_get_header_contents(const void *fdt)
{
    struct fdt_header *header = (struct fdt_header *) malloc(sizeof(struct fdt_header)); 

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

    return header;   
}

void print_fdt_header(struct fdt_header *header)
{
    // printf("normMagic: 0x%x\n", FDT_MAGIC);
    // printf("oppMagic: 0x%x\n", OPP_MAGIC); 
    
    printf("Printing Header of Device Tree\n"); 
    printf("Magic: 0x%x\n", header->magic);
    printf("Totalsize: %u\n", header->totalsize);
    printf("off_dt_struct: %u\n", header->off_dt_struct);
    printf("off_dt_strings: %u\n", header->off_dt_strings);
    printf("off_mem_rsvmap: %u\n", header->off_mem_rsvmap);
    printf("version: %u\n", header->version);
    printf("last_comp_version: %u\n", header->last_comp_version);
    printf("boot_cpuid_phys: %u\n", header->boot_cpuid_phys);
    printf("size_dt_strings: %u\n", header->size_dt_strings);
    printf("size_dt_struct: %u\n", header->size_dt_struct); 
}