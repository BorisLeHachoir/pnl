#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
/* required for various structures related to files liked fops. */
#include <linux/semaphore.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include "ioctl_basics.h"
#include <linux/version.h>
#include <linux/uaccess.h>

static int Major;

int open(struct inode *inode, struct file *filp)
{
	printk (KERN_INFO "Inside open \n");
	return 0;
}

int release(struct inode *inode, struct file *filp)
{
	printk (KERN_INFO "Inside close \n");
	return 0;
}

long ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg)
{

	struct mesg_kill *mesg;
	switch (cmd) {

	case IOCTL_KILL:
		printk (KERN_INFO "Asked KILL");
		copy_from_user((char *)mesg, (char *)arg, sizeof(*mesg));
			printk(KERN_INFO "Asked KILL args r: %d & %d",
		mesg->signal, mesg->pid);
		break;

	case IOCTL_WAIT:
		printk (KERN_INFO "Asked WAIT");
		break;

	case IOCTL_MEMINFO:
		printk (KERN_INFO "Asked MEMINFO");
		break;

	case IOCTL_MODINFO:
		printk (KERN_INFO "Asked MODINFO");
		break;
	}

	return 0;
}

struct file_operations fops = {
	open: open,
	unlocked_ioctl : ioctl_funcs,
	release : release
};


struct cdev *kernel_cdev;


int char_arr_init (void)
{
	int ret;
	dev_t dev_no, dev;

	kernel_cdev = cdev_alloc();
	kernel_cdev->ops = &fops;
	kernel_cdev->owner = THIS_MODULE;
	printk (" Inside init module\n");
	ret = alloc_chrdev_region(&dev_no, 0, 1, "char_arr_dev");

	if (ret < 0) {
		printk ("Major number allocation is failed\n");
		return ret;
	}

	Major = MAJOR(dev_no);
	dev = MKDEV(Major, 0);
	printk (" The major number for your device is %d\n", Major);
	ret = cdev_add(kernel_cdev, dev, 1);

	if (ret < 0) {
		printk (KERN_INFO, "Unable to allocate cdev");
		return ret;
	}
	return 0;
}

void char_arr_cleanup(void)
{
	printk (KERN_INFO, " Inside cleanup_module\n");
	cdev_del(kernel_cdev);
	unregister_chrdev_region(Major, 1);
}
MODULE_LICENSE("GPL");
module_init(char_arr_init);
module_exit(char_arr_cleanup);
