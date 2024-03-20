#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "fdt_lib.h"
#include "fdt_lib_tools.h"


int fdt_get_next_token(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset)
{ 
    const void *curr_position = fdt_get_offset_in_blob(fdt_blob, curr_offset); // align? 
    uint32_t token = convert_32_to_big_endian(curr_position); 
    *next_offset = FDT_ALIGN_ON(curr_offset + FDT_TOKEN_SIZE, FDT_TOKEN_SIZE);  
    fflush(stdin); 

    return token; 
}

int fdt_get_next_property(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset)
{
    return 0; 
}

int fdt_get_next_node(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset)
{
    return 0; 
}