#include "fdt_lib.h"
#include "fdt_lib_struct.h"
#include "fdt_lib_header.h"


const char *fdt_get_string(const void *fdt_blob, int offset)
{
    uint32_t string_block_offset;
    const char *str;

    string_block_offset = fdt_get_off_dt_strings(fdt_blob);
    str = (const char *) fdt_get_offset_in_blob(fdt_blob, string_block_offset + offset);

    return str;
}


/**
 * @brief Checks if the data at the given offset is a token.
 * 
 * @param fdt_blob Pointer to the beginning of the device tree in memory
 * @param offset A given offset in the device tree
 * 
 * @return 1 if there is a valid token at the offset; < 0 if there was an error
*/
static int is_offset_a_token_(const void *fdt_blob, int offset)
{
    uint32_t token;

    if (offset < 0 || (uint32_t) offset > fdt_get_totalsize(fdt_blob))
        return -FDT_ERR_BAD_ARG;

    token = convert_32_to_big_endian(fdt_get_offset_in_blob(fdt_blob, offset));
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
            return -FDT_ERR_UNKNOWN_TOKEN;
    }
    return token;
}


/**
 * @brief Get the token at the given offset.
 * 
 * @param fdt_blob Pointer to the beginning of the device tree in memory
 * @param offset The offset of the token in the device tree
 * 
 * @return token at given offset OR < 0 if there is no known token at that offset.
*/
static int fdt_get_token_(const void *fdt_blob, int offset)
{
    int token;
    token = is_offset_a_token_(fdt_blob, offset);
    return token;
}


/**
 * @brief Skip to the next token.
 * 
 * @param fdt_blob Pointer to the beginning of the device tree in memory
 * @param offset The offset of the token in the device tree
 * 
 * @return Offset of the next token, or < 0 if there was an error.
*/
static int fdt_skip_to_next_token_(const void *fdt_blob, int offset)
{
    if (!is_offset_a_token_(fdt_blob, offset)) return -FDT_ERR_BAD_ARG;

	int token;

    token = fdt_get_token_(fdt_blob, offset);
    offset += FDT_TOKEN_SIZE;

    switch (token) {
        case FDT_BEGIN_NODE: {
            char *p;
            do {
                p = (char *) fdt_get_offset_in_blob(fdt_blob, offset++);
            } while (p && (*p != '\0'));

            offset = FDT_ALIGN_ON(offset, FDT_TOKEN_SIZE);
            break;
        }
        case FDT_PROP: {
            struct fdt_property *prop;
            prop = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, offset);
            offset += (sizeof(uint32_t) * 2) + fdt_get_property_len(prop); 

            offset = FDT_ALIGN_ON(offset, FDT_TOKEN_SIZE);
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
            return -FDT_ERR_UNKNOWN_TOKEN;
        }
    } 

	return offset;
}


const struct fdt_property *fdt_get_property(const void *fdt_blob, int offset, int *err)
{
    if (fdt_get_token_(fdt_blob, offset) != FDT_PROP) {
        if (err) *err = -FDT_ERR_BAD_ARG;
        return 0;
    }

	struct fdt_property *property;
	property = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, offset + FDT_TOKEN_SIZE);
    if (err) *err = 0;
	return property;
}


uint32_t fdt_get_prop_len_by_offset(const void *fdt_blob, int offset)
{
    struct fdt_property *prop;
    prop = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, offset);
    return convert_32_to_big_endian(&prop->len);
}


