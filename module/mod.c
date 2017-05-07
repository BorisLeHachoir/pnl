#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
/* required for various structures related to files liked fops. */
#include <linux/semaphore.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

/* meminfo */
#include <linux/mm.h>
//#include <linux/sysinfo.h>
#include <linux/swap.h>

/* modinfo */
#include <linux/module.h>

/* wait */
#include <linux/pid.h>
#include <linux/delay.h>

#include "mod.h"

static int Major;

static struct workqueue_struct *func_wq;

int myvar;

int open(struct inode *inode, struct file *filp)
{
	pr_info("Inside open\n");
	return 0;
}

int release(struct inode *inode, struct file *filp)
{
	pr_info("Inside close\n");
	return 0;
}

static void kill_function(struct work_struct *wk)
{
	
	struct pid *pid_struct;
	struct func_work * work = container_of(wk, struct func_work, work_s);

	pr_info("[KILL_FUNCTION] signal: %d pid: %d\n", work->mesg.kill->signal,
		work->mesg.kill->pid);

	/* Check signal valid */
	if(work->mesg.kill->pid > _NSIG || work->mesg.kill->signal < 1)
  work->mesg.kill->ret = -22;
		return;

	pid_struct = find_get_pid(work->mesg.kill->pid);

	if (pid_struct) {
		work->mesg.kill->ret = kill_pid(pid_struct, work->mesg.kill->signal, 1);
		pr_info("ret = %d", work->mesg.kill->ret);
  //put_pid(pid_struct);
	}
 else work->mesg.kill->ret = -3;
	return;
}

static void wait_function(struct work_struct *wk)
{
	int i, cpt = 0, pid = 0;
 struct task_struct * task_struct = NULL;
 struct task_struct * task_struct_tab[MAX_PIDS];
 struct pid * pid_struct = NULL;
 struct pid * pid_struct_tab[MAX_PIDS];

 struct func_work * work = container_of(wk, struct func_work, work_s);

 for( i=0; i < work->mesg.wait->size; i++){
  pid_struct = find_get_pid(work->mesg.wait->pids[i]);
  if(pid_struct){
   task_struct = get_pid_task(pid_struct, PIDTYPE_PID);
   if(task_struct){
    pid_struct_tab[cpt] = pid_struct;
    task_struct_tab[cpt] = task_struct;
    cpt++;
   }
   /*else{
    put_pid(pid_struct);
   }*/
  }
 }

 if(cpt == 0) 
  work->mesg.wait->ret = -2;
 else 
  while(pid == 0){
   for(i = 0; i < cpt; i++){
    task_struct = task_struct_tab[i];
    if(task_struct){
     if(pid_alive(task_struct)){
      pr_info("pid alive\n");
      ssleep(1);
     }else{   
      pr_info("fin proc\n");
      if(task_struct){
       pid = 1;
       work->mesg.wait->pid = task_struct->pid;
       work->mesg.wait->exit_value = task_struct->exit_code;
       work->mesg.wait->ret = 0;
       break;
      }
     }
    }else{
     pr_info("tast_struct null\n");
    }
    ssleep(1);
   }
  }
 
 for(i=0; i <cpt; i++) put_task_struct(task_struct_tab[i]); 

}


static void meminfo_function(struct work_struct *wk)
{
	struct sysinfo mem_val;
 struct func_work * work = container_of(wk, struct func_work, work_s);

	si_meminfo(&mem_val);
	si_swapinfo(&mem_val);
 
 work->mesg.meminfo->totalram  = mem_val.totalram;
 work->mesg.meminfo->freeram   = mem_val.freeram;
 work->mesg.meminfo->sharedram = mem_val.sharedram;
 work->mesg.meminfo->bufferram = mem_val.bufferram;
 work->mesg.meminfo->totalswap = mem_val.totalswap;
 work->mesg.meminfo->freeswap  = mem_val.freeswap;
 work->mesg.meminfo->totalhigh = mem_val.totalhigh;
 work->mesg.meminfo->freehigh  = mem_val.freehigh;
 work->mesg.meminfo->mem_unit  = mem_val.mem_unit;

 work->mesg.meminfo->ret = 0;
}

