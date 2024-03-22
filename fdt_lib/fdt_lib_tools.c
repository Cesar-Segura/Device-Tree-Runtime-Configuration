#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "fdt_lib.h"
#include "fdt_lib_tools.h"
#include "fdt_lib_header.h"

#define DEBUG_FLAG 0

#define FIRST_NODE 0
#define CHILD_NODE 1

void DEBUG(char *s)
{
    if (DEBUG_FLAG){
        printf("%s\n", s); 
    }
}

int fdt_get_next_token(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset)
{ 
    const void *curr_position = fdt_get_offset_in_blob(fdt_blob, curr_offset); // align?  
    uint32_t token = convert_32_to_big_endian(curr_position); 
    *next_offset = curr_offset + FDT_TOKEN_SIZE; 
    // *next_offset = FDT_ALIGN_ON(curr_offset + FDT_TOKEN_SIZE, FDT_TOKEN_SIZE);  

    return token; 
}

struct device_node_property *fdt_get_property(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset)
{
    DEBUG("fdt_get_property: initializing structs"); 
    uint32_t strings_block_offset = fdt_get_off_dt_strings(fdt_blob); 

    struct device_node_property *property = (struct device_node_property *) malloc(sizeof(struct device_node_property)); 
    property->next_property = NULL; 

    struct device_tree_property *dtp = (struct device_tree_property *) malloc(sizeof(struct device_tree_property)); 
    dtp->len = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, curr_offset));
    dtp->nameoff = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, curr_offset + sizeof(uint32_t)));
    curr_offset += sizeof(struct device_tree_property); // discrepancy in struct naming

    /* Set length of property value */
    property->length = dtp->len;

    DEBUG("fdt_get_property: setting property name"); 
    /* Set property name */
    uint32_t string_offset = strings_block_offset + dtp->nameoff;
    char *prop_ch; 
    property->name = (char *) malloc(sizeof(char) * 31); 
    int prop_offset = 0; 
    int prop_capacity = 31; 
    do {
        prop_ch = (char *) fdt_get_offset_in_blob(fdt_blob, string_offset++);

        /* realloc for longer string if necessary */
        if (prop_offset == prop_capacity) {
            prop_capacity *= 2; 
            property->name = (char *) realloc(property->name, sizeof(char) * prop_capacity); 
        }

        property->name[prop_offset++] = *prop_ch; 
    } while (prop_ch && (*prop_ch != '\0'));
    property->name[prop_offset] = '\0'; // null terminated string

    DEBUG("fdt_get_property: setting property value"); 
    /* Set property value */
    uint8_t *value_in_bytes = (uint8_t *) malloc(sizeof(uint8_t) * property->length); // good idea? 

    for (int i = 0; i < property->length; i++) {
        uint8_t *curr_val = (u_int8_t *) fdt_get_offset_in_blob(fdt_blob, curr_offset++); 
        value_in_bytes[i] = *curr_val;  
    }
    property->value = (void *) value_in_bytes; 

    /* Update the next offset in the blob */
    // *next_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE); 
    *next_offset = curr_offset; 

    free(dtp); 

    return property; 
}

struct device_tree_node *fdt_get_node(const void *fdt_blob, struct device_tree_node *parent, uint32_t curr_offset, uint32_t *next_offset) 
{
    int end_of_node_not_reached, token, node_depth;
    struct device_tree_node *curr_node; 

    curr_node = (struct device_tree_node *) malloc(sizeof(struct device_tree_node));
    curr_node->is_root_node = 0;  
    curr_node->node_name = NULL;
    curr_node->unit_address = NULL;
    curr_node->full_path = NULL; 
    curr_node->parent_node = parent;
    curr_node->child_node = NULL;
    curr_node->next_node = NULL; 
    curr_node->device_properties = NULL; 

    struct device_tree_node *curr_child_node = NULL;
    struct device_node_property *curr_property = NULL;  

    node_depth = FIRST_NODE; 
    end_of_node_not_reached = 1; 

