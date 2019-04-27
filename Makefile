# Rshell Makefile - RDCPP

CC = gcc
STRIP = strip

S1 = main

all: rshell

rshell: $(S1).o
	$(CC) -o rshell $(S1).o
	$(STRIP) rshell

$(S1).o: $(S1).c
	$(CC) -c $(S1).c

clean:
	rm -f *.o rshell
