#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "outil.h"

#define BUFF_SIZE 256
#define MAX_PIDS 4
#define tool_printf(...) do {printf("[ TOOL ] "); printf(__VA_ARGS__); } while (0)

const struct function_s {
	char *name;
	char *usage;
	int (*function)();
} function_t;

const struct function_s functions[] = {
	{ "list", NULL, list },
	{ "fg", "<id>", fg },
	{ "kill", "<signal> <pid>", kill },
	{ "wait", "<pid> [<pid> ...]", wait },
	{ "meminfo", NULL, meminfo },
	{ "modinfo", "<name>", modinfo },
	{ "help", NULL, help },
	{ "quit", NULL, quit },
	{ NULL, NULL, error }
};

/*
 * Return 0 if success and copy the int to *res
 * Return -1 in case of failure and *res contain garbage
 */
int get_int_from_strtol(long int *res, char *arg)
{
	char *endptr;

	if (arg == NULL || arg[0] == '\n')
		return -1;

	*res = strtol(arg, &endptr, 10);

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
				tool_printf("Usage: %s %s\n", func_name, functions[i].usage);
			else
				tool_printf("Function \"%s\" doesn't take any args\n", func_name);
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

	return 0;
}

int fg(void)
{
	char *arg;
	long int id;
	int async = 0;

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(&id, arg) == -1)
		return error_input("fg");

	arg = strtok(NULL, " ");
	async = check_for_async(arg);
	if (async == -1)
		return error_input("fg");

	if (async)
		tool_printf("fg async called with arg: %ld\n", id);
	else
		tool_printf("fg called with arg: %ld\n", id);

	return 0;
}

int kill(void)
{
	char *arg;
	long int pid, signal;
	int async = 0;

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(&signal, arg) == -1)
		return error_input("kill");

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(&pid, arg) == -1)
		return error_input("kill");

	arg = strtok(NULL, " ");
	async = check_for_async(arg);
	if (async == -1)
		return error_input("kill");

	if (async)
		tool_printf("kill async called with arg: %ld, %ld\n", signal, pid);
	else
		tool_printf("kill called with arg: %ld, %ld\n", signal, pid);

	return 0;
}

int wait(void)
{
	char *arg;
	long int pids[MAX_PIDS];
	int i = 1, async = 0;

	arg = strtok(NULL, " ");
	if (get_int_from_strtol(pids, arg) == -1)
		return error_input("wait");

	while (arg) {
		if (i == MAX_PIDS+1) {
			tool_printf("Too many pids, limit is %d\n", MAX_PIDS);
			return -1;
		}

		arg = strtok(NULL, "\n ");

		if (get_int_from_strtol(pids+i, arg) == -1) {
			async = check_for_async(arg);
			if (async == -1)
				return error_input("wait");
			break;
		}
		i++;
	}

	if (async)
		tool_printf("wait async called with: %d pids\n", i);
	else
		tool_printf("wait called with: %d pids\n", i);

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

	return 0;
}

int modinfo(void)
{
	char *arg, *module_name;
	int async = 0;

	module_name = strtok(NULL, " ");

	if (module_name == NULL)
		return error_input("modinfo");

	arg = strtok(NULL, " ");
	async = check_for_async(arg);
	if (async == -1)
		return error_input("modinfo");

	if (async)
		tool_printf("modinfo async called with %s\n", module_name);
	else
		tool_printf("modinfo called with %s\n", module_name);

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

