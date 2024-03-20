#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "fdt_lib.h" 
#include "fdt_lib_header.h"

void fdt_parse_mem_resvblock(const void *fdt, uint32_t offset)
{
    // must ensure that this is located at an 8-byte aligned offset of the device tree blob
    struct fdt_reserve_entry *entry = (struct fdt_reserve_entry *) malloc(sizeof(struct fdt_reserve_entry));

    uint32_t curr_offset = offset; 
    entry->address = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, address)); 
    entry->size = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, size));

    uint32_t increment = (uint32_t) sizeof(uint64_t); 

    printf("\nPrinting contents of Memory Reservation Block\n");

    while (entry->address != 0 || entry->size != 0) {
        // process the struct (print for now, could store later)
        printf("Address: 0x%llx\n", entry->address);
        printf("Size: %llu\n", entry->size); 

        // point to the next struct in the list
        curr_offset += increment; 
        entry->address = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, address)); 
        entry->size = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, size)); 
    }

    free(entry); 
}