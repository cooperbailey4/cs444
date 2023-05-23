#include <unistd.h>
#include <stdio.h>
#include "block.h"
#include "image.h"
#include "free.h"


unsigned char *bread(int blocknum, unsigned char *block){
    int leek = lseek(image_fd, blocknum * BLOCK_SIZE, SEEK_SET);
    if(leek == -1){
        perror("null read\n");
        return NULL;
    }
    int readint = read(image_fd, block, BLOCK_SIZE);
    if (readint == -1) {
        perror("read failed\n");
        return NULL;
    }
    if (readint != BLOCK_SIZE) {

        return NULL;
    }

    return block;
}

void bwrite(int blocknum, unsigned char *block){
    lseek(image_fd, blocknum*BLOCK_SIZE, SEEK_SET);
    if (write(image_fd, block, BLOCK_SIZE) == -1) {
        perror("write failed\n");
    }
}

int alloc(void) {
    unsigned char block[BLOCK_SIZE];

    
    bread( BLOCK_MAP, block );

    int free_bit = find_free( block );
    if (free_bit != -1) {

        set_free( block, free_bit, NON_FREE );

        bwrite( BLOCK_MAP, block );

    }

    return free_bit;
}



