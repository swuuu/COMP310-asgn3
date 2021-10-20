#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char *ram[40];

void addToRam(char *to_add[], int start){
	int error_code = 0;
	char line[1000];

	for (int i=0; i<4; i++) {
		ram[start] = to_add[i];
		start++;
	}
	return;
}

void unload(int pageTable[], int validTable[]) {
	int curr_frame;
	int frame;
	for (int i=0; i<10; i++) {
		// check if it is used by current pcb
		if (validTable[i] == 1) {
			// clear the entire frame
			frame = pageTable[i]*4;
			for (int j=0; j<4; j++) {
				curr_frame = frame + j;
				ram[curr_frame] = NULL;
			}
		}
	}
}

// clears the RAM after every exec call
void clear() {
	for (int i=0; i<40; i++) {
		ram[i] = NULL;
	}
}
