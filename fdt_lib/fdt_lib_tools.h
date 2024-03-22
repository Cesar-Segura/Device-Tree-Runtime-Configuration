#ifndef _FDT_LIB_TOOLS_H_
#define _FDT_LIB_TOOLS_H_

/**
 * @brief Represents a single node property of a device node. 
*/
struct device_node_property {
    char* name;
    void *value;
    int length; /* length of the value in bytes */

    struct device_node_property *next_property; /* pointer to the next property in a list of properties belonging to the same device node */
};

/**
 * @brief Represents a single device node in the device tree.
 * 
*/
struct device_tree_node {
    int is_root_node; 

    char *node_name;
    char *unit_address;
    char *full_path;
    int phandle; 

    struct device_node_property *device_properties; /* list of properties belonging to this device node */

    struct device_tree_node *parent_node; /* pointer to single parent node (note: root has no parent) */
    struct device_tree_node *child_node; /* pointer to list of child nodes */
    struct device_tree_node *next_node; /* pointer to the next node in a list of nodes belonging to the same parent */
};

/**
 * @brief Reads the next token from the current offset and populates next_offset with 
 * the address after the token. The token that is read is returned from the function. 
*/
int fdt_get_next_token(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset);

/**
 * @brief Get the information associated with the property at the given offset
*/
struct device_node_property *fdt_get_property(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset);

/**
 * @brief Get the information for the entirety of a single node and return a pointer to that node
*/
struct device_tree_node *fdt_get_node(const void *fdt_blob, struct device_tree_node *parent, uint32_t curr_offset, uint32_t *next_offset); 

/**
 * @brief Get the information for the root node of the device tree by parsing 
 * the entire device tree. Returns a pointer to the root node.
*/
struct device_tree_node *fdt_get_root_node(const void *fdt_blob); 

/**
 * @brief Pretty print property contents 
*/
void fdt_print_properties(struct device_node_property *prop);

/**
 * @brief Pretty print node contents 
*/
void fdt_print_node(struct device_tree_node *node);

/**
 * @brief Pretty print device tree
*/
void fdt_print_device_tree(const void *fdt_blob) ;

/**
 * @brief Free a device node's properties
*/
void free_device_node_properties(struct device_node_property *prop);

/**
 * @brief Free a device tree node
*/
void free_device_tree_node(struct device_tree_node *node);

/**
 * @brief Free the entire device tree structure from the root node 
*/
void free_device_tree(struct device_tree_node *root_node); 

#endif /* _FDT_LIB_TOOLS_H_ */