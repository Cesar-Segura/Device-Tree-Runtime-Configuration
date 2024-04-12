#ifndef _FDT_LIB_PARSE_H_
#define _FDT_LIB_PARSE_H_

/**
 * TODO: Add useful device tree APIs below 
*/


/**
 * @brief Find the node by the given path and populate iter with the offset of this node.
 * 
 * param path: path from the root node to the node of interest
 * 
 * Returns:
 *      1 if the node was found 
 *      0 if the node was not found 
 *      < 0 if there was an error
*/
int fdt_find_node_by_path(const void *fdt_blob, const char *path, iterator_t *iter);

/**
 * @brief Populate the 
*/
int fdt_populate_device_node(const void *fdt_blob, iterator_t *iter);

#endif /* _FDT_LIB_PARSE_H_ */