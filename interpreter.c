// Author: Stanley WU
#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"
#include <time.h>

// ERROR CODE LIST
// -1 = quit the shell
// 0 = normal
// 1 = no file argument given / cannot find file
// 2 = wrong number of arguments passed
// 3 = error running file (in run command)
// 5 = text file has already been loaded (in exec command)
// 6 = cannot open file

// help command
int help() {
	printf("help:            Displays all the commands\n");
	printf("quit:            Exits/ terminates the shell\n");
	printf("set VAR STRING:  Assigns a value to a shell memory\n");
	printf("print VAR:       Displays the STRING assigned to VAR\n");
	printf("run SCRIPT.TXT   Executes the file SCRIPT.TXT\n");
	return 0;
}

// quit command
int quit() {
	printf("Bye!\n");
	return -1; // error code -1 means quit the shell
}

// run command 
int run(char *words[]) {
	int error_code = 0;
	char line[1000];
	// file argument is ""
	if (!strcmp(words[1], "")) {
		printf("Error: No file given\n");
		return 1; 
	}
	FILE *p = fopen(words[1], "rt");
	// script file doesn't exist
	if (p == NULL) {
		printf("Error: Script not found\n");
		return 1;
	}
	fgets(line, 999, p);
	while (!feof(p)) {
		error_code = parse(line);
		if (error_code != 0 && error_code != -1) {
			printf("Error running file\n");
			fclose(p);
			return 3; // error code 3 is error running file
		}
		fgets(line, 999, p);
	}
	fclose(p);
	return error_code;
}

// exec command
int exec(char *words[], int w) {
	
	// clock_t start, end;
	// long cpu_time_used;
	// start = clock();

	char *tmp[w+1];
	tmp[0] = words[1];
	int len = 1; // length of tmp

	// loop through the files and initialize
	int err;
	for (int i=0; i < w-1; i++) {
		err = myinit(words[i+1]);
		if (err == 1) {
			return 6; // error code 6 means cannot find file
		}
	}
	// run the text files
	scheduler();
	// end = clock();
	// cpu_time_used = ((long) (end - start));
	// printf("%s %ld\n", "CPU cycles elapsed:", cpu_time_used);

	return 0;
}

// helper function that prints error if wrong number of arguments passed
int print_error() {
	printf("Invalid number of arguments\n");
	return 2; // error code 2 means wrong number of arguments passed
}

int interpreter(char *words[], int w) {
	int error_code = 0;

	if (!strcmp(words[0], "help\0")) {
		if (w != 1) {return print_error();}
		error_code = help();
	} else if (!strcmp(words[0], "quit\0")) {
		if (w != 1) {return print_error();}
		error_code = quit();
	} else if (!strcmp(words[0], "run\0")) {
		if (w != 2) {return print_error();}
		error_code = run(words);
	} else if (!strcmp(words[0], "set")) {
		if (w != 3) {return print_error();}
		error_code = set(words[1], words[2]);	
	} else if (!strcmp(words[0], "print")) {
		if (w != 2) {return print_error();}
		error_code = print(words[1]);
	} else if (!strcmp(words[0], "exec")) {
		if (!(2 <= w && w <= 4)) {return print_error();}
		error_code = exec(words, w);
	} else {
		printf("Unknown command\n");
		error_code = 1;
	}
	return error_code;
}
