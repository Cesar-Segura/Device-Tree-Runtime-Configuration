#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
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
	iterator_t iter;
	const struct fdt_reserve_entry *entry;

	iter = fdt_get_off_mem_rsvmap(fdt_blob);

	printf("Printing Memory Reservation Block Contents:\n");

	do {
        entry = fdt_next_reserve_entry(fdt_blob, &iter);
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

    iterator_t prop_nameoff, prop_len;
    prop_nameoff = fdt_get_property_nameoff(prop);
    prop_len = fdt_get_property_len(prop);

    const char *prop_name;

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
        uint8_t *value = (uint8_t *) prop->value; 
        for (int i = 0; i < prop_len; i++) {
            printf("%02x", value[i]); 
        }
    }

    printf(";\n"); 
}


/**
 * @brief Iterate over a set of properties.
*/
static void sample_print_properties(const void *fdt_blob, iterator_t *node_offset)
{
    int is_first_prop, err, result;
    const struct fdt_property *prop;
    iterator_t curr_offset = *node_offset;

    is_first_prop = 1;
    err = 0;
    while ((result = fdt_next_property(fdt_blob, is_first_prop, &curr_offset) > 0)) {
        is_first_prop = 0;
        prop = fdt_get_property(fdt_blob, &curr_offset, &err);
        if (err == 0) {
            fdt_print_property(fdt_blob, prop);
        } else {
            printf("ERROR: error code %d\n", -err);
            return;
        }
    }

    if (result < 0) {
        printf("ERROR: error code %d\n", -err);
        return;
    }
}

static void sample_print_node(const void *fdt_blob, iterator_t *node_offset)
{
    int is_first_child_node, result;
    iterator_t curr_offset;
    
    curr_offset = *node_offset;

    DEBUG("Printing node name");
    /* Print name */
    int err = 0;
    const char *node_name = fdt_get_node_name(fdt_blob, &curr_offset, &err);
    if (err == 0) {
        printf("%s\n", node_name);
    } else {
        printf("ERROR: error code %d\n", -err);
        return;
    }

    DEBUG("Printing node properties");
    /* Print properties */
    sample_print_properties(fdt_blob, &curr_offset); 
    printf("\n"); 

    DEBUG("Printing subnodes");

    is_first_child_node = 1;

    while((result = fdt_next_child_node(fdt_blob, is_first_child_node, &curr_offset) > 0)) {
        is_first_child_node = 0;
        sample_print_node(fdt_blob, &curr_offset);
    }

    if (result < 0) {
        printf("ERROR: erro code %d\n", -result);
        return;
    }

    DEBUG("DONE Printing subnodes");
}


/**
 * Print the structure block of the device tree
*/
static void print_struct_block(const void *fdt_blob)
{
    // have this function as an init function for iterator
	iterator_t iter;
    struct fdt_header header;
    int token, root_node_not_found;

    header.totalsize = fdt_get_totalsize(fdt_blob);
    header.off_dt_struct = fdt_get_off_dt_struct(fdt_blob);
    header.off_dt_strings = fdt_get_off_dt_strings(fdt_blob);

    iter = header.off_dt_struct;

    printf("\nPrinting Structure Block Contents:\n\n");

    root_node_not_found = 1;
    do {
        token = fdt_get_token(fdt_blob, &iter);
        switch (token) {
            case FDT_NOP: {
                DEBUG("Found NOP token");
                break;
            }
            case FDT_BEGIN_NODE: {
                DEBUG("found root node");
                root_node_not_found = 0;
                break; 
            }
            default: {
                DEBUG("in default case");
                root_node_not_found = 0;
                break;
            }
        } /* end switch */
        iter += FDT_TOKEN_SIZE;
    } while (root_node_not_found);

    iter -= FDT_TOKEN_SIZE;
    // print the entire tree by printing the root node
    sample_print_node(fdt_blob, &iter);
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