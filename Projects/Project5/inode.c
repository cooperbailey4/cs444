#include "block.h"
#include "free.h"
#include "indode.h"

int ialloc(void) {
    unsigned char block[BLOCK_SIZE];


    bread( INODE_MAP, block );

    int free_bit = find_free( block );
    if (free_bit != -1) {

        set_free( block, free_bit, NON_FREE );

        bwrite( INODE_MAP, block );
    }

    return free_bit;
}


