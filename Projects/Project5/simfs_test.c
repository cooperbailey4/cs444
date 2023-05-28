#include <stdio.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "free.h"
#include "ctest.h"
#include "inode.h"
#include "mkfs.h"
#include "pack.h"
#include "dir.h"
#include "ls.h"

#ifdef CTEST_ENABLE

// Helper Functions
void fill_bitmap(unsigned int blocknum) {
    unsigned char block[BLOCK_SIZE];
    memset(block, 0xFF, sizeof(block));
    bwrite(blocknum, block);
}


// Test functions
void test_image(void) {
    image_open("file", 1);
    int image = image_close();
    CTEST_ASSERT(image == 0, "image_open and image_close are correct\n");
    image_close();
}


void test_block_read_write(void) {
    image_open("file", 1);
    unsigned char block[BLOCK_SIZE];
    int block_num = 3;
    bwrite(block_num, block);
    CTEST_ASSERT(memcmp(block, bread(block_num, block), BLOCK_SIZE) == 0, "block functions are correct");
    image_close();
}


void test_block_alloc(void) {
    image_open("file", 1);
    int block_num = 3;
    unsigned char block[BLOCK_SIZE];
    bwrite(block_num, block);
    CTEST_ASSERT(alloc() == 0, "alloc finds and allocates a free block");
    CTEST_ASSERT(alloc() == 1, "alloc finds and allocates a free block");

    image_close();
}


void test_free(void) {
    image_open("file", 1);
    int block_num = 3;
    unsigned char block[BLOCK_SIZE];
    bwrite(block_num, block);
    CTEST_ASSERT(find_free(block) == 0, "bit location is free");
    image_close();
}


void test_mkfs(void){
    unsigned char block[BLOCK_SIZE];
    char *image = "image";
    image_open(image, 0);
    mkfs();
    bread(BLOCK_MAP, block);
    CTEST_ASSERT(find_free(block) == 8, "free bit at 7 in block");
    image_close();
}


void test_find_incore_free(void) {
    char *image = "image";
    image_open(image, 0);
    mkfs();
    struct inode* inode = find_incore_free();
    CTEST_ASSERT(inode->inode_num == 0, "found free incore inode");
    image_close();
}


void test_find_incore(void) {
    char *image = "image";
    image_open(image, 0);
    mkfs();
    struct inode* inode = find_incore_free();
    inode->inode_num = 612;
    inode->ref_count = 1;
    struct inode* new_inode = find_incore_free();
    new_inode->inode_num = 413;
    new_inode->ref_count = 8;
    CTEST_ASSERT(find_incore(413)->ref_count == 8, "find_incore returns the correct inode");
    CTEST_ASSERT(memcmp(find_incore(612), inode, sizeof(struct inode)) == 0, "found the correct inode");
    image_close();
}


void test_read_write_inode(void) {
    image_open("file", 1);
    struct inode* inode = find_incore_free();

    struct inode* new_inode = find_incore_free();
    new_inode->ref_count = 1;
    new_inode->size = 1;
    new_inode->permissions = 13;
    new_inode->block_ptr[15] = 5;
    new_inode->inode_num = 127;


    write_inode(new_inode);

    read_inode(inode, 127);


    CTEST_ASSERT(new_inode->size == inode->size, "Wrote the inode that we wanted to the correct place on disk");
    CTEST_ASSERT(new_inode->permissions == inode->permissions, "Wrote the inode that we wanted to the correct place on disk");
    CTEST_ASSERT(new_inode->block_ptr[15] == inode->block_ptr[15], "Wrote the inode that we wanted to the correct place on disk");
    CTEST_ASSERT(new_inode->inode_num == inode->inode_num, "Wrote the inode that we wanted to the correct place on disk");
    image_close();

}


void test_iget(void) {
    image_open("file", 1);
    struct inode* new_inode = find_incore_free();
    new_inode->ref_count = 1;
    new_inode->size = 1;
    new_inode->permissions = 13;
    new_inode->block_ptr[15] = 5;
    new_inode->inode_num = 127;
    write_inode(new_inode);
    struct inode* inode = iget(127);
    CTEST_ASSERT(inode->inode_num == 127, "inode_num is the same as the inode on disk");
    CTEST_ASSERT(inode->ref_count == 2, "inode has the correct reference count");

    struct inode* not_read_inode = iget(12);
    CTEST_ASSERT(not_read_inode->inode_num == 12, "not_read_inode, now exists and has space allocated for it");
    CTEST_ASSERT(not_read_inode->ref_count == 1, "not_read_inode, has a ref_count of 1 to start");
    image_close();

}


