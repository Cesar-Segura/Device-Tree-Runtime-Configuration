#ifndef _FDT_LIB_TOOLS_H_
#define _FDT_LIB_TOOLS_H_

/**
 * @brief Represents a single device node in the device tree.
 * 
*/
struct device_node {
    uint32_t offset;
    void *address;

    int is_root_node; 

    char *node_name;
    char *unit_address;
    char *full_path;
    int phandle; 

    struct fdt_property *device_properties; /* list of properties belonging to this device node */

    struct device_node *parent_node; /* pointer to single parent node (note: root has no parent) */
    struct device_node *child_node; /* pointer to list of child nodes */
    struct device_node *next_node; /* pointer to the next node in a list of nodes belonging to the same parent */

	/* pointers to these nodes within the device tree itself */
	// void *child_node;
	// void *parent_node;
	// void *next_node; 
};

/**
 * Strings
*/

/**
 * @brief Get the string at the given offset in the strings block.
 * Note: does not change the iterator.
*/
const char *fdt_get_string(const void *fdt_blob, iterator_t iter);

/**
 * Tokens
*/

/**
 * @brief Get the token at the given offset.
 * Note: does not increase iter
*/
int fdt_get_token(const void *fdt_blob, iterator_t *iter);

/**
 * @brief Checks if the data at the given offset (iter) is a token.
 * 
 * Returns:
 *      1 if the data is a token 
 *      0 otherwise
*/
static int _check_bad_token_offset(const void *fdt_blob, iterator_t *iter);

/**
 * @brief Get the next token from the given offset iter.
 * 
 * Note: updates iter to point to the beginning of the next token.
 * Note: upon error, iter is not changed.
 * 
 * Returns:
 *      One of the 5 FDT tokens 
 *      OR
 *      < 0 if there is an error 
*/
int fdt_skip_to_next_token(const void *fdt_blob, iterator_t *iter);

/**
 * Properties
*/

/**
 * @brief Get the property at the current offset (iter)
 * Note: does not change the iter offset
 * 
 * param err: holds an error code (if there is an error)
 *      0 if no error
 *      < 0 if error 
 * 
 * Returns:
 *      pointer to the beginning of an fdt_property struct
 *      OR
 *      NULL if there is an error (err populated with the error code)
*/
const struct fdt_property *fdt_get_property(const void *fdt_blob, iterator_t *iter, int *err);

/**
 * @brief Return the length value of the property at the offset iter.
*/
uint32_t fdt_get_prop_len_by_offset(const void *fdt_blob, iterator_t *iter);

/**
 * @brief Return the name offset value of the property at the offset iter.
*/
uint32_t fdt_get_prop_nameoff_by_offset(const void *fdt_blob, iterator_t *iter);

/**
 * @brief Access the attributes of the fdt_property struct
*/
uint32_t fdt_get_property_len(const struct fdt_property *prop);
uint32_t fdt_get_property_nameoff(const struct fdt_property *prop);

/**
 * @brief Get the offset of the first property of this node.
 * 
 * param iter: gives the address of the beginning of a node representation
 * 
 * Returns: 
 *      1 if the node has a property
 *          * iter holds the offset of the property from the beginning of the node
 *      0 if there are no properties.
 *          * iter holds the offset to the next token
 *      < 0 if there is an error
 *          * iter remains unchanged 
*/
static int fdt_first_property(const void *fdt_blob, iterator_t *iter);

/**
 * @brief Get the offset of the next property of this node.
 * 
 * param iter: gives the offset of the beginning of the current property
 * 
 * Returns: 
 *      1 if there is another property found
 *          * iter holds the offset of the property from the FDT_PROP token
 *      0 if there are no properties.
 *          * iter remains unchanged
 *      < 0 if there is an error
 *          * iter remains unchanged
*/
int fdt_next_property(const void *fdt_blob, int is_first_prop, iterator_t *iter);

/**
 * Nodes
*/

/**
 * @brief Get the name of the node
 * 
 * param iter: gives the offset of the beginning of the current node
 * 
 * param err: holds the error code (if any)
 *      0 if no error 
 *      < 0 if error found
 * 
 * Returns: 
 *      pointer to the name string (null terminated)
 *      OR 
 *      NULL if there is an error
 *          * err holds the error code
 * 
*/
const char *fdt_get_node_name(const void *fdt_blob, iterator_t *iter, int *err);

/**
 * @brief Get the offset of the first child node
 * 
 * param iter: the offset of the current node, starting from FDT_BEGIN_NODE token
 *      - Holds a pointer 
 * 
 * Returns: 
 *      1 if a child node is found
 *          * iter holds the offset of the property from the FDT_BEGIN_NODE token
 *      0 if there are no children nodes.
 *          * iter remains unchanged
 *      < 0 if there is an error
 *          * iter remains unchanged
*/
static int fdt_first_child_node(const void *fdt_blob, iterator_t *iter);

/**
 * @brief Get the offset of the next child node.
 * Note: Essentially, we are getting the sibling node of the current node.
 *      - Get the next node at the same depth as the current node
 * 
 * param iter: the offset of the current child node within the device tree
 * 
 * Returns: 
 *      1 if another child node is found
 *          * iter holds the offset of the property from the FDT_BEGIN_NODE token
 *      0 if there are no child nodes remaining.
 *          * iter remains unchanged
 *      < 0 if there is an error
 *          * iter remains unchanged
*/
int fdt_next_child_node(const void *fdt_blob, int is_first_child_node, iterator_t *iter);

#endif /* _FDT_LIB_TOOLS_H_ */