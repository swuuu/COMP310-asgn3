#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pcb.h"
#include "interpreter.h"
#include "kernel.h"
#include "shell.h"
#include "ram.h"
#include "memorymanager.h"

struct CPU {
	int IP; // Points to next instruction to execute from ram[]
	int offset; 
	char IR[1000]; // current executing instruction
	int quanta; // 2
};

// page fault helper function
// return values:
// 0: terminate program
// 1: put back to ready queue
int page_fault(struct PCB *pcb) {
	// 1. determine the next page
	int next_page = (pcb -> PC_page) + 1;
	// 2. check if pc_page > pages_max --> if so then terminate
	if (next_page >= (pcb -> pages_max)) {
		return 0;
	}
	// if not, check if frame exists in PageTable array
	// 3. if so, update PCB and go to step 5
	if (pcb -> validTable[next_page] == 1) {
		pcb -> validTable[pcb -> PC_page] = 0;
		pcb -> PC_page = next_page;
		pcb -> PC_offset = 0;
		pcb -> PC = pcb -> pageTable[next_page];
		return 1;
	}
	// 4. if not, a) find page in BackingStore
	// b) find space in RAM
	// c) update the page table(s)
	// d) update RAM frame 
	fetchFile(pcb -> id, next_page, pcb); // does 4b,c,d (from kernel.c) 
	// e) PC = ram[frame]??? and reset PC_offset to 0
	pcb -> validTable[pcb -> PC_page] = 0;
	pcb -> PC = pcb -> pageTable[next_page];
	pcb -> PC_page = next_page;
	pcb -> PC_offset = 0;
	// 5. place PCB to back of ready queue
	return 1;
}


int runx(int quanta) {
	for (int i = 0; i < (pointer_CPU -> quanta); i++) {
		if (quanta == 0) {
			// signals interrupt
			return -1;
		}
		strcpy(pointer_CPU -> IR, ram[(pointer_CPU -> IP)*4 + (pointer_CPU -> offset)]); 
		parse(pointer_CPU -> IR);
		// increment offset and decrement quanta needed left 
		(pointer_CPU -> offset)++;
		quanta--;
		// check if offset == 4: if so, generate pseudo-interrupt
	}
	if (quanta == 0) {
			return -1; // signals interrupt
	}
	return quanta; // return quanta left;
}



