#include "fdt_lib.h" 
#include "fdt_lib_mem_rev.h"

const struct fdt_reserve_entry *fdt_next_reserve_entry(const void *fdt_blob, int *offset)
{
    struct fdt_reserve_entry *entry;
    entry = (struct fdt_reserve_entry *) fdt_get_offset_in_blob(fdt_blob, *offset);
    *offset += sizeof(struct fdt_reserve_entry);
    return entry;
}


uint64_t fdt_get_resv_entry_addr(const struct fdt_reserve_entry *entry)
{
    return convert_64_to_big_endian(&entry->address);
}


uint64_t fdt_get_resv_entry_size(const struct fdt_reserve_entry *entry)
{
    return convert_64_to_big_endian(&entry->size); 
}