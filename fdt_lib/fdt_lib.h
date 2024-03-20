#ifndef _FDT_PARSE_LIB_
#define _FDT_PARSE_LIB_

/**
 * Important macros used in the fdt 
*/
#define FDT_MAGIC 0xd00dfeed
#define OPP_MAGIC (~FDT_MAGIC)

#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE 0x2
#define FDT_PROP 0x3 
#define FDT_NOP 0x4
#define FDT_END 0x9

#define FDT_TOKEN_SIZE sizeof(uint32_t) /* size of a token in the structure block */

#define FDT_ALIGN_ON(x, a) (((x) + (a - 1)) & ~(a - 1)) /* align the pointer "x" to an address that is a multiple of "a" */

/**
 * @brief The layout of the header for the devicetree 
 * All the headers fields are 32 bit integers, stored in big-endian format
*/
struct fdt_header {
    /** @brief 
     * This field shall contain the value 0xd00dfeed (big-endian)
    */
    uint32_t magic;

    /** @brief
     * The total size in bytes of the devicetree data structure
    */
    uint32_t totalsize;

    /** @brief
     * The offset in bytes of the structure block from the beginning of the header 
    */
    uint32_t off_dt_struct;

    /** @brief
     * The offset in bytes of the strings block from the beginning of the header  
    */ 
    uint32_t off_dt_strings;

    /** @brief
     * The offset in bytes of the memory reservation block from the beginning of the header
    */  
    uint32_t off_mem_rsvmap;

    /** @brief
     * The version of the devicetree data structure
     * (structure is 17 if using this struct)
    */   
    uint32_t version;

    /** @brief
     * The lowest version of the devicetree data structure with which the version used is backwards compatible
    */ 
    uint32_t last_comp_version;

    /** @brief
     * The physical ID of the system’s boot CPU
    */
    uint32_t boot_cpuid_phys;

    /** @brief 
     * The length in bytes of the strings block section of the devicetree blob
    */
    uint32_t size_dt_strings;

    /** @brief
     * The length in bytes of the structure block section of the devicetree blob.
    */
    uint32_t size_dt_struct;
};

/** @brief 
 * The memory reservation block consists of a list of pairs of 64-bit big-endian integers
 * Each uint64_t in the memory reservation block, and thus the memory reservation block as a whole, shall be located at an
 * 8-byte aligned offset from the beginning of the devicetree blob
*/
struct fdt_reserve_entry {
    uint64_t address;
    uint64_t size;
};

/**
 * @brief 
 * Structure representing a property in the device tree blob.
 * len: gives the length of the property's value in bytes 
 * nameoff: gives an offset into the strings block at which the property's name is stored
*/
struct device_tree_property {
    uint32_t len;
    uint32_t nameoff; 
};

/**
 * @brief Returns a pointer to the given offset in the fdt blob. The offset is assumed to be in terms of number of bytes
 * The offset is assumed to be the offset from the beginning of the fdt blob itself. 
*/
static inline const void *fdt_get_offset_in_blob(const void *fdt_blob, uint32_t offset)
{
    return (const void *) (((uint8_t *) fdt_blob) + offset); 
}

/**
 * @brief Convert the (32-bit) value pointed to by pointer to big endian format
*/
static inline uint32_t convert_32_to_big_endian(const uint32_t *pointer)
{
    const uint8_t *bytes = (const uint8_t *) pointer;
    return ((uint32_t) bytes[0] << 24)
            | ((uint32_t) bytes[1] << 16)
            | ((uint32_t) bytes[2] << 8)
            | bytes[3]; 
}

/**
 * @brief Convert the (64-bit) value pointed to by bit pointer to big endian format (64 bit value)
*/
static inline uint64_t convert_64_to_big_endian(const uint64_t *pointer)
{
    const uint8_t *bytes = (const uint8_t *) pointer;
    return ((uint64_t) bytes[0] << 56)
            | ((uint64_t) bytes[1] << 48)
            | ((uint64_t) bytes[2] << 40)
            | ((uint64_t) bytes[3] << 32)
            | ((uint64_t) bytes[4] << 24)
            | ((uint64_t) bytes[5] << 16)
            | ((uint64_t) bytes[6] << 8)
            | (bytes[7]); 
}  


#endif /* _FDT_PARSE_LIB */