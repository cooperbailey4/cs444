#ifndef DIR_H
#define DIR_H

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

// Helper Functions
char *get_dirname(const char *path, char *dirname);
char *get_basename(const char *path, char *basename);

// Main Functions
struct directory *directory_open(int inode_num);
struct inode *namei(char *path);


int directory_get(struct directory *dir, struct directory_entry *ent);
void directory_close(struct directory *d);
int directory_make(char *path);

#endif
