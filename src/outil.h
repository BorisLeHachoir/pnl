#pragma once

const char *cmd_type_str[] = {
	"LIST",
	"FG",
	"KILL",
	"WAIT",
	"MEMINFO",
	"MODINFO"
};

int error_input(char *func_name);

int error(void);

int list(void);

int fg(void);

int kill(void);

int waitf(void);

int meminfo(void);

int modinfo(void);

int help(void);

int quit(void);
