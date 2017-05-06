#include <linux/workqueue.h>

struct mesg_kill;
struct mesg_wait;
struct mesg_modinfo;

struct kill_work;
struct wait_work;
struct_modinfo_work;

#define IOC_MAGIC 'k'

#define IOCTL_KILL _IOR(IOC_MAGIC, 0, struct mesg_kill)
#define IOCTL_WAIT _IOR(IOC_MAGIC, 1, struct mesg_wait)
#define IOCTL_MEMINFO _IOR(IOC_MAGIC, 2, int)
#define IOCTL_MODINFO _IOR(IOC_MAGIC, 3, struct mesg_modinfo)

#define BUFF_SIZE 256
#define MAX_PIDS 16


struct mesg_kill{
 int async;
	int signal;
	int pid;
};

struct mesg_wait{
	int async;
	int size;
	int pids[MAX_PIDS];
};

struct mesg_modinfo{
	int async;
	char name[BUFF_SIZE];
};

struct kill_work{
	struct work_struct work_s;
	int signal;
	int pid;
};

struct wait_work{
	struct work_struct work_s;
	int size;
	int pids[MAX_PIDS];
};

struct modinfo_work{
	struct work_struct work_s;
	char name[BUFF_SIZE];
}
