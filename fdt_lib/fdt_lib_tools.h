#ifndef _FDT_LIB_TOOLS_H_
#define _FDT_LIB_TOOLS_H_


/**
 * Got a working implementation!
 * NOTE: need to update the definitions for these with the new API
*/

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
const struct fdt_property *fdt_get_property(const void *fdt_blob, int offset, int *err);

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
 * @brief Get the name of the node at the given offset.
 * 
 * @param fdt_blob Pointer to beginning of devicetree in memory.
 * @param offset offset of node within devicetree
 * @param err Holds the error code of the function (if there is an error).
 * 
 * @return pointer to the name of the node OR null if there is an error.
*/
const char *fdt_get_node_name(const void *fdt_blob, int offset, int *err);

/**
 * @brief Return the offset of the root node.
 * 
 * @param fdt_blob pointer to the beginning of the fdt in memory.
 * 
 * @return offset if the node was found; < 0 if there was an error.
*/
int fdt_find_root(const void *fdt_blob);

typedef enum {
    CHILD_NODES = 0,
    PROPERTIES
} fdt_iter_type_t;

/**
 * @brief An object representing a given iteration over the device tree.
*/
struct fdt_iter {
    int offset; // Current offset in the device tree binary.
    int end_struct_block; // holds the offset of the end of the struct block
    unsigned int num_iterations; // current number of iterations
    fdt_iter_type_t type; // what type of devicetree object are we iterating over
    const void *fdt_blob; // pointer to beginning of device tree binary.
};

/**
 * @brief Initialize an fdt_iter object 
 * 
 * @param iter pointer to the fdt_iter object to initialize
 * @param type the type of devicetree object we're iterating over (child node, property)
 * @param offset offset in the device tree binary that the pointer will start at
*/
void fdt_iter_init(struct fdt_iter *iter, uint32_t offset, fdt_iter_type_t type, const void *fdt_blob);

/**
 * @brief Given an iterator object pointing to the beginning of a node,
 * get the next object of the specified type (child node or property) 
 * 
 * @param iter FDT iterator object, holds the offset of the current device node in the device tree.
 * @param type The type of object the user wants to iterate over.
 * 
 * @return The offset within the devicetree of the next object in the iteration, or < 0 if error.
*/
int fdt_iter_get_next(struct fdt_iter *iter);

#endif /* _FDT_LIB_TOOLS_H_ */