    while (end_of_node_not_reached) {

        token = fdt_get_next_token(fdt_blob, curr_offset, next_offset);
        curr_offset = *next_offset; 

        switch (token) {
            case FDT_BEGIN_NODE: {

                DEBUG("FDT_BEGIN_NODE - child"); 
                
                switch (node_depth) {
                    case (FIRST_NODE): {
                        /* Set the node name */

                        DEBUG("FIRST_NODE - child"); 
                        char *ch; 
                        char *str = (char *) malloc(sizeof(char) * 31);
                        int str_offset = 0; 
                        int capacity = 31; 
                        do {
                            ch = (char *) fdt_get_offset_in_blob(fdt_blob, curr_offset++);

                            /* realloc for longer string if necessary */
                            if (str_offset == capacity) {
                                capacity *= 2; 
                                str = (char *) realloc(str, sizeof(char) * capacity); 
                            }

                            str[str_offset++] = *ch;
                        } while (ch && (*ch != '\0')); 
                        str[str_offset] = '\0'; // null terminated string

                        curr_node->node_name = str;

                        node_depth = CHILD_NODE; /* next encounter with FDT_BEGIN_NODE will be referring to a child node */
                        curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE);
                        break;
                    }
                    case (CHILD_NODE): {

                        DEBUG("CHILD_NODE - child"); 
                        curr_offset -= FDT_TOKEN_SIZE;
                        if (curr_child_node == NULL) {
                            /* Get the first child node */
                           curr_child_node = fdt_get_node(fdt_blob, curr_node, curr_offset, next_offset);
                           curr_offset = *next_offset;
                           curr_node->child_node = curr_child_node;  
                        } else {
                            /* Get the next child node */
                            curr_child_node->next_node = fdt_get_node(fdt_blob, curr_node, curr_offset, next_offset);
                            curr_offset = *next_offset;
                            curr_child_node = curr_child_node->next_node; 
                        }

                        break; 
                    }
                    default: {
                        printf("Error: reached default statement in FDT_BEGIN_NODE in fdt_get_node\n"); 
                        end_of_node_not_reached = 0; 
                    }
                } /* end switch node_depth */

                break; 
            }
            case FDT_NOP: {
                /* empty/removed property, skip */
                break; 
            }
            case FDT_PROP: {
                /* Get and store this property */
                DEBUG("FDT_PROP - child"); 
                if (curr_property == NULL) {
                    /* Get the first property */
                    curr_property = fdt_get_property(fdt_blob, curr_offset, next_offset);
                    curr_offset = *next_offset;
                    curr_node->device_properties = curr_property; 
                } else {
                    /* Get the next property */
                    curr_property->next_property = fdt_get_property(fdt_blob, curr_offset, next_offset);
                    curr_offset = *next_offset;
                    curr_property = curr_property->next_property; 
                }

                curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE);
                break; 
            }
            case FDT_END_NODE: {
                /* End of this node */
                end_of_node_not_reached = 0; 
                break;
            }
            case FDT_END: {
                /* Premature end since we are in a child node? */
                printf("Error: reached FDT_END from within a node that is not the root node, in fdt_get_node\n"); 
                end_of_node_not_reached = 0; 
                break; 
            }
            default: {
                printf("Error: reached default statement in fdt_get_node that should never be reached\n"); 
                end_of_node_not_reached = 0; 
            }
        } /* end switch end_of_node_not_reached */
    } /* end while */

    return curr_node; 
}

struct device_tree_node *fdt_get_root_node(const void *fdt_blob) 
{
    int end_of_tree_not_reached, token, node_depth; 
    uint32_t curr_offset;
    uint32_t *next_offset; 
    struct device_tree_node *root_node; 
    
    root_node = (struct device_tree_node *) malloc(sizeof(struct device_tree_node));
    root_node->is_root_node = 1;  
    root_node->node_name = NULL;
    root_node->unit_address = NULL;
    root_node->full_path = NULL; 
    root_node->parent_node = NULL;
    root_node->child_node = NULL;
    root_node->next_node = NULL; 
    root_node->device_properties = NULL; 

    struct device_tree_node *curr_child_node = NULL;
    struct device_node_property *curr_property = NULL;  

    node_depth = FIRST_NODE; 
    end_of_tree_not_reached = 1;

    curr_offset = fdt_get_off_dt_struct(fdt_blob);
    next_offset = (uint32_t *) malloc(sizeof(uint32_t)); 
    *next_offset = 0; 


