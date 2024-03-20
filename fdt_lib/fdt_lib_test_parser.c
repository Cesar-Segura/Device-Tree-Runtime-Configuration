#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "fdt_lib.h"
#include "fdt_lib_header.h"
#include "fdt_lib_mem_rev_block.h"
#include "fdt_lib_tools.h"

int test_fdt_header_functions(const void *fdt_blob)
{
    struct fdt_header *header = fdt_get_header_contents(fdt_blob);
    print_fdt_header(header);
    // use the numbers to check that the header is correct
    free(header); 
    return 0; 
}

int test_fdt_get_token(const void *fdt_blob)
{
    /* Test to see if we can get a begin node tag: */
    uint32_t struc_block_offset = fdt_get_off_dt_struct(fdt_blob);
    uint32_t *next_offset = malloc(sizeof(uint32_t)); 

    int token = fdt_get_next_token(fdt_blob, struc_block_offset, next_offset);
    printf("Token: %d\n", token);

    switch (token) {
        case FDT_BEGIN_NODE: {
            /* Get the string (node name) that comes after the begin node */
            char *p; 
            char *str_buffer = (char *) malloc(sizeof(char) * 50); // make room for a string of size 50
            int off = 0; 
            do {
                uint32_t offset = *(next_offset++);
                p = (char *) fdt_get_offset_in_blob(fdt_blob, offset);
                str_buffer[off++] = *p; 
            } while (p && (*p != '\0')); 
            str_buffer[off] = '\0';

            printf("Node name string: %s\n", str_buffer); 

            /* cleanup */
            free(str_buffer); 
            break; 
        }
        case FDT_PROP: {
            struct device_tree_property *dtp = (struct device_tree_property *) malloc(sizeof(struct device_tree_property)); 
            dtp->len = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, *next_offset)); 
            printf("dtp->len: %u\n", dtp->len); 
            dtp->nameoff = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, *next_offset)); 
            printf("dtp->nameoff: %u\n", dtp->nameoff); 

            /* cleanup */
            free (dtp);
            break;
        }
        case FDT_END: {
            break; 
        } /* end switch */
    } 
    return 0; 
}


int test_fdt_get_one_node(const void *fdt_blob)
{
    int token; 

    int end_node_not_reached = 1;

    uint32_t struct_block_offset = fdt_get_off_dt_struct(fdt_blob);
    uint32_t strings_block_offset = fdt_get_off_dt_strings(fdt_blob); 

    uint32_t *next_offset = malloc(sizeof(uint32_t));
    uint32_t curr_offset = struct_block_offset; 

    while (end_node_not_reached) {
        token = fdt_get_next_token(fdt_blob, curr_offset, next_offset); 
        curr_offset = *next_offset; 

        printf("token: %d\n", token); 

        switch (token) {
            case FDT_BEGIN_NODE: {
                printf("FDT_BEGIN_NODE: \n"); 
                /* Get the string (node name) that comes after the begin node */
                char *p; 
                printf("Node name: "); 
                do {
                    p = (char *) fdt_get_offset_in_blob(fdt_blob, curr_offset++);
                    printf("%c", *p);
                } while (p && (*p != '\0')); 
                printf("\n"); 

                break; 
            }
            case FDT_PROP: {
                printf("FDT_PROP:\n"); 
                struct device_tree_property *dtp = (struct device_tree_property *) malloc(sizeof(struct device_tree_property)); 
                dtp->len = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, curr_offset)); 
                // printf("dtp->len: %u\n", dtp->len); 
                dtp->nameoff = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, curr_offset + sizeof(uint32_t))); 
                // printf("dtp->nameoff: %u\n", dtp->nameoff); 

                /* print the property name */
                printf("Property name: "); 
                uint32_t string_offset = strings_block_offset + dtp->nameoff; 
                char *ch; 
                do {
                    ch = (char *) fdt_get_offset_in_blob(fdt_blob, string_offset++);
                    printf("%c", *ch); 
                } while (ch && (*ch != '\0')); 
                printf("\n"); 

                curr_offset += sizeof(struct device_tree_property);
                /* print the property value */
                printf("Property value: "); 
                // char *ch; 
                for (int i = 0; i < dtp->len; i++) {
                    // could have a problem printing the bytes in this way 
                    ch = (char *) fdt_get_offset_in_blob(fdt_blob, curr_offset++); 
                    printf("%02x", *ch); 
                }
                printf("\n"); 

                /* cleanup */
                free (dtp);

                break;
            }
            case FDT_END_NODE: {
                printf("FDT_END_NODE:\n"); 
                end_node_not_reached = 0; 
                break; 
            }
            case FDT_NOP: {
                printf("FDT_NOP:\n");
                break; 
            }
            case FDT_END: {
                printf("FDT_END:\n");
                printf("Prematurely reached end of struct block before end of node \n"); 
                end_node_not_reached = 0;
                break; 
            }
            default: {
                printf("Error: reached default, so no tag matched\n"); 
                end_node_not_reached = 0; 
                break; 
            }
        } /* end switch */

        curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE); 
    } /* end while */

    return 0; 
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

    /* Test header functions */
    if (test_fdt_header_functions(fdt_blob)) {
        printf("Test failed: test_fdt_header_functions\n"); 
    }

    /* Test fdt_lib functions */
    // if (test_fdt_get_token(fdt_blob)) {
    //     printf("Test failed: test_fdt_get_token\n"); 
    // }

    test_fdt_get_one_node(fdt_blob); 
 
    // fdt_parse_mem_resvblock(fdt_blob, header->off_mem_rsvmap); 


    /* Cleanup */
    fflush(stdin); 
    free(buffer);
    return 0; 
}