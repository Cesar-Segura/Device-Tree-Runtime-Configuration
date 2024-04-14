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
    struct fdt_iter iter;
	const struct fdt_reserve_entry *entry;

    fdt_iter_init(&iter, 0, fdt_blob);

	iter.offset = fdt_get_off_mem_rsvmap(fdt_blob);

	printf("Printing Memory Reservation Block Contents:\n");

	do {
        entry = fdt_next_reserve_entry(&iter);
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


/**
 * @brief Iterate over a set of properties.
*/
static void sample_print_properties(struct fdt_iter *iter)
{
    int err;
    const struct fdt_property *prop;
    struct fdt_iter curr_iter;

    fdt_iter_dup(&curr_iter, iter);

    err = 0;
    for (err = fdt_first_property(&curr_iter);
        err > 0;
        err = fdt_next_property(&curr_iter))
    {
        prop = fdt_get_property(&curr_iter, &err);
        if (err == 0) {
           fdt_print_property(curr_iter.fdt_blob, prop); 
        } else {
            printf("ERROR: error code %d\n", -err);
            return;
        }
    }
}

static void sample_print_node(struct fdt_iter *curr_iter)
{
    int err;
    struct fdt_iter iter;
    
    fdt_iter_dup(&iter, curr_iter);

    /* Print name */
    DEBUG("Printing node name");
    err = 0;
    const char *node_name = fdt_get_node_name(&iter, &err);
    if (err == 0) {
        printf("%s\n", node_name);
    } else {
        printf("ERROR: error code %d\n", -err);
        return;
    }

    /* Print properties */
    DEBUG("Printing node properties");
    sample_print_properties(&iter); 
    printf("\n"); 


    /* Print child nodes */
    DEBUG("Printing subnodes");
    err = 0;
    for (err = fdt_first_child_node(&iter);
        err > 0;
        err = fdt_next_child_node(&iter))
    {
        sample_print_node(&iter);
    }
}


/**
 * Print the structure block of the device tree
*/
static void print_struct_block(const void *fdt_blob)
{
    // get the root node of the fdt
    int err;
    struct fdt_iter iter;    

    err = 0;
    if ((err = fdt_find_root(&iter, fdt_blob)) > 0) {
        // print the entire fdt recursively, starting with the root node
        sample_print_node(&iter);
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