#include "block.h"
#include "free.h"
#include "inode.h"
#include "pack.h"
#include <stddef.h>

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

// helper functions
void fill_incore(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        incore[i].ref_count = 1;
    }
}

// main functions

struct inode* ialloc(void) {
    unsigned char block[BLOCK_SIZE];


    bread( INODE_MAP, block );

    int free_bit = find_free( block );
    if (free_bit != -1) {

        set_free( block, free_bit, NON_FREE );

        bwrite( INODE_MAP, block );

        struct inode* in_core = iget(free_bit);
        if (in_core == NULL) {
            return NULL;
        }
        in_core->size = 0;
        in_core->owner_id = 0;
        in_core->permissions = 0;
        in_core->flags = 0;
        for (int i = 0; i < INODE_PTR_COUNT; i++) {
            in_core->block_ptr[i] = 0;
        }
        write_inode(in_core);

        return in_core;
    }
    return NULL;

}

struct inode *find_incore_free(void) {
    for(int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count == 0) {
            return &incore[i];
        }
    }
    return NULL;
}

struct inode *find_incore(unsigned int inode_num) {
    for(int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count != 0 && incore[i].inode_num == inode_num) {
            return &incore[i];
        }
    }
    return NULL;
}

void read_inode(struct inode *in, int inode_num) {
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    unsigned char block[BLOCK_SIZE];
    bread(block_num, block);

    int block_offset = (inode_num % INODES_PER_BLOCK) * INODE_SIZE;
    unsigned char *inode_address = block + block_offset;
    in->size = read_u32(inode_address + INODE_STRUCT_SIZE_OFFSET);
    in->owner_id = read_u16(inode_address + INODE_STRUCT_OWNER_ID_OFFSET);
    in->permissions = read_u8(inode_address + INODE_STRUCT_PERMISIONS_OFFSET);
    in->flags = read_u8(inode_address + INODE_STRUCT_FLAGS_OFFSET);
    in->link_count = read_u8(inode_address + INODE_STRUCT_LINK_COUNT_OFFSET);
    int block_ptr_offset_spacing = 0;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = read_u16(inode_address + INODE_STRUCT_BLOCK_PTR_OFFSET + block_ptr_offset_spacing);
        block_ptr_offset_spacing += 2;
    }
}

void write_inode(struct inode *in) {
    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    unsigned char block[BLOCK_SIZE];
    bread(block_num, block);

    int block_offset = (in->inode_num % INODES_PER_BLOCK) * INODE_SIZE;
    unsigned char *inode_address = block + block_offset;
    write_u32(inode_address + INODE_STRUCT_SIZE_OFFSET, in->size);
    write_u16(inode_address + INODE_STRUCT_OWNER_ID_OFFSET, in->owner_id);
    write_u8(inode_address + INODE_STRUCT_PERMISIONS_OFFSET, in->permissions);
    write_u8(inode_address + INODE_STRUCT_FLAGS_OFFSET, in->flags);
    write_u8(inode_address + INODE_STRUCT_LINK_COUNT_OFFSET, in->link_count);
    int block_ptr_offset_spacing = 0;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        write_u16(inode_address + INODE_STRUCT_BLOCK_PTR_OFFSET + block_ptr_offset_spacing, in->block_ptr[i]);
        block_ptr_offset_spacing += 2;
    }

}

struct inode *iget(int inode_num) {
    struct inode* incore_inode = find_incore(inode_num);
    if (incore_inode) {
        incore_inode->ref_count++;
        return incore_inode;
    }
    struct inode* free_incore = find_incore_free();
    if (free_incore == NULL) {
        return NULL;
    }
    else {
        read_inode(free_incore, inode_num);
        free_incore->ref_count = 1;
        free_incore->inode_num = inode_num;
        return free_incore;
    }
}

void iput(struct inode *in) {
    if (in->ref_count == 0) {
        return;
    }
    in->ref_count=in->ref_count-1;
    if (in->ref_count == 0) {
        write_inode(in);
    }
}