void test_iput(void) {
    image_open("file", 1);
    struct inode* inode = find_incore_free();
    struct inode* empty_inode = find_incore_free();
    inode->ref_count = 2;
    inode->inode_num = 34;
    iput(inode);
    CTEST_ASSERT(inode->ref_count == 1, "iput subtracts from ref_count if it is not 0");
    iput(inode);
    CTEST_ASSERT(inode->ref_count == 0, "ref_count at 0 and iput does nothing if it is 0, if it subtracts it should be readable from disk");
    read_inode(empty_inode, 34);
    CTEST_ASSERT(empty_inode->inode_num == 34, "empty inode reads inode that was written to disk in iput");
    image_close();
}


void test_ialloc(void) {
    image_open("file", 1);
    int block_num = 3;
    unsigned char block[BLOCK_SIZE];

    bwrite(block_num, block);
    struct inode* x = ialloc();
    struct inode* y = ialloc();

    CTEST_ASSERT(x->inode_num == 0, "ialloc finds and allocates a free block");
    CTEST_ASSERT(y->inode_num == 1, "ialloc finds and allocates a free block, that goes up by one every call");

    image_close();
}

void test_ls(void) {
    image_open("file", 0);
    mkfs();
    ls(); // just prints out 0 and 0 for the current and parent directory's
    image_close();
}


// Failure testing

void test_image_failure(void) {
    image_open("/foo", 1);
    int image = image_close();
    CTEST_ASSERT(image == -1, "image_open and image_close are correct\n");
    image_close();
}


void test_block_read_failure(void) {
    image_open("file", 1);
    unsigned char block[BLOCK_SIZE];
    int block_num = 90000;  // Invalid block number

    // Attempt to read the block and check for failure
    unsigned char *result = bread(block_num, block);
    CTEST_ASSERT(result == NULL, "Block read failed as expected");

    image_close();
}


void test_block_alloc_failure(void) {
    image_open("file", 1);
    fill_bitmap(BLOCK_MAP);

    CTEST_ASSERT( alloc() == -1, "alloc returns -1 when there is no way to allocate a block");
    image_close();
}


void test_free_failure(void) {
    image_open("file", 1);
    fill_bitmap(BLOCK_MAP);
    unsigned char block[BLOCK_SIZE];
    bread( BLOCK_MAP, block );
    CTEST_ASSERT(find_free(block) == -1, "cannot find a free block, so it returns -1");
    image_close();
}


void test_find_incore_free_failure(void) {
    fill_incore();
    CTEST_ASSERT(find_incore_free() == NULL, "No more space incore so there are no more free incore inodes");
}


void test_find_incore_failure(void) {
    CTEST_ASSERT(find_incore(2023) == NULL, "No more space incore so there are no more free incore inodes");
}


void test_iget_failure(void) {
    fill_incore();
    CTEST_ASSERT(iget(4134) == NULL, "Can't get a new incore inode if they are all taken");
}


void test_ialloc_failure(void) {
    image_open("file", 1);
    fill_bitmap(INODE_MAP);
    CTEST_ASSERT(ialloc() == NULL, "ialloc will fail if there is no more available space in the INODE_MAP");
    image_close();
}




int main(void){
    CTEST_VERBOSE(1);

    // Success Tests
    test_image();
    test_block_read_write();
    test_block_alloc();
    test_free();
    test_mkfs();
    test_find_incore_free();
    test_find_incore();
    test_read_write_inode();
    test_iget();
    test_iput();
    test_ialloc();
    // test_ls();

    // Failure Tests
    test_image_failure();
    test_block_read_failure();
    test_block_alloc_failure();
    test_free_failure();
    test_find_incore_free_failure();
    test_find_incore_failure();
    test_iget_failure();
    test_ialloc_failure();

    test_ls();

    CTEST_RESULTS();
    CTEST_EXIT();


}

#endif
