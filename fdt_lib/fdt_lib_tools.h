#ifndef _FDT_LIB_TOOLS_H_
#define _FDT_LIB_TOOLS_H_

/**
 * Interacting with the string block
*/

/**
 * @brief Get the string at the given offset in the strings block.
 * 
 * Note: should be the offset from the beginning of the strings block.
 * 
 * @param fdt_blob pointer to beginning of fdt in memory.
 * @param offset offset of the string in the strings block.
 * @return The string at the given offset.
*/
const char *fdt_get_string(const void *fdt_blob, int offset);

/**
 * Interacting with the struct block
*/

/**
 * @brief Get the property at the current offset (iter).
 * Note: the offset MUST be pointing to an FDT token.
 * 
 * @param iter FDT iterator object; holds the offset of fdt_property.
 * @return pointer to fdt_property OR null if there is an error
*/
const struct fdt_property *fdt_get_property(struct fdt_iter *iter, int *err);

/**
 * @brief Return the length field of fdt_property struct at the offset.
 * 
 * @param fdt_blob pointer to beginning of fdt in memory
 * @param offset offset of the fdt_property struct in the fdt
 * 
 * @return the length field of the fdt_property
*/
uint32_t fdt_get_prop_len_by_offset(const void *fdt_blob, int offset);

/**
 * @brief Return the nameoff field of fdt_property struct at the offset.
 * 
 * @param fdt_blob pointer to beginning of fdt in memory
 * @param offset offset of the fdt_property struct in the fdt
 * 
 * @return the nameoff field of the fdt_property
*/
uint32_t fdt_get_prop_nameoff_by_offset(const void *fdt_blob, int offset);

/**
 * @brief Return the length field of fdt_property struct.
 * 
 * @param prop poiter to fdt_property struct
 * 
 * @return the length field of the fdt_property
*/
uint32_t fdt_get_property_len(const struct fdt_property *prop);

/**
 * @brief Return the nameoff field of fdt_property struct.
 * 
 * @param prop poiter to fdt_property struct
 * 
 * @return the nameoff field of the fdt_property
*/
uint32_t fdt_get_property_nameoff(const struct fdt_property *prop);

/**
 * @brief Get the offset of the first property for the device node at the given iteration.
 * 
 * Note: The offset in iter is NOT changed if there is an error. 
 * Otherwise, the offset points to the next token after the first property.
 * 
 * @param iter FDT iterator object; holds the offset of the current device node in the device tree.
 * @return 1 if the node has a property; 0 if there are no properties; < 0 if there is an error
*/
int fdt_first_property(struct fdt_iter *iter);

/**
 * @brief Get the offset of the next property in the device tree.
 * 
 * Note: the offset of iterator object is NOT changed if there is an error. 
 * Otherwise, the offset points to the next token after the current property.
 * 
 * @param iter FDT iterator object; holds the offset of the current property.
 * @return 1 if another property is found; 0 if no more properties; < 0 if there is an error.
*/
int fdt_next_property(struct fdt_iter *iter);

/**
 * @brief Get the name of the node at the given offset.
 * 
 * @param iter FDT iterator object; holds the given offset.
 * @param err Holds the error code of the function (if there is an error).
 * @return pointer to the name of the node OR null if there is an error.
*/
const char *fdt_get_node_name(struct fdt_iter *iter, int *err);

/**
 * @brief Point the FDT iterator object to the root node of the fdt.
 * 
 * Does not initialize iter if there is an error.
 * 
 * @param iter uninitialized FDT iterator object.
 * @param fdt_blob pointer to the beginning of the fdt in memory.
 * @return 1 if the root node was found; < 0 if there was an error.
*/
int fdt_find_root(struct fdt_iter *iter, const void *fdt_blob);

/**
 * @brief Get the offset of this node's first child node
 * 
 * Note: the offset of iterator object is NOT changed if there is an error. 
 * Otherwise, the offset points to the next token after the first child node.
 * 
 * @param iter FDT iterator object; holds the offset of the current node.
 * @return 1 if a child node is found; 0 if there are no more children left; < 0 if there is an error.
*/
int fdt_first_child_node(struct fdt_iter *iter);

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
int fdt_next_child_node(struct fdt_iter *iter);

#endif /* _FDT_LIB_TOOLS_H_ */