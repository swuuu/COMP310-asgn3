struct PCB {
	int PC;
	int pageTable[10];
	int validTable[10];
	int PC_page;
	int PC_offset;
	int pages_max;
	int lines_max; // the total amount of lines of the file
	int id;
	int temp;
};

struct PCB *makePCB(int pages_max, int id, int total_lines);
