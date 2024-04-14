#include "fdt_lib.h"
#include "fdt_lib_tools.h"
#include "fdt_lib_header.h"


#define CURR_NODE 0
#define CHILD_NODE 1


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
 * @param iter FDT iterator object; holds the offset of the token.
 * 
 * @return 1 if there is a valid token at the offset; -FDT_ERR_UNKNOWN_TOKEN otherwise.
*/
static int is_offset_a_token_(struct fdt_iter *iter)
{
    uint32_t token;

    token = convert_32_to_big_endian(fdt_get_offset_in_blob(iter->fdt_blob, iter->offset));
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
 * @param iter FDT iterator object; holds the offset of the token.
 * 
 * @return token at given offset OR < 0 if there is no known token at that offset.
*/
static int fdt_get_token(struct fdt_iter *iter)
{
    int token;
    token = is_offset_a_token_(iter);
    return token;
}


/**
 * @brief Get the token at the given offset, and skip to the offset of the next token.
 * 
 * @param iter FDT iterator object; holds the offset of the token.
 * 
 * @return A valid token (> 0) OR (< 0) if an error occured.
*/
static int fdt_skip_to_next_token(struct fdt_iter *iter)
{
    if (!is_offset_a_token_(iter)) return -FDT_ERR_BAD_ARG;

	int token;
    struct fdt_iter curr_iter;

    fdt_iter_dup(&curr_iter, iter);

    token = fdt_get_token(&curr_iter);
    curr_iter.offset += FDT_TOKEN_SIZE;

    switch (token) {
        case FDT_BEGIN_NODE: {
            char *p;
            do {
                p = (char *) fdt_get_offset_in_blob(curr_iter.fdt_blob, curr_iter.offset++);
            } while (p && (*p != '\0'));

            curr_iter.offset = FDT_ALIGN_ON(curr_iter.offset, FDT_TOKEN_SIZE);
            break;
        }
        case FDT_PROP: {
            struct fdt_property *prop;
            prop = (struct fdt_property *) fdt_get_offset_in_blob(curr_iter.fdt_blob, curr_iter.offset);
            curr_iter.offset += (sizeof(uint32_t) * 2) + fdt_get_property_len(prop); 

            curr_iter.offset = FDT_ALIGN_ON(curr_iter.offset, FDT_TOKEN_SIZE);
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

    fdt_iter_dup(iter, &curr_iter);
	return token;
}


const struct fdt_property *fdt_get_property(struct fdt_iter *iter, int *err)
{
    if (fdt_get_token(iter) != FDT_PROP) {
        if (err) *err = -FDT_ERR_BAD_ARG;
        return 0;
    }

	struct fdt_property *property;
	property = (struct fdt_property *) fdt_get_offset_in_blob(iter->fdt_blob, iter->offset + FDT_TOKEN_SIZE);
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


int fdt_first_property(struct fdt_iter *iter)
{
    if (fdt_get_token(iter) != FDT_BEGIN_NODE) return -FDT_ERR_BAD_ARG;

    int token, next_begin_node_depth;
    struct fdt_iter curr_iter;

    next_begin_node_depth = CURR_NODE;
    fdt_iter_init(&curr_iter, iter->offset, iter->fdt_blob);

    do {
        token = fdt_get_token(&curr_iter);

        switch (token) {
            case FDT_BEGIN_NODE: {
                switch (next_begin_node_depth) {
                    case CURR_NODE: {
                        next_begin_node_depth = CHILD_NODE;
                        break;
                    }
                    case CHILD_NODE: {
                        // found a child node, no properties found
                        fdt_iter_dup(iter, &curr_iter);
                        return 0;
                    }
                } /* end switch node_depth */

                break;
            }
            case FDT_PROP: {
                fdt_iter_dup(iter, &curr_iter);
                return 1;
            }
            case FDT_END_NODE: {
                switch (next_begin_node_depth) {
                    case CURR_NODE: {
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case CHILD_NODE: {
                        // end of this node; there are no properties
                        fdt_iter_dup(iter, &curr_iter);
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

        fdt_skip_to_next_token(&curr_iter);        
    } while (token != FDT_PROP);

    fdt_iter_dup(iter, &curr_iter);
    return 0;
}


int fdt_next_property(struct fdt_iter *iter)
{
    if (fdt_get_token(iter) != FDT_PROP) return -FDT_ERR_BAD_ARG;

    int token;
    struct fdt_iter curr_iter;

    fdt_iter_dup(&curr_iter, iter);

    // skip over the contents of the current property
    curr_iter.offset += FDT_TOKEN_SIZE;
    uint32_t prop_value_len = fdt_get_prop_len_by_offset(curr_iter.fdt_blob, curr_iter.offset);

    curr_iter.offset += (sizeof(uint32_t) * 2) + prop_value_len; 
    curr_iter.offset = FDT_ALIGN_ON(curr_iter.offset, FDT_TOKEN_SIZE);

    do {
        token = fdt_get_token(&curr_iter);

        switch (token) {
            case FDT_PROP: {
                // found next property
                fdt_iter_dup(iter, &curr_iter);
                return 1;
            }
            case FDT_BEGIN_NODE: {
                // found first child node; no more properties
                fdt_iter_dup(iter, &curr_iter);
                return 0;
            }
            case FDT_END_NODE: {
                // reached the end of the node; no more properties
                fdt_iter_dup(iter, &curr_iter);
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

        fdt_skip_to_next_token(&curr_iter);
    } while (token != FDT_PROP);

    fdt_iter_dup(iter, &curr_iter);
    return 0;
}


const char *fdt_get_node_name(struct fdt_iter *iter, int *err)
{
    if (fdt_get_token(iter) != FDT_BEGIN_NODE) {
        if (err) *err = -FDT_ERR_BAD_ARG;
        return 0;
    }

    char *node_name;
    node_name = (char *) fdt_get_offset_in_blob(iter->fdt_blob, (iter->offset + FDT_TOKEN_SIZE));
    if (err) *err = 0;

    return node_name;
}

int fdt_find_root(struct fdt_iter *iter, const void *fdt_blob)
{
    struct fdt_header header;
    struct fdt_iter curr_iter;
    int token;

    fdt_iter_init(&curr_iter, 0, fdt_blob);
    header.totalsize = fdt_get_totalsize(fdt_blob);
    header.off_dt_struct = fdt_get_off_dt_struct(fdt_blob);

    for (curr_iter.offset = header.off_dt_struct; 
        curr_iter.offset < header.totalsize; 
        curr_iter.offset += FDT_TOKEN_SIZE) 
    {
        token = fdt_get_token(&curr_iter);

        switch (token) {
            case FDT_BEGIN_NODE: {
                fdt_iter_dup(iter, &curr_iter);
                return 1;
            }
            case FDT_NOP: {
                break;
            }
            default: {
                return -FDT_ERR_NO_ROOT_NODE;
            }
        }
    }

    return -FDT_ERR_NO_ROOT_NODE;
}


int fdt_first_child_node(struct fdt_iter *iter)
{
    if (fdt_get_token(iter) != FDT_BEGIN_NODE) return -FDT_ERR_BAD_ARG;

    int token, next_begin_node_depth, not_found;
    struct fdt_iter curr_iter;

    fdt_iter_dup(&curr_iter, iter);

    not_found = 1;
    next_begin_node_depth = CURR_NODE;

    do {
        token = fdt_get_token(&curr_iter);

        switch (token) {
            case FDT_BEGIN_NODE: {
                switch (next_begin_node_depth) {
                    case CURR_NODE: {
                        next_begin_node_depth = CHILD_NODE;
                        break;
                    }
                    case CHILD_NODE: {
                        not_found = 0;
                        fdt_iter_dup(iter, &curr_iter);
                        return 1;
                    }
                } /* end switch node_depth */

                break;
            }
            case FDT_END_NODE: {
                switch (next_begin_node_depth) {
                    case CURR_NODE: {
                        return -FDT_ERR_BAD_STRUCTURE;
                    }
                    case CHILD_NODE: {
                        // no child nodes, end of current node representation
                        fdt_iter_dup(iter, &curr_iter);
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

        fdt_skip_to_next_token(&curr_iter);
    } while (not_found);

    fdt_iter_dup(iter, &curr_iter);
    return 0;
}


int fdt_next_child_node(struct fdt_iter *iter)
{
    if (fdt_get_token(iter) != FDT_BEGIN_NODE) return -FDT_ERR_BAD_ARG;

    int token, next_node_depth, not_found;
    struct fdt_iter curr_iter;

    next_node_depth = 0;
    fdt_iter_init(&curr_iter, iter->offset, iter->fdt_blob);
    not_found = 1;

    // find the end of the current node representation 
    do {
        token = fdt_get_token(&curr_iter);

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
                        not_found = 0;
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

        fdt_skip_to_next_token(&curr_iter);
    } while (not_found);

    not_found = 1;

    // find the start of the next node representation (if any)
    do {
        token = fdt_get_token(&curr_iter);

        switch (token) {
            case FDT_BEGIN_NODE: {
                // found the next node representation
                fdt_iter_dup(iter, &curr_iter);
                not_found = 0;
                return 1; 
            }
            case FDT_END: {
                // end of the fdt blob
                fdt_iter_dup(iter, &curr_iter);
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
                fdt_iter_dup(iter, &curr_iter);
                return 0;
            }
            default: {
                return -FDT_ERR_UNKNOWN_TOKEN;
            }
        } /* end switch token */

        fdt_skip_to_next_token(&curr_iter);
    } while (not_found);

    fdt_iter_dup(iter, &curr_iter);
    return 0;
}