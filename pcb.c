#include <stdlib.h>

struct PCB {
	int PC; // int start; 
	// int end;
	int pageTable[10];
	int validTable[10]; // 0: invalid bit, 1: valid bit
	int PC_page;
	int PC_offset;
	int pages_max;
	int lines_max; // the total amount of lines of the file
	int id; // remembers the file in BackingStore
	int temp; // used to pass the page number information when pcb is a victim
};

struct PCB *makePCB(int pages_max, int id, int total_lines) {
	struct PCB *new_PCB = (struct PCB *) malloc(sizeof(struct PCB));
	new_PCB -> PC = 0;
	new_PCB -> PC_page = 0;
	new_PCB -> PC_offset = 0;
	new_PCB -> id = id;
	new_PCB -> pages_max = pages_max;
	new_PCB -> lines_max = total_lines;
	new_PCB -> temp = 0;
	// setting all page frames to 0 and also all bits to invalid
	for (int i=0; i<10; i++) {
		new_PCB -> pageTable[i] = 0;
		new_PCB -> validTable[i] = 0;
	}
	return new_PCB;
}
