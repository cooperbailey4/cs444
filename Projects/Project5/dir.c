#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dir.h"
#include "inode.h"
#include "block.h"
#include "pack.h"
#include "mkfs.h"

// Helper functions
// Inspired by the `dirname` and `basename` Unix utilities
//
// But doesn't handle a trailing '/' on the path correctly.
//
// So don't do that.

char *get_dirname(const char *path, char *dirname) {
    strcpy(dirname, path);

    char *p = strrchr(dirname, '/');

    if (p == NULL) {
        strcpy(dirname, ".");
        return dirname;
    }

    if (p == dirname)  // Last slash is the root /
        *(p+1) = '\0';

    else
        *p = '\0';  // Last slash is not the root /

    return dirname;
}

char *get_basename(const char *path, char *basename) {
    if (strcmp(path, "/") == 0) {
        strcpy(basename, path);
        return basename;
    }

    const char *p = strrchr(path, '/');

    if (p == NULL)
        p = path;   // No slash in name, start at beginning
    else
        p++;        // Start just after slash

    strcpy(basename, p);

    return basename;
}

// Main Functions

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
    strcpy(ent->name, (char *)(block + offset_in_block + DIRECTORY_ENTRY_FILE_NAME_OFFSET));

    dir->offset += DIRECTORY_ENTRY_SIZE;

    return 0;
}

void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}


struct inode *namei(char *path) {
    if (strcmp(path, "/") == 0) {
        return iget(ROOT_DIRECTORY_INODE_NUM);
    }

    return NULL;

}


// "/foo"
int directory_make(char *path) {
    if (path[0] != '/') {
        // check path begins with / if it doesn't, failure
        return -1;
    }

    int path_len = strlen(path);
    if (path_len == 1) {
        // Root directory already exists
        return 0;
    }

    char dirname[1024];
    char parentname[1024];

    // get parent path name before dirname
    char* parent_path = get_dirname(path, parentname);

    // get directory name without parent path
    char* dir_name = get_basename(path, dirname);


    // get inode of parent path
    struct inode* parent_inode = namei(parent_path);

    if (parent_inode == NULL) {
        // Parent directory doesn't exist
        return -1;
    }

    unsigned char block[BLOCK_SIZE] = {0};

    struct inode* new_inode = ialloc();
    int new_inode_num = new_inode->inode_num;

    int new_directory_block_num = alloc();

    new_inode->flags = INODE_FLAG_FILE_ISA_DIR; //TODO
    new_inode->size = ROOT_DIRECTORY_SIZE;
    new_inode->block_ptr[0] = new_directory_block_num;

    int ent_num = 0;

    write_u16(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_INODE_NUM_OFFSET, new_inode_num);
    strcpy((char*)(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_FILE_NAME_OFFSET), ".");

    ent_num = 1;

    write_u16(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_INODE_NUM_OFFSET, ROOT_DIRECTORY_INODE_NUM);
    strcpy((char*)(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_FILE_NAME_OFFSET), "..");

    bwrite(new_directory_block_num, block);
    int block_ptr_index = parent_inode->size / BLOCK_SIZE;
    bread(parent_inode->block_ptr[block_ptr_index], block);


    int offset = parent_inode->size % BLOCK_SIZE;
    ent_num = offset / DIRECTORY_ENTRY_SIZE;

    parent_inode->size += DIRECTORY_ENTRY_SIZE;


    write_u16(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_INODE_NUM_OFFSET, new_inode_num);
    strcpy((char*)(block + DIRECTORY_ENTRY_SIZE * ent_num + DIRECTORY_ENTRY_FILE_NAME_OFFSET), dirname);

    bwrite(parent_inode->block_ptr[block_ptr_index], block);

    iput(new_inode);


    iput(parent_inode);

    return 0;
}
