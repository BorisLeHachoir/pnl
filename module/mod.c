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
#include <linux/swap.h>

/* modinfo */
#include <linux/module.h>

/* wait */
#include <linux/pid.h>
#include <linux/delay.h>

#include "mod.h"

static int Major;

static int id;
static struct func_work work_list_head;

static void auto_flushing_second(struct work_struct *wk);

static struct workqueue_struct *func_wq;
static struct delayed_work *perm_worker_first;
static struct delayed_work *perm_worker_second;

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

static void auto_flushing_first(struct work_struct *wk)
{
	pr_info("perm_worker: check for flush\n");
	ssleep(1);
	pr_info("perm_worker: stop\n");

	INIT_DELAYED_WORK(perm_worker_second, auto_flushing_second);
	if (!queue_delayed_work(func_wq, perm_worker_second, 5 * HZ)) {
		pr_info("failed launching perm_worker\n");
		return;
	}
	cancel_delayed_work(perm_worker_first);
}

static void auto_flushing_second(struct work_struct *wk)
{
	pr_info("perm_worker: check for flush\n");
	ssleep(1);
	pr_info("perm_worker: stop\n");

	INIT_DELAYED_WORK(perm_worker_first, auto_flushing_first);
	if (!queue_delayed_work(func_wq, perm_worker_first, 5 * HZ)) {
		pr_info("failed launching perm_worker\n");
		return;
	}
	cancel_delayed_work(perm_worker_second);
}

static void fg_function(struct work_struct *wk)
{       
        struct func_work *async;
        struct func_work *work = container_of(wk, struct func_work, work_s);

        list_for_each_entry(async, &work_list_head.work_list, work_list)
        {
                if(work->mesg.fg->id == (int) async->id) break;
        }
        
        if(work->mesg.fg->id == (int) async->id){

                wait_for_completion(&(async->cmd_comp));
                
                switch (async->cmd_type) {
		case CMDTYPE_LIST:
		        work->mesg.fg->mesg.list = async->mesg.list;	
			break;
		case CMDTYPE_KILL:
			work->mesg.fg->mesg.kill = async->mesg.kill;
			break;
		case CMDTYPE_WAIT:
			work->mesg.fg->mesg.wait = async->mesg.wait;
			break;
		case CMDTYPE_MEMINFO:
		        work->mesg.fg->mesg.modinfo = async->mesg.modinfo;
			break;
		case CMDTYPE_MODINFO:
		        work->mesg.fg->mesg.meminfo = async->mesg.meminfo;
			break;
		}
                work->mesg.fg->ret = 0;

        }else{
                work->mesg.fg->ret = -1;
        }
}
 


