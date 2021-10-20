// Author: Stanley Wu
// COMP 310: Assignment 1
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "interpreter.h"


int parse(char user_input[]){
	char tmp[200];
	char *tmp2;
	char *words[100];
	int a,b,i;
	int w = 0;
	
	// if enter was pressed, ignore it 
	if (user_input[0] == '\n') {
		return 0;
	}

	for (a = 0; user_input[a] == ' ' && user_input[a] == '\0' && a < 1000; a++); // skip spaces in front of command
	while (user_input[a] != '\0' && a < 1000) {
		for (i = a; user_input[i] != '\0' && user_input[i] == ' ' && i < 1000; i++, a++); // skip spaces
		for (b = 0; user_input[a] != '\0' && a < 1000 && user_input[a] != ' '; a++, b++) {
			tmp[b] = user_input[a];
		}
		tmp[b] = '\0';
		tmp2 = strtok(strdup(tmp), "\n");
		words[w] = strtok(strdup(tmp2), "\r");
		if (user_input[a] == '\0') {w++; break;}

		a++; w++;
	}
	return interpreter(words, w);
}


int shellUI(){
	int error_code = 0;
	char prompt[100] = {'$', '\0'};
	printf("Kernel 1.0 loaded!\n");
	printf("Welcome to the Stanley Wu shell!\n");
	printf("Shell version 2.0 Updated February 2021\n");
	char user_input[999];
	while(1) {
		if (isatty(STDIN_FILENO)) {
			printf("%s ", prompt);
		}
		if (fgets(user_input, 999, stdin) == NULL) {
			freopen("/dev/tty", "r", stdin);
			// break;
		} // if script file does not have quit command 
		error_code = parse(user_input);
		if (error_code == -1) {break;} // quit error code
	}
	return error_code;
}
