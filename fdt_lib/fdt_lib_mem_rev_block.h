#ifndef _FDT_PARSE_LIB_MEM_REV_BLOCK_H_
#define _FDT_PARSE_LIB_MEM_REV_BLOCK_H_

/**
 * @brief Represents a single entry in the memory reservation block
*/
struct fdt_memory_reserve_entry {
    uint64_t address;
    uint64_t size;

    struct reserve_entry *next_entry; /* next entry in the memory reservation block */
};

/**
 * @brief Returns the memory reserve entries from the memory reservation block 
*/
struct fdt_memory_reserve_entry *fdt_get_mem_resv_block(const void *fdt_blob);

/**
 * @brief Frees the memory of a list of fdt_memory_reserve_entry structs
*/
void free_fdt_memory_reserve_entry(struct fdt_memory_reserve_entry *entry);

/**
 * @brief Old implementation, will remove later
*/
void fdt_parse_mem_resvblock(const void *fdt, uint32_t offset);

#endif /* _FDT_PARSE_LIB_MEM_REV_BLOCK_H_ */