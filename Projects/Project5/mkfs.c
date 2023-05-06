#include <unistd.h>
#include "block.h"
#include "mkfs.h"
#include "image.h"

void mkfs(void) {
    unsigned char block[BLOCK_SIZE];

    for ( int i = 0; i < TOTAL_BLOCKS; i++ ) {
        write(image_fd, block, BLOCK_SIZE);
    }

    for ( int i = 0; i < PREALLOCATED; i++ ){
        alloc();
    }

}
