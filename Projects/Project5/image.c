#include <unistd.h>
#include <fcntl.h>
#include "image.h"

int image_fd;

int image_open(char *filename, int truncate){
    int trunc_flag = truncate? O_TRUNC: 0;
    image_fd =  open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
}

int image_close(void) {
    return close(image_fd);
}
