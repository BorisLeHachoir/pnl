#include <linux/workqueue.h>

/*
 * Queues Structs
 */

struct kill_work {
	struct work_struct work_s;
	int signal;
	int pid;
};

struct wait_work {
	struct work_struct work_s;
	int size;
	int pids[MAX_PIDS];
};

struct modinfo_work {
	struct work_struct work_s;
	char name[BUFF_SIZE];
};
