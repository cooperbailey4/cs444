#ifndef BLOCK_H
#define BLOCK_H
#define BLOCK_SIZE 4096
#define BLOCK_MAP 2

unsigned char *bread(int block num, unsigned char *block);
void bwrite(int block num, unsigned char *block);
int alloc(void);


#endif
