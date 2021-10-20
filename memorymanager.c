#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include "pcb.h"
#include "ram.h"
#include "kernel.h"

int p_counter = 0; // used for naming a file

int countTotalPages(FILE *f) {
	char line[1000];
	fgets(line, 999, f);
	int num_of_lines = 0;
	while (!feof(f)) {
		num_of_lines++;
		fgets(line, 999, f);
	}
	int num_page = num_of_lines / 4;
	int rem = num_of_lines % 4;
	if (num_page == 0) {
		return 1;
	} else if (rem == 0) {
		return num_page;
	} else {
		return num_page + 1;
	}
}

void loadPage(int pageNumber, FILE *f, int frameNumber) {
	int line_num = pageNumber * 4;
	// loop until pageNumber reached
	int curr_line = 0;
	char line[1000];
	fgets(line, 999, f);
	while (!feof(f)) {
		if (curr_line == line_num) {
			break;
		}
		curr_line++;
		fgets(line, 999, f);
	}
	// store the next 4 lines (1 page) in ram
	int ram_cell_start = frameNumber * 4; // the starting RAM cell of the frame
	char *frame[5];
	for (int i=0; i<4; i++) {
		frame[i] = strdup(line);
		if (fgets(line, 999, f) == NULL) {
			//frame[i] = line;
			continue;
		}
		
	}
	addToRam(frame, ram_cell_start);
}

int findFrame() {
	// search in ram for a free frame and return -1 if failure
	for (int i=0; i < 10; i++) {
		if (ram[i*4] == NULL) {
			return i; // null block found
		}
	}
	return -1;
}

// helper function for findVictim
bool is_active(struct PCB *p, int r) {
	for (int i=0; i<10; i++) {
		if (p -> pageTable[i] == r && p -> validTable[i] == 1) {
			return true;
		}
	}
	return false;
}

int findVictim(struct PCB *p) {
	// select random number	
	int r = rand() % 9;
	// check if frame belongs to PCB
	bool is_active_PCB = is_active(p, r);
	if (!is_active_PCB) {
		return r;
	}
	int i = 0;
	while (i < 11) {
		r = (r + 1) % 10;
		if (!is_active(p, r)) {
			return r;
		}
		i++;
	}
	printf("ERROR: all frames are active\n");
	return 1;
}

// victimFrame --> -1: update current PCB | victimFrame --> 0: update victim PCB
int updatePageTable(struct PCB *p, int pageNumber, int frameNumber, int victimFrame) {
	if (victimFrame == -1) { // update current process
		p -> pageTable[pageNumber] = frameNumber;
		p -> validTable[pageNumber] = 1;
	} else { // update victim process
		(p -> pageTable[pageNumber]) = frameNumber;
		p -> validTable[pageNumber] = 0;
	}
	return 0;
}

// helper function that loads a page into RAM and updates the necessary page tables of relvant PCBs
void load_into_RAM(int page_num, FILE *process_file, struct PCB *pcb) {
	// input: n = page number, 
	// HELPER FUNCTION TO LOAD PROGRAM INTO RAM
	// find an available frame
	int is_victim = false;
	int frame = findFrame();
	// find a victim if necessary 
	if (frame == -1) {
		is_victim = true;
	}
	
	if (!is_victim) { // no victim needed
		// load page into RAM
		loadPage(page_num, process_file, frame);
		updatePageTable(pcb, page_num, frame, -1);
		return;
	}
	
	// update victim pcb
	frame = findVictim(pcb); // find a victim frame first
	struct PCB *victim_PCB = victimPCBFinder(frame); // find the victim PCB in the ready queue with the corresponding frame
	// if cannot steal victim_PCB frame, find another frame
	while (victim_PCB == NULL) {
		frame = findVictim(pcb);
		victim_PCB = victimPCBFinder(frame);
	}
	// load page into RAM
	loadPage(page_num, process_file, frame);
	// update victim frame
	updatePageTable(victim_PCB, victim_PCB -> temp, -1, 0); // -1 indicates no frame 
	// update page table of current pcb
	updatePageTable(pcb, page_num, frame, -1);	
	return;
}

// helper function for 2.4.4.a: gets file from BackingStore
void fetchFile(int process_id, int page_num, struct PCB *pcb) {
	char process_s[20];
	sprintf(process_s, "%d", process_id);
	strcat(process_s, ".txt");
	char process_path[20] = "./BackingStore/";
	strcat(process_path, process_s);
	FILE *process_file = fopen(process_path, "rt");
	load_into_RAM(page_num, process_file, pcb);
	fclose(process_file);
}


int launcher(FILE *p) {
	int error_code = 0;
	// 1. copy the entire file into Backing Store
	char fname[20];
	sprintf(fname, "%d", p_counter);
	strcat(fname, ".txt"); // add .txt to end of file
	p_counter++;
	char process_path[20] = "./BackingStore/";
	strcat(process_path, fname);
	// create new file with process name
	FILE *process_file = fopen(process_path, "w+");
	// copy file 
	int num_of_lines = 0; // variable to store total number of lines (used in cpu.c)
	char line[1000];
	fgets(line, 999, p);
	while (!feof(p)) {
		fputs(line, process_file);
		fgets(line, 999, p);
		num_of_lines++;
	}
	// 2. close the file pointer pointing to the original file
	fclose(p);
	fclose(process_file);
	// 3. open the file in the backing store
	process_file = fopen(process_path, "r");
	rewind(process_file);
	// 4. load two pages of program into RAM
	int pages_needed = countTotalPages(process_file);
	rewind(process_file);
	struct PCB *pcb = makePCB(pages_needed, p_counter - 1, num_of_lines);
	if (pages_needed == 1) {
		load_into_RAM(0, process_file, pcb);
	} else {
		load_into_RAM(0, process_file, pcb);
		rewind(process_file);
		load_into_RAM(1, process_file, pcb);
	}
	// add to ready queue
	addToReady(pcb);
	fclose(process_file);
	return error_code;
}
