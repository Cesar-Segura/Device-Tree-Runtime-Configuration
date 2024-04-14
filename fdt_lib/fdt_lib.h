#ifndef _FDT_LIB_H_
#define _FDT_LIB_H_

#include <stdint.h>

#define FDT_MAGIC 0xd00dfeed

/**
 * FDT token macros
*/
#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE 0x2
#define FDT_PROP 0x3 
#define FDT_NOP 0x4
#define FDT_END 0x9

/**
 * FDT error macros
*/
#define FDT_ERR_BAD_STRUCTURE 0x12 /* Returned when a FDT_END token is found before FDT_END_NODE */
#define FDT_ERR_BAD_ARG 0x13 /* bad argument passed as a parameter to a function */
#define FDT_ERR_UNKNOWN_TOKEN 0x14 /* parser read a token that does not match the 5 tokens above */
#define FDT_ERR_NO_ROOT_NODE 0x15 /* no root node found in the entire fdt */

#define FDT_TOKEN_SIZE sizeof(uint32_t) /* size of a token in the structure block */

#define FDT_ALIGN_ON(x, a) (((x) + (a - 1)) & ~(a - 1)) /* align the pointer "x" to an address that is a multiple of "a" */

/**
 * @brief Represents the header of the devicetree 
 * All the headers fields are 32 bit integers, stored in big-endian format
*/
struct fdt_header {

    uint32_t magic; /* contains the value 0xd00dfeed (big-endian) */

    uint32_t totalsize; /* total size in bytes of the devicetree data structure */

    uint32_t off_dt_struct; /* offset in bytes of the structure block from the beginning of the header */

    uint32_t off_dt_strings; /* offset in bytes of the strings block from the beginning of the header */

    uint32_t off_mem_rsvmap; /* offset in bytes of the memory reservation block from the beginning of the header */

    uint32_t version; /* version of the devicetree data structure */

    uint32_t last_comp_version; /* The lowest version of the devicetree data structure with which the version used is backwards compatible */

    uint32_t boot_cpuid_phys; /* The physical ID of the system’s boot CPU */

    uint32_t size_dt_strings; /* length in bytes of the strings block section of the devicetree blob */

    uint32_t size_dt_struct; /* length in bytes of the structure block section of the devicetree blob */
};

/** 
 * @brief Represents one entry in the memory reservation block
*/
struct fdt_reserve_entry {
    uint64_t address;
    uint64_t size;
};

/**
 * @brief Represents a property in the device tree blob.
 * len: gives the length of the property's value in bytes 
 * nameoff: gives an offset into the strings block at which the property's name is stored
*/
struct fdt_property {
    uint32_t len;
    uint32_t nameoff; 
	uint8_t value[]; // value of the property comes immediately after the nameoffset
};

/**
 * @brief Returns a pointer to the given offset in the fdt blob. 
 * 
 * The offset should be in terms of number of bytes.
 * 
 * @param fdt_blob pointer to the beginning of the device tree 
 * @param offset given offset in the blob
 * 
 * @return pointer to the given offset in the blob.
*/
static inline const void *fdt_get_offset_in_blob(const void *fdt_blob, uint32_t offset)
{
    return (const void *) (((uint8_t *) fdt_blob) + offset); 
}

/**
 * @brief Convert the (32-bit) value pointed to by pointer to big endian format
 * 
 * @param pointer pointer to a 32-bit value
 * 
 * @return 32-bit value in big endian format.
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
 * @brief Convert the (64-bit) value pointed to by bit pointer to big endian format
 * 
 * @param pointer pointer to a 64-bit value
 * 
 * @return 64-bit value in big endian format.
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

/**
 * @brief An object representing a given iteration over the device tree.
*/
struct fdt_iter {
    int offset; // Current offset in the device tree binary.
    const void *fdt_blob; // pointer to beginning of device tree binary.
};

/**
 * @brief Initialize an fdt_iter object 
 * 
 * @param iter pointer to the fdt_iter object to initialize
 * @param offset offset in the device tree binary that the pointer will start at
*/
static inline void fdt_iter_init(struct fdt_iter *iter, uint32_t offset, const void *fdt_blob)
{
    iter->offset = offset;
    iter->fdt_blob = fdt_blob;
}

/**
 * @brief Copy all the data from src object to dest object
 * 
 * @param dest iterator object that will copy from src object.
 * @param src iterator object being copied into dest.
*/
static inline void fdt_iter_dup(struct fdt_iter *dest, struct fdt_iter *src)
{
    dest->offset = src->offset;
    dest->fdt_blob = src->fdt_blob;
}

#endif /* _FDT_LIB_H_ */