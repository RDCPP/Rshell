# Rshell Makefile - RDCPP

CC = gcc
STRIP = strip

S1 = main
S2 = func

all: rshell

rshell: $(S1).o $(S2).o
	$(CC) -o rshell $(S1).o $(S2).o
	$(STRIP) rshell

$(S1).o: $(S1).c
	$(CC) -c $(S1).c

$(S2).o: $(S2).c
	$(CC) -c $(S2).c

clean:
	rm -f *.o rshell
