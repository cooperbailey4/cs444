#include <unistd.h>
#include <string.h>
#include "block.h"
#include "mkfs.h"
#include "image.h"
#include "inode.h"
#include "pack.h"
#include <stdio.h>

#define DIRECTORY_ENTRY_INODE_NUM_OFFSET 0
#define DIRECTORY_ENTRY_FILE_NAME_OFFSET 2
#define DIRECTORY_ENTRY_RESERVED_OFFSET 18

#define DIRECTORY_ENTRY_SIZE 32
#define ROOT_DIRECTORY_SIZE DIRECTORY_ENTRY_SIZE*2

#define INODE_FLAG_UNKNOWN_TYPE 0
#define INODE_FLAG_REGULAR_FILE 1
#define INODE_FLAG_FILE_ISA_DIR 2

#define ROOT_DIRECTORY_INODE_NUM 0

void mkfs(void) {

    // Setup
    unsigned char block[BLOCK_SIZE] = {0};

    for ( int i = 0; i < TOTAL_BLOCKS; i++ ) {
        write(image_fd, block, BLOCK_SIZE);
    }

    for ( int i = 0; i < PREALLOCATED; i++ ){
        alloc();
    }

    // Create root directory

    struct inode* new_inode = ialloc();
    int new_inode_num = new_inode->inode_num;

    int directory_block_num = alloc();

    new_inode->flags = INODE_FLAG_FILE_ISA_DIR; //TODO
    new_inode->size = ROOT_DIRECTORY_SIZE;
    new_inode->block_ptr[0] = directory_block_num;

    int ent_num = 0;

    write_u16(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_INODE_NUM_OFFSET, new_inode_num);
    strcpy((char*)(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_FILE_NAME_OFFSET), ".");

    ent_num = 1;

    write_u16(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_INODE_NUM_OFFSET, ROOT_DIRECTORY_INODE_NUM);

    strcpy((char*)(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_FILE_NAME_OFFSET), "..");

    bwrite(directory_block_num, block);

    iput(new_inode);
}
