#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dir.h"
#include "inode.h"
#include "block.h"
#include "pack.h"
#include "mkfs.h"


struct directory *directory_open(int inode_num) {
    struct inode* inode = iget(inode_num);
    if (inode == NULL) {
        return NULL;
    }
    struct directory* dir = malloc(sizeof(struct directory));

    dir->inode = inode;
    dir->offset = 0;

    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent) {

    if (dir->offset >= dir->inode->size) {
        return -1;
    }


    int data_block_index = dir->offset / BLOCK_SIZE;
    int offset_in_block = dir->offset % BLOCK_SIZE;
    unsigned char block[BLOCK_SIZE];

    int data_block_num = dir->inode->block_ptr[data_block_index];

    bread(data_block_num, block);

    struct directory_entry *entry = (struct directory_entry *)(block + offset_in_block);

    ent->inode_num = read_u16(&(entry->inode_num));
    strcpy(ent->name, (char *)(block + offset_in_block + 2));

    dir->offset += DIRECTORY_ENTRY_SIZE;

    return 0;
}

void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}
