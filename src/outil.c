#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include "outil.h"
#include "../module/ioctl_basics.h"
#include <sys/ioctl.h>

#define tool_printf(...)\
	do {\
		printf("[ TOOL ] ");\
		printf(__VA_ARGS__);\
	} while (0)

const struct function_s {
	char *name;
	char *usage;
	int (*function)();
} function_t;

const struct function_s functions[] = {
	{ "list", NULL, list },
	{ "fg", "<id>", fg },
	{ "kill", "<signal> <pid>", kill },
	{ "wait", "<pid> [<pid> ...]", waitf },
	{ "meminfo", NULL, meminfo },
	{ "modinfo", "<name>", modinfo },
	{ "help", NULL, help },
	{ "quit", NULL, quit },
	{ NULL, NULL, error }
};


int perform_ioctl(int func, void *args)
{
	int fd;

	fd = open("/dev/temp", O_RDWR);

	if (fd == -1) {
		tool_printf("Error in opening file\n");
		return -1;
	}

	ioctl(fd, func, args);

	return close(fd);
}

/*
 * Return 0 if success and copy the int to *res
 * Return -1 in case of failure and *res contain garbage
 */
int get_int_from_strtol(int *res, char *arg)
{
	char *endptr;
	long tmp;

	if (arg == NULL || arg[0] == '\n')
		return -1;

	tmp = strtol(arg, &endptr, 10);
	*res = (int)tmp;

	if (arg[0] == '\0' || !(endptr[0] == '\n' || endptr[0] == '\0'))
		return -1;
	return 0;
}

int check_for_async(char *arg)
{
	if (arg != NULL) {
		if (arg[0] == '&')
			return 1;
		else
			return -1;
	}

	return 0;
}

int error_input(char *func_name)
{
	int i = 0;

	while (functions[i].name) {
		if (strcmp(functions[i].name, func_name) == 0) {
			tool_printf("Wrong call\n");
			if (functions[i].usage)
				tool_printf("Usage: %s %s\n",
					func_name, functions[i].usage);
			else
				tool_printf(
					"Function \"%s\" doesn't take any args\n",
					func_name);
			return -1;
		}
		++i;
	}
	tool_printf("Unknown input, type \"help\" for further informations\n");
	return -1;
}

int error(void)
{
	error_input("");
	return 0;
}

int list(void)
{
	char *arg;
	int async = 0;

	arg = strtok(NULL, " ");
	async = check_for_async(arg);
	if (async == -1)
		return error_input("fg");

	if (async)
		tool_printf("list async called\n");
	else
		tool_printf("list called\n");
		
	perform_ioctl(IOCTL_LIST, &async);

	return 0;
}

int fg(void)
{
	char *arg;
	int id;
	struct mesg_fg mesg;
	
	mesg.async = 0;

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(&id, arg) == -1)
		return error_input("fg");

	arg = strtok(NULL, " ");
	mesg.async = check_for_async(arg);
	if (mesg.async == -1)
		return error_input("fg");

	if (mesg.async)
		tool_printf("fg async called with arg: %d\n", id);
	else
		tool_printf("fg called with arg: %d\n", id);
		
	perform_ioctl(IOCTL_FG, &mesg);

	return 0;
}

int kill(void)
{
	char *arg;
	struct mesg_kill mesg;

	mesg.async = 0;

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(&mesg.signal, arg) == -1)
		return error_input("kill");

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(&mesg.pid, arg) == -1)
		return error_input("kill");

	arg = strtok(NULL, "\n ");
	mesg.async = check_for_async(arg);
	if (mesg.async == -1)
		return error_input("kill");

	if (mesg.async)
		tool_printf(
			"kill async called with arg: %d, %d\n",
			mesg.signal, mesg.pid);
	else
		tool_printf(
			"kill called with arg: %d, %d\n",
			mesg.signal, mesg.pid);

	perform_ioctl(IOCTL_KILL, &mesg);

	return 0;
}

int waitf(void)
{
	char *arg;
	struct mesg_wait mesg;
	int i = 1;

	mesg.async = 0;

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(mesg.pids, arg) == -1)
		return error_input("wait");

	while (*arg) {
		if (i == MAX_PIDS+1) {
			tool_printf("Too many pids, limit is %d\n", MAX_PIDS);
			return -1;
		}

		arg = strtok(NULL, "\n ");

		if (get_int_from_strtol(mesg.pids+i, arg) == -1) {
		//if (get_int_from_strtol(&buf, arg) == -1) {
			mesg.async = check_for_async(arg);
			if (mesg.async == -1)
				return error_input("wait");
			break;
		}
		i++;
	}

	mesg.size = i;

	if (mesg.async)
		tool_printf("wait async called with: %d pids\n", mesg.size);
	else
		tool_printf("wait called with: %d pids\n", mesg.size);

	perform_ioctl(IOCTL_WAIT, &mesg);

	return 0;
}

int meminfo(void)
{
	char *arg;
	int async = 0;

	arg = strtok(NULL, " ");
	async = check_for_async(arg);
	if (async == -1)
		return error_input("meminfo");

	if (async)
		tool_printf("meminfo async called\n");
	else
		tool_printf("meminfo called\n");

	perform_ioctl(IOCTL_MEMINFO, &async);

	return 0;
}

int modinfo(void)
{
	int i;
	char *arg;
	struct mesg_modinfo mesg;

	mesg.async = 0;
	memset(mesg.name, '\0', MAX_PIDS);
	arg = strtok(NULL, "\n ");

	if (arg == NULL)
		return error_input("modinfo");

	i = 0;
	while (*arg)
		mesg.name[i++] = *arg++;

	arg = strtok(NULL, "\n ");
	mesg.async = check_for_async(arg);
	if (mesg.async == -1)
		return error_input("modinfo");

	if (mesg.async)
		tool_printf("modinfo async called with %s\n", mesg.name);
	else
		tool_printf("modinfo called with %s\n", mesg.name);

	perform_ioctl(IOCTL_MODINFO, &mesg);

	return 0;
}
int help(void)
{
	int i = 0;

	tool_printf("<-------- HELP -------->\n");
	while (functions[i].name) {
		tool_printf("- %s", functions[i].name);
		if (functions[i].usage)
			printf(" %s\n", functions[i].usage);
		else
			putchar('\n');
		i++;
	}
	tool_printf("<---------------------->\n");

	return 0;
}

int quit(void)
{
	tool_printf("exit...\n");
	exit(EXIT_SUCCESS);
}


int main(void)
{
	char input[BUFF_SIZE];
	char *inp_func;

	tool_printf("<-------- TOOL -------->\n");
	tool_printf("Enter a command, or type \"help\"\n");

	while (1) {
		int i = 0;

		memset(input, '\0', BUFF_SIZE);
		printf("$> ");
		fgets(input, 256, stdin);
		inp_func = strtok(input, "\n ");
		if (inp_func == NULL)
			continue;

		while (functions[i].name) {
			if (strcmp(inp_func, functions[i].name) == 0)
				break;
			i++;
		}

		functions[i].function();
	}

	return EXIT_SUCCESS;
}

