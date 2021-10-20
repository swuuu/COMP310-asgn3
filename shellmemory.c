// Author: Stanley Wu
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int counter = 0;

struct MEM {
	char *var;
	char *value;
};

struct MEM shell_memory[1000];

int set(char name[], char new_value[]){
	struct MEM *curr;
	bool is_in = false; // is name already in shell_memory?
	for (int i=0; i<counter; i++) {
		curr = &shell_memory[i];
		// check if name is already in shell_memory
		if (strcmp(name, curr -> var) == 0) {
			curr -> value = new_value;
			is_in = true;
			break;
		}
	}
	// if name is not in shell_memory, create new variable
	if (!is_in) {
		struct MEM *curr = &shell_memory[counter];
		curr -> var = name;
		curr -> value = new_value;
		counter++;
	}
	return 0;
}

int print(char name[]) {
	struct MEM *curr;
	bool is_in = false;
	for (int i=0; i<counter; i++) {
		curr = &shell_memory[i];
		// check if name exists in shell_memory
		if (strcmp(name, curr -> var) == 0) {
			printf("%s\n", curr -> value);
			is_in = true;
			break;
		}
	}
	if (!is_in) { // variable to print does not exist 
		printf("Variable does not exists\n");
		return 1;
	}
	return 0;
}
