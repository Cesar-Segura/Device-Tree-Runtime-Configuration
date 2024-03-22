#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "fdt_lib.h" 
#include "fdt_lib_header.h"
#include "fdt_lib_mem_rev_block.h"

struct fdt_memory_reserve_entry *fdt_get_mem_resv_block(const void *fdt_blob)
{
    uint32_t *address, *size, curr_offset; 
    struct fdt_memory_reserve_entry *entry_head, *curr_entry; 

    curr_offset = fdt_get_off_mem_rsvmap(fdt_blob);
    entry_head = (struct fdt_memory_reserve_entry *) malloc(sizeof(struct fdt_memory_reserve_entry)); 
    curr_entry = NULL; 

    do {
        address = (uint32_t *) fdt_get_offset_in_blob(fdt_blob, curr_offset); 
        curr_offset += sizeof(uint64_t);
        size = (uint32_t *) fdt_get_offset_in_blob(fdt_blob, curr_offset); 
        curr_offset += sizeof(uint64_t); 

        if (curr_entry == NULL) {
            entry_head->address = *address;
            entry_head->size = *size; 
            curr_entry = entry_head; 
        } else {
            curr_entry->next_entry = (struct fdt_memory_reserve_entry *) malloc(sizeof(struct fdt_memory_reserve_entry));
            curr_entry = curr_entry->next_entry;
            curr_entry->address = *address;
            curr_entry->size = *size;  
        }

    } while (*address != 0 || *size != 0); 

    return entry_head; 
}

void free_fdt_memory_reserve_entry(struct fdt_memory_reserve_entry *entry)
{
    if (entry == NULL) return;

    free(entry->next_entry);
    free(entry);
    entry = NULL; 
}

void fdt_print_mem_resv_block(struct fdt_memory_reserve_entry *entry)
{
    while (entry != NULL) {
        printf("Address: 0x%llx\n", entry->address);
        printf("Size: %llu\n", entry->size); 
        printf("\n"); 
        entry = entry->next_entry; 
    }
}

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