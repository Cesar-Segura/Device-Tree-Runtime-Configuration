#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define FDT_MAGIC 0xd00dfeed
#define OPP_MAGIC (~FDT_MAGIC)

#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE 0x2
#define FDT_PROP 0x3 
#define FDT_NOP 0x4
#define FDT_END 0x9 /* marks the end of the structure block */

#define FDT_TOKEN_SIZE sizeof(uint32_t)

#define FDT_ALIGN_ON(x, a) (((x) + (a - 1)) & ~(a - 1))

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

struct fdt_node_property {
    uint32_t len;
    uint32_t nameoff; 
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
 * @brief Convert the value pointed to by pointer to big endian format (32 bit value)
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
 * @brief Convert the value pointed to by bit pointer to big endian format (64 bit value)
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

uint32_t fdt_get_header(const void *fdt, uint32_t field_offset)
{
    return convert_32_to_big_endian((const uint32_t *)((uint8_t *)fdt + field_offset));
}

// void fdt_get_reserve_entry_header(const void *fdt, uint32_t offset, uint64_t *address, uint64_t *size)
// {
//     const uint8_t *base = (uint8_t *)fdt + fdt_get_off_mem_rsvmap(fdt) + offset; 
//     address = convert_64_to_big_endian((const uint64_t *)base);
//     size = convert_64_to_big_endian((const uint64_t *)(base + sizeof(uint64_t))); 
// }

uint64_t fdt_get_reserve_entry_header(const void *fdt, uint32_t field_offset)
{
    return convert_64_to_big_endian((const uint64_t *)((uint8_t *)fdt + field_offset)); 
}

uint32_t fdt_get_magic(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, magic)); 
}

uint32_t fdt_get_totalsize(const void *fdt) 
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, totalsize)); 
}

uint32_t fdt_get_off_dt_struct(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, off_dt_struct)); 
}

uint32_t fdt_get_off_dt_strings(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, off_dt_strings));
}

uint32_t fdt_get_off_mem_rsvmap(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, off_mem_rsvmap));
}

uint32_t fdt_get_version(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, version));
}

uint32_t fdt_get_last_comp_version(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, last_comp_version));
}

uint32_t fdt_get_boot_cpuid_phys(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, boot_cpuid_phys));
}

uint32_t fdt_get_size_dt_strings(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, size_dt_strings)); 
}

uint32_t fdt_get_size_dt_struct(const void *fdt)
{
    return fdt_get_header(fdt, offsetof(struct fdt_header, size_dt_struct)); 
}

/**
 * @brief Get the contents of the header struct and store them in the return struct
*/
struct fdt_header *fdt_get_header_contents(const void *fdt)
{
    struct fdt_header *header = (struct fdt_header *) malloc(sizeof(struct fdt_header)); 

    header->magic = fdt_get_magic(fdt);
    header->totalsize = fdt_get_totalsize(fdt);
    header->boot_cpuid_phys = fdt_get_boot_cpuid_phys(fdt);
    header->last_comp_version = fdt_get_last_comp_version(fdt);
    header->off_dt_strings = fdt_get_off_dt_strings(fdt);
    header->off_dt_struct = fdt_get_off_dt_struct(fdt);
    header->off_mem_rsvmap = fdt_get_off_mem_rsvmap(fdt);
    header->size_dt_strings = fdt_get_size_dt_strings(fdt);
    header->size_dt_struct = fdt_get_size_dt_struct(fdt);
    header->version = fdt_get_version(fdt);

    return header;   
}

/**
 * @brief Get a pointer to the element at the given offset within the given block
*/
static inline void *fdt_get_offset_in_block(const void *fdt, uint32_t block_offset, uint32_t element_offset)
{
    return (const uint8_t *) fdt + block_offset + element_offset; 
}

/**
 * @brief Get a pointer to the given offset in the structure block 
*/
static inline void *fdt_get_el_in_struct_block(const void *fdt, uint32_t offset)
{
    return fdt_get_offset_in_block(fdt, fdt_get_off_dt_struct(fdt), offset); 
}   

/**
 * @brief Get a pointer to the given offset in the strings block 
*/
static inline void *fdt_get_el_in_strings_block(const void *fdt, uint32_t offset)
{
    return fdt_get_offset_in_block(fdt, fdt_get_off_dt_strings(fdt), offset);  
}

