#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "fdt_lib.h"
#include "fdt_lib_tools.h"
#include "fdt_lib_header.h"

#define DEBUG_FLAG 0

#define CURR_NODE 0
#define CHILD_NODE 1


static void DEBUG(char *s)
{
    if (DEBUG_FLAG){
        printf("%s\n", s); 
    }
}


/**
 * Strings
*/


const char *fdt_get_string(const void *fdt_blob, iterator_t iter)
{
    uint32_t string_block_offset;
    const char *str; 

    string_block_offset = fdt_get_off_dt_strings(fdt_blob);
    str = (const char *) fdt_get_offset_in_blob(fdt_blob, string_block_offset + iter);

    return str;
}


/**
 * Tokens
*/


int fdt_get_token(const void *fdt_blob, iterator_t *iter)
{
    const void *curr_position = fdt_get_offset_in_blob(fdt_blob, *iter); 
    uint32_t token = convert_32_to_big_endian(curr_position); 

    return token; 
}


static int _check_bad_token_offset(const void *fdt_blob, iterator_t *iter)
{
    uint32_t token;

    token = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, *iter));
    switch (token) {
        case FDT_PROP:
            break;
        case FDT_BEGIN_NODE:
            break;
        case FDT_END_NODE:
            break;
        case FDT_NOP:
            break;
        case FDT_END:
            break;
        default:
            return 0;
    }
    return 1;
}


int fdt_skip_to_next_token(const void *fdt_blob, iterator_t *iter)
{
    if (!_check_bad_token_offset(fdt_blob, iter)) return -FDT_ERR_BAD_ARG;

	int token;
    iterator_t curr_offset;

    curr_offset = *iter;
    token = fdt_get_token(fdt_blob, &curr_offset);
    curr_offset += FDT_TOKEN_SIZE;

    // skip the offset to the next token
    switch (token) {
        case FDT_BEGIN_NODE: {
            // skip the string
            char *p;
            do {
                p = (char *) fdt_get_offset_in_blob(fdt_blob, curr_offset++);
            } while (p && (*p != '\0'));

            // align to 4-byte boundary
            curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE);
            break;
        }
        case FDT_PROP: {
            // skip over the current property properties
            struct fdt_property *prop;
            prop = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, curr_offset);
            curr_offset += (sizeof(uint32_t) * 2) + fdt_get_property_len(prop); 
            // align to 4-byte boundary
            curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE);
            break;
        }
        case FDT_END: {
            break; 
        }
        case FDT_NOP: {
            break;
        }
        case FDT_END_NODE: {
            break;
        }
        default: {
            DEBUG("ERROR: unexpected value for token in fdt_skip_to_next_token");
            return -FDT_ERR_UNKNOWN_TOKEN;
        }
    } 

    *iter = curr_offset;
	return token;
}


/**
 * Properties
*/


const struct fdt_property *fdt_get_property(const void *fdt_blob, iterator_t *iter, int *err)
{
    if (!_check_bad_token_offset(fdt_blob, iter)) {
        *err = -FDT_ERR_BAD_ARG;
        return NULL;
    }

    // iter points to the token, double check that this token is an FDT_PROP token
    if (fdt_get_token(fdt_blob, iter) != FDT_PROP) {
        *err = -FDT_ERR_BAD_ARG;
        return NULL;
    }

	struct fdt_property *property;
	property = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, *iter + FDT_TOKEN_SIZE);
    *err = 0;
	return property;
}


uint32_t fdt_get_prop_len_by_offset(const void *fdt_blob, iterator_t *iter)
{
    struct fdt_property *prop;
    prop = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, *iter);
    return convert_32_to_big_endian(&prop->len);
}


uint32_t fdt_get_prop_nameoff_by_offset(const void *fdt_blob, iterator_t *iter)
{
    struct fdt_property *prop;
    prop = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, *iter);
    return convert_32_to_big_endian(&prop->nameoff);
}


uint32_t fdt_get_property_len(const struct fdt_property *prop)
{
    return convert_32_to_big_endian(&prop->len);
}


uint32_t fdt_get_property_nameoff(const struct fdt_property *prop)
{
    return convert_32_to_big_endian(&prop->nameoff);
}


