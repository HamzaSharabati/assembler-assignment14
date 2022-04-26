# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = globals.h # Dependencies for everything
EXE_DEPS = main.o utility2.o macro.o pass1.o pass2.o instructions.o table.o utility1.o output_files.o # Deps for exe

## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
main.o: main.c $(GLOBAL_DEPS)
	$(CC) -c main.c $(CFLAGS) -o $@

## helper functions:
utility2.o: utility2.c utility2.h $(GLOBAL_DEPS)
	$(CC) -c utility2.c $(CFLAGS) -o $@
## Macro
macro.o: macro.c macro.h $(GLOBAL_DEPS)
	$(CC) -c macro.c $(CFLAGS) -o $@
## First Pass:
pass1.o: pass1.c pass1.h $(GLOBAL_DEPS)
	$(CC) -c pass1.c $(CFLAGS) -o $@

## Second Pass:
pass2.o: pass2.c pass2.h $(GLOBAL_DEPS)
	$(CC) -c pass2.c $(CFLAGS) -o $@

## Instructions helper functions:
instructions.o: instructions.c instructions.h $(GLOBAL_DEPS)
	$(CC) -c instructions.c $(CFLAGS) -o $@

## Table:
table.o: table.c table.h $(GLOBAL_DEPS)
	$(CC) -c table.c $(CFLAGS) -o $@

## Useful functions:
utility1.o: utility1.c instructions.h $(GLOBAL_DEPS)
	$(CC) -c utility1.c $(CFLAGS) -o $@

## Output Files:
output_files.o: output_files.c output_files.h $(GLOBAL_DEPS)
	$(CC) -c output_files.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o