#define IOC_MAGIC 'k'

#define IOCTL_KILL _IOR(IOC_MAGIC, 0, int) 
#define IOCTL_WAIT _IOR(IOC_MAGIC, 1, int) 
#define IOCTL_MEMINFO _IOR(IOC_MAGIC, 2, int) 
#define IOCTL_MODINFO _IOR(IOC_MAGIC, 3, int) 

#define BUFF_SIZE 256
#define MAX_PIDS 16


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
};