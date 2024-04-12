#ifndef _FDT_PARSE_LIB_MEM_REV_BLOCK_H_
#define _FDT_PARSE_LIB_MEM_REV_BLOCK_H_

/**
 * @brief Get the next reserve entry at the offset indicated by iter.
 * Update the offset to the next entry.
*/
const struct fdt_reserve_entry *fdt_next_reserve_entry(const void *fdt_blob, iterator_t *iter);

/**
 * @brief Get the address value of an fdt_reserve_entry struct.
*/
uint64_t fdt_get_resv_entry_addr(const struct fdt_reserve_entry *entry);

/**
 * @brief Get the size value of an fdt_reserve_entry struct.
*/
uint64_t fdt_get_resv_entry_size(const struct fdt_reserve_entry *entry);

#endif /* _FDT_PARSE_LIB_MEM_REV_BLOCK_H_ */