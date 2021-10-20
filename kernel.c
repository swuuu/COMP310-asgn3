#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include "shell.h"
#include "pcb.h"
#include "cpu.h"
#include "ram.h"
#include "memorymanager.h"

struct Node {
	struct PCB *curr;
	struct Node *next;
};

struct Node *head = NULL;
struct Node *tail = NULL;
struct PCB *curr = NULL;
struct CPU *pointer_CPU = NULL;
int len_ll = 0;

int scheduler() {
	int new_quanta;
	struct Node *tmp;
	struct CPU *cpu = (struct CPU *) malloc(sizeof(struct CPU));
	pointer_CPU = cpu;
	cpu -> quanta = 2;
	// loop through the queue until empty
	while (len_ll != 0) {
		// check if cpu is ready
		if (!(curr == NULL)) {continue;}
		// get head of queue -> gives a PCB
		curr = head -> curr; 	
		// run the program
		cpu -> IP = curr -> pageTable[curr -> PC_page];
		cpu -> offset = curr -> PC_offset;
		int max_in_frame = 4;
		if ((curr -> PC_page) + 1 == (curr -> pages_max)) { // checking if we reached the final frame
			max_in_frame = (curr -> lines_max) % 4; // run for only remainder
			if (max_in_frame == 0) {
				max_in_frame = 4;
			}
		}
		int quanta_left = runx(max_in_frame - (curr -> PC_offset));
		int is_done = 1;
		// updating pcb
		if (quanta_left == -1) { // page fault
			is_done = page_fault(curr);
		} else {
			// update pcb field
			curr -> PC_offset = max_in_frame - quanta_left;
			curr -> PC = (curr -> PC) + (curr -> PC_offset);
		}
		// remove program if no more lines left
		if (is_done == 0) {
			curr = head -> curr; // remove program from queue
			unload(curr -> pageTable, curr -> validTable); // clear the ram
			tmp = head;
			free(head->curr); // free memory		
			head = head -> next; // shift the head
			free(tmp); // free node 
			len_ll--; // decrease length
		} else {
			// put back to program to end of queue
			if (head == tail) {
				curr = NULL;
				continue; // only 1 node case
			}
			tmp = head; // save reference of old head
			head = head -> next; // shift the head
			tail -> next = tmp; // update the next attribute of old tail
			tail = tmp; // shift the tail
			tail -> next = NULL; // update the next attribute of new tail
		}
		curr = NULL;
	}
	free(cpu);
	free(curr);
	head = NULL;
	tail = NULL;
	curr = NULL;
	pointer_CPU = NULL;
	clear(); // clear the RAM
	return 0;
}

void addToReady(struct PCB *pcb) {
	struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
	new_node -> curr = pcb;
	// if queue is empty
	if (head == NULL) {
		head = new_node;
		head -> next = NULL;
		tail = new_node;
		tail -> next = NULL;
	} else if ((head -> next) == NULL) { // if linked list has only 1 element
		tail = new_node;
		head -> next = new_node;
	} else { // if linked list has 2 elements
		tail -> next = new_node;
		tail = new_node; 
		tail -> next = NULL;
	}
	len_ll++;
}

int myinit(char *filename) {
	int start; 
	int end;
	FILE *p = fopen(filename, "rt");
	if (p == NULL) {printf("ERROR: cannot find file\n"); return 1;}
	// launch the program
	return launcher(p);
}

int kernel() {
	int error_code = shellUI();
	return error_code;
}

void boot() {
	// 1. initialize every cell of the array to null	
	for (int i=0; i<40; i++) {
		ram[i] = NULL;
	}
	// 2. prepare backing store
	// check if directory exists
	DIR* dir = opendir("BackingStore");
	if (dir) {
		// delete the directory and make a new one
		system("rm -r BackingStore");
		system("mkdir BackingStore");
	} else if (ENOENT == errno) {
		// create the directory
		system("mkdir BackingStore");
	} else {
		printf("ERROR: not suppose to happen!\n");
	}
}

// helper function that finds the victim PCB
struct PCB* victimPCBFinder(int frame) {
	struct Node *curr_Node = head;		
	struct PCB *curr_PCB;
	// loop through all PCB page tables and find the PCB containing the frame
	for (int i=0; i<len_ll; i++) {
		curr_PCB = curr_Node -> curr;
		// loop through the table of the PCB page table
		for (int j=0; j<10; j++) {
			if (curr_PCB -> pageTable[j] == frame && curr_PCB -> validTable[j] != 1) {
				// victim must have current PC NOT in the frame range
				curr_PCB -> temp = j;
				return curr_PCB;
			}
		}
		curr_Node = curr_Node -> next;
	}
	return NULL; // returns null if victim PCB has PC pointer currently in frame
}

int main(){
	int error = 0;
	boot();
	error = kernel();
	return error;
}

