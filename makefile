# Author: Stanley Wu

mysh: kernel.o shell.o interpreter.o shellmemory.o ram.o pcb.o cpu.o memorymanager.o
	gcc -o mykernel kernel.o shell.o interpreter.o shellmemory.o ram.o pcb.o cpu.o memorymanager.o

kernel.o: kernel.c kernel.h shell.h ram.h pcb.h cpu.h memorymanager.h
	gcc -c kernel.c

shell.o: shell.c shell.h interpreter.h
	gcc -c shell.c

interpreter.o: interpreter.c interpreter.h shell.h shellmemory.h kernel.h
	gcc -c interpreter.c

shellmemory.o: shellmemory.c shellmemory.h 
	gcc -c shellmemory.c

ram.o: ram.c ram.h
	gcc -c ram.c

pcb.o: pcb.c pcb.h
	gcc -c pcb.c

cpu.o: cpu.c cpu.h interpreter.h kernel.h shell.h ram.h pcb.h memorymanager.h
	gcc -c cpu.c

memorymanager.o: memorymanager.c memorymanager.h pcb.h ram.h kernel.h
	gcc -c memorymanager.c

clean:
	rm shell.o interpreter.o shellmemory.o kernel.o ram.o pcb.o cpu.o memorymanager.o

.PHONY: backup
backup:
	cp shell.c interpreter.c shellmemory.c shell.h interpreter.h shellmemory.h kernel.c kernel.h ram.c ram.h pcb.c pcb.h cpu.c cpu.h memorymanager.c memorymanager.h backup 