uint32_t fdt_get_prop_nameoff_by_offset(const void *fdt_blob, int offset)
{
    struct fdt_property *prop;
    prop = (struct fdt_property *) fdt_get_offset_in_blob(fdt_blob, offset);
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


/**
 * @brief Get the offset of the first property for the device node at the given iteration.
 * 
 * Note: The offset in iter is NOT changed unless another property is found
 * 
 * @param iter FDT iterator object; holds the offset of the current device node in the device tree.
 * @return 1 if the node has a property; 0 if there are no properties; < 0 if there is an error
*/
static int fdt_first_property_(struct fdt_iter *iter)
{
    if (fdt_get_token_(iter->fdt_blob, iter->offset) != FDT_BEGIN_NODE) 
        return -FDT_ERR_BAD_ARG;

    int token, next_node_depth, offset;

    next_node_depth = 0;

    for (offset = iter->offset; 
        offset < iter->end_struct_block; 
        offset = fdt_skip_to_next_token_(iter->fdt_blob, offset)) {
        
        if (offset < 0) return -FDT_ERR_DEBUG_PARSER;

        token = fdt_get_token_(iter->fdt_blob, offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                switch (next_node_depth) {
                    case 0: {
                        next_node_depth++;
                        break;
                    }
                    case 1: {
                        // found a child node, no properties found
                        return 0;
                    }
                } /* end switch node_depth */

                break;
            }
            case FDT_PROP: {
                iter->offset = offset;
                iter->num_iterations++;
                return 1;
            }
            case FDT_END_NODE: {
                switch (next_node_depth) {
                    case 0: {
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case 1: {
                        // end of this node; there are no properties
                        return 0;
                    }
                } /* end switch node_depth */
                break;
            }
            case FDT_END: {
                return -FDT_ERR_BAD_STRUCTURE;
            }
            case FDT_NOP: {
                break;
            }
            default: {
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */
    }

    // should have reached an FDT_END token before getting here
    return -FDT_ERR_BAD_STRUCTURE;
}


/**
 * @brief Get the offset of the next property in the device tree.
 * 
 * Note: the offset of iterator object is NOT changed if there is an error. 
 * Otherwise, the offset points to the next token after the current property.
 * 
 * @param iter FDT iterator object; holds the offset of the current property.
 * @return 1 if another property is found; 0 if no more properties; < 0 if there is an error.
*/
int fdt_next_property_(struct fdt_iter *iter)
{
    if (fdt_get_token_(iter->fdt_blob, iter->offset) != FDT_PROP) 
        return -FDT_ERR_BAD_ARG;

    int token, offset;

    offset = iter->offset;

    for (offset = fdt_skip_to_next_token_(iter->fdt_blob, offset); 
        offset < iter->end_struct_block; 
        offset = fdt_skip_to_next_token_(iter->fdt_blob, offset)) {

        if (offset < 0) return -FDT_ERR_DEBUG_PARSER;

        token = fdt_get_token_(iter->fdt_blob, offset);

        switch (token) {
            case FDT_PROP: {
                // found next property
                iter->offset = offset;
                iter->num_iterations++;
                return 1;
            }
            case FDT_BEGIN_NODE: {
                // found first child node; no more properties
                return 0;
            }
            case FDT_END_NODE: {
                // reached the end of the node; no more properties
                return 0;
            }
            case FDT_NOP: {
                break;
            }
            case FDT_END: {
                return -FDT_ERR_BAD_STRUCTURE;
            }
            default: {
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */
    }

    // should have reached a FDT_END token before getting here
    return -FDT_ERR_BAD_STRUCTURE;
}


const char *fdt_get_node_name(const void *fdt_blob, int offset, int *err)
{
    if (fdt_get_token_(fdt_blob, offset) != FDT_BEGIN_NODE) {
        if (err) *err = -FDT_ERR_BAD_ARG;
        return 0;
    }

    char *node_name;
    node_name = (char *) fdt_get_offset_in_blob(fdt_blob, (offset + FDT_TOKEN_SIZE));
    if (err) *err = 0;

    return node_name;
}

int fdt_find_root(const void *fdt_blob)
{
    struct fdt_header header;
    int token;
    uint32_t offset;

    header.totalsize = fdt_get_totalsize(fdt_blob);
    header.off_dt_struct = fdt_get_off_dt_struct(fdt_blob);

    for (offset = header.off_dt_struct; offset < header.totalsize; offset += FDT_TOKEN_SIZE) {

        token = fdt_get_token_(fdt_blob, offset);

        switch (token) {
            case FDT_BEGIN_NODE:
                return offset;
            case FDT_NOP:
                break;
            default:
                return -FDT_ERR_BAD_STRUCTURE;
        }
    }

    return -FDT_ERR_NO_ROOT_NODE;
}


/**
 * @brief Get the offset of this node's first child node
 * 
 * Note: the offset of iterator object is only changed if a child node is found
 * 
 * @param iter FDT iterator object; holds the offset of the current node.
 * @return 1 if a child node is found; 0 if there are no more children left; < 0 if there is an error.
*/
static int fdt_first_child_node_(struct fdt_iter *iter)
{
    if (fdt_get_token_(iter->fdt_blob, iter->offset) != FDT_BEGIN_NODE) 
        return -FDT_ERR_BAD_ARG;

    int token, next_node_depth, offset;

    next_node_depth = 0;

    for (offset = iter->offset; 
        offset < iter->end_struct_block; 
        offset = fdt_skip_to_next_token_(iter->fdt_blob, offset)) {

        if (offset < 0) return -FDT_ERR_DEBUG_PARSER; 

        token = fdt_get_token_(iter->fdt_blob, offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                switch (next_node_depth) {
                    case 0: {
                        next_node_depth++;
                        break;
                    }
                    case 1: {
                        iter->offset = offset;
                        iter->num_iterations++;
                        return 1;
                    }
                } /* end switch node_depth */

                break;
            }
            case FDT_END_NODE: {
                switch (next_node_depth) {
                    case 0: {
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case 1: {
                        // no child nodes, end of current node representation
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
                return -FDT_ERR_BAD_STRUCTURE;
            }
            default: {
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */
    } /* end for */

    // should have reached FDT_END token before getting here
    return -FDT_ERR_BAD_STRUCTURE;
}


/**
 * @brief Get the offset of the sibling node (the next node representation in the device tree).
 * This is equivalent to getting the next node that is at the same depth in the tree as the current node.
 * 
 * Note: the offset of iterator object is NOT changed if there is an error. 
 * Otherwise, the offset points to the next token after the current child node.
 * 
 * @param iter FDT iterator object; holds the offset of the current node.
 * @return 1 if another node is found; 0 if there are no more sibling nodes left; < 0 if there is an error.
*/
int fdt_next_child_node_(struct fdt_iter *iter)
{
    if (fdt_get_token_(iter->fdt_blob, iter->offset) != FDT_BEGIN_NODE) 
        return -FDT_ERR_BAD_ARG;

    int token, next_node_depth, found, offset;

    next_node_depth = 0;
    found = 0;

    for (offset = iter->offset; 
        !found && offset < iter->end_struct_block; 
        offset = fdt_skip_to_next_token_(iter->fdt_blob, offset)) {

        if (offset < 0) return -FDT_ERR_DEBUG_PARSER;

        token = fdt_get_token_(iter->fdt_blob, offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                next_node_depth++;
                break;
            }
            case FDT_END_NODE: {
                next_node_depth--;

                switch (next_node_depth) {
                    case -1: {
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case 0: {
                        // found the end of the current node representation
                        found = 1;
                        break;
                    }
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
                return -FDT_ERR_BAD_STRUCTURE;
            }
            default: {
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */        
    }

    if (offset >= iter->end_struct_block) 
        return -FDT_ERR_BAD_STRUCTURE; // should have reached FDT_END token before getting here

    found = 0;

    // find the start of the next node representation (if any)
    for (; 
        !found && offset < iter->end_struct_block; 
        offset = fdt_skip_to_next_token_(iter->fdt_blob, offset)) {

        if (offset < 0) return -FDT_ERR_DEBUG_PARSER;

        token = fdt_get_token_(iter->fdt_blob, offset);

        switch (token) {
            case FDT_BEGIN_NODE: {
                // found the next node representation
                iter->offset = offset;
                iter->num_iterations++;
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
                return -FDT_ERR_BAD_STRUCTURE;
            }
            case FDT_END_NODE: {
                // end of parent node
                return 0;
            }
            default: {
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */
    }

    // should have reached FDT_END before getting here
    return -FDT_ERR_BAD_STRUCTURE;
}

void fdt_iter_init(struct fdt_iter *iter, uint32_t offset, fdt_iter_type_t type, const void *fdt_blob)
{
    iter->offset = offset;
    iter->end_struct_block = fdt_get_off_dt_struct(fdt_blob) + fdt_get_size_dt_struct(fdt_blob);
    iter->num_iterations = 0;
    iter->type = type;
    iter->fdt_blob = fdt_blob;
}


int fdt_iter_get_next(struct fdt_iter *iter)
{
    switch (iter->type) {
        case PROPERTIES: {
            switch (iter->num_iterations) {
                case 0:
                    return fdt_first_property_(iter);
                default:
                    return fdt_next_property_(iter);
            }
            break;
        }
        case CHILD_NODES:
            switch (iter->num_iterations) {
                case 0:
                    return fdt_first_child_node_(iter);
                default:
                    return fdt_next_child_node_(iter);
            }
            break;
    } /* end switch type */

    return -FDT_ERR_BAD_ARG;
}