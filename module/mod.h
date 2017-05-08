#include <linux/workqueue.h>
#include <linux/list.h>

#include "ioctl_basics.h"
/*
 * Queues Structs
 */

struct func_work {
        struct work_struct work_s;
        struct list * work_list;
        unsigned int id;
        cmd_type cmd_type;        
        union{
                struct mesg_list    *  list;
                struct mesg_fg      *  fg;
                struct mesg_kill    *  kill;
                struct mesg_wait    *  wait;
                struct mesg_modinfo *  modinfo;
                struct mesg_meminfo *  meminfo;
        }mesg;
};

