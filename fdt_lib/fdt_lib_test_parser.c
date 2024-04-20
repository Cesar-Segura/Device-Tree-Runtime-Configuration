#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "fdt_lib.h"
#include "fdt_lib_header.h"
#include "fdt_lib_mem_rev_block.h"
#include "fdt_lib_tools.h"

#define DEBUG_FLAG 0

static void DEBUG(char *s)
{
    if (DEBUG_FLAG){
        printf("%s\n", s); 
    }
}

/**
 * Print the contents of the device tree header
*/
static void print_header_contents(const void *fdt_blob)
{
    printf("Printing Header of Device Tree\n"); 
    printf("Magic: 0x%x\n", fdt_get_magic(fdt_blob));
    printf("Totalsize: %u\n", fdt_get_totalsize(fdt_blob));
    printf("off_dt_struct: %u\n", fdt_get_off_dt_struct(fdt_blob));
    printf("off_dt_strings: %u\n", fdt_get_off_dt_strings(fdt_blob));
    printf("off_mem_rsvmap: %u\n", fdt_get_off_mem_rsvmap(fdt_blob));
    printf("version: %u\n", fdt_get_version(fdt_blob));
    printf("last_comp_version: %u\n", fdt_get_last_comp_version(fdt_blob));
    printf("boot_cpuid_phys: %u\n", fdt_get_boot_cpuid_phys(fdt_blob));
    printf("size_dt_strings: %u\n", fdt_get_size_dt_strings(fdt_blob));
    printf("size_dt_struct: %u\n", fdt_get_size_dt_struct(fdt_blob)); 
	printf("\n");
}

/**
 * Print the contents of the memory reservation block 
*/
static void print_mem_resv_block(const void *fdt_blob)
{
    int offset;
	const struct fdt_reserve_entry *entry;

    offset = fdt_get_off_mem_rsvmap(fdt_blob);

	printf("Printing Memory Reservation Block Contents:\n");

	do {
        entry = fdt_next_reserve_entry(fdt_blob, &offset);
        printf("Address: 0x%llx\n", fdt_get_resv_entry_addr(entry));
        printf("Size: %llu\n", fdt_get_resv_entry_size(entry)); 
	} while (entry->address != 0 || entry->size != 0);
}


/**
 * @brief Print a single property.
*/
static void fdt_print_property(const void *fdt_blob, const struct fdt_property *prop) 
{
    if (prop == NULL) return; 

    uint32_t prop_nameoff, prop_len;
    const char *prop_name;

    // Get the property name
    prop_nameoff = fdt_get_property_nameoff(prop);
    prop_name = fdt_get_string(fdt_blob, prop_nameoff);

    /* Print name */
    printf("\t%s = ", prop_name);

    /* Print value */
    if (strncmp(prop_name, "compatible", 11) == 0
       || strncmp(prop_name, "clock-names", 12) == 0
       || strncmp(prop_name, "device_type", 12) == 0
       || strncmp(prop_name, "secure-status", 14) == 0
       || strncmp(prop_name, "status", 7) == 0
       || strncmp(prop_name, "stdout-path", 12) == 0
       || strncmp(prop_name, "clock-output-names", 19) == 0) {
        // print string
        printf("\"%s\"", (char *) prop->value); 
    } else {
        // print bytes
        prop_len = fdt_get_property_len(prop);
        uint8_t *value = (uint8_t *) prop->value; 
        for (int i = 0; i < prop_len; i++) {
            printf("%02x", value[i]); 
        }
    }

    printf(";\n"); 
}


static void sample_print_node(const void *fdt_blob, int offset)
{
    struct fdt_iter prop_iter, node_iter;
    const struct fdt_property *prop;
    int err;

    err = 0;
    fdt_iter_init(&prop_iter, offset, PROPERTIES, fdt_blob);
    fdt_iter_init(&node_iter, offset, CHILD_NODES, fdt_blob);

    /* Print name */
    const char *node_name = fdt_get_node_name(fdt_blob, offset, &err);
    if (err == 0) {
        printf("%s\n", node_name);
    } else {
        printf("ERROR: error code %d\n", -err);
        return;
    }

    /* Print properties */
    for (err = fdt_iter_get_next(&prop_iter);
        err > 0;
        err = fdt_iter_get_next(&prop_iter)) {
        
        prop = fdt_get_property(fdt_blob, prop_iter.offset, &err);
        if (err == 0) {
           fdt_print_property(fdt_blob, prop); 
        } else {
            printf("ERROR: error code %d\n", -err);
            return;
        }
    }

    printf("\n");  // spacer 


    /* Print child nodes */
    for (err = fdt_iter_get_next(&node_iter);
        err > 0;
        err = fdt_iter_get_next(&node_iter)) {
        sample_print_node(fdt_blob, node_iter.offset);
    }
}


/**
 * Print the structure block of the device tree
*/
static void print_struct_block(const void *fdt_blob)
{
    int offset;
    // get the root node of the fdt
    if ((offset = fdt_find_root(fdt_blob)) > 0) {
        // print the entire fdt recursively, starting with the root node
        sample_print_node(fdt_blob, offset);
    } else {
        printf("Error: no root node found in fdt\n");
    }
}

int main(int argc, char **argv)
{ 
    if (argc != 2) {
        printf("Usage: ./parser <dtb_file_name> \n"); 
        return 1;
    }

    FILE *file = fopen(argv[1], "rb"); 
    if (file == NULL) {
        perror("Error opening file"); 
        return 1; 
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET); 

    char *buffer = (char *) malloc(file_size); 
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return 1; 
    }

    fread(buffer, 1, file_size, file);
    fclose(file); 

    const void *fdt_blob = (const void *) buffer; 

	print_header_contents(fdt_blob);
	print_mem_resv_block(fdt_blob);
	print_struct_block(fdt_blob); 

    /* Cleanup */
    fflush(stdin); 
    free(buffer);
    return 0; 
}