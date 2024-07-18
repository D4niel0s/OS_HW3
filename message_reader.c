#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_slot.h"


int main(int argc, char **argv){

    int fd, ret, len, id;
    char buf[BUF_LEN];

    if(argc != 3){
        perror("Error: invalid amount of arguments\n");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
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

    len = read(fd, buf, BUF_LEN);
    if(len < 0){
        perror("Error: read failed\n");
        exit(1);
    }

    ret = write(1, buf, len);
    if(ret < 0){
        perror("Error: write failed\n");
        exit(1);
    }

    close(fd);
    exit(0);
}