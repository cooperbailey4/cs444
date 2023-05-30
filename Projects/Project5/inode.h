#ifndef INODE_H
#define INODE_H
#define INODE_MAP 1
#define BLOCK_SIZE 4096
#define INODE_SIZE 64
#define INODE_FIRST_BLOCK 3
#define MAX_SYS_OPEN_FILES 64
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define INODE_PTR_COUNT 16
#define INODE_STRUCT_SIZE_OFFSET 0
#define INODE_STRUCT_OWNER_ID_OFFSET 4
#define INODE_STRUCT_PERMISIONS_OFFSET 6
#define INODE_STRUCT_FLAGS_OFFSET 7
#define INODE_STRUCT_LINK_COUNT_OFFSET 8
#define INODE_STRUCT_BLOCK_PTR_OFFSET 9

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count;  // in-core only
    unsigned int inode_num;
};

void fill_incore(void);
void clear_incore_inodes(void);

struct inode* ialloc(void);
struct inode *find_incore_free(void);
struct inode *find_incore(unsigned int inode_num);
void read_inode(struct inode *in, int inode_num);
void write_inode(struct inode *in);
struct inode *iget(int inode_num);
void iput(struct inode *in);

#endif

