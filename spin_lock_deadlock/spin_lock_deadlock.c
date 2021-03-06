

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
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>

#include <asm/system.h>         /* cli(), *_flags */
#include <asm/uaccess.h>        /* copy_*_user */

int spin_lock_deadlock_major=0;
int spin_lock_deadlock_minor=0;

MODULE_AUTHOR("Steven Tao");
MODULE_VERSION("1.0.0");
MODULE_LICENSE("Dual BSD/GPL");

static char message[128]={0};
static struct task_struct* thread_a, *thread_b;
static spinlock_t lock_a = SPIN_LOCK_UNLOCKED;
static spinlock_t lock_b = SPIN_LOCK_UNLOCKED;

int spin_lock_deadlock_open (struct inode * inode, struct file * file)
{
    printk(KERN_INFO "spin_lock_deadlock: dummyChar has been opened\n");    	
	return 0;

}

ssize_t spin_lock_deadlock_read(struct file* fp, char __user * buf, size_t count, loff_t * f_pos)
{
    copy_to_user(buf,message,120);
    printk(KERN_INFO "spin_lock_deadlock: copy_to_user\n");    	

}

ssize_t spin_lock_deadlock_write (struct file * fp, const char __user * buf, size_t count, loff_t * f_pos)
{
    sprintf(message,"%s XXX", buf);	

	printk(KERN_WARNING "spin_lock_deadlock : received msg from the user space\n");

}
static struct file_operations spin_lock_deadlock_fops = 
{
	.owner = THIS_MODULE,
	.open = spin_lock_deadlock_open,
	.read = spin_lock_deadlock_read,
	.write = spin_lock_deadlock_write,
};

void spin_lock_deadlock_exit(void)
{
	printk(KERN_WARNING "spin_lock_deadlock: exiting  \n");
}

static void dummy_setup_cdev()
{
	int err;
	dev_t devno;

    struct cdev *chardummy_cdev = cdev_alloc();
    chardummy_cdev->ops = &spin_lock_deadlock_fops;

	cdev_init(chardummy_cdev,&spin_lock_deadlock_fops);
	printk(KERN_WARNING "after cdev_init\n");
	chardummy_cdev->owner = THIS_MODULE;
	//dev->cdev.ops = & &scull_fops;
	devno = MKDEV(spin_lock_deadlock_major,spin_lock_deadlock_minor);

    err = cdev_add (chardummy_cdev, devno, 1);
	if (err)
	{
		printk(KERN_WARNING "Errot %d setup_cdev", err);
	}
		
}

void thread_a_fn()
{
    spin_lock(&lock_a); 
	printk(KERN_WARNING "spin_lock_deadlock: lock_a is locked in thread_a_fn\n");
    msleep(10*1000);
    
    spin_lock(&lock_b);
	printk(KERN_WARNING "spin_lock_deadlock: lock_b is locked in thread_a_fn\n");
    msleep(10*1000);
    
    spin_unlock(&lock_a);
	printk(KERN_WARNING "spin_lock_deadlock: lock_a is unlocled in thread_a_fn\n");
    spin_unlock(&lock_b);
	printk(KERN_WARNING "spin_lock_deadlock: lock_b is unlocked in thread_a_fn\n");
}

void thread_b_fn()
{
    spin_lock(&lock_b);
	printk(KERN_WARNING "spin_lock_deadlock: lock_b is locked in thread_b_fn\n");
    msleep(10*1000);
    
    spin_lock(&lock_a); 
	printk(KERN_WARNING "spin_lock_deadlock: lock_a is locked in thread_b_fn\n");
    msleep(10*1000);
    
    
    spin_unlock(&lock_a);
	printk(KERN_WARNING "spin_lock_deadlock: lock_a is unlocled in thread_b_fn\n");
    spin_unlock(&lock_b);
	printk(KERN_WARNING "spin_lock_deadlock: lock_b is unlocked in thread_b_fn\n");

}

void spin_lock_deadlock(void)
{
    const char thread_name_a [] = "thread A";
    const char thread_name_b [] = "thread B"; 

    thread_a = kthread_create (thread_a_fn,NULL,thread_name_a);
    if (thread_a)
    {
        wake_up_process(thread_a);
    }

    thread_b = kthread_create (thread_b_fn,NULL,thread_name_b);
    if (thread_b)
    {
        wake_up_process(thread_b);
    }


}


int spin_lock_deadlock_init(void)
{
	int result;
	int i = 0;
	dev_t dev = 0;

	result = alloc_chrdev_region(&dev, 0, 1,"spin_lock_deadlock");
	if(result < 0) 
	{
		printk(KERN_WARNING "spin_lock_deadlock: can't get major \n");
	}
	spin_lock_deadlock_major = MAJOR(dev);
	printk(KERN_WARNING "spin_lock_deadlock: major is %d \n",spin_lock_deadlock_major);

    dummy_setup_cdev();
    spin_lock_deadlock();
	printk(KERN_WARNING "back to spin_lock_deadlock_init\n");

	return 0; // succeed
fail:
	spin_lock_deadlock_exit();
	return result;
}

module_init(spin_lock_deadlock_init);
module_exit(spin_lock_deadlock_exit);