static void list_function(struct work_struct *wk)
{
	struct func_work *async;

	struct func_work *work = container_of(wk, struct func_work, work_s);

	pr_info("[LIST_FUNCTION]\n");

	work->mesg.list->size = 0;

	list_for_each_entry(async, &work_list_head.work_list, work_list) {
		pr_info("ENTRY\n");
<<<<<<< HEAD
		work->mesg.list->cmd_array[work->mesg.list->size].id =
		    (int)async->id;
		work->mesg.list->cmd_array[work->mesg.list->size].cmd_type =
		    async->cmd_type;
		switch (async->cmd_type) {
		case CMDTYPE_LIST:
			work->mesg.list->cmd_array[work->mesg.list->size].mesg.
			    list = async->mesg.list;
			break;
		case CMDTYPE_KILL:
			work->mesg.list->cmd_array[work->mesg.list->size].
			    mesg.kill = async->mesg.kill;
			break;
		case CMDTYPE_WAIT:
			work->mesg.list->cmd_array[work->mesg.list->size].
			    mesg.wait = async->mesg.wait;
			break;
		case CMDTYPE_MEMINFO:
			work->mesg.list->cmd_array[work->mesg.list->size].
			    mesg.meminfo = async->mesg.meminfo;
			break;
		case CMDTYPE_MODINFO:
			work->mesg.list->cmd_array[work->mesg.list->size].
			    mesg.modinfo = async->mesg.modinfo;
			break;
		}
=======
		work->mesg.list->cmd_array[work->mesg.list->size].id = (int)async->id;
		work->mesg.list->cmd_array[work->mesg.list->size].cmd_type = async->cmd_type;
			switch (async->cmd_type) {
			case CMDTYPE_LIST:
			        work->mesg.list->cmd_array[work->mesg.list->size].mesg.list = async->mesg.list;
			        break;
		        case CMDTYPE_KILL:
			        work->mesg.list->cmd_array[work->mesg.list->size].mesg.
			        kill = async->mesg.kill;
			        break;
		        case CMDTYPE_WAIT:
			        work->mesg.list->cmd_array[work->mesg.list->size].mesg.
			        wait = async->mesg.wait;
			        break;
		        case CMDTYPE_MEMINFO:
			        work->mesg.list->cmd_array[work->mesg.list->size].mesg.
			        meminfo = async->mesg.meminfo;
			        break;
		        case CMDTYPE_MODINFO:
			        work->mesg.list->cmd_array[work->mesg.list->size].mesg.
			        modinfo = async->mesg.modinfo;
			        break;
		        }

>>>>>>> d61bb73537c02e432e608371481181058700ccac
		work->mesg.list->size++;
                pr_info("FINENTRY\n");
	}
         complete(&(work->cmd_comp));
         pr_info("FIN\n");
}

static void kill_function(struct work_struct *wk)
{
	struct pid *pid_struct;
	struct func_work *work = container_of(wk, struct func_work, work_s);

	pr_info("[KILL_FUNCTION] signal: %d pid: %d\n", work->mesg.kill->signal,
		work->mesg.kill->pid);

	/* Check signal valid */
	if (work->mesg.kill->signal > _NSIG || work->mesg.kill->signal < 1){
		work->mesg.kill->ret = -22;
                complete(&(work->cmd_comp));
	        return;
        }

	pid_struct = find_get_pid(work->mesg.kill->pid);

	if (pid_struct) {
		work->mesg.kill->ret =
		    kill_pid(pid_struct, work->mesg.kill->signal, 1);
		pr_info("ret = %d", work->mesg.kill->ret);
		/*put_pid(pid_struct); */
	} else{
		work->mesg.kill->ret = -3;
        }
        complete(&(work->cmd_comp));
}

static void wait_function(struct work_struct *wk)
{
	int i, cpt = 0, pid = 0;
	struct task_struct *task_struct = NULL;
	struct task_struct *task_struct_tab[MAX_PIDS];
	struct pid *pid_struct = NULL;
	struct pid *pid_struct_tab[MAX_PIDS];

	struct func_work *work = container_of(wk, struct func_work, work_s);

	for (i = 0; i < work->mesg.wait->size; i++) {
		pid_struct = find_get_pid(work->mesg.wait->pids[i]);
		if (pid_struct) {
			task_struct = get_pid_task(pid_struct, PIDTYPE_PID);
			if (task_struct) {
				pid_struct_tab[cpt] = pid_struct;
				task_struct_tab[cpt] = task_struct;
				cpt++;
			}
			/*else{
			   put_pid(pid_struct);
			   } */
		}
	}

	if (cpt == 0)
		work->mesg.wait->ret = -2;
	else
		while (pid == 0) {
			for (i = 0; i < cpt; i++) {
				task_struct = task_struct_tab[i];
				if (task_struct) {
					if (pid_alive(task_struct)) {
						ssleep(1);
					} else {
						pid = 1;
						work->mesg.wait->pid =
						    task_struct->pid;
						work->mesg.wait->exit_value =
						    task_struct->exit_code;
						work->mesg.wait->ret = 0;
						break;

					}
				} else
					pr_info("tast_struct null\n");
				ssleep(1);
			}
		}

	for (i = 0; i < cpt; i++)
		put_task_struct(task_struct_tab[i]);
        complete(&(work->cmd_comp));
}