static void modinfo_function(struct work_struct *wk)
{
	struct module *module_tofind;

 struct func_work * work = container_of(wk, struct func_work, work_s);

	module_tofind = find_module(work->mesg.modinfo->name);
	if (module_tofind) {
		pr_info("nom: %s\n", module_tofind->name);
		strcpy(work->mesg.modinfo->res_name, module_tofind->name);
		pr_info("version: %s\n", module_tofind->version);
		strcpy(work->mesg.modinfo->res_version, module_tofind->version);
		pr_info("load adr: %p\n", module_tofind->module_core);
		pr_info("Avant");
		work->mesg.modinfo->res_core = module_tofind->module_core;
		pr_info("Apres");
		if(module_tofind->args) {
			pr_info("args: %s\n", module_tofind->args);
			strcpy(work->mesg.modinfo->res_args, module_tofind->args);
		}
	}
	pr_info("fin modinfo function");
}

long ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg)
{

 struct func_work * func_work;
	int i;

 func_work = kmalloc(sizeof(func_work), GFP_KERNEL);
 if(!func_work){
  pr_info("kmalloc failed\n");
  return -1;
 }

	switch (cmd) {

  case IOCTL_LIST:
		pr_info("ASKED LIST");
  copy_from_user(&i, (char *)arg, sizeof(int));
		pr_info("recv from user: list %c\n", i ? '&' : ' ');
		break;

	case IOCTL_FG:
		pr_info("ASKED FG");
  func_work->mesg.fg = kmalloc(sizeof(struct mesg_fg), GFP_KERNEL);
  if(!func_work->mesg.fg){
   pr_info("kmalloc failed\n");
   kfree((void *) func_work);
   return -1;
  }
		copy_from_user(func_work->mesg.fg, (char *)arg, sizeof(struct mesg_fg));
		pr_info("recv from user: fg %d %c\n",
			func_work->mesg.fg->id, func_work->mesg.fg->async ? '&' : ' ');

 // kfree((void*) func_work->mesg.);
		break;

	case IOCTL_KILL:
		pr_info("ASKED KILL");
  func_work->mesg.kill = kmalloc(sizeof(struct mesg_kill), GFP_KERNEL);
  if(!func_work->mesg.kill){
   pr_info("kmalloc failed\n");
   kfree((void *) func_work);
   return -1;
  }
		copy_from_user(func_work->mesg.kill, (char *)arg, sizeof(struct mesg_kill));
		pr_info("recv from user: kill %d %d %c\n",
		func_work->mesg.kill->signal, func_work->mesg.kill->pid, func_work->mesg.kill->async ? '&':' ');
  
		
		
		INIT_WORK( &(func_work->work_s), kill_function);
  
			
		if(! queue_work( func_wq,  &(func_work->work_s))){
			pr_info("work was already on a queue\n");
   func_work->mesg.kill->ret = -2;
   copy_to_user((char *) arg, func_work->mesg.kill, sizeof(struct mesg_kill));
   kfree((void*) func_work->mesg.kill);
   kfree((void *) func_work);
			return -1;
			}
	 
  pr_info("AVANT FLUSH");
		flush_work(&(func_work->work_s));
		pr_info("AVANT COPYTOUSER");
		copy_to_user((char *)arg, func_work->mesg.kill, sizeof(struct mesg_kill));
		pr_info("APRES COPYTOUSER");
 
  kfree((void *) func_work->mesg.kill);
  kfree((void *) func_work);
		break;

	case IOCTL_WAIT:
		pr_info("Asked WAIT");

  func_work->mesg.wait = kmalloc(sizeof(struct mesg_wait), GFP_KERNEL);
  if(!func_work->mesg.wait){
   pr_info("kmalloc failed\n");
   kfree((void *) func_work);
   return -1;
  }

  copy_from_user(func_work->mesg.wait, (char *)arg, sizeof(struct mesg_wait));

		pr_info("recv from user: wait ");
		for (i = 0; i < func_work->mesg.wait->size-1; ++i)
			pr_info("%d ", func_work->mesg.wait->pids[i]);

		pr_info("\n%c\n", func_work->mesg.wait->async ? '&' : ' ');

	 INIT_WORK(&(func_work->work_s), wait_function);

		if (!queue_work(func_wq, &(func_work->work_s))) {
			pr_info("work was already on a queue\n");
   func_work->mesg.wait->ret = -2;
   copy_to_user((char *) arg, func_work->mesg.wait, sizeof(struct mesg_wait));
   kfree((void *) func_work->mesg.wait);
   kfree((void *) func_work);
			return -1;
		}
	
  pr_info("AVANT FLUSH");
	 flush_work(&(func_work->work_s));
		pr_info("AVANT COPYTOUSER");
		copy_to_user((char *)arg, func_work->mesg.wait, sizeof(struct mesg_wait));
		pr_info("APRES COPYTOUSER");
 
  kfree((void *) func_work->mesg.wait);
  kfree((void *) func_work);

		break;

	case IOCTL_MEMINFO:
		pr_info("Asked MEMINFO");
  func_work->mesg.meminfo = kmalloc(sizeof(struct mesg_meminfo), GFP_KERNEL);
  if(!func_work->mesg.meminfo){
   pr_info("kmalloc failed\n");
   kfree((void *) func_work);
   return -1;
  }
  
  copy_from_user(func_work->mesg.meminfo, (char *)arg, sizeof(struct mesg_meminfo));
		pr_info("recv from user: meminfo %c\n", func_work->mesg.meminfo->async ? '&' : ' ');

		INIT_WORK(&(func_work->work_s), meminfo_function);

		if (!queue_work(func_wq, &(func_work->work_s))) {
			pr_info("work was already on a queue\n");
   func_work->mesg.meminfo->ret = -2;
   copy_to_user((char *) arg, func_work->mesg.meminfo, sizeof(struct mesg_meminfo));
   kfree((void *) func_work->mesg.meminfo);
			kfree((void *) func_work);
   return -1;
		}

  pr_info("AVANT FLUSH");
	 flush_work(&(func_work->work_s));
		pr_info("AVANT COPYTOUSER");
		copy_to_user((char *)arg, func_work->mesg.meminfo, sizeof(struct mesg_meminfo));
		pr_info("APRES COPYTOUSER");
		
  kfree((void *) func_work->mesg.meminfo);
  kfree((void *) func_work);

		break;

	case IOCTL_MODINFO:
		pr_info("Asked MODINFO");
  func_work->mesg.modinfo = kmalloc(sizeof(struct mesg_modinfo), GFP_KERNEL);
  if(!func_work->mesg.modinfo){
   pr_info("kmalloc failed\n");
   kfree((void *) func_work);
   return -1;
  }
 
  copy_from_user(func_work->mesg.modinfo, (char *)arg, sizeof(struct mesg_modinfo));

	 pr_info("recv from user: modinfo %s %c\n", func_work->mesg.modinfo->name,
				func_work->mesg.modinfo->async ? '&' : ' ');

		INIT_WORK(&(func_work->work_s), modinfo_function);

		if (!queue_work(func_wq, &(func_work->work_s))) {
			pr_info("work was already on a queue\n");
   kfree((void *) func_work->mesg.modinfo);
			kfree((void *) func_work);
   return -1;
		}
		pr_info("AVANT FLUSH");
		flush_work(&(func_work->work_s));
		pr_info("AVANT COPYTOUSER");
		copy_to_user((char *)arg, func_work->mesg.modinfo, sizeof(struct mesg_modinfo));
		pr_info("APRES COPYTOUSER");
		
  kfree((void *) func_work->mesg.modinfo);
  kfree((void *) func_work);

		break;
	}

 return 0;
}

