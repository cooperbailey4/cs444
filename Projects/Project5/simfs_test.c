
#include <stdio.h>
#include <string.h>
#include "image.h"
#include "block.h"
#include "free.h"
#include "ctest.h"
#include "inode.h"

int image(char *filename, int x) {
    image_open(filename, x);
    return image_close();
}

#ifdef CTEST_ENABLE

void test_image(void) {
    CTEST_ASSERT(image("file", 1) == 0, "image_open and image_close are correct\n");
}

void test_block(void) {
    int block_num = 3;
    unsigned char myChar = 255;
    unsigned char* block = &myChar;
    bwrite(block_num, block);
    CTEST_ASSERT(memcmp(block, bread(block_num, block), BLOCK_SIZE) == 0, "block functions are correct");
}

void test_block_alloc(void) {
    int block_num = 3;
    unsigned char myChar = 255;
    unsigned char* block = &myChar;
    bwrite(block_num, block);
    //don't know why but this returns 2, need to look into it
    CTEST_ASSERT(alloc() == BLOCK_MAP, "ialloc finds and allocates block");
}

void test_free(void) {
    int block_num = 3;
    unsigned char myChar = 255;
    unsigned char* block = &myChar;
    bwrite(block_num, block);
    CTEST_ASSERT(find_free(block) == 10, "bit location is free");
}

void test_inode(void) {
    int block_num = 3;
    unsigned char myChar = 255;
    unsigned char* block = &myChar;
    bwrite(block_num, block);
    CTEST_ASSERT(ialloc() == INODE_MAP, "ialloc finds and allocates block");
}



int main(void){
    CTEST_VERBOSE(1);
    test_image();
    test_block();
    test_free();
    test_inode();
    test_block_alloc();
    CTEST_RESULTS();
    CTEST_EXIT();
}

#endif