static void meminfo_function(struct work_struct *wk)
{
	struct sysinfo mem_val;
	struct func_work *work = container_of(wk, struct func_work, work_s);

	si_meminfo(&mem_val);
	si_swapinfo(&mem_val);

	work->mesg.meminfo->totalram = mem_val.totalram;
	work->mesg.meminfo->freeram = mem_val.freeram;
	work->mesg.meminfo->sharedram = mem_val.sharedram;
	work->mesg.meminfo->bufferram = mem_val.bufferram;
	work->mesg.meminfo->totalswap = mem_val.totalswap;
	work->mesg.meminfo->freeswap = mem_val.freeswap;
	work->mesg.meminfo->totalhigh = mem_val.totalhigh;
	work->mesg.meminfo->freehigh = mem_val.freehigh;
	work->mesg.meminfo->mem_unit = mem_val.mem_unit;

	work->mesg.meminfo->ret = 0;

        complete(&(work->cmd_comp));
}

static void modinfo_function(struct work_struct *wk)
{
	struct module *module_tofind;

	struct func_work *work = container_of(wk, struct func_work, work_s);

	if (mutex_lock_interruptible(&module_mutex) != 0) {
		work->mesg.modinfo->ret = -EINTR;
                complete(&(work->cmd_comp));
		return;
	}

	module_tofind = find_module(work->mesg.modinfo->name);
	if (module_tofind) {
		pr_info("nom: %s\n", module_tofind->name);
		strcpy(work->mesg.modinfo->res_name, module_tofind->name);
		pr_info("version: %s\n", module_tofind->version);
		strcpy(work->mesg.modinfo->res_version, module_tofind->version);
		pr_info("load adr: %p\n", module_tofind->module_core);
		work->mesg.modinfo->res_core = module_tofind->module_core;
		if (module_tofind->args) {
			pr_info("args: %s\n", module_tofind->args);
			strcpy(work->mesg.modinfo->res_args,
			       module_tofind->args);
		}
	}

	mutex_unlock(&module_mutex);
        complete(&(work->cmd_comp));
	pr_info("fin modinfo function");
}

static inline int process_ioctl_list(struct func_work *func_work,
				     unsigned long arg)
{
	pr_info("ASKED LIST");
	func_work->mesg.list = kmalloc(sizeof(struct mesg_list), GFP_KERNEL);
	if (!func_work->mesg.list) {
		pr_info("kmalloc failed\n");
		kfree((void *)func_work);
		return -1;
	}
	copy_from_user(func_work->mesg.list, (char *)arg,
		       sizeof(struct mesg_list));
	pr_info("recv from user: list %c\n",
		func_work->mesg.list->async ? '&' : ' ');
	func_work->id = id++;
	func_work->cmd_type = CMDTYPE_LIST;
        init_completion(&(func_work->cmd_comp));

	if (func_work->mesg.list->async)
		list_add_tail(&(func_work->work_list),
			      &work_list_head.work_list);

	INIT_WORK(&(func_work->work_s), list_function);

	if (!queue_work(func_wq, &(func_work->work_s))) {
		pr_info("work was already on a queue\n");
		func_work->mesg.list->ret = -2;
		copy_to_user((char *)arg, func_work->mesg.list,
			     sizeof(struct mesg_list));
		if (func_work->mesg.list->async)
			list_del(&func_work->work_list);

		kfree((void *)func_work->mesg.list);
		kfree((void *)func_work);
		return -1;
	}

	if (!func_work->mesg.list->async) {
		flush_work(&(func_work->work_s));
		copy_to_user((char *)arg, func_work->mesg.list,
			     sizeof(struct mesg_list));

		kfree((void *)func_work->mesg.list);
		kfree((void *)func_work);
	}
	return 0;
}

