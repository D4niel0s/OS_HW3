// Declare what kind of code we want
// from the header files. Defining __KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/
#include <linux/slab.h>   /* for kmalloc() */

MODULE_LICENSE("GPL");

#include "message_slot.h"

/* An array that will hold every message slot's channels linked list*/
channels all_slots[256];



static int device_open( struct inode* inode, struct file* file){
  /* open() does nothing since we only need to do something when a new channel is opened*/
  return SUCCESS;
}


static int device_release( struct inode* inode, struct file* file){
  return SUCCESS;
}


static ssize_t device_read(struct file* file, char __user* buffer, size_t length, loff_t* offset){
  channel *chan;

  chan = (channel *)(file->private_data);
  if(!chan){ /*If channel is not set*/
    return -EINVAL;
  }

  if(chan->msg_len == 0){
    return -EWOULDBLOCK;
  }

  if(length < chan->msg_len){
    return -ENOSPC;
  }

  /* Channel is set, write to channel*/
  if(copy_to_user(buffer, chan->msg, chan->msg_len)){
    return -1;
  }

  return chan->msg_len;
}


static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset){
  channel *chan;

  if(length == 0 || length > 128 || buffer == NULL){
    return -EMSGSIZE;
  }

  chan = (channel *)(file->private_data);
  if(!chan){ /*If channel is not set*/
    return -EINVAL;
  }

  /* Channel is set, write to channel*/
  if(copy_from_user(chan->msg, buffer, length)){
    return -1;
  }
  chan->msg_len = length;
  return length;
}



static long device_ioctl(struct file* file, unsigned int ioctl_command_id, unsigned long ioctl_param){
  unsigned int minor, i;
  channel *iterator, *last, *tmp;

  if(ioctl_command_id != MSG_SLOT_CHANNEL || ioctl_param == 0){
    return -EINVAL;
  }

  minor = iminor(file_inode(file));

  iterator = all_slots[minor].head;
  i=0;
  while(iterator != NULL){
    i++;

    if(iterator->channel_id == ioctl_param){
      break;
    }
    last = iterator;
    iterator = iterator->next;
  }

  if(iterator == NULL){ /*Channel does not exist - either because empty list or because it wasn't created yet*/
    tmp = (channel *)kmalloc(sizeof(channel), GFP_KERNEL);

    if(!tmp){
      return -1;
    }

    tmp->channel_id = ioctl_param;
    tmp->msg[0] = '\0';
    tmp->msg_len = 0;
    tmp->next = NULL;

    if(i == 0){ /*Empty channels list*/
      all_slots[minor].head = tmp;
    }else{
      last->next = tmp;
    }
    iterator = tmp;
  }

  file->private_data = (void *)iterator;
  return SUCCESS;
  }


struct file_operations Fops = {
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};


static int __init simple_init(void){
  int rc = -1 ,i;

  rc = register_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME, &Fops);

  if(rc < 0) {
    printk( KERN_ALERT "%s registraion failed for  %d\n", DEVICE_FILE_NAME, MAJOR_NUM );
    return rc;
  }

  for(i=0;i<256;++i){
    all_slots[i].head = NULL; /* Init all lists to be empty */
  }
  return SUCCESS;
}

static void __exit simple_cleanup(void){
  channel *h, *prev;
  int i;

  /* Iterate all message slots */
  for(i=0;i<256;++i){
    
    /* Free all channels of a given message slot*/
    h = all_slots[i].head;
    while(h != NULL){
      prev = h;
      h=h->next;

      kfree(prev);
    }
  }

  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}


module_init(simple_init);
module_exit(simple_cleanup);