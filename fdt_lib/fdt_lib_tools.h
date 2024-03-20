#ifndef _FDT_LIB_TOOLS_H_
#define _FDT_LIB_TOOLS_H_

/**
 * @brief Reads the next token from the current offset and populates next_offset with 
 * the address after the token. The token that is read is returned from the function. 
*/
int fdt_get_next_token(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset);

int fdt_get_next_property(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset);

int fdt_get_next_node(const void *fdt_blob, uint32_t curr_offset, uint32_t *next_offset);

#endif /* _FDT_LIB_TOOLS_H_ */