    while (end_of_tree_not_reached) {

        token = fdt_get_next_token(fdt_blob, curr_offset, next_offset);
        curr_offset = *next_offset; 

        switch (token) {
            case FDT_BEGIN_NODE: {
                
                DEBUG("FDT_BEGIN_NODE - root"); 

                switch (node_depth) {
                    case (FIRST_NODE): {
                        /* Set the node name */

                        DEBUG("FDT_FIRST_NODE - root"); 

                        char *ch; 
                        char *str = (char *) malloc(sizeof(char) * 31);
                        int str_offset = 0; 
                        int capacity = 31; 
                        do {
                            ch = (char *) fdt_get_offset_in_blob(fdt_blob, curr_offset++);

                            /* realloc for longer string if necessary */
                            if (str_offset == capacity) {
                                capacity *= 2; 
                                str = (char *) realloc(str, sizeof(char) * capacity); 
                            }

                            str[str_offset++] = *ch;
                        } while (ch && (*ch != '\0')); 
                        str[str_offset] = '\0'; // null terminated string

                        root_node->node_name = str;

                        node_depth = CHILD_NODE; /* next encounter with FDT_BEGIN_NODE will be referring to a child node */
                        curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE);
                        break;
                    }
                    case (CHILD_NODE): {

                        DEBUG("FDT_CHILD_NODE - root"); 
                        curr_offset -= FDT_TOKEN_SIZE; 
                        if (curr_child_node == NULL) {
                            /* Get the first child node */
                           curr_child_node = fdt_get_node(fdt_blob, root_node, curr_offset, next_offset);
                           curr_offset = *next_offset;
                           root_node->child_node = curr_child_node;  
                        } else {
                            /* Get the next child node */
                            curr_child_node->next_node = fdt_get_node(fdt_blob, root_node, curr_offset, next_offset);
                            curr_offset = *next_offset;
                            curr_child_node = curr_child_node->next_node; 
                        }

                        break; 
                    }
                    default: {
                        printf("Error: reached default statement in FDT_BEGIN_NODE in fdt_get_node\n"); 
                        end_of_tree_not_reached = 0; 
                    }
                } /* end switch node_depth */

                break; 
            }
            case FDT_NOP: {
                /* empty/removed property, skip */
                break; 
            }
            case FDT_PROP: {
                DEBUG("FDT_PROP - root"); 
                /* Get and store this property */
                if (curr_property == NULL) {
                    /* Get the first property */
                    curr_property = fdt_get_property(fdt_blob, curr_offset, next_offset);
                    curr_offset = *next_offset;
                    root_node->device_properties = curr_property; 
                } else {
                    /* Get the next property */
                    curr_property->next_property = fdt_get_property(fdt_blob, curr_offset, next_offset);
                    curr_offset = *next_offset;
                    curr_property = curr_property->next_property; 
                }

                curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE);
                break; 
            }
            case FDT_END_NODE: {
                /* End of this node */
                printf("End of the root node reached\n"); 
                break;
            }
            case FDT_END: {
                /* End of device tree */
                end_of_tree_not_reached = 0; 
                break; 
            }
            default: {
                printf("Error: reached default statement in fdt_get_root_node that should never be reached\n"); 
                end_of_tree_not_reached = 0; 
            }
        } /* end switch end_of_node_not_reached */ 
    } /* end while */ 

    free(next_offset);

    return root_node; 
}

void fdt_print_properties(struct device_node_property *prop) 
{
    if (prop == NULL) return; 

    /* Print name */
    printf("\t%s = ", prop->name);

    /* Print value */
    if (strncmp(prop->name, "compatible", 11) == 0
       || strncmp(prop->name, "clock-names", 12) == 0
       || strncmp(prop->name, "device_type", 12) == 0
       || strncmp(prop->name, "secure-status", 14) == 0
       || strncmp(prop->name, "status", 7) == 0
       || strncmp(prop->name, "stdout-path", 12) == 0
       || strncmp(prop->name, "clock-output-names", 19) == 0) {
        // print string
        printf("\"%s\"", (char *) prop->value); 
    } else {
        // print bytes
        uint8_t *value = (uint8_t *) prop->value; 
        for (int i = 0; i < prop->length; i++) {
            printf("%02x", value[i]); 
        }
    }

    printf(";\n"); 
    
    /* Print next property */
    fdt_print_properties(prop->next_property); 
}

void fdt_print_node(struct device_tree_node *node)
{
    if (node == NULL) return ; 

    /* Print name */
    printf("%s\n", node->node_name);

    /* Print properties */
    fdt_print_properties(node->device_properties); 
    printf("\n"); 

    /* print subnodes */
    printf("\n");
    fdt_print_node(node->child_node); 

    /* print next node */
    printf("\n");
    fdt_print_node(node->next_node); 

    printf("\n"); 
}

void fdt_print_device_tree(const void *fdt_blob) 
{

    struct device_tree_node *root = fdt_get_root_node(fdt_blob);

    printf("\n\nPrinting device tree\n\n"); 
    fdt_print_node(root);

    free_device_tree(root); 

    printf("\n\nEnd printing device tree\n\n");  
}

void free_device_node_properties(struct device_node_property *prop)
{
    if (prop == NULL) return ; 
    free_device_node_properties(prop->next_property);
    free(prop->name);
    free(prop->value); 
    free(prop);
    prop = NULL; 
}

void free_device_tree_node(struct device_tree_node *node)
{
    if (node == NULL) return; 
    free_device_tree_node(node->child_node);
    free_device_tree_node(node->next_node);
    free_device_node_properties(node->device_properties); 
    free(node->full_path);
    free(node->unit_address);
    free(node->node_name); 
    free(node); 
    node = NULL; 
}

void free_device_tree(struct device_tree_node *root_node)
{
    if (root_node == NULL) return; 
    free_device_tree_node(root_node); 
}