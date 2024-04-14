#ifndef _FDT_LIB_MEM_REV_BLOCK_H_
#define _FDT_LIB_MEM_REV_BLOCK_H_

/**
 * @brief Get the next reserve entry at the given offset.
 * 
 * Note: updates the offset in iter to the next reserve entry struct in fdt.
 * 
 * @param iter FDT iterator object; holds the given offset.
 * @return pointer to fdt_reserve_entry struct at the given offset.
*/
const struct fdt_reserve_entry *fdt_next_reserve_entry(struct fdt_iter *iter);

/**
 * @brief Get the address field of the given fdt_reserve_entry struct.
 * 
 * @param entry pointer to fdt_reserve_entry struct.
 * @return address field of the given fdt_reserve_entry struct.
*/
uint64_t fdt_get_resv_entry_addr(const struct fdt_reserve_entry *entry);

/**
 * @brief Get the size field of the given fdt_reserve_entry struct.
 * 
 * @param entry pointer to fdt_reserve_entry struct.
 * @return size field of the given fdt_reserve_entry struct.
*/
uint64_t fdt_get_resv_entry_size(const struct fdt_reserve_entry *entry);

#endif /* _FDT_LIB_MEM_REV_BLOCK_H_ */