/**
 * @brief Get the contents of the Memory Reservation block
*/
void fdt_parse_mem_resvblock(const void *fdt, uint32_t offset)
{
    // must ensure that this is located at an 8-byte aligned offset of the device tree blob
    struct fdt_reserve_entry *entry = (struct fdt_reserve_entry *) malloc(sizeof(struct fdt_reserve_entry));

    uint32_t curr_offset = offset; 
    entry->address = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, address)); 
    entry->size = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, size));

    uint32_t increment = (uint32_t) sizeof(uint64_t); 

    printf("\nPrinting contents of Memory Reservation Block\n");

    while (entry->address != 0 || entry->size != 0) {
        // process the struct (print for now, could store later)
        printf("Address: 0x%llx\n", entry->address);
        printf("Size: %llu\n", entry->size); 

        // point to the next struct in the list
        curr_offset += increment; 
        entry->address = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, address)); 
        entry->size = fdt_get_reserve_entry_header(fdt, curr_offset + offsetof(struct fdt_reserve_entry, size)); 
    }

    free(entry); 
}


/**
 * @brief Find the next token in the device tree 
*/
uint32_t fdt_get_next_token(const void *fdt, uint32_t curr_offset)
{
    uint32_t token;

    token = convert_32_to_big_endian((const uint32_t *) fdt_get_el_in_struct_block(fdt, curr_offset)); 

    uint32_t next_offset = curr_offset + FDT_TOKEN_SIZE; 

    switch (token) {
        case FDT_BEGIN_NODE: {
            break; 
        }
        case FDT_PROP: {
            break;
        }
        case FDT_NOP:
        case FDT_END_NODE: {
            break;
        }
        case FDT_END: {
            break; 
        }
    }
}

/**
 * @brief Find the next node in the device tree
*/
uint32_t fdt_get_next_node(const void *fdt, uint32_t curr_offset)
{

}

void fdt_parse_structure_block(const void *fdt, uint32_t offset, uint32_t size_dt_struct)
{
    uint32_t curr_offset = 0; 

    // check that the first thing you get is the FDT_BEGIN
    uint32_t curr_tag = convert_32_to_big_endian(fdt_get_el_in_struct_block(fdt, curr_offset)); 

    if (curr_tag == FDT_BEGIN_NODE) printf("Begin node found\n"); 

    // check that the last byte in the structure is the FDT_END 
    uint32_t last_entry = convert_32_to_big_endian((fdt_get_el_in_struct_block(fdt, size_dt_struct - FDT_TOKEN_SIZE)));
    if (last_entry == FDT_END) printf("End node found\n"); 
}   

/**
 * @brief Print the contents of the header struct
*/
void print_fdt_header(struct fdt_header *header)
{
    // printf("normMagic: 0x%x\n", FDT_MAGIC);
    // printf("oppMagic: 0x%x\n", OPP_MAGIC); 
    
    printf("Printing Header of Device Tree\n"); 
    printf("Magic: 0x%x\n", header->magic);
    printf("Totalsize: %u\n", header->totalsize);
    printf("off_dt_struct: %u\n", header->off_dt_struct);
    printf("off_dt_strings: %u\n", header->off_dt_strings);
    printf("off_mem_rsvmap: %u\n", header->off_mem_rsvmap);
    printf("version: %u\n", header->version);
    printf("last_comp_version: %u\n", header->last_comp_version);
    printf("boot_cpuid_phys: %u\n", header->boot_cpuid_phys);
    printf("size_dt_strings: %u\n", header->size_dt_strings);
    printf("size_dt_struct: %u\n", header->size_dt_struct); 
}

int
main(int argc, char **argv)
{ 
    if (argc != 2) {
        printf("Usage: ./parser <dtb_file_name> \n"); 
        return 1;
    }

    // const void *fdt_blob = argv[1];
    FILE *file = fopen(argv[1], "rb"); 
    if (file == NULL) {
        perror("Error opening file"); 
        return 1; 
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET); 

    char *buffer = (char *) malloc(file_size); 
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return 1; 
    }

    fread(buffer, 1, file_size, file);
    fclose(file); 

    const void *fdt_blob = (const void *) buffer; 

    struct fdt_header *header = fdt_get_header_contents(fdt_blob);
    print_fdt_header(header); 
    fdt_parse_mem_resvblock(fdt_blob, header->off_mem_rsvmap); 
    fdt_parse_structure_block(fdt_blob, header->off_dt_struct, header->size_dt_struct); 

    fflush(stdin); 
    free(header); 
    free(buffer);
    return 0; 
}