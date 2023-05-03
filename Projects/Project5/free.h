#ifndef FREE_H
#define FREE_H
#define NON_FREE 1
#define FREE 0

void set_free(unsigned char *block, int num, int set);
int find_free(unsigned char *block);

#endif
