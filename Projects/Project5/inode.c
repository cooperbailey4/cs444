#include "block.h"
#include "free.h"
#include "inode.h"
#include <stddef.h>

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

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

struct inode *find_incore_free(void) {
    for(int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count == 0) {
            return &incore[i];
        }
    }
    return NULL;
}

struct inode *find_incore(unsigned int inode_num) {
    for(int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count != 0 && incore[i].inode_num == inode_num) {
            return &incore[i];
        }
    }
    return NULL;
}