static int fdt_first_property(const void *fdt_blob, iterator_t *iter)
{
    if (!_check_bad_token_offset(fdt_blob, iter)) return -FDT_ERR_BAD_ARG;

    // assert that pointer points to beginning of fdt_property representation
    if (fdt_get_token(fdt_blob, iter) != FDT_BEGIN_NODE) return -FDT_ERR_BAD_ARG;

    int token, node_depth;
    iterator_t curr_offset, prev_offset;

    node_depth = CURR_NODE;
    curr_offset = *iter;

    /* skip tokens until the first property token appears */
    /* if there are no properties, return 0; */
    do {
        prev_offset = curr_offset;
        token = fdt_skip_to_next_token(fdt_blob, &curr_offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                switch (node_depth) {
                    case CURR_NODE: {
                        // the next encounter with a FDT_BEGIN_NODE token will be a child node
                        node_depth = CHILD_NODE;
                        break;
                    }
                    case CHILD_NODE: {
                        // no properties found
                        return 0;
                    }
                    default: {
                        DEBUG("ERROR: unexpected value for node_depth in fdt_first_property (FDT_BEGIN_NODE)");
                        return 0;
                    }
                } /* end switch node_depth */
                /**
                 * if you reach a begin token twice, this implies that there are no properties
                */
                break;
            }
            case FDT_PROP: {
                *iter = prev_offset;
                return 1;
            }
            case FDT_END_NODE: {
                switch (node_depth) {
                    case CURR_NODE: {
                        // error with the structure of the device tree
                        // (FDT_END_NODE token coming before an expected FDT_BEGIN_TOKEN)
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case CHILD_NODE: {
                        // end of this node; there are no properties
                        return 0;
                    }
                    default: {
                        DEBUG("ERROR: unexpected value for node_depth in fdt_first_property (FDT_END_NODE)");
                        return 0;
                    }
                } /* end switch node_depth */
                break;
            }
            case FDT_END: {
                // error with the structure of the device tree 
                // (FDT_END token reached before an FDT_END_NODE token)
                return -FDT_ERR_BAD_STRUCTURE;
            }
            case FDT_NOP: {
                break;
            }
            default: {
                DEBUG("ERROR: unexpected value for token in fdt_first_property");
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */
    } while (token != FDT_PROP);

    return 1; // should never reach this case
}


int fdt_next_property(const void *fdt_blob, int is_first_prop, iterator_t *iter)
{
    if (is_first_prop) return fdt_first_property(fdt_blob, iter);

    if (!_check_bad_token_offset(fdt_blob, iter)) return -FDT_ERR_BAD_ARG;

    // assert that pointer points to beginning of fdt_property representation
    if (fdt_get_token(fdt_blob, iter) != FDT_PROP) return -FDT_ERR_BAD_ARG;

    int token;
    iterator_t prev_offset, curr_offset;

    /* skipping over the current property, get the offset of the next property */
    curr_offset = *iter;

    curr_offset += FDT_TOKEN_SIZE;
    uint32_t prop_value_len = fdt_get_prop_len_by_offset(fdt_blob, &curr_offset);

    curr_offset += (sizeof(uint32_t) * 2) + prop_value_len; // hard-coded?
    curr_offset = FDT_ALIGN_ON(curr_offset, FDT_TOKEN_SIZE);

    do {
        prev_offset = curr_offset;
        token = fdt_skip_to_next_token(fdt_blob, &curr_offset);

        switch (token) {
            case FDT_PROP: {
                // found next property
                *iter = prev_offset;
                return 1; 
            }
            case FDT_BEGIN_NODE: {
                // found a child property
                return 0;
            }
            case FDT_END_NODE: {
                // reached the end of the node
                return 0;
            }
            case FDT_NOP: {
                // shouldn't be here, but not necessarily detrimental to the structure
                break;
            }
            case FDT_END: {
                // error: bad fdt structure 
                // (FDT_END token found before end of node representation)
                return -FDT_ERR_BAD_STRUCTURE;
            }
            default: {
                DEBUG("ERROR: unexpected value for token in fdt_next_property");
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */

    } while (token != FDT_PROP);

    return 1; // should never reach this case
}


/**
 * Nodes
*/


const char *fdt_get_node_name(const void *fdt_blob, iterator_t *iter, int *err)
{
    if (!_check_bad_token_offset(fdt_blob, iter)) {
        *err = -FDT_ERR_BAD_ARG;
        return NULL;
    }
    /* ensure iter points to beginning of node */
    if (fdt_get_token(fdt_blob, iter) != FDT_BEGIN_NODE) {
        *err = -FDT_ERR_BAD_ARG;
        return NULL;
    }

    char *node_name;
    node_name = (char *) fdt_get_offset_in_blob(fdt_blob, (*iter + FDT_TOKEN_SIZE));
    *err = 0;
    return node_name;
}


static int fdt_first_child_node(const void *fdt_blob, iterator_t *iter)
{
    if (!_check_bad_token_offset(fdt_blob, iter)) return -FDT_ERR_BAD_ARG;

    // ensure that this offset points to the beginning of a node representation 
    if (fdt_get_token(fdt_blob, iter) != FDT_BEGIN_NODE) return -FDT_ERR_BAD_ARG;

    int token, node_depth, not_found;
    iterator_t prev_offset, curr_offset;

    curr_offset = *iter;
    not_found = 1;
    node_depth = CURR_NODE;

    do {
        prev_offset = curr_offset;
        token = fdt_skip_to_next_token(fdt_blob, &curr_offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                switch (node_depth) {
                    case CURR_NODE: {
                        // token for the beginning of the current node 
                        node_depth = CHILD_NODE;
                        break;
                    }
                    case CHILD_NODE: {
                        not_found = 0;
                        *iter = prev_offset;
                        return 1;
                    }
                    default: {
                        DEBUG("ERROR: unexpected value for node_depth in fdt_first_child_node (FDT_BEGIN_NODE)");
                        return 0;
                    }
                } /* end switch node_depth */
                break;
            }
            case FDT_END_NODE: {
                switch (node_depth) {
                    case CURR_NODE: {
                        // error: bad structure of fdt
                        // (found FDT_END_NODE token before an expected FDT_BEGIN_NODE token)
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case CHILD_NODE: {
                        // no child nodes, end of current node representation
                        return 0;
                    }
                    default: {
                        DEBUG("ERROR: unexpected value for node_depth in fdt_first_child_node (FDT_END_NODE)");
                        return 0;
                    }
                } /* end switch node_depth */
                break;
            }
            case FDT_PROP: {
                break;
            }
            case FDT_NOP: {
                break;
            }
            case FDT_END: {
                // error: bad structure of fdt
                // (found FDT_END before end of node representation - FDT_END_NODE)
                return -FDT_ERR_BAD_STRUCTURE;
            }
            default: {
                DEBUG("ERROR: unexpected value for token in fdt_first_child_node");
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */

    } while (not_found);

    return 1; // should never reach here
}


int fdt_next_child_node(const void *fdt_blob, int is_first_child_node, iterator_t *iter)
{
    if (is_first_child_node) return fdt_first_child_node(fdt_blob, iter);

    if (!_check_bad_token_offset(fdt_blob, iter)) return -FDT_ERR_BAD_ARG;

    // ensure that this offset points to the beginning of a node representation
    if (fdt_get_token(fdt_blob, iter) != FDT_BEGIN_NODE) return -FDT_ERR_BAD_ARG;

    /* get the sibling node of the node at offset iter */
    int token, next_node_depth, not_found;
    iterator_t prev_offset, curr_offset;

    next_node_depth = 0;
    curr_offset = *iter;
    not_found = 1;

    // find the end of the current node representation 
    do {
        prev_offset = curr_offset;
        token = fdt_skip_to_next_token(fdt_blob, &curr_offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                next_node_depth++;
                break;
            }
            case FDT_END_NODE: {
                next_node_depth--;

                switch (next_node_depth) {
                    case -1: {
                        // error: bad structure of fdt 
                        // (found FDT_END_NODE token before expected FDT_BEGIN_NODE token)
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case 0: {
                        // found the end of the current node representation
                        not_found = 0;
                        break;
                    }
                    default:
                        break;
                } /* end switch next_node_depth */
                break;
            }
            case FDT_NOP: {
                break;
            }
            case FDT_PROP: {
                break;
            }
            case FDT_END: {
                // error: bad structure of fdt
                // (premature end of node representation in fdt)
                return -FDT_ERR_BAD_STRUCTURE;
            }
            default: {
                DEBUG("ERROR: unexpected value for token in fdt_next_child_node (1st loop)");
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */
    } while (not_found);


    not_found = 1;
    next_node_depth = 0;

    // find the start of the next node representation (if any)
    do {
        prev_offset = curr_offset;
        token = fdt_skip_to_next_token(fdt_blob, &curr_offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                *iter = prev_offset;
                not_found = 0;
                return 1; 
            }
            case FDT_END: {
                // end of the fdt blob
                return 0;
            }
            case FDT_NOP: {
                break;
            }
            case FDT_PROP: {
                // error: bad structure of fdt
                // (found FDT_PROP, but not within a node representation)
                return -FDT_ERR_BAD_STRUCTURE;
            }
            case FDT_END_NODE: {
                // ambiguous
                // could be end of parent node representation, or
                // could be badly structured fdt if no parent is present 
                return 0;
            }
            default: {
                DEBUG("ERROR: unexpected value for token in fdt_next_child_node (2nd loop)");
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */
    } while (not_found);

    return 1; // should never reach here
}