static inline int process_ioctl_fg(struct func_work *func_work,
				   unsigned long arg)
{
	pr_info("ASKED FG");
	func_work->mesg.fg = kmalloc(sizeof(struct mesg_fg), GFP_KERNEL);
	if (!func_work->mesg.fg) {
		pr_info("kmalloc failed\n");
		kfree((void *)func_work);
		return -1;
	}
	copy_from_user(func_work->mesg.fg, (char *)arg, sizeof(struct mesg_fg));
	pr_info("recv from user: fg %d\n", func_work->mesg.fg->id);
	func_work->id = id++;

        INIT_WORK(&(func_work->work_s), fg_function);

        if( (!queue_work(func_wq, &(func_work->work_s)))){
                pr_info("work was already on a queue\n");
                func_work->mesg.fg->ret = -2;
                copy_to_user((char*) arg, func_work->mesg.fg, 
                                        sizeof(struct mesg_fg));
                kfree((void *) func_work->mesg.fg);
                kfree((void *) func_work);
                return -1;
        }

        flush_work(&(func_work->work_s));
	copy_to_user((char *)arg, func_work->mesg.fg,
				sizeof(struct mesg_fg));
	kfree((void *)func_work->mesg.fg);
	kfree((void *)func_work);

	return 0;
}

static inline int process_ioctl_kill(struct func_work *func_work,
				     unsigned long arg)
{
	pr_info("ASKED KILL");
	func_work->mesg.kill = kmalloc(sizeof(struct mesg_kill), GFP_KERNEL);
	if (!func_work->mesg.kill) {
		pr_info("kmalloc failed\n");
		kfree((void *)func_work);
		return -1;
	}
	copy_from_user(func_work->mesg.kill, (char *)arg,
		       sizeof(struct mesg_kill));
	pr_info("recv from user: kill %d %d %c\n", func_work->mesg.kill->signal,
		func_work->mesg.kill->pid,
		func_work->mesg.kill->async ? '&' : ' ');
	func_work->id = id++;
	func_work->cmd_type = CMDTYPE_KILL;
        init_completion(&(func_work->cmd_comp));

	if (func_work->mesg.kill->async)
		list_add_tail(&(func_work->work_list),
			      &work_list_head.work_list);

	INIT_WORK(&(func_work->work_s), kill_function);

	if (!queue_work(func_wq, &(func_work->work_s))) {
		pr_info("work was already on a queue\n");
		func_work->mesg.kill->ret = -2;
		copy_to_user((char *)arg, func_work->mesg.kill,
			     sizeof(struct mesg_kill));
		if (func_work->mesg.kill->async)
			list_del(&func_work->work_list);

		kfree((void *)func_work->mesg.kill);
		kfree((void *)func_work);
		return -1;
	}

	if (!func_work->mesg.kill->async) {
		flush_work(&(func_work->work_s));
		copy_to_user((char *)arg, func_work->mesg.kill,
			     sizeof(struct mesg_kill));
		kfree((void *)func_work->mesg.kill);
		kfree((void *)func_work);
	}

	return 0;
}

static inline int process_ioctl_wait(struct func_work *func_work,
				     unsigned long arg)
{
	int i;

	pr_info("Asked WAIT");

	func_work->mesg.wait = kmalloc(sizeof(struct mesg_wait), GFP_KERNEL);
	if (!func_work->mesg.wait) {
		/*pr_info("kmalloc failed\n"); */
		kfree((void *)func_work);
		return -1;
	}

	copy_from_user(func_work->mesg.wait, (char *)arg,
		       sizeof(struct mesg_wait));

	func_work->id = id++;
	func_work->cmd_type = CMDTYPE_WAIT;
        init_completion(&(func_work->cmd_comp));

	pr_info("recv from user: wait ");
	for (i = 0; i < func_work->mesg.wait->size - 1; ++i)
		pr_info("%d ", func_work->mesg.wait->pids[i]);

	pr_info("\n%c\n", func_work->mesg.wait->async ? '&' : ' ');

	if (func_work->mesg.wait->async)
		list_add_tail(&(func_work->work_list),
			      &work_list_head.work_list);

	INIT_WORK(&(func_work->work_s), wait_function);

	if (!queue_work(func_wq, &(func_work->work_s))) {
		pr_info("work was already on a queue\n");
		func_work->mesg.wait->ret = -2;
		copy_to_user((char *)arg, func_work->mesg.wait,
			     sizeof(struct mesg_wait));
		if (func_work->mesg.wait->async)
			list_del(&func_work->work_list);
		kfree((void *)func_work->mesg.wait);
		kfree((void *)func_work);
		return -1;
	}

	if (!func_work->mesg.wait->async) {
		flush_work(&(func_work->work_s));
		copy_to_user((char *)arg, func_work->mesg.wait,
			     sizeof(struct mesg_wait));
		kfree((void *)func_work->mesg.wait);
		kfree((void *)func_work);
	}
	return 0;
}