const struct file_operations fops = {
 open:	open,
 unlocked_ioctl:ioctl_funcs,
 release:release
};

struct cdev *kernel_cdev;

int char_arr_init(void)
{
	int ret;
	dev_t dev_no, dev;

	kernel_cdev = cdev_alloc();
	kernel_cdev->ops = &fops;
	kernel_cdev->owner = THIS_MODULE;
	pr_info(" Inside init module\n");
	ret = alloc_chrdev_region(&dev_no, 0, 1, "char_arr_dev");

	if (ret < 0) {
		pr_info("Major number allocation is failed\n");
		return ret;
	}

	Major = MAJOR(dev_no);
	dev = MKDEV(Major, 0);
	pr_info(" The major number for your device is %d\n", Major);
	pr_info(" usage: sudo mknod /dev/temp c %d 0\n", Major);
	ret = cdev_add(kernel_cdev, dev, 1);

	if (ret < 0) {
		pr_info("Unable to allocate cdev");
		return ret;
	}

	func_wq = create_workqueue("function_queue");

	return 0;
}

void char_arr_cleanup(void)
{
	pr_info(" Inside cleanup_module\n");
	cdev_del(kernel_cdev);
	unregister_chrdev_region(Major, 1);

	flush_workqueue(func_wq);
	destroy_workqueue(func_wq);
}

MODULE_LICENSE("GPL");
MODULE_VERSION("0.5.4");
module_init(char_arr_init);
module_exit(char_arr_cleanup);
