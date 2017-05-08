struct mesg_kill;
struct mesg_wait;
struct mesg_modinfo;
struct mesg_list;

#define IOC_MAGIC 'k'

#define IOCTL_LIST _IOR(IOC_MAGIC, 4, struct mesg_list)
#define IOCTL_FG _IOR(IOC_MAGIC, 5, struct mesg_fg)
#define IOCTL_KILL _IOR(IOC_MAGIC, 0, struct mesg_kill)
#define IOCTL_WAIT _IOR(IOC_MAGIC, 1, struct mesg_wait)
#define IOCTL_MEMINFO _IOR(IOC_MAGIC, 2, int)
#define IOCTL_MODINFO _IOR(IOC_MAGIC, 3, struct mesg_modinfo)

#define BUFF_SIZE 256
#define MAX_ASYNC 128
#define MAX_PIDS 16

enum cmd_type {
	CMDTYPE_LIST,
	CMDTYPE_KILL,
	CMDTYPE_WAIT,
	CMDTYPE_MEMINFO,
	CMDTYPE_MODINFO
};

struct mesg_fg {
	int id;
	int ret;
	enum cmd_type cmd_type;
	union {
		struct mesg_list *list;
		struct mesg_kill *kill;
		struct mesg_wait *wait;
		struct mesg_modinfo *modinfo;
		struct mesg_meminfo *meminfo;
	} mesg;
};

struct cmd_list {
	int id;
	enum cmd_type cmd_type;
	union {
		struct mesg_list *list;
		struct mesg_kill *kill;
		struct mesg_wait *wait;
		struct mesg_modinfo *modinfo;
		struct mesg_meminfo *meminfo;
	} mesg;
};

struct mesg_list {
	int async;
	int ret;
	int size;
	struct cmd_list cmd_array[MAX_ASYNC];
};

struct mesg_kill {
	int async;
	int signal;
	int pid;
	int ret;
};

struct mesg_wait {
	int async;
	int size;
	int pids[MAX_PIDS];
	int ret;
	int pid;
	int exit_value;
};

struct mesg_modinfo {
	int async;
	int ret;
	char name[BUFF_SIZE];
	char res_name[BUFF_SIZE];
	char res_version[BUFF_SIZE];
	void *res_core;
	char res_args[BUFF_SIZE];
};

struct mesg_meminfo {
	int async;
	int ret;
	unsigned long totalram;	/* Total usable main memory size */
	unsigned long freeram;	/* Available memory size */
	unsigned long sharedram;	/* Amount of shared memory */
	unsigned long bufferram;	/* Memory used by buffers */
	unsigned long totalswap;	/* Total swap space size */
	unsigned long freeswap;	/* Swap space still available */
	unsigned long totalhigh;	/* Total high memory size */
	unsigned long freehigh;	/* Available high memory size */
	int mem_unit;		/* Memory unit size in bytes */
};