static inline int process_ioctl_meminfo(struct func_work *func_work,
					unsigned long arg)
{
	pr_info("Asked MEMINFO");
	func_work->mesg.meminfo =
	    kmalloc(sizeof(struct mesg_meminfo), GFP_KERNEL);
	if (!func_work->mesg.meminfo) {
		pr_info("kmalloc failed\n");
		kfree((void *)func_work);
		return -1;
	}

	copy_from_user(func_work->mesg.meminfo, (char *)arg,
		       sizeof(struct mesg_meminfo));
	pr_info("recv from user: meminfo %c\n",
		func_work->mesg.meminfo->async ? '&' : ' ');
	func_work->id = id++;
	func_work->cmd_type = CMDTYPE_MEMINFO;
        init_completion(&(func_work->cmd_comp));

	if (func_work->mesg.meminfo->async) {
		list_add(&(func_work->work_list), &work_list_head.work_list);
		pr_info("ASYNC MEMINFO");
	}

	INIT_WORK(&(func_work->work_s), meminfo_function);
	if (!queue_work(func_wq, &(func_work->work_s))) {
		pr_info("work was already on a queue\n");
		func_work->mesg.meminfo->ret = -2;
		copy_to_user((char *)arg, func_work->mesg.meminfo,
			     sizeof(struct mesg_meminfo));
		if (func_work->mesg.meminfo->async)
			list_del(&func_work->work_list);

		kfree((void *)func_work->mesg.meminfo);
		kfree((void *)func_work);
		return -1;
	}

	if (!func_work->mesg.meminfo->async) {
		flush_work(&(func_work->work_s));
		copy_to_user((char *)arg, func_work->mesg.meminfo,
			     sizeof(struct mesg_meminfo));

		kfree((void *)func_work->mesg.meminfo);
		kfree((void *)func_work);
	}
	return 0;
}

static inline int process_ioctl_modinfo(struct func_work *func_work,
					unsigned long arg)
{
	pr_info("Asked MODINFO");
	func_work->mesg.modinfo =
	    kmalloc(sizeof(struct mesg_modinfo), GFP_KERNEL);
	if (!func_work->mesg.modinfo) {
		pr_info("kmalloc failed\n");
		kfree((void *)func_work);
		return -1;
	}

	copy_from_user(func_work->mesg.modinfo, (char *)arg,
		       sizeof(struct mesg_modinfo));

	pr_info("recv from user: modinfo %s %c\n",
		func_work->mesg.modinfo->name,
		func_work->mesg.modinfo->async ? '&' : ' ');
	func_work->id = id++;
	func_work->cmd_type = CMDTYPE_MODINFO;
        init_completion(&(func_work->cmd_comp));

	if (func_work->mesg.modinfo->async)
		list_add_tail(&(func_work->work_list),
			      &work_list_head.work_list);

	INIT_WORK(&(func_work->work_s), modinfo_function);

	if (!queue_work(func_wq, &(func_work->work_s))) {
		pr_info("work was already on a queue\n");
		func_work->mesg.modinfo->ret = -2;
		copy_to_user((char *)arg, func_work->mesg.modinfo,
			     sizeof(struct mesg_modinfo));
		if (func_work->mesg.modinfo->async)
			list_del(&func_work->work_list);

		kfree((void *)func_work->mesg.modinfo);
		kfree((void *)func_work);
		return -1;
	}

	if (!func_work->mesg.modinfo->async) {
		flush_work(&(func_work->work_s));
		copy_to_user((char *)arg, func_work->mesg.modinfo,
			     sizeof(struct mesg_modinfo));

		kfree((void *)func_work->mesg.modinfo);
		kfree((void *)func_work);
	}

	return 0;
}

long ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct func_work *func_work;

	func_work = kmalloc(sizeof(func_work), GFP_KERNEL);

	if (!func_work) {
		/*pr_info("kmalloc failed\n"); */
		return -1;
	}

	switch (cmd) {
	case IOCTL_LIST:
		return process_ioctl_list(func_work, arg);

	case IOCTL_FG:
		return process_ioctl_fg(func_work, arg);

	case IOCTL_KILL:
		return process_ioctl_kill(func_work, arg);

	case IOCTL_WAIT:
		return process_ioctl_wait(func_work, arg);

	case IOCTL_MEMINFO:
		return process_ioctl_meminfo(func_work, arg);

	case IOCTL_MODINFO:
		return process_ioctl_modinfo(func_work, arg);

	default:
		pr_info("Error ioctl: cmd no found !");
		kfree((void *)func_work);
		return -1;
	}
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

	id = 0;

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

	INIT_LIST_HEAD(&work_list_head.work_list);

	perm_worker_first = kmalloc(sizeof(struct delayed_work), GFP_KERNEL);
	if (!perm_worker_first) {
		/*pr_info("Kmalloc perm_worker failed"); */
		return -1;
	}
	perm_worker_second = kmalloc(sizeof(struct delayed_work), GFP_KERNEL);
	if (!perm_worker_second) {
		/*pr_info("Kmalloc perm_worker failed"); */
		return -1;
	}

	INIT_DELAYED_WORK(perm_worker_first, auto_flushing_first);

	if (!schedule_delayed_work(perm_worker_first, 5 * HZ)) {
		pr_info("failed launching perm_worker\n");
		return -1;
	}

	return 0;
}

void char_arr_cleanup(void)
{
	struct func_work *func_work_tmp;

	pr_info(" Inside cleanup_module\n");
	cdev_del(kernel_cdev);
	unregister_chrdev_region(Major, 1);

	cancel_delayed_work(perm_worker_first);
	cancel_delayed_work(perm_worker_second);

	if (perm_worker_first)
		kfree((void *)
		      perm_worker_first);
	if (perm_worker_second)
		kfree((void *)perm_worker_second);

	while (&work_list_head.work_list != work_list_head.work_list.next) {
                
		func_work_tmp = NULL;
		func_work_tmp =
		    container_of(work_list_head.work_list.next,
				 struct func_work, work_list);
		if (func_work_tmp) {
			switch (func_work_tmp->cmd_type) {
			case CMDTYPE_LIST:
				if (func_work_tmp->mesg.list)
					kfree((void *)func_work_tmp->mesg.list);
				break;
			case CMDTYPE_KILL:
				if (func_work_tmp->mesg.kill)
					kfree((void *)func_work_tmp->mesg.kill);
				break;
			case CMDTYPE_WAIT:
				if (func_work_tmp->mesg.wait)
					kfree((void *)func_work_tmp->mesg.wait);
				break;
			case CMDTYPE_MEMINFO:
				if (func_work_tmp->mesg.meminfo)
					kfree((void *)func_work_tmp->mesg.
					      meminfo);
				break;
			case CMDTYPE_MODINFO:
				if (func_work_tmp->mesg.modinfo)
					kfree((void *)func_work_tmp->mesg.
					      modinfo);
				break;
			}
			kfree((void *)func_work_tmp);
		}

		list_del(work_list_head.work_list.next);
	}

	flush_workqueue(func_wq);
	destroy_workqueue(func_wq);
}

MODULE_LICENSE("GPL");
MODULE_VERSION("0.5.4");
module_init(char_arr_init);
module_exit(char_arr_cleanup);
