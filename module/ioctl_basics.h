struct mesg_kill;
struct mesg_wait;
struct mesg_modinfo;

#define IOC_MAGIC 'k'

#define IOCTL_LIST _IOR(IOC_MAGIC, 4, unsigned long)
#define IOCTL_FG _IOR(IOC_MAGIC, 5, struct mesg_fg)
#define IOCTL_KILL _IOR(IOC_MAGIC, 0, struct mesg_kill)
#define IOCTL_WAIT _IOR(IOC_MAGIC, 1, struct mesg_wait)
#define IOCTL_MEMINFO _IOR(IOC_MAGIC, 2, int)
#define IOCTL_MODINFO _IOR(IOC_MAGIC, 3, struct mesg_modinfo)

#define BUFF_SIZE 256
#define MAX_PIDS 16


struct mesg_fg {
	int async;
	int id;
};

struct mesg_kill {
	int async;
	int signal;
	int pid;
};

struct mesg_wait {
	int async;
	int size;
	int pids[MAX_PIDS];
};

struct mesg_modinfo {
	int async;
	char name[BUFF_SIZE];
	char res_name[BUFF_SIZE];
	char res_version[BUFF_SIZE];
	void *res_core;
	char res_args[BUFF_SIZE];
};
