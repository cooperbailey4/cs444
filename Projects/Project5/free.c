#include "block.h"
int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;

    return -1;
}

void set_free(unsigned char *block, int num, int set) {
    int byte_num = num / 8;  // 8 bits per byte
    int bit_num = num % 8;

    if (set == 1) {
        block[byte_num] |= (1 << bit_num);
    }
    else {
        block[byte_num] &= ~(1 << bit_num);
    }


}

int find_free(unsigned char *block) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        int clear_bit = find_low_clear_bit(block[i]);
        if (clear_bit != -1) {
            return i*8 + clear_bit;
        }
    }
    return -1;
}

