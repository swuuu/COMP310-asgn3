// Author: Stanley WU

struct CPU {
	int IP; // Points to next instruction to execute from ram[]
	int offset;
	char IR[1000]; // current executing instruction
	int quanta; // 2
};

int page_fault(struct PCB *pcb);
int runx(int quanta);
