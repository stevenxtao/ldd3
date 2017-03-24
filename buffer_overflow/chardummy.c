

//#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/system.h>         /* cli(), *_flags */
#include <asm/uaccess.h>        /* copy_*_user */

#include "chardummy.h"              /* local definitions */


int chardummy_major=0;
int chardummy_minor=0;

MODULE_AUTHOR("Steven Tao");
MODULE_VERSION("1.0.0");
MODULE_LICENSE("Dual BSD/GPL");

static char message[16]={0};


int chardummy_open (struct inode * inode, struct file * file)
{
    printk(KERN_INFO "dummy: dummyChar has been opened\n");    	
	return 0;

}

ssize_t chardummy_read(struct file* fp, char __user * buf, size_t count, loff_t * f_pos)
{
    char msg[8];
    strcpy(msg, message);
    copy_to_user(buf,msg,128);
    printk(KERN_INFO "dummy_read: copy_to_user: msg=%s\n", msg);    	

}

ssize_t chardummy_write (struct file * fp, const char __user * buf, size_t count, loff_t * f_pos)
{
    sprintf(message,"%s XXX", buf);	

	printk(KERN_WARNING "dummy_write : received msg from the user space, message buffer may over here ! count = %d\n", count);
	printk(KERN_WARNING "dummy_write : message:%s\n", message);

}
static struct file_operations chardummy_fops = 
{
	.owner = THIS_MODULE,
	.open = chardummy_open,
	.read = chardummy_read,
	.write = chardummy_write,
};

void chardummy_exit(void)
{
	printk(KERN_WARNING "dummy: exiting  \n");
}

static void dummy_setup_cdev()
{
	int err;
	dev_t devno;

    struct cdev *chardummy_cdev = cdev_alloc();
    chardummy_cdev->ops = &chardummy_fops;

	cdev_init(chardummy_cdev,&chardummy_fops);
	printk(KERN_WARNING "after cdev_init\n");
	chardummy_cdev->owner = THIS_MODULE;
	//dev->cdev.ops = & &scull_fops;
	devno = MKDEV(chardummy_major,chardummy_minor);

    err = cdev_add (chardummy_cdev, devno, 1);
	if (err)
	{
		printk(KERN_WARNING "Errot %d setup_cdev", err);
	}
		
}


int chardummy_init(void)
{
	int result;
	int i = 0;
	dev_t dev = 0;

	result = alloc_chrdev_region(&dev, 0, DUMMY_NR_DEVS,"chardummy");
	if(result < 0) 
	{
		printk(KERN_WARNING "dummy: can't get major \n");
	}
	chardummy_major = MAJOR(dev);
	printk(KERN_WARNING "dummy: major is %d \n",chardummy_major);

    dummy_setup_cdev();
	printk(KERN_WARNING "back to dummy_init\n");

	return 0; // succeed
fail:
	chardummy_exit();
	return result;
}

module_init(chardummy_init);
module_exit(chardummy_exit);
