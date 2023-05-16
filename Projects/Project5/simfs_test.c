#include <stdio.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "free.h"
#include "ctest.h"
#include "inode.h"
#include "mkfs.h"

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

void test_inode(void) {
    image_open("file", 1);
    int block_num = 3;
    unsigned char block[BLOCK_SIZE];
    bwrite(block_num, block);
    CTEST_ASSERT(ialloc() == 0, "ialloc finds and allocates a free block");
    CTEST_ASSERT(ialloc() == 1, "ialloc finds and allocates a free block");

    image_close();
}

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
}



int main(void){
    CTEST_VERBOSE(1);
    test_image();
    test_block();
    test_block_alloc();
    test_free();
    test_inode();
    test_mkfs();
    test_find_incore_free();
    test_find_incore();
    CTEST_RESULTS();
    CTEST_EXIT();
}

#endif
