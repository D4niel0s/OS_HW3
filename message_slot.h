#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>


#define MAJOR_NUM 235
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)
#define DEVICE_RANGE_NAME "message_slot"
#define BUF_LEN 128
#define DEVICE_FILE_NAME "message_slot"
#define SUCCESS 0

/* A single channel in the message slot*/
typedef struct chan{
    unsigned int channel_id;
    char msg[BUF_LEN];
    int msg_len;
    struct chan *next;
} channel;

/* A linked list which will contain all channels of current message slot*/
typedef struct chans{
    channel *head;
} channels;



#endif
