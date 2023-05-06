#include <unistd.h>
#include "block.h"
#include "image.h"
#include "free.h"


unsigned char *bread(int blocknum, unsigned char *block){
    lseek(image_fd, blocknum * BLOCK_SIZE, SEEK_SET);
    read(image_fd, block, BLOCK_SIZE);

    return block;
}
// bwrite(blocknum, block)
// test memcmp(block == bread(blocknum, block));

void bwrite(int blocknum, unsigned char *block){
    lseek(image_fd, blocknum*4096, SEEK_SET);
    write(image_fd, block, BLOCK_SIZE);
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



