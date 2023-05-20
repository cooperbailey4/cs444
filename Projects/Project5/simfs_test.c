#include <stdio.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "free.h"
#include "ctest.h"
#include "inode.h"
#include "mkfs.h"
#include "pack.h"

#ifdef CTEST_ENABLE

void test_image(void) {
    image_open("file", 1);
    int image = image_close();
    CTEST_ASSERT(image == 0, "image_open and image_close are correct\n");

}

void test_block(void) {
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

// void test_inode(void) {
//     image_open("file", 1);
//     int block_num = 3;
//     unsigned char block[BLOCK_SIZE];
//     bwrite(block_num, block);

//     CTEST_ASSERT(ialloc() == 0, "ialloc finds and allocates a free block");
//     CTEST_ASSERT(ialloc() == 1, "ialloc finds and allocates a free block");

//     image_close();
// }

void test_mkfs(void){
    unsigned char block[BLOCK_SIZE];
    char *image = "image";
    image_open(image, 0);
    mkfs();
    bread(BLOCK_MAP, block);
    CTEST_ASSERT(find_free(block) == 7, "free bit at 7 in block");
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
    iput(inode);
    CTEST_ASSERT(inode->ref_count == 0, "ref_count starts at 0 and iput does nothing if it is 0");

    inode->ref_count = 2;
    CTEST_ASSERT(inode->ref_count == 1, "iput subtracts from ref_count if it is not 0");
    image_close();
}

void test_ialloc(void) {
    image_open("file", 1);
    int block_num = 3;
    unsigned char block[BLOCK_SIZE];
    bwrite(block_num, block);

    CTEST_ASSERT(ialloc()->inode_num == 0, "ialloc finds and allocates a free block");
    CTEST_ASSERT(ialloc()->inode_num == 1, "ialloc finds and allocates a free block");

    image_close();
}



int main(void){
    CTEST_VERBOSE(1);
    test_image();
    test_block();
    test_block_alloc();
    test_free();
    test_mkfs();
    test_find_incore_free();
    test_find_incore();
    test_read_write_inode();
    test_iget();
    test_ialloc();
    CTEST_RESULTS();
    CTEST_EXIT();
}

#endif
