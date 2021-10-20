extern struct CPU *pointer_CPU;
int scheduler();
int myinit(char *filename);
void addToReady(struct PCB *pcb);
struct PCB* victimPCBFinder(int frame);
