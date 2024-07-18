#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_slot.h"



int main(int argc, char **argv){
    int fd, len, ret, id;

    if(argc != 4){
        perror("Error: invalid amount of arguments\n");
        exit(1);
    }

    fd = open(argv[1], O_WRONLY);
    if(fd < 0){
        perror("Error: could not open file\n");
        exit(1);
    }
    
    id = atoi(argv[2]);
    ret = ioctl(fd, MSG_SLOT_CHANNEL, id);
    if(ret < 0){
        perror("Error: ioctl failed\n");
        exit(1);
    }

    len = strlen(argv[3]);
    len = write(fd, argv[3], len);
    if(len < 0){
        perror("Error: write failed\n");
        exit(1);
    }

    close(fd);
    exit(0);
}